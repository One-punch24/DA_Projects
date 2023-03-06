#include <chrono>
#include <iostream>
#include <thread>
#include <signal.h>
#include <sys/socket.h>
#include <thread>
#include <map>
#include<string>
// #include <windows.h>
#include "parse.hpp"
#include "PerfectLink.hpp"
#include "hello.h"
#include "utils.h"
#include "URBroadcast.hpp"
#include "FifoBroadcast.hpp"
const char *fn;
vector<Message> messa_send;
vector<Message> messa_todeliver;
vector<map<long unsigned int,Message>> urb_delivered;
int socket_fd;
unsigned long int window_send_main;
std::mutex bq_mutex;
std::mutex bq_mutex_array[1000];

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
  // for(long unsigned int i=0;i<messa_todeliver.size();i++){
  //   cout<<"d "<< messa_todeliver[i].original_id<<" "<<messa_todeliver[i].seq_n<<endl;
  // }
  OutVectoFile(fn, messa_todeliver);
  cout<<messa_todeliver.size()<<endl;
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
  struct sockaddr_in myaddr = host_dicts[static_cast<int>(myid)];

  std::cout << "\n";

  std::cout << "Path to output:\n";
  std::cout << "===============\n";
  std::cout << parser.outputPath() << "\n\n";

  std::cout << "Path to config:\n";
  std::cout << "===============\n";
  std::cout << parser.configPath() << "\n\n";

  std::cout << "Doing some initialization...\n\n";

  // Initialize Socket for current Process.


  unsigned long target_id;
  unsigned long int num_messages;
  std::ifstream ConfigFile(parser.configPath());
  std::ifstream ConfigFile_2;
  // ConfigFile_2=ConfigFile;

  if (!ConfigFile.is_open())
  {
    std::ostringstream os;
    os << "`" << parser.configPath() << "` does not exist.";
    throw std::invalid_argument(os.str());
  }
  else
  {
    ConfigFile >> num_messages >> target_id;
  }
  cout << "Messages Num to Send: " << num_messages << endl;

  FifoBroadcast fb(host_dicts,myid,num_messages,15);

  // std::cout << "Broadcasting and delivering messages...\n\n";

  // Fifo
  if(argc<9){
    cout<<"MYDebug Final"<<endl;
      thread send_t = thread(& FifoBroadcast::broadcast,&fb);
      send_t.detach();
      thread deliver_t = thread(& FifoBroadcast::deliver,&fb);
      deliver_t.detach();
  }else if(string(argv[9])==string("send")){
      cout<<"MYDebug "<<argv[9]<<endl;
      thread send_t = thread(& FifoBroadcast::broadcast,&fb);
      send_t.detach();
  }else if(string(argv[9])==string("deliver")){
      cout<<"MYDebug "<<argv[9]<<endl;
      thread deliver_t = thread(& FifoBroadcast::deliver,&fb);
      deliver_t.detach();
  }else{
      cout<<"MYDebug "<<argv[9]<<endl;
      thread send_t = thread(& FifoBroadcast::broadcast,&fb);
      send_t.detach();
      thread deliver_t = thread(& FifoBroadcast::deliver,&fb);
      deliver_t.detach();
  }


    


  // After a process finishes broadcasting,
  // it waits forever for the delivery of messages.
  cout << "FinalSize" << messa_todeliver.size() << endl;
  while (true)
  {
    std::this_thread::sleep_for(std::chrono::hours(1));
  }

  return 0;
}
