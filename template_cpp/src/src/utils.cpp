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
// #include"message.hpp"


using namespace std;
struct sockaddr_in ConstructNetAddr(in_addr_t dest_ip, unsigned short dest_port){

        struct sockaddr_in dest_addr;
        bzero(&dest_addr,sizeof(dest_addr));
        dest_addr.sin_family=AF_INET;
        dest_addr.sin_addr.s_addr=dest_ip;
        dest_addr.sin_port=dest_port;
        return dest_addr;
}


void OuttoFile(const char* fn){

        ofstream out_file(fn);
        if (!out_file.is_open()) {
                
        }
        else{
            for(long unsigned int i=0;i<Todecide.size();i++){
                for(auto it=Todecide[i].begin();it!=Todecide[i].end();it++){
                   out_file<<*it<<" ";
                }
                out_file<<endl;
            }
           
        }
              
}




