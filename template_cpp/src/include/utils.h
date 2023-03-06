# pragma once
#include<iostream>
#include<cstring>
#include<sys/socket.h>
#include<arpa/inet.h>
#include "message.hpp"
#include<vector>
#include<map>
#include<mutex>
#include "utils.h"
using namespace std;
extern vector<Message> messa_todeliver;
extern vector<map<long unsigned,Message>> urb_delivered;
extern unsigned long int window_send_main;
extern mutex bq_mutex;
extern mutex bq_mutex_array[1000];
class BlockingQueue{
private:
    map<int long,set<Message>> messa_to_onehost;
    //mutable mutex bq_mutex2;
    

public:
    
    //Set a lock here haha!
    
    // Seq_n---> Sets;
    
    long unsigned int myid;
    long unsigned int sourceid;
    
    BlockingQueue(long unsigned int myid,long unsigned int sourceid){
        this->myid=myid;
        this->sourceid=sourceid;
        this->messa_to_onehost=map<int long,set<Message>>();
    }

    pair<set<Message>::iterator,bool> push(Message m,long unsigned int & current_num){
        // Lock
        cout<<"Lock!"<<endl;
        bq_mutex_array[sourceid-1].lock();
        auto it=messa_to_onehost.find(m.seq_n);
        if(it==messa_to_onehost.end()){
            messa_to_onehost.insert({m.seq_n,set<Message>()});
            auto insert_it=messa_to_onehost[m.seq_n].insert(m);
            current_num=messa_to_onehost[m.seq_n].size();
            bq_mutex_array[sourceid-1].unlock();
            return insert_it;
            
        }
        else{
            auto insert_it=messa_to_onehost[m.seq_n].insert(m);
            current_num=messa_to_onehost[m.seq_n].size();
            bq_mutex_array[sourceid-1].unlock();
            return insert_it;
        }
        
        
    }

    void mypush(long unsigned int start,long unsigned end){
        // Lock
        //bq_mutex.lock();
        bq_mutex_array[sourceid-1].lock();
        for(long unsigned i=start;i<=end;i++){
            messa_to_onehost.insert({i,set<Message>()});
            window_send_main=end;
            //WARN: we delete this!
            //messa_to_onehost[i].insert(Message(myid,myid,i));
        }
        bq_mutex_array[sourceid-1].unlock();
        
    }

    void sendrange(PerfectLink & pl_link,vector<struct sockaddr_in> & host_dicts){
        // TODO: Lock;
        bq_mutex_array[sourceid-1].lock();
        for(long unsigned i=0;i<host_dicts.size();i++){
            // Not send to myself;
            if(i+1!=myid){
                
                for(auto iter=messa_to_onehost.begin();
                // condtion: && j<window_size
                iter!=messa_to_onehost.end() ;iter++){
                    pl_link.send(Message(sourceid,myid,iter->first),host_dicts[i]);
                    //cout<<iter->first<<endl;        
                }
            }
        }
        bq_mutex_array[sourceid-1].unlock();
    }
        /* WARN: Order Matters.*/
    void print(){
        cout<<"Check Blocking Queue myid "<<myid<<" sourceid"<<sourceid<<endl;
        for(auto iter =messa_to_onehost.begin();iter!=messa_to_onehost.end();iter++){
            cout<<"Each seq_n "<<iter->first<<" SIZE "<<iter->second.size()<<endl;
        }
    }
};

class MessaToBroadcast{
public:
    long unsigned int host_num;
    long unsigned int messa_num;
    
    /*
    1. We maintain a vector of BlockingQueue.
    2. bq_i is for message from source i. 
    3. We set a slide window, only to send to 
    */
    //vector<int > acks_start;
    vector<BlockingQueue> messa_to_broadcast;
    MessaToBroadcast(){};
    MessaToBroadcast(long unsigned int host_num,long unsigned int messa_num,long unsigned int window_send,long unsigned int myid){
        this->host_num=host_num;
        this->messa_num=messa_num;
        for(long unsigned int i=0;i<host_num;i++){
            if(myid==i+1){
                long unsigned int current_num;
                this->messa_to_broadcast.push_back(BlockingQueue(myid,i+1));
                long unsigned int end=window_send<messa_num?window_send:messa_num;
                messa_to_broadcast[i].mypush(1,end);

            }
            else{
                this->messa_to_broadcast.push_back(BlockingQueue(myid,i+1));
            }
            
            
        }

    }

};


struct sockaddr_in ConstructNetAddr(in_addr_t dest_ip, unsigned short dest_port);
// void OutVectoFile(const char* fn,vector<Message>& br_queue, vector<Message>& dl_queue);
void OutVectoFile( const char* fn,vector<Message> &dl_queue);
vector<Message> BroadcastMessages(map<long unsigned int,struct sockaddr_in> host_dicts, unsigned long myid, int num,unsigned long target_id);
