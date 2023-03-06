#include<iostream>
#include<cstring>
#include<vector>
#include<map>
#include<set>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<fstream>
#include "PerfectLink.hpp"
#include"utils.h"
#include"message.hpp"


using namespace std;
struct sockaddr_in ConstructNetAddr(in_addr_t dest_ip, unsigned short dest_port){

        struct sockaddr_in dest_addr;
        bzero(&dest_addr,sizeof(dest_addr));
        dest_addr.sin_family=AF_INET;
        dest_addr.sin_addr.s_addr=dest_ip;
        dest_addr.sin_port=dest_port;
        return dest_addr;
}


void OutVectoFile(const char* fn,vector<Message> &dl_queue){

        ofstream out_file(fn);
        if (!out_file.is_open()) {
                
        }
        else{
                for(long unsigned int i=0;i<window_send_main;i++){
                        out_file<<"b "<<i+1<<endl;
                }
                for(long unsigned int i=0;i<dl_queue.size();i++){
                        out_file<<"d "<<dl_queue[i].original_id<<" "<<dl_queue[i].seq_n<<endl;
                }
           
        }
              
}




