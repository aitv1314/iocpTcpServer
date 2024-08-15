#ifndef IOCP_EVENTBASE_HPP
#define IOCP_EVENTBASE_HPP

#include <set>
#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#include <functional>
#include <memory>

#include "util.hpp"

using Callback = std::function<void(DWORD)>;

class Channel;
class EventBase {
 public:
  EventBase();

  void AddChannel(Channel* ch);
  void DelChannel(Channel* ch);

  void Loop() ;
  void WakeUp();

  auto IOCP() const noexcept { return iocp_; }

 private:
  HANDLE iocp_;
  std::set<Channel*> channels_;

  std::unique_ptr<Channel> wake_up_;

  CRITICAL_SECTION lock_;
};

class Channel {
 public:
  Channel(EventBase* base, SOCKET fd, Callback&& cb) : base_(base), fd_(fd), cb_(cb) { base_->AddChannel(this); }
  ~Channel() { base_->DelChannel(this); }

 public:
  auto Handle() { return (HANDLE)fd_; }
  void OnCallback(DWORD bytes) {
    CHECK(cb_, "invalid cb");
    cb_(bytes);
  }

 private:
  EventBase* base_;
  SOCKET fd_;
  Callback cb_;
};

#endif // IOCP_EVENTBASE_HPP