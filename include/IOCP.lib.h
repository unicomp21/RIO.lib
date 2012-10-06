#pragma once

#include "Primitives.lib.h"
#include <stdio.h>
#include <vector>
#include <queue>
#include <sstream>
#include <hash_map>
#include <iostream>
#include <set>

namespace MurmurBus {

	////////////////
	class ISession {
	public:
		virtual __int64 get_session_id() = 0;
	public:
		virtual void Send(TMessage &message) = 0;
	public:
		virtual bool IsServerSession() = 0;
	public:
		virtual std::string Description() = 0;
	}; // ISession
	typedef std::shared_ptr<ISession> ISessionPtr;

	///////////////////////
	class ISessionManager {
	public:
		virtual ISessionPtr NewSession(bool isServerSession, ISocketPtr socket) = 0;
	public:
		virtual bool Send(__int64 session_id, TMessage &message) = 0;
	public:
		virtual void Process(__int64 session_id, TMessage &message) = 0;
	public:
		virtual ~ISessionManager() { }
	}; // ISessionManager
	typedef std::shared_ptr<ISessionManager> ISessionManagerPtr;

	///////////////////////
	class IProcessMessage {
	public:
		virtual void Process(__int64 session_id, TMessage &message) = 0;
	}; // IProcessMessage

	///////////////////
	class TWSAStartup {
	private:
		WSADATA wsaData;
	public:
		TWSAStartup::TWSAStartup() {
			WORD wVersionRequested = MAKEWORD(2, 2);
			int err = ::WSAStartup(wVersionRequested, &wsaData);
			Verify(0 == err);
			Verify(LOBYTE(wsaData.wVersion) == 2 || HIBYTE(wsaData.wVersion) == 2);
		}
	public:
		TWSAStartup::~TWSAStartup() {
			int err = ::WSACleanup();
			Verify(0 == err);
		}
	}; // TWSAStartup

	//////////////////////////////////////////
	class TIOCPEvented : public IIOCPEvented {
	private:
		TIOCPEvented();
	public:
		TIOCPEvented(IEventPtr completions_waiting) : completions_waiting(completions_waiting) { }
	private:
		IEventPtr completions_waiting;
	private:
		IIOCPEvented::operator HANDLE() { return *completions_waiting; }
	private:
		TIOCP _completion_port;
	private:
		TIOCP &completion_port() { return _completion_port; }
	}; // TIOCPEvented

	//////////////////////////
	class TWinsockExtensions {
	private:
		SOCKET socket;
	private:
		TWinsockExtensions();
	public:
		TWinsockExtensions(SOCKET socket) :
			socket(socket), lpfnTransmitFile(NULL), lpfnAcceptEx(NULL), lpfnGetAcceptExSockAddrs(NULL),
			lpfnTransmitPackets(NULL), lpfnConnectEx(NULL), lpfnDisconnectEx(NULL)
		{
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
		int Send(LPWSABUF lpBuffers, DWORD dwBufferCount, LPOVERLAPPED lpOverlapped) {
			Verify(NULL != socket);
			return ::WSASend(socket, lpBuffers, dwBufferCount, NULL, 0, lpOverlapped, NULL);
		}
	public:
		int Send(char *buffer, unsigned long len, LPOVERLAPPED lpOverlapped) {
			WSABUF wsaBuf;
			wsaBuf.buf = buffer;
			wsaBuf.len = len;
			return Send(&wsaBuf, 1, lpOverlapped);
		}
	public:
		int Recv(LPWSABUF lpBuffers, DWORD dwBufferCount, LPOVERLAPPED lpOverlapped) {
			Verify(NULL != socket);
			DWORD flags = 0;
			return ::WSARecv(socket, lpBuffers, dwBufferCount, NULL, &flags, lpOverlapped, NULL);
		}
	public:
		int Recv(char *buffer, unsigned long len, LPOVERLAPPED lpOverlapped) {
			WSABUF wsaBuf;
			wsaBuf.buf = buffer;
			wsaBuf.len = len;
			return Recv(&wsaBuf, 1, lpOverlapped);
		}
	}; // TWinsockExtensions
	typedef std::shared_ptr<TWinsockExtensions> TWinsockExtensionsPtr;

