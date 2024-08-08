#ifndef IOCP_TCPSERVER_HPP
#define IOCP_TCPSERVER_HPP

#include <string_view>
#include <map>

#include "tcpconnection.hpp"
#include "util.hpp"

class EventBase;
class TcpServer {
 public:
  TcpServer(EventBase* base, const ULONG ip, int port);
  ~TcpServer() {
    if (client_ != INVALID_SOCKET) closesocket(client_);
    channel_.reset(nullptr);
    if (listener_ != INVALID_SOCKET) closesocket(listener_);
  }
  using ReadCallback = TcpConnection::ReadCallback;
  void SetReadCb(ReadCallback&& cb) { read_cb_ = std::move(cb); }
  void OnRead(const TcpConnection::SPtr& conn, std::string_view buffer);
  void OnClose(const TcpConnection::SPtr& conn) { connections_.erase(conn->Id()); }
  void OnAccept();

 private:
  void LaunchAccept();

 private:
  EventBase* base_;
  std::unique_ptr<Channel> channel_;

  SOCKET listener_{INVALID_SOCKET};
  SOCKET client_{INVALID_SOCKET};

  int seed_{1000};
  std::map<int, TcpConnection::SPtr> connections_;
  ReadCallback read_cb_;
  OVERLAPPED overlapped_;
};

#endif