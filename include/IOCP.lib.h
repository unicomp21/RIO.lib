#pragma once

#include "Primitives.lib.h"
#include <stdio.h>
#include <vector>
#include <sstream>
#include <hash_map>

namespace MurmurBus { namespace IOCP {
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
	public:
		TWinsockExtensions() : lpfnTransmitFile(NULL), lpfnAcceptEx(NULL), lpfnGetAcceptExSockAddrs(NULL),
			lpfnTransmitPackets(NULL), lpfnConnectEx(NULL), lpfnDisconnectEx(NULL), 
			socket(INVALID_SOCKET) { }
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
	public:
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
	}; // TWinsockExtensions

	////////////////////////////////
	class TSocket : public ISocket {
	private:
		SOCKET hSocket;
	private:
		TWinsockExtensions winsockExtensions;
	private:
		TSocket() { NotImplemented(); }
	public:
		TSocket(int type, int protocol, int flags) : hSocket(NULL) {
			hSocket = ::WSASocket(AF_INET, type, protocol,
				NULL, 0, flags);
			Verify(INVALID_SOCKET != hSocket);
			winsockExtensions.Init(hSocket);
		}
	private:
		BOOL ISocket::TransmitFile(HANDLE hFile, DWORD nNumberOfBytesToWrite,
			DWORD nNumberOfBytesPerSend, LPOVERLAPPED lpOverlapped,
			LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers, DWORD dwReserved) {
				return winsockExtensions.TransmitFile(hFile, nNumberOfBytesToWrite,
					nNumberOfBytesPerSend, lpOverlapped,
					lpTransmitBuffers, dwReserved
					);
		}
	private:
		BOOL ISocket::AcceptEx(SOCKET sAcceptSocket, PVOID lpOutputBuffer,
			DWORD dwReceiveDataLength, DWORD dwLocalAddressLength, DWORD dwRemoteAddressLength,
			LPDWORD lpdwBytesReceived, LPOVERLAPPED lpOverlapped) {
				return winsockExtensions.AcceptEx(
					sAcceptSocket, lpOutputBuffer,
					dwReceiveDataLength, dwLocalAddressLength, dwRemoteAddressLength,
					lpdwBytesReceived, lpOverlapped
					);
		}
	private:
		void ISocket::GetAcceptExSockAddrs(PVOID lpOutputBuffer,	DWORD dwReceiveDataLength,
			DWORD dwLocalAddressLength,	DWORD dwRemoteAddressLength, struct sockaddr **LocalSockaddr,
			LPINT LocalSockaddrLength, struct sockaddr **RemoteSockaddr, LPINT RemoteSockaddrLength) {
				return winsockExtensions.GetAcceptExSockAddrs(
					lpOutputBuffer,	dwReceiveDataLength,
					dwLocalAddressLength,	dwRemoteAddressLength, LocalSockaddr,
					LocalSockaddrLength, RemoteSockaddr, RemoteSockaddrLength
					);
		}
	private:
		BOOL ISocket::TransmitPackets(LPTRANSMIT_PACKETS_ELEMENT lpPacketArray,                               
			DWORD nElementCount, DWORD nSendSize, LPOVERLAPPED lpOverlapped,                  
			DWORD dwFlags) {
				return winsockExtensions.TransmitPackets(
					lpPacketArray,                               
					nElementCount, nSendSize, lpOverlapped,                  
					dwFlags
					);
		}
	private:
		BOOL ISocket::ConnectEx(const struct sockaddr FAR *name,
			int namelen, PVOID lpSendBuffer, DWORD dwSendDataLength,
			LPDWORD lpdwBytesSent, LPOVERLAPPED lpOverlapped) {
				return winsockExtensions.ConnectEx(
					name, namelen, lpSendBuffer, dwSendDataLength,
					lpdwBytesSent, lpOverlapped
					);
		}
	private:
		BOOL ISocket::DisconnectEx(LPOVERLAPPED lpOverlapped, DWORD  dwFlags, DWORD  dwReserved) {
			return winsockExtensions.DisconnectEx(
				lpOverlapped, dwFlags, dwReserved
				);
		}
	private:
		BOOL ISocket::Send(LPWSABUF lpBuffers, DWORD dwBufferCount, LPOVERLAPPED lpOverlapped) {
			return winsockExtensions.Send(
				lpBuffers, dwBufferCount, lpOverlapped
				);
		}
	private:
		BOOL ISocket::Recv(LPWSABUF lpBuffers, DWORD dwBufferCount, LPOVERLAPPED lpOverlapped) {
			return winsockExtensions.Recv(
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
		TSocketTcp() : TSocket(SOCK_STREAM, IPPROTO_TCP, WSA_FLAG_OVERLAPPED) {
		}
	}; // TSocketTcp

	/////////////////////////////////////
	class TOverlappedRecv : TOverlapped {
	private:
		TOverlappedRecv() : TOverlapped(NULL) { NotImplemented(); }
	public:
		TOverlappedRecv(ICompletionResult *iCompletionResult, size_t buffer_size) : 
			TOverlapped(iCompletionResult), buffer(buffer_size) { }
	public:
		std::vector<char> buffer;
	}; // TOverlappedRecv

	/////////////////////////////////////
	class TOverlappedSend : TOverlapped {
	private:
		TOverlappedSend() : TOverlapped(NULL) { NotImplemented(); }
	public:
		TOverlappedSend(ICompletionResult *iCompletionResult) :
			TOverlapped(iCompletionResult) { }
	}; // TOverlappedSend

	///////////////////////////////////////////////
	class TListenerEx : public ICompletionResult {
	private:
		TListenerEx() { NotImplemented(); }
	private:
		class TOverlappedListener : public TOverlapped {
		public:
			std::vector<char> addresses_buffer;
		public:
			enum { address_reserve = sizeof(SOCKADDR_IN) + 16 };
		public:
			DWORD byte_count;
		public:
			ISocketPtr acceptee;
		private:
			TOverlappedListener() : TOverlapped(NULL), byte_count(0) { }
		public:
			TOverlappedListener(ICompletionResult *ICompletionResult) : 
				byte_count(0), addresses_buffer(2 * address_reserve),
				TOverlapped(ICompletionResult) {
					acceptee = ISocketPtr(new TSocketTcp());
			}
		};
	private:
		ISocketPtr acceptor;
	public:
		operator SOCKET() { return *acceptor; }
	public:
		TListenerEx(TIOCP &iocp, std::string intfc, short port, int depth) {
			acceptor = ISocketPtr(new TSocketTcp());
			iocp.Attach(*acceptor);

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
				TOverlappedListener *overlapped_listener = new TOverlappedListener(this);
				BOOL check = acceptor->AcceptEx(*overlapped_listener->acceptee, 
					&overlapped_listener->addresses_buffer[0], 0,
					TOverlappedListener::address_reserve, TOverlappedListener::address_reserve, 
					&overlapped_listener->byte_count, overlapped_listener);
				if(FALSE == check)
					Verify(ERROR_IO_PENDING == ::WSAGetLastError());
			}
		}
	private:
		void ICompletionResult::Completed(BOOL status, DWORD byte_count, TOverlapped *overlapped) {
			std::auto_ptr<TOverlappedListener> prev_overlapped_listener(
				reinterpret_cast<TOverlappedListener*>(overlapped));
			Accepted(status, prev_overlapped_listener->acceptee);
			TOverlappedListener *next_overlapped_listener = new TOverlappedListener(this);
			BOOL check = acceptor->AcceptEx(*next_overlapped_listener->acceptee, 
				&next_overlapped_listener->addresses_buffer[0], 0,
				TOverlappedListener::address_reserve, TOverlappedListener::address_reserve, 
				&next_overlapped_listener->byte_count, *next_overlapped_listener);
			if(FALSE == check)
				Verify(ERROR_IO_PENDING == ::WSAGetLastError());
		}
	public:
		virtual void Accepted(BOOL status, ISocketPtr socket) = 0;
	}; // TListenerEx

