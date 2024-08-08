#ifndef IOCP_TCPCONNECTION_HPP
#define IOCP_TCPCONNECTION_HPP

#include <memory>
#include <functional>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string_view>

#include "util.hpp"
#include "eventbase.hpp"

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
 public:
  using SPtr = std::shared_ptr<TcpConnection>;
  using ReadCallback = std::function<void(const SPtr& conn, std::string_view buffer)>;
  using CloseCallback = std::function<void(const SPtr& conn)>;

  TcpConnection(EventBase* base, SOCKET fd, int id) : base_(base), fd_(fd), id_(id) {
    ZeroMemory(&overlapped_, sizeof(OVERLAPPED));

    memset(buffer, 0, sizeof(buffer));
    buf_.buf = buffer;
    buf_.len = READ_BUFFER_LEN;

    channel_ = std::make_unique<Channel>(base_, fd_, [this](DWORD bytes) { this->OnRead(bytes); });
  }
  ~TcpConnection() {
    if (fd_ != INVALID_SOCKET) closesocket(fd_);
  }

  auto Id() const noexcept { return id_; }
  auto Handle() const noexcept { return fd_; }
  LPOVERLAPPED OverLapped() noexcept { return &overlapped_; }

  void SetReadCb(ReadCallback&& cb) { reader_ = std::move(cb); }
  void OnRead(DWORD bytes);
  void SetCloseCb(CloseCallback&& cb) { close_ = std::move(cb); }
  void OnClose() {
    CHECK(close_, "close cb empty");

    std::cout << "on close" << std::endl;
    close_(shared_from_this());

    if (fd_ != INVALID_SOCKET) {
      closesocket(fd_);
      fd_ = INVALID_SOCKET;
    }
  }

  void LaunchRead();

 private:
  void DoOnRead(DWORD bytes);

 private:
  EventBase* base_;
  SOCKET fd_;
  int id_{-1};

  std::unique_ptr<Channel> channel_;
  ReadCallback reader_;
  CloseCallback close_;

 private:
  static constexpr int READ_BUFFER_LEN = 1024;
  char buffer[READ_BUFFER_LEN];
  WSABUF buf_;
  OVERLAPPED overlapped_;
  DWORD bytesTransferred_{0};
  DWORD flag_{0};
};

#endif