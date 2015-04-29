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
#include "exactio.h"

#define CHECK_RET(v) if(v) {printf("BAD RET: [%s](%i) at line %i.\n", #v, v, __LINE__); return 2;}

using nblog::Log;
using nblog::SExpr;

std::vector<Log *> args;
std::vector<Log *> rets;

int main(int argc, const char * argv[]) {
    CrossFunc("e", NULL, {"a", "b"});
    
    std::string instance_name;
    
    if (argc > 1) {
        printf("using name: [%s]\n", argv[1]);
        instance_name = std::string(argv[1]);
    } else {
        printf("using null name.\n");
    }
    
    printf("using %li functions:\n", FUNCS.size());
    for (int i = 0; i < FUNCS.size(); ++i)
        printf("\t%s(%lu)\n", FUNCS[i].name.c_str(), FUNCS[i].args.size());
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
    
    CHECK_RET(send_exact(fd, 4, &net_order));
    CHECK_RET(recv_exact(fd, 4, &net_order, MAX_WAIT));
    
    if (net_order != 0) {
        printf("malformed init val: 0x%x\n", net_order);
        return 1;
    }
    
    printf("sending functions...\n");
    
    net_order = htonl(FUNCS.size());
    CHECK_RET(send_exact(fd, 4, &net_order));
    
    //Could put this in SExprs, but it's already working like this...
    std::ostringstream format;
    for (int i = 0; i < FUNCS.size(); ++i) {
        format << FUNCS[i].name << '=';
        for (int j = 0; j < FUNCS[i].args.size(); ++j) {
            format << ' ' << FUNCS[i].args[j];
        }
        
        format << '\n';
    }
    
    std::string funcstr = format.str();
    std::vector<SExpr> contents= {
        SExpr("nfuncs", (int) FUNCS.size()),
        SExpr("name", instance_name)
    };
    
    Log functions("nbcross", "nbcross/main", time(NULL), NBCROSS_VERSION, contents, funcstr);
    
    CHECK_RET(functions.send(fd));
    
    //Confirm java got the right number of functions.
    CHECK_RET(recv_exact(fd, 4, &net_order, MAX_WAIT));
    host_order = ntohl(net_order);
    if (host_order != FUNCS.size()) {
        printf("java sent wrong confirmation of FUNCS.size(): %i\n", host_order);
        return 1;
    }
    
    printf("functions sent... waiting for calculation requests.\n");
    for (;;) {
        CHECK_RET(recv_exact(fd, 4, &net_order, MAX_WAIT));
        if (ntohl(net_order) == 0) {
            host_order = 0;
            CHECK_RET(send_exact(fd, 4, &host_order));
            usleep(10000);
            continue;
        } else if (ntohl(net_order) != 1) {
            printf("java sent wrong function call request: 0x%x\n", net_order);
            return 1;
        }
        
        uint32_t findex;
        CHECK_RET(recv_exact(fd, 4, &findex, MAX_WAIT));
        findex = ntohl(findex);
    
        assert(findex >= 0);
        assert(findex < FUNCS.size());
        
        assert(args.size() == 0);
        assert(rets.size() == 0);
        
        int na = FUNCS[findex].args.size();
        
        for (int i = 0; i < na; ++i) {
            
            Log * recvd = Log::recv(fd, MAX_WAIT);
            CHECK_RET(recvd != NULL);
            
            SExpr * contents = recvd->tree().find("contents");
            
            if (!contents || !contents->get(0)->isAtom()) {
                printf("arg %i wrong format!\n", i);
                return 1;
            }
            
            std::string type = contents->get(0)->value();
            if (type != FUNCS[findex].args[i]) {
                printf("arg %i [%s] did NOT match type=%s!\n", i, type.c_str(), FUNCS[findex].args[i].c_str());
                return 1;
            }
            
            args.push_back(recvd);
        }
        
        assert(args.size() == FUNCS[findex].args.size());
        printf("calling function [%s]"
               "\n-------------------------------------------\n", FUNCS[findex].name.c_str());
        
        int ret = FUNCS[findex].func();
        
        printf("\n-------------------------------------------\n");
        
        printf("function returned with ret:%i, sending %lu output logs.\n", ret, rets.size());
        net_order = htonl(ret);
        CHECK_RET(send_exact(fd, 4, &net_order));
        net_order = htonl(rets.size());
        CHECK_RET(send_exact(fd, 4, &net_order));
        
        for (int i = 0; i < rets.size(); ++i) {
            CHECK_RET(rets[i]->send(fd));
        }
        
        CHECK_RET(recv_exact(fd, 4, &net_order, MAX_WAIT));
        
        if (ntohl(net_order) != rets.size()) {
            printf("java sent bad confirmation of end function call (wanted %lu, got %i)\n", rets.size(), ntohl(net_order));
            return 1;
        }
        
        printf("cleaning up function... ");
        for (int i = 0; i < args.size(); ++i) {delete args[i];}
        args.clear();
        
        for (int i = 0; i < rets.size(); ++i) {delete rets[i];}
        rets.clear();
        printf("done\n");
        
        printf("function call completed\n");
    }
    
    return 0;
}


