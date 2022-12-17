#pragma once
#include<iostream>
#include<cstring>
#include<thread>
#include <map>
#include<queue>
#include"message.hpp"
#include"PerfectLink.hpp"
#include"utils.h"

using namespace std;
/****************************************************
 *  Sucessful URB
****************************************************/

class URBroadcast{
public:

    int socket_fd;
    long unsigned int myid;
    long unsigned int numof_messa;
    long unsigned int host_num;
    long unsigned int my_window_send;
    long unsigned int my_window_step;
    long unsigned int my_count_receive;


    vector<struct sockaddr_in> host_dicts;
    PerfectLink pl_link;

    MessaToBroadcast ToBroadCast;
    vector<set<int>> ToDeliver;
    URBroadcast(){};
    URBroadcast(vector<struct sockaddr_in> & host_dicts,long unsigned  myid,long unsigned numof_messa,long unsigned int window_send){
        // Notice: host_dicts starts from zero;
        this->host_dicts=host_dicts;
        this->myid=myid;
        this->numof_messa=numof_messa;
        this->my_window_send=this->my_window_step=window_send;
        this->my_count_receive=0;
        this->host_num=host_dicts.size();

        // Step1: Initial PerfectLink;
        struct sockaddr_in myaddr=host_dicts[static_cast<int>(myid-1)];
        socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (socket_fd < 0){
            perror("Socket Receiver Creation Fail.");
        }
        if (bind(socket_fd, reinterpret_cast<const struct sockaddr *>(&myaddr), sizeof(myaddr)) < 0)
        {
            perror("Cannot Bind.");
        }
        pl_link=PerfectLink(myid,myaddr,1,this->socket_fd,host_num);

        // Step2: Initial Message_to_Broadcast;
        ToBroadCast= MessaToBroadcast(host_num,numof_messa, window_send,myid);

        // Step3: Initial ToDeliver;
        for(long unsigned int i=0;i<host_num;i++){
            ToDeliver.push_back(set<int>());
        }


    }



    void broadcast()
    {
        //Traverse the source of the message

        cout<<"Start BroadCasting!"<<endl;
        ToBroadCast.messa_to_broadcast[myid-1].print();
        while(1){
            for(long unsigned int i=0;i<this->host_num;i++){
                if(this->myid==(i+1)){
                    ToBroadCast.messa_to_broadcast[i].sendrange(this->pl_link,this->host_dicts);
                    //cout<<"For source id "<<i+1<<endl;
                }else{
                    ToBroadCast.messa_to_broadcast[i].sendrange(this->pl_link,this->host_dicts);
                    
                }
                
            }
        }
    }

    Message deliver(){
        Message m=pl_link.deliver();
        long unsigned int current_num;
        if(m.seq_n!=0){
            
            //Step 1:

            BlockingQueue & bq=ToBroadCast.messa_to_broadcast[m.original_id-1];
            
            // Ensure only deliver once and insert just above host_num/2;
            auto ret =bq.push(m,current_num).second;

            //cout<<"PreURBDeliver"<<m.original_id<<" "<<m.seq_n<<endl;
            if(current_num+1>host_num/2 && current_num+1<=host_num/2+1){
                if(m.original_id==myid){
                    //cout<<"chao!"<<endl;
                    // Enlarge the window send of broadcast list;
                    my_count_receive++;
                    if(my_window_send<numof_messa && my_count_receive>=my_window_send/2 ){
                        long unsigned int start=my_window_send+1;
                        this->my_window_send+=this->my_window_step;
                        this->my_window_send=this->my_window_send>numof_messa? numof_messa:my_window_send;
                        long unsigned int end=my_window_send;
                        bq.mypush(start,end);
                        
                    }

                }
                
                return m;
            }

        }
        //cout<<"chao"<<endl;
        return Message(0,0,0);
    }
    
};



