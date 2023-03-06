# pragma once
#include<iostream>
#include<cstring>
#include<sys/socket.h>
#include<arpa/inet.h>
#include "message.hpp"
#include<vector>
#include<map>
using namespace std;
extern vector<Message> messa_todeliver;
struct sockaddr_in ConstructNetAddr(in_addr_t dest_ip, unsigned short dest_port);
void OutputFile(const char* fn,vector<Message>& br_queue, vector<Message>& dl_queue);
vector<Message> BroadcastMessages(map<long unsigned int,struct sockaddr_in> host_dicts, unsigned long myid, int num,unsigned long target_id);
