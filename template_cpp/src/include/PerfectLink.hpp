#pragma once
#include <iostream>
#include <iostream>
#include <sys/socket.h>
#include <set>
#include "message.hpp"
#include "parse.hpp"
#include "FairlossLink.hpp"
#include "StubbornLink.hpp"
#include <queue>
#include <vector>
#include <thread>

using namespace std;

class PerfectLink
{
public:
    StubbornLink st_link;
    vector<set<Message>> delivered;
    vector<Message> messa_send_queue;
    int socket_fd;

    PerfectLink(){};
    PerfectLink(unsigned long host_id, struct sockaddr_in host_addr, int max_send_times,int socket_fd,unsigned long int host_num)
    {
        this->st_link = StubbornLink(host_id, host_addr, max_send_times,socket_fd);
        for(long unsigned i=0;i<host_num;i++){
            delivered.push_back(set<Message>());
        }
    };

    void send(Message pl_m, sockaddr_in dest_addr)
    {
        this->st_link.send(pl_m, dest_addr);
    }

    Message deliver(long unsigned current_iter)
    {
        pair<set<Message>::iterator, bool> ret;
        Message pl_m = this->st_link.deliver();
        
        if(pl_m.iter>current_iter){
            return Message(); 
        }
        ret = delivered[pl_m.original_id-1].insert(pl_m);
        if (ret.second == true)
        {
            return pl_m;
        }
        else
        {
            return Message(); // For the upper layer, if you see a message with original_id=0, that is a signal one (not effective);
        }
    }
};