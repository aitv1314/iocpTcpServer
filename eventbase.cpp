#include "eventbase.hpp"

EventBase::EventBase() {
  iocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
  CHECK(iocp_ != INVALID_HANDLE_VALUE, "invalid iocp");

  std::cout << "iocp = " << (uint64_t)iocp_ << std::endl;
  wake_up_ = std::make_unique<Channel>(this, (SOCKET)INVALID_HANDLE_VALUE, [](DWORD bytes) {
    (void)bytes;
    std::cout << "hello world" << std::endl;
  });
}

void EventBase::AddChannel(Channel* ch) {
  CHECK(channels_.find(ch) == channels_.end(), "already in");
  channels_.emplace(ch);
  if (ch->Handle() == INVALID_HANDLE_VALUE) return;
  auto res = CreateIoCompletionPort(ch->Handle(), iocp_, (ULONG_PTR)ch, 0);
  CHECK(res != NULL, "add channel failed");

  std::cout << "add channel, fd = " << (uint64_t)ch->Handle() << " res = " << (uint64_t)res << std::endl;
}
void EventBase::DelChannel(Channel* ch) {
  CHECK(channels_.find(ch) != channels_.end(), "not in");
  channels_.erase(ch);
  if (ch->Handle() == INVALID_HANDLE_VALUE) return;
  auto res = CreateIoCompletionPort(ch->Handle(), NULL, 0, 0);
  std::cout << "del channel, fd = " << (uint64_t)ch->Handle() << " res = " << (uint64_t)res << std::endl;
}

void EventBase::Loop() {
  std::cout << "eventbase loop start here" << std::endl;
  while (true) {
    auto res =
        GetQueuedCompletionStatusEx(iocp_, events_, MAX_EVENTS, (PULONG)&current_activated_events_, INFINITE, FALSE);
    std::cout << "GetQueuedCompletionStatusEx res = " << res << std::endl;
    if (!res) {
      auto err = GetLastError();
      CHECK(err == WAIT_TIMEOUT, err);
      continue;
    }

    for (ULONG i = 0; i < current_activated_events_; ++i) {
      Channel* ch = (Channel*)(events_[i].lpCompletionKey);
      if ((ch == nullptr)) continue;
      ch->OnCallback(events_[i].dwNumberOfBytesTransferred);
    }
  }
}

void EventBase::WakeUp() { PostQueuedCompletionStatus(iocp_, 0, (ULONG_PTR)wake_up_.get(), NULL); }