	////////////////////////////////
	class TSocket : public ISocket {
	private:
		TWSAStartup wsaStartup;
	private:
		SOCKET hSocket;
	private:
		TWinsockExtensionsPtr winsockExtensions;
	private:
		TSocket::TSocket();
	public:
		TSocket::TSocket(int type, int protocol, int flags) : hSocket(NULL) {
			hSocket = ::WSASocket(AF_INET, type, protocol,
				NULL, 0, flags);
			Verify(INVALID_SOCKET != hSocket);
			winsockExtensions = TWinsockExtensionsPtr(new TWinsockExtensions(hSocket));

			BOOL val = TRUE;
			int err = ::setsockopt(hSocket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&val), sizeof(val));
			Verify(SOCKET_ERROR != err);
		}
	private:
		BOOL ISocket::TransmitFile(HANDLE hFile, DWORD nNumberOfBytesToWrite,
			DWORD nNumberOfBytesPerSend, LPOVERLAPPED lpOverlapped,
			LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers, DWORD dwReserved) {
				return winsockExtensions->TransmitFile(hFile, nNumberOfBytesToWrite,
					nNumberOfBytesPerSend, lpOverlapped,
					lpTransmitBuffers, dwReserved
					);
		}
	private:
		BOOL ISocket::AcceptEx(SOCKET sAcceptSocket, PVOID lpOutputBuffer,
			DWORD dwReceiveDataLength, DWORD dwLocalAddressLength, DWORD dwRemoteAddressLength,
			LPDWORD lpdwBytesReceived, LPOVERLAPPED lpOverlapped) {
				return winsockExtensions->AcceptEx(
					sAcceptSocket, lpOutputBuffer,
					dwReceiveDataLength, dwLocalAddressLength, dwRemoteAddressLength,
					lpdwBytesReceived, lpOverlapped
					);
		}
	private:
		void ISocket::GetAcceptExSockAddrs(PVOID lpOutputBuffer,	DWORD dwReceiveDataLength,
			DWORD dwLocalAddressLength,	DWORD dwRemoteAddressLength, struct sockaddr **LocalSockaddr,
			LPINT LocalSockaddrLength, struct sockaddr **RemoteSockaddr, LPINT RemoteSockaddrLength) {
				return winsockExtensions->GetAcceptExSockAddrs(
					lpOutputBuffer,	dwReceiveDataLength,
					dwLocalAddressLength,	dwRemoteAddressLength, LocalSockaddr,
					LocalSockaddrLength, RemoteSockaddr, RemoteSockaddrLength
					);
		}
	private:
		BOOL ISocket::TransmitPackets(LPTRANSMIT_PACKETS_ELEMENT lpPacketArray,                               
			DWORD nElementCount, DWORD nSendSize, LPOVERLAPPED lpOverlapped,                  
			DWORD dwFlags) {
				return winsockExtensions->TransmitPackets(
					lpPacketArray,                               
					nElementCount, nSendSize, lpOverlapped,                  
					dwFlags
					);
		}
	private:
		BOOL ISocket::ConnectEx(const struct sockaddr FAR *name,
			int namelen, PVOID lpSendBuffer, DWORD dwSendDataLength,
			LPDWORD lpdwBytesSent, LPOVERLAPPED lpOverlapped) {
				return winsockExtensions->ConnectEx(
					name, namelen, lpSendBuffer, dwSendDataLength,
					lpdwBytesSent, lpOverlapped
					);
		}
	private:
		BOOL ISocket::DisconnectEx(LPOVERLAPPED lpOverlapped, DWORD  dwFlags, DWORD  dwReserved) {
			return winsockExtensions->DisconnectEx(
				lpOverlapped, dwFlags, dwReserved
				);
		}
	private:
		int ISocket::Send(LPWSABUF lpBuffers, DWORD dwBufferCount, LPOVERLAPPED lpOverlapped) {
			return winsockExtensions->Send(
				lpBuffers, dwBufferCount, lpOverlapped
				);
		}
	private:
		int ISocket::Recv(LPWSABUF lpBuffers, DWORD dwBufferCount, LPOVERLAPPED lpOverlapped) {
			return winsockExtensions->Recv(
				lpBuffers, dwBufferCount, lpOverlapped
				);
		}
	private:
		std::string ISocket::get_name() {
			SOCKADDR_IN name;
			memset(&name, 0, sizeof(name));
			int len = sizeof(name);
			int err = ::getsockname(hSocket, reinterpret_cast<LPSOCKADDR>(&name), &len);
			Verify(err != SOCKET_ERROR);
			std::stringstream out;
			out << ::inet_ntoa(name.sin_addr) << ":" << ::htons(name.sin_port);
			return out.str();
		}
	private:
		std::string ISocket::get_peername() {
			SOCKADDR_IN name;
			memset(&name, 0, sizeof(name));
			int len = sizeof(name);
			int err = ::getpeername(hSocket, reinterpret_cast<LPSOCKADDR>(&name), &len);
			Verify(err != SOCKET_ERROR);
			std::stringstream out;
			out << ::inet_ntoa(name.sin_addr) << ":" << ::htons(name.sin_port);
			return out.str();
		}
	private:
		ISocket::operator HANDLE() { return reinterpret_cast<HANDLE>(hSocket); }
	private:
		ISocket::operator SOCKET() { return hSocket; }
	public:
		virtual ~TSocket() {
			int err = ::closesocket(hSocket);
			Verify(0 == err);
			hSocket = NULL;
		}
	}; // TSocket

