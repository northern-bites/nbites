//
//  main.cpp
//  nbcross
//
//  Created by Philip Koch on 11/27/14.
//  Copyright (c) 2014 pkoch. All rights reserved.
//

#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORTNUM 30002

#include "nbfuncs.h"

#define RECV_E(fd, b, n, f) {int __RET = recv_e(fd,b,n,f); \
        if (__RET) {printf("Bad recv() [%i] at line %i.", __RET, __LINE__); return 2;} }

#define SEND_E(fd, b, n, f) {int __RET = send_e(fd,b,n,f); \
if (__RET) {printf("Bad send() [%i] at line %i.", __RET, __LINE__); return 3;} }

int recv_e(int fd, void * b, size_t n, int f) {
    size_t read = 0;
    while (read < n) {
        int ret = (int) recv(fd, ((uint8_t *) b) + read, n - read, f);
        
        if (ret < 0)
            return ret;
        if (ret == 0)
            usleep(50);
        else
            read += ret;
    }
    
    return 0;
}

int send_e(int fd, void * b, size_t n, int f) {
    size_t written = 0;
    while (written < n) {
        
        int ret = (int) send(fd, ((uint8_t *) b) + written, n - written, f);
        
        if (ret < 0)
            return ret;
        if (ret == 0)
            usleep(50); //Random amount...not even sure this is necessary with blocking... oh well.
        else
            written += ret;
    }
    
    return 0;
}


