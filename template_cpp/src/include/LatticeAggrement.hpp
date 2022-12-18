#pragma once
#include<iostream>
#include<cstring>
#include<thread>
#include <map>
#include<queue>
#include<set>
#include"message.hpp"
#include"PerfectLink.hpp"
#include"utils.h"

class LatteAggrement{
public:
    long unsigned current_iter;
    long unsigned ack_count;
    long unsigned nack_count;
    long unsigned active_proposal_num;

    long unsigned myid;
    long unsigned max_iters;
    long unsigned host_num;
    vector<struct sockaddr_in> host_dicts;
    int socket_fd;
    PerfectLink pl_link;
    vector<Message> Tobroadcast; // first index stands for iter;
    vector<vector<Message>> Tosend; // first index stands for to-Pi;
    set<long unsigned> proposed_value;
    vector<set<long unsigned>> accepted_value_vec;



    LatteAggrement(vector<struct sockaddr_in> & host_dicts,
    long unsigned  myid,int socket_fd,long unsigned max_its){
        cout<<"LA Init Start"<<endl;
        this->current_iter=1;
        this->ack_count=0;
        this->nack_count=0;
        this->active_proposal_num=0;
        this->myid=myid;
        this->socket_fd=socket_fd;
        this->max_iters=max_its;
        this->host_dicts=host_dicts;
        struct sockaddr_in myaddr=host_dicts[(myid-1)];
        
        this->host_num=host_dicts.size();
        cout<<"There are hosts"<<this->host_num<<endl;
        this->pl_link=PerfectLink(myid,myaddr,1,this->socket_fd,this->host_num);  
        
        // Initialize the proposed value 
        for(auto it=vec_proposals[current_iter-1].begin();
        it!=vec_proposals[current_iter-1].end();it++){
            proposed_value.insert(*it);
        }

        // Trigger the beb of the first proposed_value;
        Tosend=vector<vector<Message>>(128);
        this->accepted_value_vec.push_back(set<long unsigned>());
        // Add the forst Proposal
        Tobroadcast.push_back(Message(0,\
        current_iter,myid,active_proposal_num,proposed_value));

        cout<<"LA Init End"<<endl;

    }
    void broadcast_send_mixed(){

        //WARN: Later we might need to modify this while loop;
        while(1){
            // BroadCast
            // cout<<"Broadcast"<<endl;
            unsigned int k=0;
            mut_broadcast.lock();
            for(long unsigned i=0;i<Tobroadcast.size();i++){ 
                    sendrange(Tobroadcast[i]);

            }
            mut_broadcast.unlock();
            // Send
            mut_send.lock();
            for(long unsigned i=0;i<Tosend.size();i++){
                for(long unsigned j=0;j<Tosend[i].size();j++){
                    this->pl_link.send(Tosend[i][j],host_dicts[i]);

                    
                }
                
            }
            mut_send.unlock();
        }

    }

    void sendrange(Message & m){
        //cout<<m.m_type<< " "<<m.value_set[0]<<endl;
        for(long unsigned i=0;i<this->host_num;i++){
            this->pl_link.send(m,host_dicts[i]);

        }

    }