	///////////////////////////////////
	class TSocketTcp : public TSocket {
	public:
		TSocketTcp::TSocketTcp() : TSocket(SOCK_STREAM, IPPROTO_TCP, WSA_FLAG_OVERLAPPED) {
		}
	}; // TSocketTcp

	////////////////////////////////////////////
	class TOverlappedRecv : public TOverlapped {
	private:
		TOverlappedRecv::TOverlappedRecv();
	public:
		TOverlappedRecv::TOverlappedRecv(ICompletionResult *iCompletionResult, HANDLE hEvent) : 
			TOverlapped(iCompletionResult, hEvent) { }
	}; // TOverlappedRecv

	////////////////////////////////////////////
	class TOverlappedSend : public TOverlapped {
	private:
		TOverlappedSend::TOverlappedSend();
	public:
		TOverlappedSend::TOverlappedSend(ICompletionResult *iCompletionResult, HANDLE hEvent) :
			TOverlapped(iCompletionResult, hEvent) { }
	}; // TOverlappedSend

	///////////////////////////////////////////////
	class TAcceptEx : public ICompletionResult {
	private:
		TAcceptEx::TAcceptEx();
	private:
		class TOverlappedListener : public TOverlapped {
		public:
			TBytes addresses_buffer;
		public:
			enum { address_reserve = sizeof(SOCKADDR_IN) + 16 };
		public:
			DWORD byte_count;
		public:
			ISocketPtr acceptee;
		private:
			TOverlappedListener();
		public:
			TOverlappedListener(ICompletionResult *ICompletionResult, HANDLE hEvent) : 
				byte_count(0), addresses_buffer(2 * address_reserve),
				TOverlapped(ICompletionResult, hEvent) {
					acceptee = ISocketPtr(new TSocketTcp());
			}
		};
	private:
		ISocketPtr acceptor;
	public:
		ISocketPtr get_acceptor() { return acceptor; }
	private:
		IIOCPEventedPtr iocp;
	public:
		operator SOCKET() { return *acceptor; }
	public:
		TAcceptEx::TAcceptEx(IIOCPEventedPtr iocp, std::string intfc, short port, int depth) : iocp(iocp) {
			acceptor = ISocketPtr(new TSocketTcp());
			iocp->completion_port().Attach(*acceptor);

			SOCKADDR_IN addr;
			memset(&addr, 0, sizeof(addr));
			addr.sin_addr.S_un.S_addr = ::inet_addr(intfc.c_str());
			addr.sin_family = AF_INET;
			addr.sin_port = ::htons(port);
			int check = ::bind(*acceptor, reinterpret_cast<LPSOCKADDR>(&addr), sizeof(addr));
			Verify(SOCKET_ERROR != check);
			check = ::listen(*acceptor, SOMAXCONN);
			Verify(SOCKET_ERROR != check);

			for(int i = 0; i < depth; i++)
				PostAccept();
		}
	private:
		void TAcceptEx::PostAccept() {
			TOverlappedListener *overlapped_listener = new TOverlappedListener(this, *iocp);
			BOOL check = acceptor->AcceptEx(*overlapped_listener->acceptee, 
				&overlapped_listener->addresses_buffer[0], 0,
				TOverlappedListener::address_reserve, TOverlappedListener::address_reserve, 
				&overlapped_listener->byte_count, overlapped_listener);
			if(FALSE == check)
				Verify(ERROR_IO_PENDING == ::WSAGetLastError());
			else
				Verify(0 == overlapped_listener->byte_count);
		}
	private:
		void ICompletionResult::Completed(BOOL status, DWORD byte_count, TOverlapped *overlapped) {
			std::auto_ptr<TOverlappedListener> prev_overlapped_listener(
				reinterpret_cast<TOverlappedListener*>(overlapped));
			//std::cout << "Accepted" << std::endl;
			Verify(0 == byte_count);
			iocp->completion_port().Attach(*prev_overlapped_listener->acceptee);
			Accepted(status, prev_overlapped_listener->acceptee);
			PostAccept();
		}
	public:
		virtual void TAcceptEx::Accepted(BOOL status, ISocketPtr socket) = 0;
	}; // TAcceptEx

