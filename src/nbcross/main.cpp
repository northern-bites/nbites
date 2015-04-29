//
//  main.cpp
//  nbcross
//
//  Created by Philip Koch on 11/27/14.
//

#include <sstream>
#include <iostream>
#include <vector>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORTNUM 30002
#define MAX_WAIT 1 //seconds

#include "nbfuncs.h"

#define CHECK_RET(v) if(v) {printf("BAD RET: [%s](%i) at line %i.\n", #v, v, __LINE__); return 2;}


int main(int argc, const char * argv[]) {
    
    register_funcs();
    printf("using %li functions:\n", FUNCS.size());
    for (int i = 0; i < FUNCS.size(); ++i)
        printf("\t%s(%lu)\n", FUNCS[i].name, FUNCS[i].args.size());
    printf("\n\n-----------------------------------------\n");
    
    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET ;
    server.sin_port = htons(PORTNUM);
    
    struct hostent * host = gethostbyname("127.0.0.1");
    bcopy(host->h_addr, &server.sin_addr.s_addr, host->h_length);
    
    printf("sock fd\t= %i\n", fd);
    
    int ret = connect(fd, (struct sockaddr *) &server, (socklen_t) sizeof(struct sockaddr_in) );
    printf("connect\t= %i\n",ret);
    if (ret < 0) return ret;
    
    //Init ping.
    uint32_t host_order = 0; //Host order
    uint32_t net_order = 0; //Network order
    
    CHECK_RET(logio::send_exact(fd, 4, &net_order));
    CHECK_RET(logio::recv_exact(fd, 4, &net_order, MAX_WAIT));
    
    if (net_order != 0) {
        printf("malformed init val: 0x%x\n", net_order);
        return 1;
    }
    
    printf("sending functions...\n");
    
    net_order = htonl(FUNCS.size());
    CHECK_RET(logio::send_exact(fd, 4, &net_order));
    
    std::ostringstream format;
    for (int i = 0; i < FUNCS.size(); ++i) {
        format << FUNCS[i].name << '=';
        for (int j = 0; j < FUNCS[i].args.size(); ++j) {
            format << ' ' << FUNCS[i].args[j];
        }
        
        format << '\n';
    }
    
    std::string funcstr = format.str();
    //printf("\"\n%s\"\n", funcstr.c_str());
    
    char buf[100];
    snprintf(buf, 100, "type=functionlist fn=%lu", FUNCS.size());
    
    logio::log_t functionsLog;
    functionsLog.desc = buf;
    functionsLog.dlen = funcstr.size();
    functionsLog.data = (uint8_t *) funcstr.data();
    
    CHECK_RET(logio::send_log(fd, &functionsLog));
    
    //Confirm java got the right number of functions.
    CHECK_RET(logio::recv_exact(fd, 4, &net_order, MAX_WAIT));
    host_order = ntohl(net_order);
    if (host_order != FUNCS.size()) {
        printf("java sent wrong confirmation of FUNCS.size(): %i\n", host_order);
        return 1;
    }
    
    printf("functions sent... waiting for calculation requests.\n");
    
    for (;;) {
        CHECK_RET(logio::recv_exact(fd, 4, &net_order, MAX_WAIT));
        if (ntohl(net_order) == 0) {
            host_order = 0;
            CHECK_RET(logio::send_exact(fd, 4, &host_order));
            usleep(10000);
            continue;
        } else if (ntohl(net_order) != 1) {
            printf("java sent wrong function call request: 0x%x\n", net_order);
            return 1;
        }
        
        uint32_t findex;
        CHECK_RET(logio::recv_exact(fd, 4, &findex, MAX_WAIT));
        findex = ntohl(findex);
    
        assert(findex >= 0);
        assert(findex < FUNCS.size());
        
        for (int i = 0; i < args.size(); ++i) {free(args[i].desc); free(args[i].data);}
        args.clear();
        
        int na = FUNCS[findex].args.size();
        
        for (int i = 0; i < na; ++i) {
            logio::log_t arg;
            logio::recv_log(fd, &arg, MAX_WAIT);
            
            const char * type = FUNCS[findex].args[i];
            std::string need = "type=";
            need.append(type, strlen(type));
            std::string got(arg.desc);
            
            if (got.find(need) == std::string::npos) {
                printf("arg %i [%s] did NOT match type=%s!\n", i, arg.desc, type);
                return 1;
            }
            
            args.push_back(arg);
        }
        
        assert(args.size() == FUNCS[findex].args.size());
        printf("calling function [%s]"
               "\n-------------------------------------------\n", FUNCS[findex].name);
        //Clear ret logs, call function.
        for (int i = 0; i < rets.size(); ++i) {free(rets[i].desc); free(rets[i].data);}
        rets.clear();
        int ret = FUNCS[findex].func();
        
        printf("\n-------------------------------------------\n");
        
        printf("function returned with ret:%i, sending %lu output logs.\n", ret, rets.size());
        net_order = htonl(ret);
        CHECK_RET(logio::send_exact(fd, 4, &net_order));
        net_order = htonl(rets.size());
        CHECK_RET(logio::send_exact(fd, 4, &net_order));
        
        for (int i = 0; i < rets.size(); ++i) {
            CHECK_RET(logio::send_log(fd, &(rets[i])));
        }
        
        CHECK_RET(logio::recv_exact(fd, 4, &net_order, MAX_WAIT));
        
        if (ntohl(net_order) != rets.size()) {
            printf("java sent bad confirmation of end function call (wanted %lu, got %i)\n", rets.size(), ntohl(net_order));
            return 1;
        }
        
        printf("function call completed\n");
    }
    
    return 0;
}


