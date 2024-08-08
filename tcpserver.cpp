#include "tcpserver.hpp"
#include "eventbase.hpp"

TcpServer::TcpServer(EventBase* base, const ULONG ip, int port) : base_(base) {
  listener_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  SetNonblock(listener_);

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(ip);
  addr.sin_port = htons(port);
  bind(listener_, (SOCKADDR*)&addr, sizeof(addr));
  listen(listener_, SOMAXCONN);

  channel_ = std::make_unique<Channel>(base_, listener_, [this](DWORD bytes) {
    (void)bytes;
    this->OnAccept();
  });

  client_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  setsockopt(listener_, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&client_, sizeof(&client_));
  SetNonblock(client_);

  LaunchAccept();
}

void TcpServer::OnRead(const TcpConnection::SPtr& conn, std::string_view buffer) { read_cb_(conn, buffer); }

void TcpServer::OnAccept() {
  std::cout << "on accept" << std::endl;

  auto id = seed_++;
  auto conn = std::make_shared<TcpConnection>(base_, client_, id);
  conn->SetReadCb([this](const TcpConnection::SPtr& conn, std::string_view buffer) { OnRead(conn, buffer); });
  conn->SetCloseCb([this](const TcpConnection::SPtr& conn) { OnClose(conn); });
  connections_[id] = conn;

  client_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  setsockopt((SOCKET)channel_->Handle(), SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&client_, sizeof(&client_));
  SetNonblock(client_);

  conn->LaunchRead();

  LaunchAccept();
}

void TcpServer::LaunchAccept() {
  char addrbuf[1];
  DWORD pending = 0;

  ZeroMemory(&overlapped_, sizeof(OVERLAPPED));

  CHECK(lpfnAcceptEx != NULL, "empty lpfnAcceptEx");
  WSASetLastError(0);
  auto res = lpfnAcceptEx((SOCKET)channel_->Handle(), client_, addrbuf, 0, sizeof(sockaddr_in) + 16,
                          sizeof(sockaddr_in) + 16, &pending, &overlapped_);
  std::cout << "launch accept res = " << res << " handle = " << (uint64_t)channel_->Handle()
            << " client handle = " << (uint64_t)client_ << std::endl;
  if (res) {
    std::cout << "impossible" << std::endl;
  } else {
    auto err = WSAGetLastError();
    CHECK(err == ERROR_IO_PENDING, err);
  }
}