#pragma once

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <Windows.h>
#include <map>
#include <vector>
#include <sstream>
#include <memory>
#include <rpc.h>

//////////////////////////////
#pragma comment(lib, "Ws2_32")
#pragma comment(lib, "rpcrt4")

namespace MurmurBus {
	////////////////////////////////
	inline void Verify(bool check) {
		if(!check)
			::DebugBreak();
	}

	//////////////////////////////
	inline void NotImplemented() {
		::__debugbreak();
		throw std::exception("not implemented");
	}

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
		operator UUID() { return uuid; }
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
	}; // TUUID

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
	}; // TEvent

	///////////////
	class TNumber {
	private:
		enum { reserve_capacity = 128 };
	public:
		TNumber() {
			out_writer_cache.reserve(16);
			out_buffer.reserve(reserve_capacity);
		}
	private:
		std::string in_buffer;
	public:
		size_t /*end_offset*/ Read(std::vector<char> &buffer, size_t start_offset, __int64 *num) {
			in_buffer.clear();
			Verify(start_offset < buffer.size());
			size_t len = buffer[start_offset] - '0';
			start_offset++;
			Verify((start_offset + len) < buffer.size());
			size_t end_offset = start_offset + len;
			in_buffer.insert(in_buffer.begin(), 
				buffer.begin() + start_offset, buffer.begin() + end_offset);
			int val = 0;
			std::stringstream in_reader(in_buffer);
			in_reader >> val;
			*num = val;
			return end_offset;
		}
	private:
		std::string out_buffer;
	private:
		std::string out_writer_cache;
	public:
		void Append(std::vector<char> &buffer, __int64 val) {
			out_writer_cache.clear();
			std::stringstream out_writer(out_writer_cache);
			out_writer << val;
			Verify(out_writer.str().length() <= 9);
			Verify(out_writer.str().length() > 0);
			out_buffer.clear();
			out_buffer += static_cast<char>(out_writer.str().length() + '0');
			out_buffer += out_writer.str();
			buffer.insert(buffer.end(), out_buffer.begin(), out_buffer.end());
		}
	}; // TNumber

	////////////// LiPS - Length Prefixed Strings //////////////
	class TMessage : public std::map<std::string, std::string> {
	private:
		TNumber number;
	public:
		TMessage() { }
	private:
		std::string key, val;
	public:
		size_t Read(std::vector<char> &buffer, size_t offset) {
			__int64 count = 0;
			offset = number.Read(buffer, offset, &count);
			__int64 len_key = 0, len_val = 0;
			for(__int64 i = 0; i < count; i++) {
				len_key = 0;
				offset = number.Read(buffer, offset, &len_key);
				key.clear(); key.insert(key.begin(), 
					buffer.begin() + offset, buffer.begin() + (offset + static_cast<size_t>(len_key)));
				offset += static_cast<size_t>(len_key);
				len_val = 0;
				offset = number.Read(buffer, offset, &len_val);
				val.clear(); val.insert(val.begin(), 
					buffer.begin() + offset, buffer.begin() + (offset + static_cast<size_t>(len_val)));
				offset += static_cast<size_t>(len_val);
				(*this)[key] = val;
			}
			return offset;
		}
	public:
		void Append(std::vector<char> &buffer) {
			number.Append(buffer, this->size());
			for(iterator iter = begin(); iter != end(); iter++) {
				const std::string &key = iter->first;
				number.Append(buffer, key.length());
				buffer.insert(buffer.end(), key.begin(), key.end());
				std::string &val = iter->second;
				number.Append(buffer, val.length());
				buffer.insert(buffer.end(), val.begin(), val.end());
			}
		}
	public:
		void Dump(std::ostream &out) {
			for(iterator iter = begin(); iter != end(); iter++) {
				out << iter->first << ": " << iter->second << std::endl;
			}
			out << std::endl;
		}
	}; // TMessage

	/////////////////////////
	class TOverlapped;

	/////////////////////////
	class ICompletionResult {
	public:
		virtual void Completed(BOOL status, DWORD byte_count, TOverlapped *overlapped) = 0;
	}; // ICompletionResult

	///////////////////////////////////////
	class TOverlapped : public OVERLAPPED {
		friend class TIOCP;
	public:
		void Reset() { LPOVERLAPPED clear = this; memset(clear, 0, sizeof(OVERLAPPED)); }
	private:
		TOverlapped() : iCompletion(NULL) { NotImplemented(); }
	private:
		ICompletionResult *iCompletion;
	public:
		operator LPOVERLAPPED() { return this; }
	public:
		TOverlapped(ICompletionResult *iCompletion) : iCompletion(iCompletion) {
			Reset();
		}
	}; // TOverlapped

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
		void Attach(HANDLE hChild) {
			HANDLE hCheck = ::CreateIoCompletionPort(
				hChild, hIOCP, 0, 0);
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
			NotImplemented();
			//todo, running = false
		}
	public:
		void FlushQueueEx() {
			NotImplemented();
			//todo, GetQueuedCompletionStatusEx
		}
	public:
		virtual ~TIOCP() {
			BOOL check = ::CloseHandle(hIOCP);
			Verify(TRUE == check);
			hIOCP = NULL;
		}
	}; // TIOCP

	///////////////
	class ISocket {
	public:
		virtual operator HANDLE() = 0;
	public:
		virtual operator SOCKET() = 0;
	public:
		virtual BOOL TransmitFile(HANDLE hFile,	DWORD nNumberOfBytesToWrite,
			DWORD nNumberOfBytesPerSend, LPOVERLAPPED lpOverlapped,
			LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers, DWORD dwReserved) = 0;
	public:
		virtual BOOL AcceptEx(SOCKET sAcceptSocket,	PVOID lpOutputBuffer,
			DWORD dwReceiveDataLength, DWORD dwLocalAddressLength, DWORD dwRemoteAddressLength,
			LPDWORD lpdwBytesReceived, LPOVERLAPPED lpOverlapped) = 0;
	public:
		virtual void GetAcceptExSockAddrs(PVOID lpOutputBuffer,	DWORD dwReceiveDataLength,
			DWORD dwLocalAddressLength,	DWORD dwRemoteAddressLength, struct sockaddr **LocalSockaddr,
			LPINT LocalSockaddrLength, struct sockaddr **RemoteSockaddr, LPINT RemoteSockaddrLength) = 0;
	public:
		virtual BOOL TransmitPackets(LPTRANSMIT_PACKETS_ELEMENT lpPacketArray,                               
			DWORD nElementCount, DWORD nSendSize, LPOVERLAPPED lpOverlapped,                  
			DWORD dwFlags) = 0;
	public:
		virtual BOOL ConnectEx(const struct sockaddr FAR *name,
			int namelen, PVOID lpSendBuffer, DWORD dwSendDataLength,
			LPDWORD lpdwBytesSent, LPOVERLAPPED lpOverlapped) = 0;
	public:
		virtual BOOL DisconnectEx(LPOVERLAPPED lpOverlapped, DWORD  dwFlags, DWORD  dwReserved) = 0;
	public:
		virtual BOOL Send(LPWSABUF lpBuffers, DWORD dwBufferCount, LPOVERLAPPED lpOverlapped) = 0;
	public:
		virtual BOOL Recv(LPWSABUF lpBuffers, DWORD dwBufferCount, LPOVERLAPPED lpOverlapped) = 0;
	public:
		virtual ~ISocket() { }
	}; // ISocket

	typedef std::shared_ptr<ISocket> ISocketPtr;

	////////////////////
	class IIOCPEvented {
	public:
		virtual TEvent &completions_waiting() = 0;
	public:
		virtual TIOCP &completion_port() = 0;
	}; // IIOCPEvented

	typedef std::shared_ptr<IIOCPEvented> IIOCPEventedPtr;
}
