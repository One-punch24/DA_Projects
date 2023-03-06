#pragma once
#include<iostream>
#include<sys/socket.h>
#include"message.hpp"
#include"parse.hpp"

using namespace std;

class UDPReceiver{
public:
    struct sockaddr_in host_addr;
    
    unsigned long host_id;

    int socket_fd;


    UDPReceiver(){

    }
    UDPReceiver(unsigned long host_id, struct sockaddr_in host_addr,int socket_fd){
        this->host_id=host_id;
        this->host_addr=host_addr;
        this->socket_fd=socket_fd;


    }

    Message receive(){
        // Later we might need to change this char array.

        char messa_netstr[sizeof(Message)];
        
        struct sockaddr * sender_addr=new struct sockaddr;
        socklen_t slen=sizeof(sender_addr);   
        if(       
            recvfrom(socket_fd,messa_netstr,sizeof(Message),0,\
            sender_addr,&slen)>0){
                //cout<<"Revice Some In"<<endl;
            Message messa;
            memcpy(&messa,messa_netstr,sizeof(messa_netstr));
            //cout<<"d at receiver "<<messa.original_id<<" "<<messa.seq_n<<endl;
            return messa;
        }
        else{
            perror("Something wrong");
            struct sockaddr_in nmd;
            return Message(0,0,0);
        }
      
    }

};

class UDPSender{
public:
    struct sockaddr_in host_addr;
    
    unsigned long host_id;

    int socket_fd;


    UDPSender(){
    }


    UDPSender(unsigned long host_id, struct sockaddr_in host_addr,int socket_fd){
        this->host_id=host_id;
        this->host_addr=host_addr;
        this->socket_fd=socket_fd;
    }

    void send(Message messa,struct sockaddr_in dest_addr ){
                
        char messa_netstr[sizeof(messa)];
        //cout<<messa.original_id<<" "<<messa.seq_n<<endl;
        messa.to_byte_for_socket(messa_netstr); 
        ssize_t ref=sendto(socket_fd,messa_netstr,sizeof(messa),0, \
        reinterpret_cast<const struct sockaddr *>(& dest_addr),sizeof(struct sockaddr));

        if(ref<0){
            cout<<"Fail to send"<<endl;
        }
        
    }

};
