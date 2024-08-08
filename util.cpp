#include "util.hpp"

LPFN_ACCEPTEX lpfnAcceptEx = NULL;
void GlobalFnInit() {
  GUID GuidAcceptEx = WSAID_ACCEPTEX;
  DWORD dwBytes = 0;
  auto sk = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  int iResult = WSAIoctl(sk, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GuidAcceptEx), &lpfnAcceptEx,
                         sizeof(lpfnAcceptEx), &dwBytes, NULL, NULL);
  CHECK(iResult != SOCKET_ERROR && lpfnAcceptEx, "cannot load acceptEx...");
  std::cout << "lpfnAcceptEx addr = " << (uint64_t)lpfnAcceptEx << std::endl;
  closesocket(sk);
}