	/////////////////////////////////////////////
	class TConnectEx : public ICompletionResult {
	private:
		IIOCPEventedPtr iocp;
	private:
		ISocketPtr connector;
	public:
		operator TConnectEx::SOCKET() { return *connector; }
	private:
		std::shared_ptr<TOverlapped> connect_notify;
	private:
		TConnectEx::TConnectEx();
	public:
		TConnectEx::TConnectEx(IIOCPEventedPtr iocp, std::string intfc, std::string remote, short port) : iocp(iocp)
		{
			connector = ISocketPtr(new TSocketTcp());
			iocp->completion_port().Attach(*connector);

			SOCKADDR_IN intfc_addr;
			memset(&intfc_addr, 0, sizeof(intfc_addr));
			intfc_addr.sin_addr.S_un.S_addr = ::inet_addr(intfc.c_str());
			intfc_addr.sin_family = AF_INET;
			intfc_addr.sin_port = ::htons(0);
			int check = ::bind(*connector, reinterpret_cast<LPSOCKADDR>(&intfc_addr), sizeof(intfc_addr));
			Verify(SOCKET_ERROR != check);

			SOCKADDR_IN remote_addr;
			memset(&remote_addr, 0, sizeof(remote_addr));
			remote_addr.sin_addr.S_un.S_addr = ::inet_addr(remote.c_str());
			remote_addr.sin_family = AF_INET;
			remote_addr.sin_port = ::htons(port);

			connect_notify = std::shared_ptr<TOverlapped>(new TOverlapped(this, *iocp));

			check = connector->ConnectEx(reinterpret_cast<LPSOCKADDR>(&remote_addr), sizeof(remote_addr),
				NULL, 0, NULL, *connect_notify);
			if(FALSE == check)
				Verify(ERROR_IO_PENDING == ::WSAGetLastError());
		}
	private:
		void ICompletionResult::Completed(BOOL status, DWORD byte_count, TOverlapped *overlapped) {
			Verify(TRUE == status);
			Verify(0 == byte_count);
			int err = setsockopt(*connector, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0);
			Verify(SOCKET_ERROR != err);
			Connected(status, connector);
		}
	protected:
		virtual void TConnectEx::Connected(BOOL status, ISocketPtr socket) = 0;
	}; // TConnectEx

	///////////////////////
	class TConnectExQueue {
	private:
		IIOCPEventedPtr iocp;
	private:
		TConnectExQueue::TConnectExQueue();
	public:
		TConnectExQueue(IIOCPEventedPtr iocp) : iocp(iocp), next_id(0) { }
	private:
		__int64 next_id;
	private:
		////////////////////////////////////////////
		class TConnectExClient : public TConnectEx {
		private:
			__int64 id;
		private:
			TConnectExQueue *queue;
		private:
			TConnectExClient::TConnectExClient();
		public:
			TConnectExClient::TConnectExClient(__int64 id, IIOCPEventedPtr iocp, std::string intfc, std::string remote, short port, 
				TConnectExQueue *queue) : 
			id(id), queue(queue), TConnectEx(iocp, intfc, remote, port)
			{
				Verify(NULL != queue);
				Verify(iocp);
			}
		private:
			void TConnectEx::Connected(BOOL status, ISocketPtr socket) {
				//std::cout << __FUNCTION__ << std::endl;
				queue->Connected(status, socket);
				queue->pending.erase(id);
			}
		}; // TConnectExClient
		friend class TConnectExClient;
		typedef std::shared_ptr<TConnectExClient> TConnectExClientPtr;
	private:
		std::hash_map<__int64, TConnectExClientPtr> pending;
	public:
		virtual void TConnectExQueue::Connected(BOOL status, ISocketPtr socket) = 0;
	public:
		void TConnectExQueue::Connect(std::string intfc, std::string remote, short port) {
			next_id++;
			pending[next_id] = TConnectExClientPtr(new TConnectExClient(next_id, iocp, intfc, remote, port, this));
		}
	};

	///////////////////////////////////
	class TSocketUdp : public TSocket {
	public:
		TSocketUdp::TSocketUdp() : TSocket(SOCK_DGRAM, IPPROTO_UDP, WSA_FLAG_OVERLAPPED) {
		}
	}; // TSocketUdp

