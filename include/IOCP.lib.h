#pragma once

#include "Primitives.lib.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <stdio.h>
#include <memory>
#include <vector>
#include <sstream>
#include <hash_map>
#include <rpc.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "Ws2_32")
#pragma comment(lib, "rpcrt4")

/////////////
class TUUID {
private:
	UUID uuid;
public:
	TUUID() {
		UUID clear = { 0 };
		uuid = clear;
		RPC_STATUS status = ::UuidCreate(&uuid);
		Verify(RPC_S_OK == status);
	}
public:
	operator std::string() {
		RPC_CSTR buffer = NULL;
		RPC_STATUS status = ::UuidToStringA(&uuid, &buffer);
		Verify(RPC_S_OK == status);
		std::string out(reinterpret_cast<char*>(buffer));
		status = ::RpcStringFreeA(&buffer);
		Verify(RPC_S_OK == status);
		return out;
	}
};

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

/////////////////////////
class TOverlapped;
class ICompletionResult {
public:
	virtual void Completed(BOOL status, DWORD byte_count, TOverlapped *overlapped) = 0;
};

////////////
class TIOCP;
class TOverlapped : public OVERLAPPED {
	friend class TIOCP;
public:
	void Reset() { LPOVERLAPPED clear = this; memset(clear, 0, sizeof(OVERLAPPED)); }
public:
	TOverlapped() : iCompletion(NULL) { __debugbreak(); /* not implemented */ }
private:
	ICompletionResult *iCompletion;
public:
	operator LPOVERLAPPED() { return this; }
public:
	TOverlapped(ICompletionResult *iCompletion) : iCompletion(iCompletion) {
		Reset();
	}
};

