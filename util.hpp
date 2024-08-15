#ifndef IOCP_UTIL_HPP
#define IOCP_UTIL_HPP

#include <winsock2.h>
#include <mswsock.h>
#include <iostream>
#include <ntstatus.h>

#define CHECK(cond, log)                                                   \
  {                                                                        \
    if (!(cond)) {                                                         \
      std::cout << __FILE__ << " " << __LINE__ << " " << log << std::endl; \
      Sleep(1000);                                                         \
      exit(0);                                                             \
    }                                                                      \
  }

extern inline void SetNonblock(SOCKET sk) {
  unsigned long ul = 1;
  ioctlsocket(sk, FIONBIO, &ul);
}

extern LPFN_ACCEPTEX lpfnAcceptEx;
extern void GlobalFnInit();
extern bool UnbindIOCP(SOCKET sk);
#endif // IOCP_UTIL_HPP