	/////////////////////////////////////////////
	class TClientEx : public ICompletionResult {
	private:
		ISocketPtr connector;
	public:
		operator SOCKET() { return *connector; }
	private:
		std::shared_ptr<TOverlapped> connect_notify;
	private:
		TClientEx() { NotImplemented(); }
	public:
		TClientEx(TIOCP &iocp, std::string intfc, std::string remote, short port)
		{
			connector = ISocketPtr(new TSocketTcp());
			iocp.Attach(*connector);

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

			connect_notify = std::shared_ptr<TOverlapped>(new TOverlapped(this));

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
		virtual void Connected(BOOL status, ISocketPtr socket) = 0;
	}; // TClientEx

	///////////////////////////////////
	class TSocketUdp : public TSocket {
	public:
		TSocketUdp() : TSocket(SOCK_DGRAM, IPPROTO_UDP, WSA_FLAG_OVERLAPPED) {
		}
	}; // TSocketUdp

	///////////////////////
	class ISessionManager {
		//todo
	}; // ISessionManager

	////////////////
	class TSession {
	public:
		TUUID id;
	private:
		ISessionManager *iSessionManager;
	private:
		TSession() { NotImplemented(); }
	public:
		TSession(ISessionManager *iSessionManager) :
			iSessionManager(iSessionManager) { }
	}; // TSession

	//////////////////////
	class TBufferManager {
	private:
		int block_count;
	private:
		int block_size;
	private:
		__int64 next_seqno;
	private:
		std::vector<char> parent_buffer;
	private:
		TBufferManager() : block_count(0), block_size(0), next_seqno(0),
			parent_buffer(0) { NotImplemented(); }
	public:
		TBufferManager(int block_count, int block_size = 1024) :
			block_count(block_count), block_size(block_size),
			next_seqno(0), parent_buffer(block_count * block_size) { }
	public:
		struct TBlock {
			DWORD topic;
			DWORD size;
		};
	public:
		__int64 current_seqno() { return next_seqno; }
	public:
		DWORD get_block_size() { return block_size; }
	public:
		bool WriteMessage(DWORD topic, std::vector<char> &write_buffer) {
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
		bool ReadMessage(__int64 &start_seqno, DWORD topic, std::vector<char> &read_buffer) {
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
		TBlock *GetBlock(__int64 &start_seqno, DWORD topic) {
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

	///////////////////////
	class TSessionManager {
	private:
		std::hash_map<std::string /* uuid */, TSession> sessions;
	private:
		TBufferManager bufferManager;
	public:
		//todo
	}; // TSessionManager
} /*MurmurBus*/ } /* IOCP */

