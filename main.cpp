
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <stdio.h>
#include <iostream>

#include "eventbase.hpp"
#include "tcpserver.hpp"

#pragma comment(lib, "Ws2_32.lib")

int main() {
  WSADATA wsaData;
  WSAStartup(MAKEWORD(2, 2), &wsaData);

  GlobalFnInit();

  EventBase base;
  TcpServer server(&base, INADDR_ANY, 44819);
  auto read_cb = [](const TcpConnection::SPtr& conn, std::string_view buffer) {
    std::cout << "recv buffer = " << buffer << std::endl;
    static int cnt = 1;

    std::string data = "this is china";
    WSABUF buf;
    buf.buf = data.data();
    buf.len = data.size();

    DWORD bytesTransferred = 0;
    std::cout << "start send" << std::endl;
    auto res = WSASend(conn->Handle(), &buf, 1, &bytesTransferred, 0, NULL, NULL);
    if (res == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
      std::cout << "wsa send failed" << std::endl;
    }
    std::cout << "end send, res = " << res << " err = " << WSAGetLastError() << std::endl;
  };

  server.SetReadCb(std::move(read_cb));
  base.Loop();

  WSACleanup();
  return 0;
}