    void upon_change_ack_nack(Message & m){
        if(nack_count>0 && (ack_count+nack_count>=host_num/2+host_num%2) &&
         this->current_iter==m.iter){
            active_proposal_num++;
            ack_count=0;
            nack_count=0;

            // Start the beb next round;
            mut_broadcast.lock();
            Tobroadcast.push_back(Message(0,
            current_iter,myid,active_proposal_num,proposed_value));// WARN: Modify Here
            mut_broadcast.unlock();
        }
        else if(ack_count>=(host_num/2+host_num%2) && this->current_iter==m.iter) {
            //decide
            Todecide.push_back(proposed_value);
            cout<<"Decide"<<endl;
            // this->current_iter++; // Enter the next proposal stage(iter);
            /****************************************
             * From last version code
             * 
            */
            this->ack_count=0;
            this->nack_count=0;
            this->active_proposal_num=0;
            this->accepted_value_vec.push_back(set<long unsigned>());
            this->proposed_value.clear();
            this->current_iter++;

            // Start next round
            if(current_iter<=max_iters){
                mut_broadcast.lock();
                
                for(auto it=vec_proposals[current_iter-1].begin();
                    it!=vec_proposals[current_iter-1].end();it++){
                    proposed_value.insert(*it);
                }
                this->Tobroadcast.push_back(Message(0,
                this->current_iter,myid,active_proposal_num,proposed_value));
                mut_broadcast.unlock();
            }
        }

    }
    // Warn: Need to be changed.
    bool mergeset(set<long unsigned> & acc_value,Message & pro_m){
        
        vector<long unsigned> acc_vals_temp; // Store True acc_value;
        for(auto it = acc_value.begin();it!=acc_value.end();it++){
            acc_vals_temp.push_back(*it);
        }
        acc_value.clear();

    
        // Convert to set at first----Now acc_value stores proposed_value
        for(long unsigned it=0;it<pro_m.value_set_num;it++)
        {
            acc_value.insert(pro_m.value_set[it]);
        }
        long unsigned pro_num_previous=acc_value.size();

        // Merge
        for(long unsigned it=0;it<acc_vals_temp.size();it++)
        {
            acc_value.insert(acc_vals_temp[it]);
        }

        // Evaluate the relation;
        

        if(pro_num_previous==acc_value.size()){
            return true; // ack
        }
        return false;

    }

    void deliver(){
        Message m;
        while(1){
            // Pay Attention: I would force perfect link only
            // deliver message belonging to the same 'iter' 
            // and same 'propose_num'
            m=this->pl_link.deliver(current_iter);

            if(m.original_id!=0){
                // ACK
                
                if((m.m_type)==1&&m.proposal_num==active_proposal_num &&m.iter==current_iter){
                    ack_count++;
                    cout<<"Receive ACK Oid:"<<m.original_id<<" Iter:"<<m.iter<<" Type:"<<m.m_type<<" Value:";
                    cout<<" activeround:"<<m.proposal_num<<"myround:"<<active_proposal_num <<endl;
                    upon_change_ack_nack(m);
                    // cout<<"hello man"<<endl;
                }

                // NACK
                if(m.m_type==2&&m.proposal_num==active_proposal_num&&m.iter==current_iter){
                    // Add the modification above m.iter==current_iter
                    nack_count++;
                    for(long unsigned k=0;k<m.value_set_num;k++){
                        proposed_value.insert(m.value_set[k]);
                    }   
                    cout<<"Receive NACK Oid:"<<m.original_id<<" Iter:"<<m.iter<<" Type:"<<m.m_type<<" Value:";
                    for(long unsigned i=0;i<m.value_set_num;i++){
                        cout<<m.value_set[i]<<" ";
                    }
                    cout<<" activeround:"<<m.proposal_num<<"myround:"<<active_proposal_num <<endl;
                    upon_change_ack_nack(m);
                    
                }
                // PROPOSAL
                // if(m.iter>=accepted_value_vec.size()){
                //     accepted_value_vec.push
                // }
                bool tag=mergeset(accepted_value_vec[m.iter-1],m);
                if(m.m_type==0 && tag){
                    // ACK
                    mut_send.lock();
                    Tosend[m.original_id-1].push_back(Message(1,
                    m.iter,myid,m.proposal_num));
                    mut_send.unlock();

                    cout<<"Send Ack Oid:"<<myid<<" To "<<m.original_id<<" Iter:"<<m.iter<<" Type:"<<m.m_type;
                    cout<<" activeround:"<<m.proposal_num<<"myround:"<<active_proposal_num <<endl;

                }
                else if(m.m_type==0 && !tag){
                    // NACK
                    mut_send.lock();
                    //m.iter not current iter
                    Tosend[m.original_id-1].push_back(Message(2,
                    m.iter,myid,m.proposal_num,accepted_value_vec[m.iter-1]));
                    mut_send.unlock();

                    cout<<"Send NACK Oid:"<<myid<<" To "<<m.original_id <<" Iter:"<<m.iter<<" Type:"<<m.m_type<<" Value:";
                        for(auto it= accepted_value_vec[m.iter-1].begin();it!=accepted_value_vec[m.iter-1].end();it++){
                            cout<<*it<<" ";
                        }
                        cout<<" activeround:"<<m.proposal_num<<"myround:"<<active_proposal_num <<endl;
                }

                
            }

            
        }

    }





};