int main(int argc, const char * argv[]) {
    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET ;
    server.sin_port = htons(PORTNUM);
    
    struct hostent * host = gethostbyname("127.0.0.1");
    bcopy(host->h_addr, &server.sin_addr.s_addr, host->h_length);
    
    printf("fd:%i host pointer:%p\n", fd, host->h_addr);
    
    int ret = connect(fd, (struct sockaddr *) &server, (socklen_t) sizeof(struct sockaddr_in) );
    printf("connect = %i\n",ret);
    if (ret < 0) return ret;
    
    register_funcs();
    printf("using %li functions:\n", FUNCS.size());
    for (int i = 0; i < FUNCS.size(); ++i)
        printf("\t%s\n", FUNCS[i].name);
    printf("\n\n-----------------------------------------\n");
    
    //Init ping.
    uint32_t host_order = 0; //Host order
    uint32_t net_order = 0; //Network order
    
    SEND_E(fd, &net_order, 4, 0);
    RECV_E(fd, &net_order, 4, 0);
    
    if (net_order != 0) {
        printf("malformed init val: 0x%x\n", net_order);
        return 1;
    }
    
    printf("sending functions...\n");
    
    net_order = htonl(FUNCS.size());
    SEND_E(fd, &net_order, 4, 0);
    
    for (int i = 0 ; i < FUNCS.size(); ++i) {
        host_order = strlen(FUNCS[i].name);
        net_order = htonl(host_order);
        printf("\t%s:\n", FUNCS[i].name);
        SEND_E(fd, &net_order, 4, 0);
        SEND_E(fd, FUNCS[i].name, host_order, 0);
        
        host_order = FUNCS[i].num_args;
        net_order = htonl(host_order);
        SEND_E(fd, &net_order, 4, 0);
        for (int j = 0; j < FUNCS[i].num_args; ++j) {
            host_order = strlen(FUNCS[i].arg_names[j]);
            net_order = htonl(host_order);
            printf("\t\t%s\n", FUNCS[i].arg_names[j]);
            
            SEND_E(fd, &net_order, 4, 0);
            SEND_E(fd, FUNCS[i].arg_names[j], host_order, 0);
        }
    }
    
    //Confirm java got the right number of args.
    RECV_E(fd, &net_order, 4, 0);
    host_order = ntohl(net_order);
    if (host_order != FUNCS.size()) {
        printf("java sent wrong confirmation of FUNCS.size(): %i\n", host_order);
        return 1;
    }
    printf("functions sent... waiting for calculation requests.\n");
    for (;;) {
        RECV_E(fd, &net_order, 4, 0);
        if (ntohl(net_order) == 0) {
            host_order = 0;
            SEND_E(fd, &host_order, 4, 0);
            usleep(200000);
            continue;
        } else if (ntohl(net_order) != 1) {
            printf("java sent wrong function call request: 0x%x\n", net_order);
            return 1;
        }
        
        //Send call ack.
        host_order = 1;
        net_order = htonl(host_order);
        SEND_E(fd, &net_order, 4, 0);
        
        RECV_E(fd, &net_order, 4, 0);
        int func_index = ntohl(net_order);
        RECV_E(fd, &net_order, 4, 0);
        host_order = ntohl(net_order);
        
        //Save strlen in host_order for string comparison in a few lines.
        char * buf = (char *) malloc(host_order + 1);
        RECV_E(fd, buf, host_order, 0);
        buf[host_order] = '\0';
        
        RECV_E(fd, &net_order, 4, 0);
        int num_args = ntohl(net_order);
        
        //Check sanity.
        if ( !(func_index < FUNCS.size()) ||
            (FUNCS[func_index].num_args != num_args) ||
            (strncmp(FUNCS[func_index].name, buf, host_order))) {
            printf("java got some part of function request wrong: f:%i n:%i name:%s\n", func_index, num_args, buf);
            return 1;
        }
        
        printf("beginning to service function request [%i] [%s]\n", func_index, buf);
        free(buf);
        
        for (int i = 0; i < args.size(); ++i) {free(args[i].desc); free(args[i].data);}
        args.clear();
        
        for (int i = 0; i < num_args; ++i) {
            nbopaquelog_t log;
            RECV_E(fd, &net_order, 4, 0);
            host_order = ntohl(net_order);
            log.desc = (char *) malloc(host_order + 1);
            log.desc[host_order]= '\0';
            
            RECV_E(fd, log.desc, host_order, 0);
            
            printf("\t getting arg: %s\n", log.desc);
            
            RECV_E(fd, &net_order, 4, 0);
            log.data_len = ntohl(net_order);
            log.data = (uint8_t *) malloc(log.data_len);
            
            RECV_E(fd, log.data, log.data_len, 0);
            
            //Check that we actually got the right type of arg.
            std::string str(log.desc);
            std::string arg_type("type=");
            arg_type.append(FUNCS[func_index].arg_names[i]);
            
            if (str.find(arg_type) == std::string::npos) {
                printf("got log with desc\n\t%s\n\twanted: %s\n", log.desc, FUNCS[func_index].arg_names[i]);
                return 1;
            }
            
            args.push_back(log);
        }
        
        net_order = htonl(num_args);
        SEND_E(fd, &net_order, 4, 0);
        
        printf("calling function [%s]"
               "\n-------------------------------------------\n", FUNCS[func_index].name);
        //Clear ret logs, call function.
        for (int i = 0; i < rets.size(); ++i) {free(rets[i].desc); free(rets[i].data);}
        rets.clear();
        int ret = FUNCS[func_index].func();
        
        printf("\n-------------------------------------------\n");
        
        net_order = htonl(2);
        SEND_E(fd, &net_order, 4, 0);
        RECV_E(fd, &net_order, 4, 0);
        if (ntohl(net_order) != 2) {
            printf("java's response to function returned was not kosher (wanted 2, got 0x[%x]).\n", net_order);
            return 1;
        }
        
        printf("function returned with ret:%i, sending %lu output logs.\n", ret, rets.size());
        net_order = htonl(ret);
        SEND_E(fd, &net_order, 4, 0);
        net_order = htonl(rets.size());
        SEND_E(fd, &net_order, 4, 0);
        
        for (int i = 0; i < rets.size(); ++i) {
            nbopaquelog_t log = rets[i];
            host_order = strlen(log.desc);
            net_order = htonl(host_order);
            SEND_E(fd, &net_order, 4, 0);
            SEND_E(fd, rets[i].desc, host_order, 0);
            host_order = rets[i].data_len;
            net_order = htonl(host_order);
            SEND_E(fd, &net_order, 4, 0);
            SEND_E(fd, rets[i].data, host_order, 4);
        }
        
        host_order = 0;
        SEND_E(fd, &host_order, 4, 0);
        RECV_E(fd, &host_order, 4, 0);
        if (host_order != 0) {
            printf("java sent bad confirmation of end function call (wanted 0, got 0x%x)\n", host_order);
            return 1;
        }
        
        printf("function call completed\n");
    }
    
    return 0;
}


