#include <Simple.hpp>
#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

int main(int argc, char const *argv[]) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
  WSADATA wsaData;
  int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (err != 0) {
    printf("WSAStartup failed with error: %d\n", err);
    return 1;
  }
#endif
  Simple::ServerTCP serverTCP;
  std::cout << "Server OK!\n";
  std::this_thread::sleep_for(std::chrono::seconds(500));
}
