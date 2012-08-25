#pragma once

#include "Primitives.lib.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

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

///////////////
class TSocket {
private:
	SOCKET hSocket;
private:
	TSocket() {}
public:
	enum TSocketType { TCP = 1, UDP = 2 };
public:
	TSocket(int type, int protocol, int flags) : hSocket(NULL) {
		hSocket = ::WSASocket(AF_INET, type, protocol,
			NULL, 0, flags);
		Verify(INVALID_SOCKET != hSocket);
	}
public:
	operator HANDLE() { return reinterpret_cast<HANDLE>(hSocket); }
public:
	operator SOCKET() { return hSocket; }
public:
	~TSocket() {
		int err = ::closesocket(hSocket);
		Verify(0 == err);
		hSocket = NULL;
	}
};

//////////////////////////
class TWinsockExtentions {
public:
	TWinsockExtentions() : lpfnTransmitFile(NULL), lpfnAcceptEx(NULL), lpfnGetAcceptExSockAddrs(NULL),
		lpfnTransmitPackets(NULL), lpfnConnectEx(NULL), lpfnDisconnectEx(NULL), 
		socket(INVALID_SOCKET) {}
private:
	LPFN_TRANSMITFILE lpfnTransmitFile;
public:
	BOOL TransmitFile(
		SOCKET hSocket,
		HANDLE hFile,
		DWORD nNumberOfBytesToWrite,
		DWORD nNumberOfBytesPerSend,
		LPOVERLAPPED lpOverlapped,
		LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
		DWORD dwReserved
		)
	{
		Verify(NULL != socket);
		Verify(NULL != lpfnTransmitFile);
		return lpfnTransmitFile(
			hSocket,
			hFile,
			nNumberOfBytesToWrite,
			nNumberOfBytesPerSend,
			lpOverlapped,
			lpTransmitBuffers,
			dwReserved
			);
	}
private:
	LPFN_ACCEPTEX lpfnAcceptEx;
public:
	BOOL AcceptEx(
		SOCKET sListenSocket,
		SOCKET sAcceptSocket,
		PVOID lpOutputBuffer,
		DWORD dwReceiveDataLength,
		DWORD dwLocalAddressLength,
		DWORD dwRemoteAddressLength,
		LPDWORD lpdwBytesReceived,
		LPOVERLAPPED lpOverlapped
		)
	{
		Verify(NULL != socket);
		Verify(NULL != lpfnAcceptEx);
		return lpfnAcceptEx(
			sListenSocket,
			sAcceptSocket,
			lpOutputBuffer,
			dwReceiveDataLength,
			dwLocalAddressLength,
			dwRemoteAddressLength,
			lpdwBytesReceived,
			lpOverlapped
			);
	}
private:
	LPFN_GETACCEPTEXSOCKADDRS lpfnGetAcceptExSockAddrs;
public:
	void GetAcceptExSockAddrs(
		PVOID lpOutputBuffer,
		DWORD dwReceiveDataLength,
		DWORD dwLocalAddressLength,
		DWORD dwRemoteAddressLength,
	struct sockaddr **LocalSockaddr,
		LPINT LocalSockaddrLength,
	struct sockaddr **RemoteSockaddr,
		LPINT RemoteSockaddrLength
		)
	{
		Verify(NULL != socket);
		Verify(NULL != lpfnGetAcceptExSockAddrs);
		return lpfnGetAcceptExSockAddrs(
			lpOutputBuffer,
			dwReceiveDataLength,
			dwLocalAddressLength,
			dwRemoteAddressLength,
			LocalSockaddr,
			LocalSockaddrLength,
			RemoteSockaddr,
			RemoteSockaddrLength
			);
	}
private:
	LPFN_TRANSMITPACKETS lpfnTransmitPackets;
public:
	BOOL TransmitPackets(
		SOCKET hSocket,                             
		LPTRANSMIT_PACKETS_ELEMENT lpPacketArray,                               
		DWORD nElementCount,                
		DWORD nSendSize,                
		LPOVERLAPPED lpOverlapped,                  
		DWORD dwFlags                               
		)
	{
		Verify(NULL != socket);
		Verify(NULL != lpfnTransmitPackets);
		return lpfnTransmitPackets(
			hSocket,                             
			lpPacketArray,                               
			nElementCount,                
			nSendSize,                
			lpOverlapped,                  
			dwFlags                               
			);
	}
private:
	LPFN_CONNECTEX lpfnConnectEx;
public:
	BOOL ConnectEx(
		SOCKET s,
		const struct sockaddr FAR *name,
		int namelen,
		PVOID lpSendBuffer,
		DWORD dwSendDataLength,
		LPDWORD lpdwBytesSent,
		LPOVERLAPPED lpOverlapped
		)
	{
		Verify(NULL != lpfnConnectEx);
		Verify(NULL != socket);
		return lpfnConnectEx(
			s,
			name,
			namelen,
			lpSendBuffer,
			dwSendDataLength,
			lpdwBytesSent,
			lpOverlapped
			);
	}
private:
	LPFN_DISCONNECTEX lpfnDisconnectEx;
public:
	BOOL DisconnectEx(
		SOCKET s,
		LPOVERLAPPED lpOverlapped,
		DWORD  dwFlags,
		DWORD  dwReserved
		)
	{
		Verify(NULL != socket);
		Verify(NULL != lpfnDisconnectEx);
		return lpfnDisconnectEx(
			s,
			lpOverlapped,
			dwFlags,
			dwReserved
			);
	}
public:
	BOOL Send(LPWSABUF lpBuffers, DWORD dwBufferCount, LPOVERLAPPED lpOverlapped) {
		Verify(NULL != socket);
		return ::WSASend(socket, lpBuffers, dwBufferCount, NULL, 0, lpOverlapped, NULL);
	}
public:
	BOOL Recv(LPWSABUF lpBuffers, DWORD dwBufferCount, LPOVERLAPPED lpOverlapped) {
		Verify(NULL != socket);
		return ::WSARecv(socket, lpBuffers, dwBufferCount, NULL, NULL, lpOverlapped, NULL);
	}
private:
	SOCKET socket;
public:
	void Init(SOCKET socket) {
		Verify(NULL != socket);

		DWORD dwBytes = 0;
		int check = 0;

		GUID GUID_WSAID_TRANSMITFILE = WSAID_TRANSMITFILE;
		check = WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GUID_WSAID_TRANSMITFILE,
			sizeof(GUID_WSAID_TRANSMITFILE), &lpfnTransmitFile, sizeof (lpfnTransmitFile), &dwBytes, NULL, NULL);
		Verify(SOCKET_ERROR != check);

