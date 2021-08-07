#include <Simple/ServerTCP.hpp>

#include <unistd.h>
#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iphlpapi.h>
#include <stdio.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

using namespace Simple;

ServerTCP::ServerTCP(std::string _ipNumber, int _portNumber, int _bufferSize)
    : ipNumber{_ipNumber},
      portNumber{_portNumber},
      bufferSize{_bufferSize},
      exit{false},
      listenSocket{-1},
      socketFileDescriptor{-1} {
  listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0) {
    std::cout << "ERROR: cannot open the socket listener [" << listenSocket << "]." << std::endl;
    close(listenSocket);
    return;
  }
  if (!optionsAndBind()) {
    std::cout << "ERROR: in bind and options." << std::endl;
    close(listenSocket);
    return;
  }
  threadReceiver = std::unique_ptr<std::thread>(new std::thread(&ServerTCP::run, this));
}

ServerTCP::~ServerTCP() {
  std::cout << "- ~ServerTCP\n";
  quit();
  if (threadReceiver && threadReceiver->joinable())
    threadReceiver->join();

  if (threadTransmitter && threadTransmitter->joinable())
    threadTransmitter->join();

  std::cout << "Server closed." << std::endl;
}

void ServerTCP::quit() {
  std::cout << "- quit\n";
  exit = true;
  if (listenSocket >= 0) {
    close(listenSocket);
    listenSocket = -1;
  }
  if (socketFileDescriptor >= 0) {
    close(socketFileDescriptor);
    socketFileDescriptor = -1;
  }
}

void ServerTCP::run() {
  std::cout << "- run\n";
  char buffer[bufferSize];
  while (!exit) {
    if (accept()) {
      if (threadTransmitter == nullptr)
        threadTransmitter = std::unique_ptr<std::thread>(new std::thread(&ServerTCP::txChat, this));
      std::cout << "- txChat\n";

      while (!exit) {
        if (receive(buffer) <= 0) {
          std::cout << "ERROR: Message size is less than zero." << std::endl;
          close(socketFileDescriptor);
          socketFileDescriptor = -1;
          break;
        }
        std::cout << buffer;
        memset(buffer, 0, bufferSize);
      }
    }
  }
}

void ServerTCP::txChat() {
  std::cout << "- txChat\n";
  while (!exit) {
    std::cout << "(Server) Insert the mex to be sent: ";
    std::string consoleMessage;
    std::getline(std::cin, consoleMessage);
    if (exit)
      break;
    if (send(consoleMessage.c_str(), consoleMessage.length()) >= 0)
      send("\n", 1);
  }
}

bool ServerTCP::optionsAndBind() {
  std::cout << "- optionsAndBind\n";
  int option(1);
  setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&option, sizeof(option));
  struct sockaddr_in my_addr = {0};
  my_addr.sin_family         = AF_INET;
  my_addr.sin_port           = htons(portNumber);
  my_addr.sin_addr.s_addr    = htonl(INADDR_ANY);

  if (bind(listenSocket, (struct sockaddr*)&my_addr, sizeof(my_addr)) < 0) {
    std::cout << "ERROR: cannot bind the socket." << std::endl;
    if (listenSocket >= 0) {
      close(listenSocket);
    }
    return false;
  }

  if (listen(listenSocket, 1) < 0) {
    std::cout << "ERROR: cannot listen in that socket." << std::endl;
    return false;
  }
  return true;
}

bool ServerTCP::accept() {
  std::cout << "- accept\n";
  struct sockaddr_in client_addr;
  socklen_t addr_l     = sizeof(client_addr);
  socketFileDescriptor = ::accept(listenSocket, (struct sockaddr*)&client_addr, &addr_l);
  if (socketFileDescriptor < 0) {
    std::cout << "ERROR: cannot accept the connection." << std::endl;
    if (listenSocket >= 0) {
      close(listenSocket);
      listenSocket = -1;
    }
    return false;
  }
  std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << std::endl;
  ipNumber = inet_ntoa(client_addr.sin_addr);
  return true;
}

int ServerTCP::receive(char* _buffer) {
  int rcv_size = ::recv(socketFileDescriptor, _buffer, bufferSize, 0);
  if (rcv_size < 0) {
    std::cout << "ERROR: receive error." << std::endl;
    if (socketFileDescriptor >= 0) {
      close(socketFileDescriptor);
      socketFileDescriptor = -1;
    }
  }
  return rcv_size;
}

int ServerTCP::send(const char* _buffer, size_t _messageSize) {
  if (socketFileDescriptor < 0)
    return -1;
  int sent_size = ::send(socketFileDescriptor, _buffer, _messageSize, 0);
  if (sent_size < 0) {
    std::cout << "ERROR: send error." << std::endl;
    if (socketFileDescriptor >= 0) {
      close(socketFileDescriptor);
      socketFileDescriptor = -1;
    }
    return -4;
  }
  return sent_size;
}
