#ifndef SIMPLE_CLIENTTCP_H
#define SIMPLE_CLIENTTCP_H

#include <memory>
#include <string>
#include <thread>

namespace Simple {
class ClientTCP {
  int portNumber;
  std::string ipServerNumber;
  const int bufferSize;
  bool exit;
  int socketFileDescriptor;
  std::unique_ptr<std::thread> threadReceiver;
  std::unique_ptr<std::thread> threadTransmitter;

  void quit();
  void run();
  void txChat();

 public:
  ClientTCP(std::string _ipServerNumber = "127.0.0.1", int _portNumber = 8080, int _bufferSize = 1024);
  ~ClientTCP();
  bool connect();
  int recv(char* _buffer);
  int send(const char* _buffer, size_t _messageSize);
};
}  // namespace Simple

#endif