#include "eventbase.hpp"

EventBase::EventBase() {
  iocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
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
  auto res = CreateIoCompletionPort(ch->Handle(), iocp_, (ULONG_PTR)ch, 1);
  CHECK(res != NULL, "add channel failed");

  std::cout << "add channel, fd = " << (uint64_t)ch->Handle() << " res = " << (uint64_t)res << std::endl;
}
void EventBase::DelChannel(Channel* ch) {
  CHECK(channels_.find(ch) != channels_.end(), "not in");
  channels_.erase(ch);
  if (ch->Handle() == INVALID_HANDLE_VALUE) return;

  auto res = CancelIo(ch->Handle());
  CHECK(res, "");
  res = UnbindIOCP((SOCKET)ch->Handle());
  CHECK(res, "");
  std::cout << "del channel, fd = " << (uint64_t)ch->Handle() << " res = " << (uint64_t)res << std::endl;
}

void EventBase::Loop() {
  static constexpr DWORD MAX_EVENTS = 64;
  OVERLAPPED_ENTRY events[MAX_EVENTS];
  ULONG current_activated_events = 0;

  std::cout << "eventbase loop start here" << std::endl;
  while (true) {
    auto res = GetQueuedCompletionStatusEx(iocp_, events, MAX_EVENTS, &current_activated_events, INFINITE, FALSE);
    std::cout << "GetQueuedCompletionStatusEx res = " << res << std::endl;
    if (!res) {
      auto err = GetLastError();
      CHECK(err == WAIT_TIMEOUT, err);
      continue;
    }

    for (ULONG i = 0; i < current_activated_events; ++i) {
      Channel* ch = (Channel*)(events[i].lpCompletionKey);
      if ((ch == nullptr)) continue;
      ch->OnCallback(events[i].dwNumberOfBytesTransferred);
    }

    current_activated_events = 0;
  }
}

void EventBase::WakeUp() { PostQueuedCompletionStatus(iocp_, 0, (ULONG_PTR)wake_up_.get(), NULL); }