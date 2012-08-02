#pragma once
#include <WinSock2.h>
#include <Windows.h>

////////////////////////////////
inline void Verify(bool check) {
	if(!check)
		::DebugBreak();
}

///////////////////
class TWSAStartup {
private:
	WSADATA wsaData;
public:
	TWSAStartup() {
		WORD wVersionRequested = MAKEWORD(2, 2);
		int err = ::WSAStartup(wVersionRequested, &wsaData);
		Verify(0 == err);
		Verify(LOBYTE(wsaData.wVersion) == 2 || HIBYTE(wsaData.wVersion) == 2);
	}
public:
	~TWSAStartup() {
		int err = ::WSACleanup();
		Verify(0 == err);
	}
};

//////////////
class TEvent {
private:
	HANDLE hEvent;
public:
	TEvent() : hEvent(NULL) {
		hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		Verify(NULL != hEvent);
	}
public:
	operator HANDLE() { return hEvent; }
public:
	~TEvent() {
		BOOL check = ::CloseHandle(hEvent);
		Verify(TRUE == check);
		hEvent = NULL;
	}
};

/////////////
class TIOCP {
private:
	HANDLE hIOCP;
public:
	TIOCP() {
		hIOCP = ::CreateIoCompletionPort(
			INVALID_HANDLE_VALUE, NULL, 0, 0);
		Verify(NULL != hIOCP);
	}
public:
	void Attach(HANDLE hStream) {
		HANDLE hCheck = ::CreateIoCompletionPort(
			hStream, hIOCP, 0, 0);
		Verify(NULL != hCheck);
	}
public:
	~TIOCP() {
		BOOL check = ::CloseHandle(hIOCP);
		Verify(TRUE == check);
		hIOCP = NULL;
	}
};

//////////////////
class TSocketTcp {
private:
	SOCKET hSocket;
public:
	TSocketTcp() : hSocket(NULL) {
		hSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,
			NULL, 0, WSA_FLAG_OVERLAPPED);
		Verify(INVALID_SOCKET != hSocket);
	}
public:
	operator HANDLE() { return reinterpret_cast<HANDLE>(hSocket); }
public:
	operator SOCKET() { return hSocket; }
public:
	~TSocketTcp() {
		int err = ::closesocket(hSocket);
		Verify(0 == err);
		hSocket = NULL;
	}
};
