#pragma once
#include<iostream>
#include<cstring>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<thread>
class Message{
public:
    unsigned long original_id;
    unsigned long forwarder_id;
    int long seq_n;
    struct sockaddr_in dest_addr;
    Message(){} 
    Message(unsigned long original_id,unsigned long forwarder_id,int seq_n,struct sockaddr_in dest_addr){
        this->original_id=original_id;
        this->forwarder_id=forwarder_id;
        this->seq_n=seq_n;  
        this->dest_addr=dest_addr;
    }
    void to_byte_for_socket(char * message){
        //char message[sizeof(this)];
        memcpy(message, this,sizeof(Message));
    }
    bool operator <(const Message & m)const {
        if(this->seq_n<m.seq_n){
            return true;
        }
        else if( this->seq_n==m.seq_n&&this->original_id<m.original_id){
            return true;
        }
        else{
            return false;
        }
    } 
};

