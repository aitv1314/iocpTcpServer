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

typedef enum _FILE_INFORMATION_CLASS {
  FileReplaceCompletionInformation = 61,
} FILE_INFORMATION_CLASS;
typedef struct _IO_STATUS_BLOCK {
  union {
    NTSTATUS Status;
    PVOID Pointer;
  };
  ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;
typedef struct _FILE_COMPLETION_INFORMATION {
  HANDLE Port;
  PVOID Key;
} FILE_COMPLETION_INFORMATION, *PFILE_COMPLETION_INFORMATION;
typedef NTSTATUS(WINAPI* PFN_NtSetInformationFile)(HANDLE FileHandle, PIO_STATUS_BLOCK IoStatusBlock,
                                                   PVOID FileInformation, ULONG Length,
                                                   FILE_INFORMATION_CLASS FileInformationClass);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
bool UnbindIOCP(SOCKET sk) {
  FILE_COMPLETION_INFORMATION buf;
  IO_STATUS_BLOCK block;
  ZeroMemory(&buf, sizeof(buf));
  ZeroMemory(&block, sizeof(block));

  HMODULE ntdll = LoadLibrary(TEXT("ntdll.dll"));
  CHECK(ntdll, "");
  PFN_NtSetInformationFile pfunc = (PFN_NtSetInformationFile)GetProcAddress(ntdll, "NtSetInformationFile");
  CHECK(pfunc, "");
  NTSTATUS res = pfunc((HANDLE)sk, &block, &buf, sizeof(buf), FileReplaceCompletionInformation);
  FreeLibrary(ntdll);
  return res == STATUS_SUCCESS;
}
#pragma GCC diagnostic pop