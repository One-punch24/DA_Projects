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
    unsigned long seq_n;
    
    Message(){} 
    Message(unsigned long original_id,unsigned long forwarder_id,long unsigned int seq_n){
        this->original_id=original_id;
        this->forwarder_id=forwarder_id;
        this->seq_n=seq_n;  

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
        else if(this->seq_n==m.seq_n&&this->original_id==m.original_id&& this->forwarder_id<m.forwarder_id){
            return true;
        }
        else{
            return false;
        }
    } 
};

