#include "tcpconnection.hpp"

void TcpConnection::LaunchRead() {
  auto res = WSARecv(fd_, &buf_, 1, &bytesTransferred_, &flag_, &overlapped_, nullptr);
  auto err = GetLastError();

  if (res == 0) {
    std::cout << "impossible" << std::endl;
    if (bytesTransferred_ != 0) DoOnRead(bytesTransferred_);
  } else {
    if (err == WSA_IO_PENDING) return;
    std::cerr << "launch read failed with error: " << GetLastError() << " res = " << res << " fd = " << (uint64_t)fd_
              << std::endl;
  }
}

void TcpConnection::OnRead(DWORD bytes) {
  std::cout << "buffer read before = " << buffer << " bytesTransferred_ = " << bytesTransferred_ << " bytes = " << bytes
            << std::endl;

  if (bytes != 0) DoOnRead(bytes);

  auto res = WSARecv(fd_, (LPWSABUF)&buf_, 1, &bytesTransferred_, &flag_, &overlapped_, nullptr);
  auto err = WSAGetLastError();

  if (res == 0) {
    if (bytesTransferred_ != 0) DoOnRead(bytesTransferred_);
  } else if (res == SOCKET_ERROR) {
    if (err == WSAEWOULDBLOCK || err == WSA_IO_PENDING) return;
    if (err == WSAECONNABORTED || err == WSAECONNRESET) OnClose();
  }

  std::cout << "buffer read after = " << buffer << std::endl;
}

void TcpConnection::DoOnRead(DWORD bytes) {
  CHECK(reader_, "invalid read callback");
  CHECK(bytes <= READ_BUFFER_LEN, "overflow");
  reader_(shared_from_this(), std::string_view(buffer, bytes));
  memset(buffer, 0, sizeof(buffer));
  bytesTransferred_ = 0;
  flag_ = 0;
}