/////////////
class TIOCP {
private:
	HANDLE hIOCP;
public:
	TIOCP() : running(true) {
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
private:
	volatile bool running;
public:
	void FlushQueue() {
		LPOVERLAPPED lpOverlapped = NULL;
		ULONG_PTR completion_key = 0;
		DWORD byte_count = 0;
		BOOL status = ::GetQueuedCompletionStatus(
			hIOCP, &byte_count, &completion_key, &lpOverlapped, INFINITE);
		TOverlapped *pOverlapped = reinterpret_cast<TOverlapped*>(lpOverlapped);
		if(TRUE == status) {
			Verify(NULL != pOverlapped->iCompletion);
			pOverlapped->iCompletion->Completed(status, byte_count, pOverlapped);
		} else {
			if(NULL != lpOverlapped) {
				Verify(NULL != pOverlapped->iCompletion);
				pOverlapped->iCompletion->Completed(status, byte_count, pOverlapped);
			} else {
				//timeout
			}
		}
	}
public:
	void Run() {
		while(running) {
			FlushQueue();
		}
	}
public:
	void Stop() {
		__debugbreak();
		//todo, running = false
	}
public:
	void FlushQueueEx() {
		__debugbreak();
		//todo, GetQueuedCompletionStatusEx
	}
public:
	~TIOCP() {
		BOOL check = ::CloseHandle(hIOCP);
		Verify(TRUE == check);
		hIOCP = NULL;
	}
};

//////////////////////////
class TWinsockExtensions {
public:
	TWinsockExtensions() : lpfnTransmitFile(NULL), lpfnAcceptEx(NULL), lpfnGetAcceptExSockAddrs(NULL),
		lpfnTransmitPackets(NULL), lpfnConnectEx(NULL), lpfnDisconnectEx(NULL), 
		socket(INVALID_SOCKET) {}
private:
	LPFN_TRANSMITFILE lpfnTransmitFile;
public:
	BOOL TransmitFile(
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
			socket,
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
			socket,
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
			socket,                             
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
			socket,
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
		LPOVERLAPPED lpOverlapped,
		DWORD  dwFlags,
		DWORD  dwReserved
		)
	{
		Verify(NULL != socket);
		Verify(NULL != lpfnDisconnectEx);
		return lpfnDisconnectEx(
			socket,
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
protected:
	void Init(SOCKET socket) {
		Verify(NULL != socket);
		this->socket = socket;

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

///////////////////////////////////////////
class TSocket : public TWinsockExtensions {
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
		TWinsockExtensions::Init(hSocket);
	}
public:
	operator HANDLE() { return reinterpret_cast<HANDLE>(hSocket); }
public:
	operator SOCKET() { return hSocket; }
public:
	virtual ~TSocket() {
		int err = ::closesocket(hSocket);
		Verify(0 == err);
		hSocket = NULL;
	}
};

///////////////////////////////////
class TSocketTcp : public TSocket {
public:
	TSocketTcp() : TSocket(SOCK_STREAM, IPPROTO_TCP, WSA_FLAG_OVERLAPPED) {
	}
};

///////////////////////////////////////////////
class TListenerEx : private ICompletionResult {
private:
	class TOverlappedListener : public TOverlapped {
	public:
		std::vector<char> addresses_buffer;
	public:
		enum { address_reserve = sizeof(SOCKADDR_IN) + 16 };
	public:
		DWORD byte_count;
	public:
		std::shared_ptr<TSocket> acceptee;
	private:
		TOverlappedListener() : byte_count(0) { }
	public:
		TOverlappedListener(ICompletionResult *ICompletionResult) : 
			byte_count(0), addresses_buffer(2 * address_reserve),
			TOverlapped(ICompletionResult) {
			acceptee = std::shared_ptr<TSocket>(new TSocketTcp());
		}
	};
private:
	std::shared_ptr<TSocketTcp> acceptor;
public:
	TListenerEx(std::string intfc, short port, int depth) {
		acceptor = std::shared_ptr<TSocketTcp>(new TSocketTcp());

		SOCKADDR_IN addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_addr.S_un.S_addr = ::inet_addr(intfc.c_str());
		addr.sin_family = AF_INET;
		addr.sin_port = ::htons(port);
		int check = ::bind(*acceptor, reinterpret_cast<LPSOCKADDR>(&addr), sizeof(addr));
		Verify(SOCKET_ERROR != check);
		check = ::listen(*acceptor, SOMAXCONN);
		Verify(SOCKET_ERROR != check);

		for(int i = 0; i < depth; i++) {
			TOverlappedListener *overlapped = new TOverlappedListener(this);
			BOOL check = acceptor->AcceptEx(*overlapped->acceptee, 
				&overlapped->addresses_buffer[0], 0,
				TOverlappedListener::address_reserve, TOverlappedListener::address_reserve, 
				&overlapped->byte_count, overlapped);
			if(FALSE == check)
				Verify(ERROR_IO_PENDING == ::WSAGetLastError());
		}
	}
private:
	virtual void ICompletionResult::Completed(BOOL status, DWORD byte_count, TOverlapped *overlapped) {
		std::auto_ptr<TOverlappedListener> overlapped_listener(
			reinterpret_cast<TOverlappedListener*>(overlapped));
		Accepted(status, std::shared_ptr<TSocket>(overlapped_listener->acceptee));
	}
public:
	virtual void Accepted(BOOL status, std::shared_ptr<TSocket> socket) = 0;
};

/////////////////////////////////////
class TClientEx : private ICompletionResult {
private:
	SOCKADDR_IN addr;
private:
	std::shared_ptr<TSocketTcp> connector;
private:
	std::shared_ptr<TOverlapped> connect_notify;
public:
	TClientEx(TIOCP &iocp, std::string intfc, short port)
	{
		memset(&addr, 0, sizeof(addr));
		addr.sin_addr.S_un.S_addr = ::inet_addr(intfc.c_str());
		addr.sin_family = AF_INET;
		addr.sin_port = ::htons(port);
		int check = ::bind(*connector, reinterpret_cast<LPSOCKADDR>(&addr), sizeof(addr));
		Verify(SOCKET_ERROR != check);
		check = ::listen(*connector, SOMAXCONN);
		Verify(SOCKET_ERROR != check);

		connector = std::shared_ptr<TSocketTcp>(new TSocketTcp());
		connect_notify = std::shared_ptr<TOverlapped>(new TOverlapped(this));

		check = connector->ConnectEx(reinterpret_cast<LPSOCKADDR>(&addr), sizeof(addr),
			NULL, 0, NULL, *connect_notify);
		if(FALSE == check)
			Verify(ERROR_IO_PENDING == ::WSAGetLastError());
	}
private:
	virtual void Completed(BOOL status, DWORD byte_count) {
		Connected(status);
	}
protected:
	virtual void Connected(BOOL status) = 0;
};

///////////////////////////////////
class TSocketUdp : public TSocket {
public:
	TSocketUdp() : TSocket(SOCK_DGRAM, IPPROTO_UDP, WSA_FLAG_OVERLAPPED) {
	}
};

///////////////////////
class ISessionManager {
};

////////////////
class TSession {
public:
	TUUID id;
private:
	ISessionManager *iSessionManager;
private:
	TSession() { }
public:
	TSession(ISessionManager *iSessionManager) :
		iSessionManager(iSessionManager) { }
};

//////////////////////
