# pragma once
#include<iostream>
#include<sys/socket.h>
#include"UDP.hpp"
#include"parse.hpp"

class FairlossLink{
public:
    UDPSender fl_udpsender;
    UDPReceiver fl_udpreceiver;
    int socket_fd;
    FairlossLink(){};

    FairlossLink(unsigned long host_id, struct sockaddr_in host_addr,int socket_fd){
        this->fl_udpsender=UDPSender( host_id, host_addr,socket_fd);
        this->fl_udpreceiver=UDPReceiver(host_id, host_addr,socket_fd);
        this->socket_fd=socket_fd;
    }

    void send(Message m,struct sockaddr_in dest_addr){        
        fl_udpsender.send(m,dest_addr); 
    }

    Message deliver(){
        return this->fl_udpreceiver.receive();
        
    }


};

