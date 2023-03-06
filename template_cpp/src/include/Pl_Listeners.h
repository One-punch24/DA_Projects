// #pragma once

#include"PerfectLink.hpp"


void SendListener(PerfectLink& pl_link,vector<Message> & messa_queue);
void DeliverListener(PerfectLink& pl_link,vector<Message> & messa_delivered);