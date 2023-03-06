#pragma once
#include<iostream>
#include<sys/socket.h>

#include"parse.hpp"
#include"FairlossLink.hpp"

class StubbornLink{
public:

    FairlossLink fl_link;
    int max_send_times;
    int socket_fd;
    StubbornLink(){

    }
    StubbornLink(unsigned long host_id, struct sockaddr_in host_addr,int max_send_times,int socket_fd){
        this->fl_link=FairlossLink(host_id, host_addr,socket_fd);
        this->max_send_times=max_send_times;
        this->socket_fd=socket_fd;
    }

    void send(Message sl_m, sockaddr_in dest_addr){
        for(int i=0;i<max_send_times;i++){
            this->fl_link.send(sl_m,  dest_addr);
        }
        // while(true){
        //     this->fl_link.send(sl_m,  dest_addr);
        // }
    }

    Message deliver(){
        return fl_link.deliver();

    }


};