	//////////////////////
	class TRingBufferManager {
	private:
		int block_count;
	private:
		int block_size;
	private:
		__int64 next_seqno;
	private:
		TBytes parent_buffer;
	private:
		TRingBufferManager::TRingBufferManager();
	public:
		TRingBufferManager::TRingBufferManager(int block_count, int block_size = 1024) :
			block_count(block_count), block_size(block_size),
			next_seqno(0), parent_buffer(block_count * block_size) { }
	public:
		struct TBlock {
			DWORD topic;
			DWORD size;
		};
	public:
		__int64 TRingBufferManager::current_seqno() { return next_seqno; }
	public:
		DWORD TRingBufferManager::get_block_size() { return block_size; }
	public:
		bool TRingBufferManager::WriteMessage(DWORD topic, TBytes &write_buffer) {
			DWORD max_payload = block_size - sizeof(TBlock);
			if(parent_buffer.size() >= max_payload) {
				Verify(false);
				return false;
			} else {
				DWORD mod_block_count = static_cast<DWORD>(next_seqno % block_count);
				memcpy(&parent_buffer[mod_block_count * block_size], 
					&write_buffer[0], write_buffer.size());
				return true;
			}
		}
	public:
		bool TRingBufferManager::ReadMessage(__int64 &start_seqno, DWORD topic, TBytes &read_buffer) {
			for(; start_seqno < next_seqno; start_seqno++) {
				DWORD mod_block_count = static_cast<DWORD>(next_seqno % block_count);
				TBlock *block = reinterpret_cast<TBlock*>(&parent_buffer[mod_block_count * block_size]);
				if(topic == block->topic) {
					read_buffer.resize(block->size);
					memcpy(&read_buffer[0], &block[1], block->size);
					return true;
				}
			}
			return false;
		}
	public:
		TBlock *TRingBufferManager::GetBlock(__int64 &start_seqno, DWORD topic) {
			for(; start_seqno < next_seqno; start_seqno++) {
				DWORD mod_block_count = static_cast<DWORD>(next_seqno % block_count);
				TBlock *block = reinterpret_cast<TBlock*>(&parent_buffer[mod_block_count * block_size]);
				if(topic == block->topic) {
					return block;
				}
			}
			return NULL;
		}
	}; // TBufferManager

	//////////////////////////////////
	class TSession : public ISession {
	private:
		__int64 session_id;
	public:
		__int64 get_session_id() { return session_id; }
	private:
		ISessionManager *iSessionManager;
	private:
		IIOCPEventedPtr iocp;
	private:
		bool isServerSession;
	private:
		TSession::TSession();
	public:
		TSession::TSession(
			bool isServerSession, IIOCPEventedPtr iocp, __int64 session_id,
			ISessionManager *iSessionManager, ISocketPtr socket
			) : isServerSession(isServerSession), iocp(iocp), session_id(session_id),
			iSessionManager(iSessionManager), socket(socket),
			recv_loop(socket, this), send_queue(socket, this)
		{
			Verify(iocp);
			Verify(NULL != iSessionManager);
			Verify(socket);
			recv_loop.PostRecv();
		}
	private:
		ISocketPtr socket;
	private:
		bool ISession::IsServerSession() { return isServerSession; }
	private:
		////////////////////////////////////////////////////////////////////////
		class TSessionRecv : private ICompletionResult, public TOverlappedRecv {
			friend class TSession;
		private:
			TBytes recv_buffer;
		private:
			TSession *session;
		private:
			ISocketPtr socket;
		private:
			TSessionRecv::TSessionRecv();
		public:
			TSessionRecv::TSessionRecv(ISocketPtr socket, TSession *session) : 
				socket(socket), TOverlappedRecv(this, *session->iocp),
				session(session), recv_buffer(constants::max_msg_size, 0) { }
		private:
			void TSessionRecv::PostRecv() {
				Verify(recv_buffer.size() > 0);
				Reset();
				WSABUF wsaBuf;
				wsaBuf.buf = &recv_buffer[0];
				wsaBuf.len = static_cast<ULONG>(recv_buffer.size());
				int err = socket->Recv(&wsaBuf, 1, this);
				if(SOCKET_ERROR == err)
					Verify(ERROR_IO_PENDING == ::GetLastError());
			}
		private:
			void ICompletionResult::Completed(BOOL status, DWORD byte_count, TOverlapped *overlapped)
			{
				Verify(TRUE == status); //todo
				session->ProcessMessage(recv_buffer, byte_count);
			}
		} recv_loop;
		typedef std::shared_ptr<TSessionRecv> TSessionRecvPtr;
		friend class TSessionRecv;
		//////////////////////////
	private:
		class TSessionSend : private ICompletionResult, public TOverlappedSend {
			friend class TSession;
		private:
			std::string send_buffer;
		private:
			TSession *session;
		private:
			ISocketPtr socket;
		private:
			bool pending;
		private:
			TSessionSend::TSessionSend();
		public:
			TSessionSend::TSessionSend(ISocketPtr socket, TSession *session) : pending(false),
				socket(socket), TOverlappedSend(this, *session->iocp),
				session(session)
			{ 
				send_buffer.reserve(constants::max_msg_size);
			}
		private:
			std::queue<TMessage> message_queue;
		private:
			void Send(TMessage &message) {
				message_queue.push(message);
				Flush();
			}
		private:
			void Flush() {
				if(false == pending) {
					send_buffer.clear();
					if(message_queue.size() > 0) {
						TMessage message = message_queue.front();
						message_queue.pop();
						message.Append(send_buffer);
						Reset();
						WSABUF wsaBuf;
						wsaBuf.buf = &send_buffer[0];
						wsaBuf.len = static_cast<ULONG>(send_buffer.size());
						int err = socket->Send(&wsaBuf, 1, this);
						if(SOCKET_ERROR == err)
							Verify(ERROR_IO_PENDING == ::GetLastError());
						pending = true;
					}
				}
			}
		private:
			void ICompletionResult::Completed(BOOL status, DWORD byte_count, TOverlapped *overlapped)
			{
				Verify(TRUE == status); //todo
				pending = false;
				Flush();
			}
		} send_queue;
		typedef std::shared_ptr<TSessionSend> TSessionSendPtr;
		friend class TSessionSend;
		//////////////////////////
	private:
		std::string message_builder;
	private:
		TMessage message;
	private:
		void TSession::ProcessMessage(TBytes &recv_buffer, DWORD byte_count) {
			message_builder.insert(message_builder.end(), recv_buffer.begin(), recv_buffer.begin() + byte_count);
			size_t end_offset = 0;
			message.SoftClear();
			while(true == message.Read(message_builder, 0, &end_offset)) {
				message_builder.erase(message_builder.begin(), message_builder.begin() + end_offset);
				iSessionManager->Process(session_id, message);
				end_offset = 0;
				message.SoftClear();
			}
			recv_loop.PostRecv();
		}
	private:
		void ISession::Send(TMessage &message) {
			send_queue.Send(message);
		}
	private:
		std::string ISession::Description() {
			std::stringstream out;
			out << "local: " << socket->get_name() << ", remote: " << socket->get_peername();
			return out.str();
		}
	}; // TSession

