#ifndef SIMPLE_SERVERTCP_H
#define SIMPLE_SERVERTCP_H

#include <memory>
#include <string>
#include <thread>

namespace Simple {
class ServerTCP {
  int portNumber;
  std::string ipNumber;
  const int bufferSize;
  bool exit;
  int listenSocket;
  int socketFileDescriptor;
  std::unique_ptr<std::thread> threadReceiver;
  std::unique_ptr<std::thread> threadTransmitter;

  bool optionsAndBind();
  void quit();
  void run();
  void txChat();

 public:
  ServerTCP(std::string _ipNumber = "127.0.0.1", int _portNumber = 8080, int _bufferSize = 1024);
  ~ServerTCP();
  bool accept();
  int receive(char* _buffer);
  int send(const char* _buffer, size_t _bufferSize);
};
}  // namespace Simple

#endif