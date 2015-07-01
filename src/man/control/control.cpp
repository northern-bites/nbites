#include "Log.h"
#include "exactio.h"
#include "nbdebug.h"
#include "DebugConfig.h"
#include "control.h"
#include "../log/logging.h"

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <limits.h>
#include <signal.h>
#include <time.h>

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <exception>
#include "../../share/logshare/SExpr.h"

using nblog::SExpr;
using nblog::Log;

namespace control {
    
    pthread_t control_thread;
    static bool STARTED = false;
    
    uint32_t cnc_test(Log * arg) {
        printf("\tcnc_test:[%s] %lu bytes of data.\n", arg->description().c_str(),
               arg->data().size());
        return 0;
    }
    
    volatile uint8_t flags[num_flags];
    //expects two bytes of data:
    //  flag index
    //  new flag value (0 or 1)
    uint32_t cnc_setFlag(Log * arg) {
        
        size_t u = arg->data().size();
        
        printf("cnc_setFlag() len=%lu\n", u);
        
        if (u != 2) { //need (index, value)
            printf("cnc_setFlag() wrong number of bytes.\n");
            return 1;
        }
        
        uint8_t index = arg->data()[0];
        uint8_t value = arg->data()[1];
        
        if (index >= num_flags) {
            printf("cnc_setFlag() flag index OOB: %i\n", index);
            return 2;
        }
        
        if (value > 1) {
            printf("cnc_setFlag() flag value OOB: %i\n", value);
            return 3;
        }
        
        switch (index) {
            case serv_connected:
                printf("cnc_setFlag(): ERROR: CANNOT SET serv_connected!\n");
                break;
            case control_connected:
                printf("cnc_setFlag(): ERROR: CANNOT SET cnc_connected!\n");
                break;
            case fileio: //setting fileio
                if (value) {
                    memcpy(nblog::fio_start, nblog::total, sizeof(nblog::io_state_t) * NUM_LOG_BUFFERS);
                    
                    nblog::fio_upstart = time(NULL);
                }
                
                flags[fileio] = value;
                break;
                
            default:
                flags[index] = value;
                break;
        }
        
        return 0;
    }

    uint32_t cnc_setCameraParams(Log * arg) {
        size_t u = arg->data().size();
        bool success = receivedParams.ParseFromString(arg->data());
        if(!success) {
            std::cerr<<"Failed to Parse Params\n";
        } else {
            SExpr s;

            SExpr h = SExpr("hflip",receivedParams.h_flip());
            SExpr v = SExpr("vflip",receivedParams.v_flip());
            SExpr ae = SExpr("autoexposure",receivedParams.auto_exposure());
            SExpr b = SExpr("brightness",receivedParams.brightness());
            SExpr c = SExpr("contrast",receivedParams.contrast());
            SExpr sat = SExpr("saturation",receivedParams.saturation());
            SExpr hue = SExpr("hue",receivedParams.hue());
            SExpr sharp = SExpr("sharpness",receivedParams.sharpness());
            SExpr gamma = SExpr("gamma",receivedParams.gamma());
            SExpr awb = SExpr("auto_whitebalance",receivedParams.autowhitebalance());
            SExpr expo = SExpr("exposure",receivedParams.exposure());
            SExpr gain = SExpr("gain",receivedParams.gain());
            SExpr wb = SExpr("white_balance",receivedParams.whitebalance());
            SExpr ftb = SExpr("fade_to_black",receivedParams.fadetoblack());

            s.append(h);
            s.append(v);
            s.append(ae);
            s.append(b);
            s.append(c);
            s.append(sat);
            s.append(hue);
            s.append(sharp);
            s.append(gamma);
            s.append(awb);
            s.append(expo);
            s.append(gain);
            s.append(wb);
            s.append(ftb);

            std::string stringToWrite = s.serialize();

            #ifdef V5_ROBOT
                std::cout<<"Saving as V5"<<std::endl;
                if(receivedParams.whichcamera() == "TOP"){
                    std::cout<<"TOP Params Received"<<std::endl;
                    std::ofstream file("/home/nao/nbites/Config/V5topCameraParams.txt");
                    std::cout<<stringToWrite<<std::endl;
                    file << stringToWrite;
                    file.close();
                    std::cout<<"Saving Done"<<std::endl;
                } else  {
                    std::cout<<"Bottom Params Received"<<std::endl;
                    std::ofstream file("/home/nao/nbites/Config/V5bottomCameraParams.txt");
                    std::cout<<stringToWrite<<std::endl;
                    file << stringToWrite;
                    file.close();
                    std::cout<<"Saving Done"<<std::endl;
                }
            #else
                std::cout<<"Saving as V4"<<std::endl;
                if(receivedParams.whichcamera() == "TOP"){
                    std::cout<<"TOP Params Received"<<std::endl;
                    std::ofstream file("/home/nao/nbites/Config/V4topCameraParams.txt");
                    file << stringToWrite;
                    file.close();
                    std::cout<<"Saving Done"<<std::endl;
                } else  {
                    std::cout<<"Bottom Params Received"<<std::endl;
                    std::ofstream file("/home/nao/nbites/Config/V4bottomCameraParams.txt");
                    file << stringToWrite;
                    file.close();
                    std::cout<<"Saving Done"<<std::endl;
                }
            #endif
        }
        return 0;
    }
    