	////////////////////////////////////////////////
	class TSessionManager : public ISessionManager {
	private:
		std::hash_map<__int64 /*session_id*/, ISessionPtr> sessions;
	private:
		IIOCPEventedPtr iocp;
	private:
		IProcessMessage *iListenerProcessMessage;
	private:
		TSessionManager::TSessionManager();
	public:
		TSessionManager::TSessionManager(IProcessMessage *iListenerProcessMessage, IIOCPEventedPtr iocp) : 
			iListenerProcessMessage(iListenerProcessMessage), iocp(iocp), next_session_id(1)
		{
			Verify(NULL != iListenerProcessMessage);
			Verify(iocp);
		}
	private:
		__int64 next_session_id;
	private:
		ISessionPtr ISessionManager::NewSession(bool isServerSession, ISocketPtr socket) {
			Verify(socket);
			ISessionPtr session = ISessionPtr(new TSession(isServerSession, iocp, ++next_session_id, this, socket));
			sessions[session->get_session_id()] = session;
			//if(session->get_session_id() % 100 == 0) std::cout << "NewSession: " << session->get_session_id() << std::endl;
			return session;
		}
	private:
		bool ISessionManager::Send(__int64 session_id, TMessage &message) {
			auto iter = sessions.find(session_id);
			if(iter != sessions.end()) {
				iter->second->Send(message);
				return true;
			} else {
				return false;
			}
		}
	private:
		void ISessionManager::Process(__int64 session_id, TMessage &message) {
			iListenerProcessMessage->Process(session_id, message);
		}
	}; // TSessionManager

