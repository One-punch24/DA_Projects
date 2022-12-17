#pragma once
#include<iostream>
#include<cstring>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<thread>
#include<vector>
using namespace std;

class Message{
public:
    unsigned long m_type; // 0 - Proposal 1-ACK 2-NACK 
    unsigned long iter;
    unsigned long original_id;
    unsigned long proposal_num; // For aggrement multiple rotation
    unsigned long value_set_num;
    unsigned long value_set[1024];
    
    Message(){
        this->original_id=0;
        this->value_set_num=0;
    } 
    Message(unsigned long type_,unsigned long iter,unsigned long original_id, 
    unsigned long proposal_num,set<long unsigned> & value_set_){
        this->original_id=original_id;
        this->iter=iter;
        this->m_type=type_;
        this->proposal_num=proposal_num;
        this->value_set_num=value_set_.size();
        int i=0;
        for (auto it=value_set_.begin();it!=value_set_.end();it++){
            value_set[i]=*it;
            i++;
        }
    }

    // For Ack
    Message(unsigned long type_,unsigned long iter,unsigned long original_id, 
    unsigned long proposal_num){
        this->original_id=original_id;
        this->iter=iter;
        this->m_type=type_; // Must be 1 ;
        this->proposal_num=proposal_num;

    }
    void to_byte_for_socket(char * message){
        //char message[sizeof(this)];
        memcpy(message, this,sizeof(Message));
    }
    bool operator <(const Message & m)const {
        if(this->original_id<m.original_id){
            return true;
        }
        else if( this->original_id==m.original_id && this->iter<m.iter){
            return true;
        }
        else if(this->iter==m.iter&&this->proposal_num<m.proposal_num){
            return true;
        }
        else if(this->proposal_num==m.proposal_num && this->m_type<m.m_type){
            return true;
        }
        else if(this->m_type==2 && m.m_type==2 && this->value_set_num<m.value_set_num){
            return true;
        }
        else{
            return false;
        }
    } 
};