		GUID GUID_WSAID_ACCEPTEX = WSAID_ACCEPTEX;
		check = WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GUID_WSAID_ACCEPTEX,
			sizeof(GUID_WSAID_ACCEPTEX), &lpfnAcceptEx, sizeof (lpfnAcceptEx), &dwBytes, NULL, NULL);
		Verify(SOCKET_ERROR != check);

		GUID GUID_WSAID_GETACCEPTEXSOCKADDRS = WSAID_GETACCEPTEXSOCKADDRS;
		check = WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GUID_WSAID_GETACCEPTEXSOCKADDRS,
			sizeof(GUID_WSAID_GETACCEPTEXSOCKADDRS), &lpfnGetAcceptExSockAddrs, sizeof (lpfnGetAcceptExSockAddrs), &dwBytes, NULL, NULL);
		Verify(SOCKET_ERROR != check);

		GUID GUID_WSAID_TRANSMITPACKETS = WSAID_TRANSMITPACKETS;
		check = WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GUID_WSAID_TRANSMITPACKETS,
			sizeof(GUID_WSAID_TRANSMITPACKETS), &lpfnTransmitPackets, sizeof (lpfnTransmitPackets), &dwBytes, NULL, NULL);
		Verify(SOCKET_ERROR != check);

		GUID GUID_WSAID_CONNECTEX = WSAID_CONNECTEX;
		check = WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GUID_WSAID_CONNECTEX,
			sizeof(GUID_WSAID_CONNECTEX), &lpfnConnectEx, sizeof (lpfnConnectEx), &dwBytes, NULL, NULL);
		Verify(SOCKET_ERROR != check);

		GUID GUID_WSAID_DISCONNECTEX = WSAID_DISCONNECTEX;
		check = WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GUID_WSAID_DISCONNECTEX,
			sizeof(GUID_WSAID_DISCONNECTEX), &lpfnDisconnectEx, sizeof (lpfnDisconnectEx), &dwBytes, NULL, NULL);
		Verify(SOCKET_ERROR != check);
	}
};

//////////////////////////////////////////////////////////////
class TSocketTcp : public TSocket, public TWinsockExtentions {
	TSocketTcp() : TSocket(SOCK_STREAM, IPPROTO_TCP, WSA_FLAG_OVERLAPPED) {
		Init(*this);
	}
};

//////////////////////////////////////////////////////////////
class TSocketUdp : public TSocket, public TWinsockExtentions {
	TSocketUdp() : TSocket(SOCK_DGRAM, IPPROTO_UDP, WSA_FLAG_OVERLAPPED) {
		Init(*this);
	}
};

