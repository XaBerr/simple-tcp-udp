#include <Simple/ClientTCP.hpp>

#include <unistd.h>
#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <winsock2.h>
#include <windows.h>
#include <Ws2tcpip.h>
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

ClientTCP::ClientTCP(std::string _ipServerNumber, int _portNumber, int _bufferSize)
    : ipServerNumber{_ipServerNumber},
      portNumber{_portNumber},
      bufferSize{_bufferSize},
      exit{false},
      socketFileDescriptor{-1} {
  socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
  threadReceiver       = std::unique_ptr<std::thread>(new std::thread(&ClientTCP::run, this));
}

ClientTCP::~ClientTCP() {
  std::cout << "- ~ClientTCP\n";
  quit();
  if (threadReceiver && threadReceiver->joinable()) {
    threadReceiver->join();
  }
  if (threadTransmitter && threadTransmitter->joinable()) {
    threadTransmitter->join();
  }
  std::cout << "Client closed." << std::endl;
}

void ClientTCP::quit() {
  std::cout << "- quit\n";
  exit = true;
  if (socketFileDescriptor >= 0) {
    close(socketFileDescriptor);
    socketFileDescriptor = -1;
  }
}

void ClientTCP::run() {
  std::cout << "- run\n";
  char buffer[bufferSize];
  if (connect()) {
    if (threadTransmitter == nullptr) {
      threadTransmitter = std::unique_ptr<std::thread>(
          new std::thread(&ClientTCP::txChat, this));
      std::cout << "- txChat\n";
    }
    while (!exit) {
      if (recv(buffer) <= 0) {
        std::cout << "ERROR: message size is less than zero." << std::endl;
        close(socketFileDescriptor);
        socketFileDescriptor = -1;
        break;
      }
      std::cout << buffer;
      memset(buffer, 0, bufferSize);
    }
  }
}

void ClientTCP::txChat() {
  std::cout << "- txChat\n";
  while (!exit) {
    std::cout << "(Client) Insert the mex to be sent: ";
    std::string consoleMessage;
    std::getline(std::cin, consoleMessage);
    if (exit) {
      break;
    }
    if (send(consoleMessage.c_str(), consoleMessage.length()) >= 0) {
      send("\n", 1);
    }
  }
}

bool ClientTCP::connect() {
  std::cout << "- connect\n";
  int option(1);
  setsockopt(socketFileDescriptor, SOL_SOCKET, SO_REUSEADDR, (char*)&option, sizeof(option));

  struct sockaddr_in dest_addr = {0};
  dest_addr.sin_family         = AF_INET;
  dest_addr.sin_port           = htons(portNumber);

  if (inet_pton(AF_INET, ipServerNumber.c_str(), &dest_addr.sin_addr) <= 0) {
    std::cout << "ERROR: converting IP to internet address." << std::endl;
    close(socketFileDescriptor);
    return false;
  }

  if (::connect(socketFileDescriptor, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) < 0) {
    std::cout << "ERROR: cannot connect." << std::endl;
    close(socketFileDescriptor);
    return false;
  }
  return true;
}

int ClientTCP::recv(char* _buffer) {
  int rcv_size = ::recv(socketFileDescriptor, _buffer, bufferSize, 0);
  if (rcv_size < 0) {
    std::cout << "ERROR: receive." << std::endl;
    if (socketFileDescriptor >= 0) {
      close(socketFileDescriptor);
      socketFileDescriptor = -1;
    }
  }
  return rcv_size;
}

int ClientTCP::send(const char* _buffer, size_t _messageSize) {
  if (socketFileDescriptor < 0) {
    return -1;
  }
  int sent_size = ::send(socketFileDescriptor, _buffer, _messageSize, 0);
  if (sent_size < 0) {
    std::cout << "ERROR: send." << std::endl;
    if (socketFileDescriptor >= 0) {
      close(socketFileDescriptor);
      socketFileDescriptor = -1;
    }
    return -4;
  }
  return sent_size;
}