	//////////////////////
	class TListenConnect {
		friend class TListenAccept;
	private:
		IProcessMessage *iProcessMessage;
	private:
		TListenConnect::TListenConnect();
	public:
		TListenConnect::TListenConnect(IIOCPEventedPtr iocp, IProcessMessage *iProcessMessage) : 
			iocp(iocp), listen_id(0), iProcessMessage(iProcessMessage), 
			connect_queue(iocp, this), bridgeIProcessMessage(this)
		{
			Verify(NULL != iProcessMessage);
			Verify(iocp);
			sessionManager = ISessionManagerPtr(new TSessionManager(&bridgeIProcessMessage, iocp));
		}
	private:
		friend class TProcessMessage;
		class TProcessMessage : public IProcessMessage {
		private:
			TListenConnect *listenConnect;
		public:
			TProcessMessage(TListenConnect *listenConnect) : listenConnect(listenConnect) { }
		private:
			void IProcessMessage::Process(__int64 session_id, TMessage &message) {
				listenConnect->Process(session_id, message);
			}
		} bridgeIProcessMessage;
	public:
		__int64 /*listen_id*/ Listen(std::string intfc, short port, int accept_depth) {
			++listen_id;
			acceptors[listen_id] = TListenAcceptPtr(
				new TListenAccept(iocp, intfc, port, accept_depth, this));
			return listen_id;
		}
	public:
		bool UnListen(__int64 remove_listen_id) {
			if(acceptors.find(remove_listen_id) != acceptors.end()) {
				acceptors.erase(remove_listen_id);
				return true;
			}
			return false;
		}
	private:
		ISessionManagerPtr sessionManager;
	private:
		IIOCPEventedPtr iocp;
	private:
		////////////////////////////////////////
		class TListenAccept : public TAcceptEx {
			friend class TListenConnect;
		private:
			TListenConnect *listener;
		private:
			TListenAccept::TListenAccept();
		private:
			TListenAccept(
				IIOCPEventedPtr iocp, std::string intfc, short port, 
				int depth, TListenConnect *listener
				) :	TAcceptEx(iocp, intfc, port, depth), listener(listener)
			{ 
				Verify(NULL != listener);
			}
		private:
			void TAcceptEx::Accepted(BOOL status, ISocketPtr socket) {
				Verify(TRUE == status);
				SOCKET val = *get_acceptor();
				int err = setsockopt(*socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, 
					reinterpret_cast<char*>(&val), sizeof(val));
				Verify(SOCKET_ERROR != err);
				ISessionPtr session = listener->sessionManager->NewSession(true, socket);
				listener->Connected(session);
			}
		};
		typedef std::shared_ptr<TListenAccept> TListenAcceptPtr;
		std::hash_map<__int64, TListenAcceptPtr> acceptors;
	private:
		__int64 listen_id;
	private:
		void Process(__int64 session_id, TMessage &message) {
			iProcessMessage->Process(session_id, message);
		}
	public:
		bool TListenConnect::Send(__int64 session_id, TMessage &message) {
			return sessionManager->Send(session_id, message);
		}
	private:
		/////////////////////////////////////////////////////
		class TConnectExQueueLocal : public TConnectExQueue {
		private:
			TListenConnect *listener;
		private:
			TConnectExQueueLocal::TConnectExQueueLocal();
		public:
			TConnectExQueueLocal::TConnectExQueueLocal(IIOCPEventedPtr iocp, TListenConnect *listener) : 
				TConnectExQueue(iocp), listener(listener) { }
		private:
			void TConnectExQueue::Connected(BOOL status, ISocketPtr socket) {
				Verify(TRUE == status);
				ISessionPtr session = listener->sessionManager->NewSession(false, socket);
				listener->Connected(session);
			}
		} /*TConnectExQueueLocal*/ connect_queue;
		friend class TConnectExQueueLocal;
	public:
		void TListenConnect::Connect(std::string intfc, std::string remote, short port) {
			connect_queue.Connect(intfc, remote, port);
		}
	private:
		virtual void TListenConnect::Connected(ISessionPtr session) = 0;
	};
	typedef std::shared_ptr<TListenConnect> TListenConnectPtr;

	///////////////
	class IPubSub {
	public:
		//////////////////////////
		class ISubscribeCallback {
		public:
			virtual void Update(std::string topic, TMessage &message) = 0;
		};
	public:
		////////////////////////
		class IConnectCallback {
		public:
			virtual void Connected() = 0;
		};
	public:
		virtual void Listen(std::string intfc, short port) = 0;
	public:
		virtual void Connect(
			std::string intfc, std::string remote, short port, 
			IConnectCallback *iConnectCallback) = 0;
	public:
		virtual void Publish(std::string topic, TMessage &message) = 0;
	public:
		virtual void Subscribe(std::string topic, ISubscribeCallback *iSubscribeCallback) = 0;
	public:
		virtual ~IPubSub() { }
	};
	typedef std::shared_ptr<IPubSub> IPubSubPtr;

