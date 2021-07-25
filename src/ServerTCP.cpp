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
    std::cout << "ERROR: OPEN SOCKET " << listenSocket << std::endl;
    close(listenSocket);
    return;
  }
  if (!optionsAndBind()) {
    std::cout << "ERROR: BIND" << std::endl;
    close(listenSocket);
    return;
  }
  threadReceiver = std::unique_ptr<std::thread>(new std::thread(&ServerTCP::run, this));
}

ServerTCP::~ServerTCP() {
  quit();
  if (threadReceiver && threadReceiver->joinable())
    threadReceiver->join();

  if (threadTransmitter && threadTransmitter->joinable())
    threadTransmitter->join();

  std::cout << "joint" << std::endl;
}

void ServerTCP::quit() {
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
  char buffer[bufferSize];
  while (!exit) {
    if (accept()) {
      if (threadTransmitter == nullptr) {
        threadTransmitter = std::unique_ptr<std::thread>(new std::thread(&ServerTCP::txChat, this));
      }
      while (!exit) {
        if (receive(buffer) <= 0) {
          std::cout << "Read size less than zero." << std::endl;
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
  while (!exit) {
    std::string to_tx;
    std::getline(std::cin, to_tx);
    if (exit) {
      break;
    }
    if (send(to_tx.c_str(), to_tx.length()) >= 0) {
      send("\n", 1);
    }
  }
}

bool ServerTCP::optionsAndBind() {
  int option(1);
  setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&option, sizeof(option));
  struct sockaddr_in my_addr = {0};
  my_addr.sin_family         = AF_INET;

  my_addr.sin_port = htons(portNumber);

  my_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(listenSocket, (struct sockaddr*)&my_addr, sizeof(my_addr)) < 0) {
    std::cout << "ERROR: BIND SOCKET" << std::endl;
    if (listenSocket >= 0) {
      close(listenSocket);
    }
    return false;
  }

  if (listen(listenSocket, 1) < 0) {
    std::cout << "ERROR: LISTEN SOCKET" << std::endl;

    return false;
  }
  return true;
}

bool ServerTCP::accept() {
  struct sockaddr_in client_addr;
  socklen_t addr_l     = sizeof(client_addr);
  socketFileDescriptor = ::accept(listenSocket, (struct sockaddr*)&client_addr, &addr_l);
  if (socketFileDescriptor < 0) {
    std::cout << "ERROR: ACCEPT CONNECTION" << std::endl;
    if (listenSocket >= 0) {
      close(listenSocket);
      listenSocket = -1;
    }
    return false;
  }

  std::cout << "New connection from "
            << inet_ntoa(client_addr.sin_addr) << std::endl;
  ipNumber = inet_ntoa(client_addr.sin_addr);
  return true;
}

int ServerTCP::receive(char* _buffer) {
  int rcv_size = ::recv(socketFileDescriptor, _buffer, bufferSize, 0);
  if (rcv_size < 0) {
    std::cout << "ERROR: RECV" << std::endl;
    if (socketFileDescriptor >= 0) {
      close(socketFileDescriptor);
      socketFileDescriptor = -1;
    }
  }
  return rcv_size;
}

int ServerTCP::send(const char* _buffer, size_t _messageSize) {
  if (socketFileDescriptor < 0) {
    return -1;
  }
  int sent_size = ::send(socketFileDescriptor, _buffer, _messageSize, 0);
  if (sent_size < 0) {
    std::cout << "ERROR: SEND" << std::endl;
    if (socketFileDescriptor >= 0) {
      close(socketFileDescriptor);
      socketFileDescriptor = -1;
    }
    return -4;
  }
  return sent_size;
}
