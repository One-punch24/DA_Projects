# pragma once
#include "utils.h"
#include "URBroadcast.hpp"

class FifoBroadcast{
public:
    long unsigned int myid;
    long unsigned int numof_messa;
    long unsigned int host_num;
    long unsigned int window_send;

    vector<struct sockaddr_in> host_dicts;
    vector<long unsigned int> nexts;
    URBroadcast urb;

    // urbtodeliver
    // fifobtodeliver

    FifoBroadcast(vector<struct sockaddr_in>& host_dicts,long unsigned myid, long unsigned numof_messa,
    long unsigned window_send){
        this->host_dicts=host_dicts;
        this->host_num=host_dicts.size();
        urb=URBroadcast(host_dicts,myid, numof_messa,
        window_send);
        for(long unsigned int i=0;i<host_num;i++){
            nexts.push_back(0);
        }

    }

    void broadcast(){
        urb.broadcast();
    }

    void deliver(){
        Message m;
        // Init urb_delivered;
        for(long unsigned int i=0;i<host_num;i++){
            
            urb_delivered.push_back(map<long unsigned, Message>());
        }
        while(1){
            m=urb.deliver();
            if(m.seq_n!=0){
                //cout<<"Deliver "<<m.original_id<<" "<<m.seq_n<<endl;
                /*******************************************
                 *  case1 : Test URB;
                *******************************************/
                // messa_todeliver.push_back(m);

                /*******************************************
                 *  case2 : Test FIFOURB;
                *******************************************/
                if(m.seq_n==nexts[m.original_id-1]+1){
                    // WARN: Maybe mistake, twice deliver?
                    messa_todeliver.push_back(m);
                    nexts[m.original_id-1]++;
                    /*******************************************
                     *  case2 Implimentation 1: Test FIFOURB;
                    *******************************************/
                    while(urb_delivered[m.original_id-1].begin()!= urb_delivered[m.original_id-1].end() 
                    && urb_delivered[m.original_id-1].begin()->second.seq_n==nexts[m.original_id-1]+1){
                        messa_todeliver.push_back(urb_delivered[m.original_id-1].begin()->second);
                        //cout<<"erase"<<endl;
                        urb_delivered[m.original_id-1].erase(urb_delivered[m.original_id-1].begin());
                        // WARN: Important! Pay attention to whether it is 0 or 1.
                            
                            nexts[m.original_id-1]++;
                    }
                    //TODO: Adjust window side here.
  
                }
                else{
                    urb_delivered[m.original_id-1].insert({m.seq_n,m});
                }
            }
            
        }
    }

};