    /*
     THIS IS WHERE YOU PUT NEW CONTROL FUNCTIONS!
     
     note that the function prototype must exacty match what's listed below.
     */
    std::map<std::string, uint32_t (*)(Log *)> init_fmap() {
        std::map<std::string, uint32_t (*)(Log *)> ret;
        
        ret["test"] = &cnc_test;
        ret["setFlag"] = &cnc_setFlag;
        ret["setCameraParams"] = &cnc_setCameraParams;
        
        return ret;
    }
    std::map<std::string, uint32_t (*)(Log *)> fmap = init_fmap();
    
#define CHECK_RET(r) {if (r) goto connection_died;}
    
    void * cnc_loop(void * cntxt) {
        int listenfd = -1, connfd = -1;
        struct sockaddr_in serv_addr;
        
        //Network socket, TCP streaming protocol, default options
        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        NBDEBUG( "control listenfd=[%i]\n", listenfd);
        //memset(&serv_addr, '0', sizeof(serv_addr));
        memset(&serv_addr, 0, sizeof(serv_addr));
        
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(CONTROL_PORT);
        
        bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
                
        //Accepting connections on this socket, no queue.
        listen(listenfd, 0);
        
        NBDEBUG("control listening... port = %i\n", CONTROL_PORT);
        
        for (;;) {
            connfd = block_accept(listenfd, CONTROL_USLEEP_WAITING);
            NBDEBUG("control FOUND CLIENT [%i]\n", connfd);
            nblog::cnc_upstart = time(NULL);
            flags[control_connected] = 1;
            
            for (;; usleep(CONTROL_USLEEP_WAITING)) {
                uint32_t ping = 0;
                uint32_t resp;
                
                CHECK_RET(send_exact(connfd, 4, (uint8_t *) &ping) )
                CHECK_RET(recv_exact(connfd, 4, (uint8_t *) &resp, IO_SEC_TO_BREAK))
                
                if (ntohl(resp) == 1) {
                    //cnc call coming in:
                    
                    Log * found = Log::recv(connfd, IO_SEC_TO_BREAK);
                    if (!found)
                        goto connection_died;
                    SExpr& desc = found->tree();
                    
                    if (desc.count() < 2 ||
                        !(desc.get(0)->isAtom()) ||
                        !(desc.get(0)->value() == nblog::COMMAND_FIRST_ATOM_S) ||
                        !(desc.get(1)->isAtom())    //command name
                        ) {
                        NBDEBUG("control: INVALID COMMAND LOG!\n");
                        delete found;
                        goto connection_died;
                    }
                    
                    std::string name = desc.get(1)->value();
                    
                    if (fmap.find(name) == fmap.end()) {
                        NBDEBUG( "control: could not find command[%s] in fmap!\n", name.c_str());
                        delete found;
                        goto connection_died;
                    }
                    uint32_t ret = -1;
                    try {
                        ret = fmap[name](found);
                    } catch (...) {
                        printf("ERROR: Caught exception while running control function %s!\n",
                               name.c_str());
                    }
                    NBDEBUG( "control command returned %i\n", ret);
                    delete found;
                    
                    uint32_t nret = htonl(ret);
                    CHECK_RET(send_exact(connfd, 4, &nret))
                    
                    NBDEBUG( "control call finished.\n\n");
                    
                } else {
                    if (resp != 0) {
                        NBDEBUG( "control got bad ping reply! %i\n", resp);
                        goto connection_died;
                    }
                    
                    usleep(CONTROL_USLEEP_WAITING);
                }
            }
            
            //Acts like an exception.
        connection_died:
            flags[control_connected] = 0;
            close(connfd);
            
            NBDEBUG("control loop broken, connection closed.\n");
        }
        
        return NULL;
    }

    
    void control_init() {
        NBDEBUG("control_init() with %i functions.\n", fmap.size());
        NBLassert(!STARTED);
        STARTED = true;
        
        bzero((void *) flags, num_flags);

        pthread_create(&control_thread, NULL, &cnc_loop, NULL);
        pthread_detach(control_thread);
    }
    
}
