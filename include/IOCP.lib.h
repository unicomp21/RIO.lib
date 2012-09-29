#pragma once

#include "Primitives.lib.h"
#include <stdio.h>
#include <vector>
#include <sstream>
#include <hash_map>
#include <iostream>

namespace MurmurBus { namespace IOCP {

	////////////////
	class ISession {
	public:
		virtual __int64 get_session_id() = 0;
	public:
		virtual void Send(TMessage &message) = 0;
	}; // ISession
	typedef std::shared_ptr<ISession> ISessionPtr;

	///////////////////////
	class ISessionManager {
	public:
		virtual ISessionPtr NewSession(ISocketPtr socket) = 0;
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
		TEvent _completions_waiting;
	private:
		TEvent &IIOCPEvented::completions_waiting() { return _completions_waiting; }
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
		BOOL Send(LPWSABUF lpBuffers, DWORD dwBufferCount, LPOVERLAPPED lpOverlapped) {
			Verify(NULL != socket);
			return ::WSASend(socket, lpBuffers, dwBufferCount, NULL, 0, lpOverlapped, NULL);
		}
	public:
		BOOL Send(char *buffer, unsigned long len, LPOVERLAPPED lpOverlapped) {
			WSABUF wsaBuf;
			wsaBuf.buf = buffer;
			wsaBuf.len = len;
			return Send(&wsaBuf, 1, lpOverlapped);
		}
	public:
		BOOL Recv(LPWSABUF lpBuffers, DWORD dwBufferCount, LPOVERLAPPED lpOverlapped) {
			Verify(NULL != socket);
			DWORD flags = 0;
			return ::WSARecv(socket, lpBuffers, dwBufferCount, NULL, &flags, lpOverlapped, NULL);
		}
	public:
		BOOL Recv(char *buffer, unsigned long len, LPOVERLAPPED lpOverlapped) {
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
		BOOL ISocket::Send(LPWSABUF lpBuffers, DWORD dwBufferCount, LPOVERLAPPED lpOverlapped) {
			return winsockExtensions->Send(
				lpBuffers, dwBufferCount, lpOverlapped
				);
		}
	private:
		BOOL ISocket::Recv(LPWSABUF lpBuffers, DWORD dwBufferCount, LPOVERLAPPED lpOverlapped) {
			return winsockExtensions->Recv(
				lpBuffers, dwBufferCount, lpOverlapped
				);
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
			TOverlappedListener *overlapped_listener = new TOverlappedListener(this, iocp->completions_waiting());
			BOOL check = acceptor->AcceptEx(*overlapped_listener->acceptee, 
				&overlapped_listener->addresses_buffer[0], 0,
				TOverlappedListener::address_reserve, TOverlappedListener::address_reserve, 
				&overlapped_listener->byte_count, overlapped_listener);
			if(FALSE == check)
				Verify(ERROR_IO_PENDING == ::WSAGetLastError());
		}
	private:
		void ICompletionResult::Completed(BOOL status, DWORD byte_count, TOverlapped *overlapped) {
			std::auto_ptr<TOverlappedListener> prev_overlapped_listener(
				reinterpret_cast<TOverlappedListener*>(overlapped));
			std::cout << "Accepted" << std::endl;
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

			connect_notify = std::shared_ptr<TOverlapped>(new TOverlapped(this, iocp->completions_waiting()));

			check = connector->ConnectEx(reinterpret_cast<LPSOCKADDR>(&remote_addr), sizeof(remote_addr),
				NULL, 0, NULL, *connect_notify);
			if(FALSE == check)
				Verify(ERROR_IO_PENDING == ::WSAGetLastError());
		}
	private:
		void ICompletionResult::Completed(BOOL status, DWORD byte_count, TOverlapped *overlapped) {
			Verify(0 == byte_count);
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
				std::cout << __FUNCTION__ << std::endl;
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
		TSession::TSession();
	public:
		TSession::TSession(IIOCPEventedPtr iocp, __int64 session_id, ISessionManager *iSessionManager, ISocketPtr socket) :
			iocp(iocp), session_id(session_id), iSessionManager(iSessionManager), socket(socket),
			recv_loop(socket, this)
		{
			Verify(iocp);
			Verify(NULL != iSessionManager);
			Verify(socket);
			recv_loop.PostRecv();
		}
	private:
		ISocketPtr socket;
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
				socket(socket), TOverlappedRecv(this, session->iocp->completions_waiting()),
				session(session), recv_buffer(constants::max_msg_size, 0) { }
		private:
			void TSessionRecv::PostRecv() {
				Verify(recv_buffer.size() > 0);
				Reset();
				WSABUF wsaBuf;
				wsaBuf.buf = &recv_buffer[0];
				wsaBuf.len = static_cast<ULONG>(recv_buffer.size());
				socket->Recv(&wsaBuf, 1, this);
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
		TBytes message_builder;
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
			NotImplemented(); //todo
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
			iListenerProcessMessage(iListenerProcessMessage), iocp(iocp), next_session_id(0)
		{
			Verify(NULL != iListenerProcessMessage);
			Verify(iocp);
		}
	private:
		__int64 next_session_id;
	private:
		ISessionPtr ISessionManager::NewSession(ISocketPtr socket) {
			Verify(socket);
			ISessionPtr session = ISessionPtr(new TSession(iocp, ++next_session_id, this, socket));
			sessions[session->get_session_id()] = session;
			std::cout << "NewSession: " << session->get_session_id() << std::endl;
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

	//////////////////////////////////////////
	class TListener : public IProcessMessage {
		friend class TListenAccept;
	private:
		IProcessMessage *iProcessMessage;
	private:
		TListener::TListener();
	public:
		TListener::TListener(
			IIOCPEventedPtr iocp, std::string intfc, short port, int depth, IProcessMessage *iProcessMessage) : 
			iocp(iocp), acceptor(iocp, intfc, port, depth, this), iProcessMessage(iProcessMessage), connect_queue(iocp, this)
		{
			Verify(NULL != iProcessMessage);
			Verify(iocp);
			sessionManager = ISessionManagerPtr(new TSessionManager(this, iocp));
		}
	private:
		ISessionManagerPtr sessionManager;
	private:
		IIOCPEventedPtr iocp;
	private:
		////////////////////////////////////////
		class TListenAccept : public TAcceptEx {
			friend class TListener;
		private:
			TListener *listener;
		private:
			TListenAccept::TListenAccept();
		private:
			TListenAccept(IIOCPEventedPtr iocp, std::string intfc, short port, int depth, TListener *listener) :
				TAcceptEx(iocp, intfc, port, depth), listener(listener)
			{ 
				Verify(NULL != listener);
			}
		private:
			void TAcceptEx::Accepted(BOOL status, ISocketPtr socket) {
				Verify(TRUE == status);
				listener->sessionManager->NewSession(socket);
			}
		} acceptor;
	private:
		void IProcessMessage::Process(__int64 session_id, TMessage &message) {
			NotImplemented();
			//todo, message processing
		}
	public:
		bool TListener::Send(__int64 session_id, TMessage &message) {
			return sessionManager->Send(session_id, message);
		}
	private:
		/////////////////////////////////////////////////////
		class TConnectExQueueLocal : public TConnectExQueue {
		private:
			TListener *listener;
		private:
			TConnectExQueueLocal::TConnectExQueueLocal();
		public:
			TConnectExQueueLocal::TConnectExQueueLocal(IIOCPEventedPtr iocp, TListener *listener) : 
				TConnectExQueue(iocp), listener(listener) { }
		private:
			void TConnectExQueue::Connected(BOOL status, ISocketPtr socket) {
				Verify(TRUE == status);
				ISessionPtr session = listener->sessionManager->NewSession(socket);
				listener->Connected(session);
			}
		} /*TConnectExQueueLocal*/ connect_queue;
		friend class TConnectExQueueLocal;
	public:
		void TListener::Connect(std::string intfc, std::string remote, short port) {
			connect_queue.Connect(intfc, remote, port);
		}
	private:
		virtual void TListener::Connected(ISessionPtr session) = 0;
	};
	typedef std::shared_ptr<TListener> TListenerPtr;

	//////////////////////////////////////////
	class TEchoTest : public IProcessMessage {
	private:
		TEchoTest();
	public:
		TEchoTest(IIOCPEventedPtr iocp, std::string intfc, short port, int depth) :
			listener(iocp, intfc, port, depth, this) { }
	private:
		/////////////////////////////////////////
		class TListenerLocal : public TListener {
		private:
			std::string intfc;
		private:
			short port;
		private:
			int client_count;
		private:
			TListenerLocal::TListenerLocal();
		public:
			TListenerLocal(IIOCPEventedPtr iocp, std::string intfc, short port, int depth, IProcessMessage *iProcessMessage) :
				TListener(iocp, intfc, port, depth, iProcessMessage), intfc(intfc), port(port), client_count(0)
			{
				Connect(intfc, intfc, port);
			}
		private:
			void TListener::Connected(ISessionPtr session) {
				client_count++;
				if(client_count < 1024) // 1k echo clients
					Connect(intfc, intfc, port);

				TMessage message;
				message["command"] = "echo";
				message["hops"] = "0";
				session->Send(message);
			}
		} listener;
	private:
		void IProcessMessage::Process(__int64 session_id, TMessage &message) {
			int hops = 0;
			std::stringstream parser(message["hops"]);
			parser >> hops;
			hops++;
			if(hops % 100)
				std::cout << "session_id: " << session_id << ", " << hops << std::endl;
			std::stringstream out;
			out  << hops;
			message["hops"] = hops;
			listener.Send(session_id, message);
		}
	};

} /*MurmurBus*/ } /* IOCP */
