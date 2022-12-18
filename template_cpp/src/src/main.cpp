#include <chrono>
#include <iostream>
#include <thread>
#include <signal.h>
#include <sys/socket.h>
#include <thread>
#include <map>
#include<string>
#include<sstream>
#include <mutex>
#include "parse.hpp"
#include "hello.h"
#include "utils.h"

using  namespace std;
// #include <windows.h>

#include "PerfectLink.hpp"
#include "LatticeAggrement.hpp"
const char *fn;
int socket_fd;
unsigned long int window_send_main;
vector<set<long unsigned>> Todecide;
vector<vector<long unsigned>> vec_proposals;
mutex mut_send;
mutex mut_broadcast;

static void stop(int)
{
  // reset signal handlers to default
  signal(SIGTERM, SIG_DFL);
  signal(SIGINT, SIG_DFL);

  // immediately stop network packet processing
  std::cout << "Immediately stopping network packet processing.\n";
  // cout << messa_todeliver.size() << "nmd" << endl;
  //

  // write/flush output file if necessary
  std::cout << "Writing output.\n";
  OuttoFile(fn);

  // exit directly from signal handler
  exit(0);
}

int main(int argc, char **argv)
{
  signal(SIGTERM, stop);
  signal(SIGINT, stop);
  // MYDEBUG
  //cout<<argv[9]<<endl;
  // `true` means that a config file is required.
  // Call with `false` if no config file is necessary.
  bool requireConfig = true;

  Parser parser(argc, argv);
  parser.parse();

  // Outputfile
  fn = parser.outputPath();
  hello();
  std::cout << std::endl;

  std::cout << "My PID: " << getpid() << "\n";
  std::cout << "From a new terminal type `kill -SIGINT " << getpid() << "` or `kill -SIGTERM "
            << getpid() << "` to stop processing packets\n\n";

  std::cout << "My ID: " << parser.id() << "\n\n";

  std::cout << "List of resolved hosts is:\n";
  std::cout << "==========================\n";
  auto hosts = parser.hosts();
  vector<struct sockaddr_in> host_dicts;

  // Others First
  for (auto &host : hosts)
  {
    struct sockaddr_in host_addr;

    std::cout << host.id << "\n";
    std::cout << "Human-readable IP: " << host.ipReadable() << "\n";
    std::cout << "Machine-readable IP: " << host.ip << "\n";
    std::cout << "Human-readbale Port: " << host.portReadable() << "\n";
    std::cout << "Machine-readbale Port: " << host.port << "\n";
    std::cout << "\n";
    host_addr = ConstructNetAddr(host.ip, host.port);
    host_dicts.push_back(host_addr);
  }

  unsigned long int myid = parser.id();
  cout<<myid<<endl;
  struct sockaddr_in myaddr = host_dicts[static_cast<int>(myid-1)];

  std::cout << "\n";

  std::cout << "Path to output:\n";
  std::cout << "===============\n";
  std::cout << parser.outputPath() << "\n\n";

  std::cout << "Path to config:\n";
  std::cout << "===============\n";
  std::cout << parser.configPath() << "\n\n";

  std::cout << "Doing some initialization...\n\n";

  // Initialize Socket for current Process.


  unsigned long proposal_round;
  unsigned long vs;
  unsigned long distinct_num;
  unsigned long int num_messages;
  std::ifstream ConfigFile(parser.configPath());

  
  if (!ConfigFile.is_open())
  {
    std::ostringstream os;
    os << "`" << parser.configPath() << "` does not exist.";
    throw std::invalid_argument(os.str());
  }
  else
  {
    // Initial the Proposal
    string str; 
    long unsigned value;
    getline(ConfigFile,str);
    stringstream sstream(str);
    char ire;
    sstream >> proposal_round>> vs>>distinct_num;
    for(long unsigned i=0;i<proposal_round;i++){

      vector<long unsigned> vec_set_single_pro;
      getline(ConfigFile,str);
      stringstream sstream(str);
      while(sstream>>value){
        vec_set_single_pro.push_back(value);
        //cout<<value<<endl;
      }
      vec_proposals.push_back(vec_set_single_pro);
      //cout<<"hello"<<endl;
    }
  }
  cout<<vec_proposals.size()<<endl;
  // cout<<vec_proposals[1].size()<<endl;
  cout << vec_proposals[0][0] << endl;

  // Initialize Socket for current Process.
  socket_fd=socket(AF_INET,SOCK_DGRAM,0);
  if(socket_fd<0){
            perror("Socket Receiver Creation Fail.");
  }
  if(bind(socket_fd,reinterpret_cast<const struct sockaddr *>(& myaddr),sizeof(myaddr))<0){
    perror("Cannot Bind.");
    }
  

  // Lattice Aggrement Debug seperately for send and deliver;
  LatteAggrement LatteA(host_dicts,myid,socket_fd,proposal_round);
  std::cout << "Broadcasting and delivering messages...\n\n";
  cout<<argc<<endl;
  if(argc<9){
    cout<<"MYDebug Final"<<endl;
      thread send_t = thread(& LatteAggrement::broadcast_send_mixed,&LatteA);
      send_t.detach();
      thread deliver_t = thread(& LatteAggrement::deliver,&LatteA);
      deliver_t.detach();
  }else if(string(argv[8])==string("send")){
      cout<<"MYDebug "<<argv[8]<<endl;
      thread send_t = thread(& LatteAggrement::broadcast_send_mixed,&LatteA);
      send_t.detach();
  }else if(string(argv[8])==string("deliver")){
      cout<<"MYDebug "<<argv[8]<<endl;
      thread deliver_t = thread(& LatteAggrement::deliver,&LatteA);
      deliver_t.detach();
  }else{
      cout<<"MYDebug "<<argv[8]<<endl;
      thread send_t = thread(& LatteAggrement::broadcast_send_mixed,&LatteA);
      send_t.detach();
      thread deliver_t = thread(& LatteAggrement::deliver,&LatteA);
      deliver_t.detach();
  }


    


  // After a process finishes broadcasting,
  // it waits forever for the delivery of messages.
  while (true)
  {
    std::this_thread::sleep_for(std::chrono::hours(1));
  }

  return 0;
}
