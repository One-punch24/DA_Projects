#include<iostream>
#include<cstring>
#include<vector>
#include<map>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<fstream>
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

vector<Message> BroadcastMessages(map<long unsigned int,struct sockaddr_in> host_dicts, unsigned long myid, int num,unsigned long target_id){
       vector<Message> messa_send;
        // if(target_id==myid){
        //         for(int i =0;i<num;i++){
        //                 Message m(myid,0,i+1,host_dicts[target_id]);
        //                 messa_todeliver.push_back(m);
        //         } 
        // }
        // else 
        if(target_id!=myid){
             
                for(int i =0;i<num;i++){
                        Message m(myid,0,i+1,host_dicts[target_id]);
                        messa_send.push_back(m);
                }
        }
        return messa_send;
}

void OutputFile(const char* fn,vector<Message> &br_queue, vector<Message> &dl_queue){

        ofstream out_file(fn);
        if (!out_file.is_open()) {
                
        }
        else{
           
                for(long unsigned int i=0;i<br_queue.size();i++){       
                        out_file<<"b "<<br_queue[i].seq_n<<endl;
                }
                for(long unsigned int i=0;i<dl_queue.size();i++){
                        out_file<<"d "<<dl_queue[i].original_id<<" "<<dl_queue[i].seq_n<<endl;
                }
        }
              
}

