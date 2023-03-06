#include"Pl_Listeners.h"
#include<iostream>
using namespace std;

void SendListener(PerfectLink& pl_link,vector<Message> & messa_send){
    cout<<"SendListener Start!"<<endl;
    while(true)
    {
        while(!messa_send.empty()){
            // cout<<"Send !"<<endl;
            for(long unsigned int  i=0;i<messa_send.size();i++){
                
                pl_link.send(messa_send[i],messa_send[i].dest_addr);
                //cout<<messa_send[i].original_id<<" "<<messa_send[i].seq_n<<endl;
            }
        }
    }
}


void DeliverListener(PerfectLink& pl_link,vector<Message> & messa_delivered){
    cout<<"DeliverListener Start!"<<endl;
    while(true){
        
        Message m= pl_link.deliver();
        //cout<<"Sounds bad!"<<endl;
        //cout<<"d "<<m.original_id<<" "<<m.seq_n<<" "<<m.forwarder_id<<endl;
        if(m.original_id==0){
            //  cout<<"d "<<m.original_id<<" "<<m.seq_n<<" "<<m.forwarder_id<<endl;
            // messa_delivered.push_back(m);
        }
        else{
            cout<<"d "<<m.original_id<<" "<<m.seq_n<<" "<<m.forwarder_id<<endl;
            messa_delivered.push_back(m);
        }
        
    }
}