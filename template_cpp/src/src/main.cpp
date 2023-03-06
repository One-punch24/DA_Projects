#include <chrono>
#include <iostream>
#include <thread>
#include <signal.h>
#include <sys/socket.h>
#include <thread>
#include <map>

#include "parse.hpp"
#include "PerfectLink.hpp"
#include "hello.h"
#include "Pl_Listeners.h"
#include "utils.h"
const char *fn;
vector<Message> messa_send;
vector<Message> messa_todeliver;
int socket_fd;



static void stop(int)
{
  // reset signal handlers to default
  signal(SIGTERM, SIG_DFL);
  signal(SIGINT, SIG_DFL);

  // immediately stop network packet processing
  std::cout << "Immediately stopping network packet processing.\n";
  //cout << messa_todeliver.size() << "nmd" << endl;
  OutputFile(fn, messa_send,messa_todeliver);

  // write/flush output file if necessary
  std::cout << "Writing output.\n";

  // exit directly from signal handler
  exit(0);
}

int main(int argc, char **argv)
{
  signal(SIGTERM, stop);
  signal(SIGINT, stop);

  // `true` means that a config file is required.
  // Call with `false` if no config file is necessary.
  bool requireConfig = true;

  Parser parser(argc, argv);
  parser.parse();

  hello();
  std::cout << std::endl;

  std::cout << "My PID: " << getpid() << "\n";
  std::cout << "From a new terminal type `kill -SIGINT " << getpid() << "` or `kill -SIGTERM "
            << getpid() << "` to stop processing packets\n\n";

  std::cout << "My ID: " << parser.id() << "\n\n";

  std::cout << "List of resolved hosts is:\n";
  std::cout << "==========================\n";
  auto hosts = parser.hosts();
  map<long unsigned, struct sockaddr_in> host_dicts;

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
    host_dicts.insert({host.id, host_addr});
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
  socket_fd=socket(AF_INET,SOCK_DGRAM,0);
  if(socket_fd<0){
            perror("Socket Receiver Creation Fail.");
  }
  if(bind(socket_fd,reinterpret_cast<const struct sockaddr *>(& myaddr),sizeof(myaddr))<0){
    perror("Cannot Bind.");
    }
  PerfectLink pl_link = PerfectLink(myid, myaddr, 1,socket_fd);

  unsigned long target_id;
  int num_messages;
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

  messa_send = BroadcastMessages(host_dicts, myid, num_messages, target_id);
  fn = parser.outputPath();
  //OutputFile(parser.outputPath(), messa_send, true);

  std::cout << "Broadcasting and delivering messages...\n\n";

  thread send_t = thread(SendListener, std::ref(pl_link), std::ref(messa_send));
  thread deliver_t = thread(DeliverListener, std::ref(pl_link), std::ref(messa_todeliver));
  send_t.detach();
  deliver_t.detach();

  // After a process finishes broadcasting,
  // it waits forever for the delivery of messages.
  cout << "FinalSize" << messa_todeliver.size() << endl;
  while (true)
  {
    std::this_thread::sleep_for(std::chrono::hours(1));
  }

  return 0;
}