	////////////////////////////////////////////////////////////////////////////////////
	class TPubSubHub : public IPubSub, private IProcessMessage, private TListenConnect {
	private:
		TPubSubHub();
	public:
		TPubSubHub(IIOCPEventedPtr iocp) : session_id(0), 
			TListenConnect(iocp, this), iConnectCallback(NULL)
		{
			payload_parser.reserve(128);
		}
	private:
		void IPubSub::Listen(std::string intfc, short port) {
			TListenConnect::Listen(intfc, port, 128);
		}
	private:
		TOneShot oneshot_connect;
	private:
		IPubSub::IConnectCallback *iConnectCallback;
	private:
		void IPubSub::Connect(
			std::string intfc, std::string remote, short port,
			IConnectCallback *iConnectCallback) 
		{
			Verify(oneshot_connect);
			this->iConnectCallback = iConnectCallback;
			TListenConnect::Connect(intfc, remote, port);
		}
	private:
		std::string payload_parser;
	private:
		TMessage envelope_parser;
	private:
		__int64 session_id;
	private:
		void IPubSub::Publish(std::string topic, TMessage &message) {
			Verify(0 != session_id);
			payload_parser.clear();
			message.Append(payload_parser);
			envelope_parser.SoftClear();
			envelope_parser["command"] = "publish";
			envelope_parser["topic"] = topic;
			envelope_parser["payload"] = payload_parser;
			TListenConnect::Send(session_id, envelope_parser);
		}
	private:
		void IPubSub::Subscribe(
			std::string topic, IPubSub::ISubscribeCallback *iSubscribeCallback
			) 
		{
			Verify(NULL != iSubscribeCallback);
			Verify(0 != session_id);
			envelope_parser.SoftClear();
			envelope_parser["command"] = "subscribe";
			std::stringstream session_id_out(envelope_parser["session_id"]);
			session_id_out << session_id;
			TListenConnect::Send(session_id, envelope_parser);
		}
	private:
		typedef std::set<__int64> TSubscribers;
		std::hash_map<std::string /*topic*/, TSubscribers /*session_id*/> subscribers;
	private:
		std::hash_map<std::string /*topic*/, TMessage /*snap*/> snapshots;
	private:
		std::string command_parser, topic_parser;
	private:
		void IProcessMessage::Process(__int64 session_id, TMessage &message) {
			if(
				message.TryGet(std::string("command"), command_parser) &&
				message.TryGet(std::string("topic"), topic_parser)
				) 
			{
				if(command_parser == "publish") {
					auto iter_subscribers = subscribers.find(topic_parser);
					if(iter_subscribers != subscribers.end()) {
						auto iter_subscriber = iter_subscribers->second.begin();
						for(; iter_subscriber != iter_subscribers->second.end(); iter_subscriber++)
							TListenConnect::Send(*iter_subscriber, message);
					} else Verify(false);
					auto iter_snapshot = snapshots.find(topic_parser);
					if(iter_snapshot != snapshots.end()) {
						iter_snapshot->second.Merge(message);
					} else Verify(false);
				} else if(command_parser == "subscribe") {
					if(subscribers.find(topic_parser) == subscribers.end())
						subscribers[topic_parser] = TSubscribers();
					subscribers[topic_parser].insert(session_id);
					envelope_parser.SoftClear();
					Send(session_id, envelope_parser);
					envelope_parser["command"] = "snapshot";
					payload_parser.clear();
					auto iter = snapshots.find(topic_parser);
					if(iter != snapshots.end())
						iter->second.Append(payload_parser);
					envelope_parser["payload"] = payload_parser;
					Send(session_id, envelope_parser);
				} else Verify(false);
			} else Verify(false);
		}
	private:
		void TListenConnect::Connected(ISessionPtr session) {
			session_id = session->get_session_id();
		}
	};

	///////////////////////////////////////////////////////////////////
	class TEchoTest : private IProcessMessage, private TListenConnect {
	private:
		int client_count;
	private:
		std::string intfc;
	private:
		short port;
	private:
		TEchoTest();
	public:
		TEchoTest(
			IIOCPEventedPtr iocp, std::string intfc, short port, int accept_depth
			) : intfc(intfc), port(port), TListenConnect(iocp, this), client_count(0)
		{
			Listen(intfc, port, accept_depth);
			Connect(intfc, intfc, port);
		}
	private:
		void TListenConnect::Connected(ISessionPtr session) {
			if(session->IsServerSession()) {
				TMessage message;
				message["command"] = "echo";
				message["hops"] = "0";
				session->Send(message);
				std::cout << "accepted: " << session->get_session_id() << ", " <<
					session->Description() << std::endl;
			} else { // client
				if(client_count < 1024) {
					client_count++;
					Connect(intfc, intfc, port);
					std::cout << "conected: " << session->get_session_id() << ", " <<
						session->Description() << std::endl;
				} else
					std::cout << client_count << " echo sockets connected." << std::endl;
			}
		}
	private:
		std::stringstream parser;
	private:
		std::stringstream out;
	private:
		void IProcessMessage::Process(__int64 session_id, TMessage &message) {
			int hops = 0;
			parser.str(message["hops"]);
			parser >> hops;
			hops++;
			out.str("");
			out  << hops;
			message["hops"] = hops;
			TListenConnect::Send(session_id, message);
		}
	};

	///////////////////////////////////////////////////////
	class TPubSubTest : private IPubSub::IConnectCallback {
	public:
		TPubSubTest(IIOCPEventedPtr iocp) {
		}
	private:
		void IPubSub::IConnectCallback::Connected() {
			//todo
		}
	};
} /* MurmurBus*/
