#pragma once

#include "IOCP.lib.h"

//////////////////////////////////////
class TRioSocketTcp : public TSocket {
	TRioSocketTcp() : TSocket(SOCK_STREAM, IPPROTO_TCP, WSA_FLAG_REGISTERED_IO) {
		Init(*this);
	}
};

//////////////////////////////////////
class TRioSocketUdp : public TSocket {
	TRioSocketUdp() : TSocket(SOCK_DGRAM, IPPROTO_UDP, WSA_FLAG_REGISTERED_IO) {
		Init(*this);
	}
};

//////////////////////
class TRioExtensions {
public:
	TRioExtensions() : socket(INVALID_SOCKET) {
		memset(&function_table, 0, sizeof(function_table));
		function_table.cbSize = sizeof(function_table);
	}
private:
	SOCKET socket;
private:
	RIO_EXTENSION_FUNCTION_TABLE function_table;
public:
	BOOL RIOReceive(
		RIO_RQ SocketQueue,
		PRIO_BUF pData,
		ULONG DataBufferCount,
		DWORD Flags,
		PVOID RequestContext
		)
	{
		Verify(INVALID_SOCKET != socket);
		Verify(NULL != function_table.RIOReceive);
		return function_table.RIOReceive(
			SocketQueue,
			pData,
			DataBufferCount,
			Flags,
			RequestContext
			);
	}
public:
	BOOL RIOReceiveEx(
		RIO_RQ SocketQueue,
		PRIO_BUF pData,
		ULONG DataBufferCount,
		PRIO_BUF pLocalAddress,
		PRIO_BUF pRemoteAddress,
		PRIO_BUF pControlContext,
		PRIO_BUF pFlags,
		DWORD Flags,
		PVOID RequestContext
		)
	{
		Verify(INVALID_SOCKET != socket);
		Verify(NULL != function_table.RIOReceiveEx);
		return function_table.RIOReceiveEx(
			SocketQueue,
			pData,
			DataBufferCount,
			pLocalAddress,
			pRemoteAddress,
			pControlContext,
			pFlags,
			Flags,
			RequestContext
			);
	}
public:
	BOOL RIOSend(
		RIO_RQ SocketQueue,
		PRIO_BUF pData,
		ULONG DataBufferCount,
		DWORD Flags,
		PVOID RequestContext
		)
	{
		Verify(INVALID_SOCKET != socket);
		Verify(NULL != function_table.RIOSend);
		return function_table.RIOSend(
			SocketQueue,
			pData,
			DataBufferCount,
			Flags,
			RequestContext
			);
	}
public:
	BOOL RIOSendEx(
		RIO_RQ SocketQueue,
		PRIO_BUF pData,
		ULONG DataBufferCount,
		PRIO_BUF pLocalAddress,
		PRIO_BUF pRemoteAddress,
		PRIO_BUF pControlContext,
		PRIO_BUF pFlags,
		DWORD Flags,
		PVOID RequestContext
		)
	{
		Verify(INVALID_SOCKET != socket);
		Verify(NULL != function_table.RIOSendEx);
		return function_table.RIOSendEx(
			SocketQueue,
			pData,
			DataBufferCount,
			pLocalAddress,
			pRemoteAddress,
			pControlContext,
			pFlags,
			Flags,
			RequestContext
			);
	}
public:
	VOID RIOCloseCompletionQueue(
		RIO_CQ CQ
		)
	{
		Verify(INVALID_SOCKET != socket);
		Verify(NULL != function_table.RIOCloseCompletionQueue);
		return function_table.RIOCloseCompletionQueue(
			CQ
			);
	}
public:
	RIO_CQ RIOCreateCompletionQueue(
		DWORD QueueSize,
		PRIO_NOTIFICATION_COMPLETION NotificationCompletion
		)
	{
		Verify(INVALID_SOCKET != socket);
		Verify(NULL != function_table.RIOCreateCompletionQueue);
		return function_table.RIOCreateCompletionQueue(
			QueueSize,
			NotificationCompletion
			);
	}
public:
	RIO_RQ RIOCreateRequestQueue(
		ULONG MaxOutstandingReceive,
		ULONG MaxReceiveDataBuffers,
		ULONG MaxOutstandingSend,
		ULONG MaxSendDataBuffers,
		RIO_CQ ReceiveCQ,
		RIO_CQ SendCQ,
		PVOID SocketContext
		)
	{
		Verify(INVALID_SOCKET != socket);
		Verify(NULL != function_table.RIOCreateRequestQueue);
		return function_table.RIOCreateRequestQueue(
			socket,
			MaxOutstandingReceive,
			MaxReceiveDataBuffers,
			MaxOutstandingSend,
			MaxSendDataBuffers,
			ReceiveCQ,
			SendCQ,
			SocketContext
			);
	}
public:
	ULONG RIODequeueCompletion(
		RIO_CQ CQ,
		PRIORESULT Array,
		ULONG ArraySize
		)
	{
		Verify(INVALID_SOCKET != socket);
		Verify(NULL != function_table.RIODequeueCompletion);
		return function_table.RIODequeueCompletion(
			CQ,
			Array,
			ArraySize
			);
	}
public:
	VOID RIODeregisterBuffer(
		RIO_BUFFERID BufferId
		)
	{
		Verify(INVALID_SOCKET != socket);
		Verify(NULL != function_table.RIODeregisterBuffer);
		return function_table.RIODeregisterBuffer(
			BufferId
			);
	}
public:
	INT RIONotify(
		RIO_CQ CQ
		)
	{
		Verify(INVALID_SOCKET != socket);
		Verify(NULL != function_table.RIONotify);
		return function_table.RIONotify(
			CQ
			);
	}
public:
	RIO_BUFFERID RIORegisterBuffer(
		PCHAR DataBuffer,
		DWORD DataLength
		)
	{
		Verify(INVALID_SOCKET != socket);
		Verify(NULL != function_table.RIORegisterBuffer);
		return function_table.RIORegisterBuffer(
			DataBuffer,
			DataLength
			);
	}
public:
	BOOL RIOResizeCompletionQueue(
		RIO_CQ CQ,
		DWORD QueueSize
		)
	{
		Verify(INVALID_SOCKET != socket);
		Verify(NULL != function_table.RIOResizeCompletionQueue);
		return function_table.RIOResizeCompletionQueue(
			CQ,
			QueueSize
			);
	}
public:
	BOOL RIOResizeRequestQueue(
		RIO_RQ RQ,
		DWORD MaxOutstandingReceive,
		DWORD MaxOutstandingSend
		)
	{
		Verify(INVALID_SOCKET != socket);
		Verify(NULL != function_table.RIOResizeRequestQueue);
		return function_table.RIOResizeRequestQueue(
			RQ,
			MaxOutstandingReceive,
			MaxOutstandingSend
			);
	}
public:
	void Init(SOCKET socket) {
		Verify(INVALID_SOCKET != socket);
		Verify(NULL != socket);
		this->socket = socket;

		DWORD dwBytes = 0;
		int check = 0;

		GUID GUID_WSAID_MULTIPLE_RIO = WSAID_MULTIPLE_RIO;
		check = WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GUID_WSAID_MULTIPLE_RIO,
			sizeof(GUID_WSAID_MULTIPLE_RIO), &function_table, sizeof (function_table), &dwBytes, NULL, NULL);
		Verify(SOCKET_ERROR != check);
	}
};

/////////////////////////
class TRioBufferManager {
	friend class TRioSocketQueue;
private:
	TRioBufferManager() { }
private:
	TRioExtensions rio_extensions;
private:
	std::vector<CHAR> parent_buffer;
private:
	struct TBlockHeader {
		__int64 seqno;
		__int64 topic;
		RIO_BUF rio_buf;
	};
private:
	RIO_BUFFERID bufferid;
public:
	operator RIO_BUFFERID() { return bufferid; }
private:
	DWORD block_size;
private:
	DWORD block_count;
private:
	DWORD max_payload_size;
private:
	TRioBufferManager(SOCKET socket, DWORD block_count, DWORD block_size = 1024) : 
		parent_buffer(block_size * block_count), bufferid(RIO_INVALID_BUFFERID),
		next_seqno(0), block_size(block_size), max_payload_size(block_size - sizeof(TBlockHeader)),
		block_count(block_count)
	{
		rio_extensions.Init(socket);
		bufferid = rio_extensions.RIORegisterBuffer(
			&parent_buffer[0], static_cast<DWORD>(block_size * block_count));
		Verify(RIO_INVALID_BUFFERID != bufferid);
	}
private:
	__int64 next_seqno;
public:
	__int64 current_seqno() { return next_seqno; }
public:
	DWORD get_block_size() { return block_size; }
public:
	bool WriteMessage(__int64 topic, std::vector<char> &write_buffer) {
		if(write_buffer.size() < max_payload_size) {
			TBlockHeader *header = reinterpret_cast<TBlockHeader*>
				(&parent_buffer[(next_seqno % block_count) * block_size]);
			header->rio_buf.BufferId = bufferid;
			header->rio_buf.Length = static_cast<DWORD>(write_buffer.size());
			header->rio_buf.Offset = 
				(next_seqno % block_count) * block_size + sizeof(TBlockHeader);
			header->seqno = next_seqno;
			header->topic = topic;
			next_seqno++;
			return true;
		} else {
			Verify(false);
			return false;
		}
	}
public:
	bool ReadMessage(__int64 &seqno, __int64 topic, std::vector<char> &read_buffer) {
		Verify(seqno >= 0);
		if((next_seqno - seqno) >= block_count) {
			Verify(false);
			return false;
		} else if(seqno < next_seqno) {
			read_buffer.resize(0);
			for(__int64 i = seqno; i < next_seqno; i++) {
				TBlockHeader *header = reinterpret_cast<TBlockHeader*>
					(&parent_buffer[(i % block_count) * block_size]);
				Verify(header->seqno == seqno);
				if(header->topic == topic) {
					char *payload = reinterpret_cast<char*>(&header[1]);
					read_buffer.resize(header->rio_buf.Length);
					memcpy(&read_buffer[0], &header[1], header->rio_buf.Length);
					return true;
				}
			}
			return false;
		} else {
			Verify(false);
			return false;
		}
	}
public:
	~TRioBufferManager() {
		rio_extensions.RIODeregisterBuffer(bufferid);
	}
};

///////////////////////////
class TRioCompletionQueue {
	friend class TRioSocketQueue;
private:
	TRioCompletionQueue() { }
private:
	TRioExtensions rio_extensions;
private:
	RIO_CQ cq;
private:
	TRioCompletionQueue(SOCKET socket, DWORD queue_size, HANDLE hEvent) : cq(RIO_INVALID_CQ) {
		rio_extensions.Init(socket);
		RIO_NOTIFICATION_COMPLETION notification_completion;
		memset(&notification_completion, 0, sizeof(notification_completion));
		notification_completion.Type = RIO_EVENT_COMPLETION;
		notification_completion.Event.EventHandle = hEvent;
		notification_completion.Event.NotifyReset = TRUE;
		cq = rio_extensions.RIOCreateCompletionQueue(
			queue_size,
			&notification_completion
			);
		Verify(RIO_INVALID_CQ != cq);
	}
public:
	BOOL Resize(DWORD size) {
		Verify(RIO_INVALID_CQ != cq);
		return rio_extensions.RIOResizeCompletionQueue(cq, size);
	}
public:
	INT Notify() {
		Verify(RIO_INVALID_CQ != cq);
		return rio_extensions.RIONotify(cq);
	}
public:
	ULONG Dequeue(
		PRIORESULT Array,
		ULONG ArraySize
		)
	{
		Verify(RIO_INVALID_CQ != cq);
		return rio_extensions.RIODequeueCompletion(
			cq,
			Array,
			ArraySize
			);
	}
public:
	~TRioCompletionQueue() {
		rio_extensions.RIOCloseCompletionQueue(cq);
		cq = RIO_INVALID_CQ;
	}
};

///////////////////////
class TRioSocketQueue {
private:
	TSocket socket;
private:
	TRioExtensions rio_extensions;
private:
	RIO_RQ rq;
public:
	TRioSocketQueue(int type, int protocol, unsigned long depth, RIO_CQ completion_queue) : 
		socket(type, protocol, WSA_FLAG_REGISTERED_IO), rq(NULL) {
		rio_extensions.Init(socket);
		rq = rio_extensions.RIOCreateRequestQueue(
			depth, 1, depth, 1, completion_queue, completion_queue, NULL);
		Verify(RIO_INVALID_RQ != rq);
	}
public:
	operator TSocket&() { return socket; }
public:
	std::shared_ptr<TRioBufferManager> CreateBufferManager(DWORD block_count, DWORD block_size = 1024) {
		std::shared_ptr<TRioBufferManager> buffer_manager =
			std::shared_ptr<TRioBufferManager>(
			new TRioBufferManager(socket, block_count, block_size));
		return buffer_manager;
	}
public:
	std::shared_ptr<TRioCompletionQueue> CreateCompletionQueue(
		DWORD queue_size, HANDLE hEvent)
	{
		std::shared_ptr<TRioCompletionQueue> completion_queue =
			std::shared_ptr<TRioCompletionQueue>(new TRioCompletionQueue(socket, queue_size, hEvent));
		return completion_queue;
	}
public:
	BOOL ResizeRequestQueue(
		DWORD MaxOutstandingReceive,
		DWORD MaxOutstandingSend
		)
	{
		return rio_extensions.RIOResizeRequestQueue(
			rq, MaxOutstandingReceive, MaxOutstandingSend);
	}
public:
	BOOL Receive(
		PRIO_BUF pData,
		ULONG DataBufferCount,
		DWORD Flags,
		PVOID RequestContext
		)
	{
		return rio_extensions.RIOReceive(
			rq,
			pData,
			DataBufferCount,
			Flags,
			RequestContext
			);
	}
public:
	int ReceiveEx(
		PRIO_BUF pData,
		ULONG DataBufferCount,
		PRIO_BUF pLocalAddress,
		PRIO_BUF pRemoteAddress,
		PRIO_BUF pControlContext,
		PRIO_BUF pFlags,
		DWORD Flags,
		PVOID RequestContext
		)
	{
		return rio_extensions.RIOReceiveEx(
			rq,
			pData,
			DataBufferCount,
			pLocalAddress,
			pRemoteAddress,
			pControlContext,
			pFlags,
			Flags,
			RequestContext
			);
	}
public:
	BOOL Send(
		PRIO_BUF pData,
		ULONG DataBufferCount,
		DWORD Flags,
		PVOID RequestContext
		)
	{
		return rio_extensions.RIOSend(
			rq,
			pData,
			DataBufferCount,
			Flags,
			RequestContext
			);
	}
public:
	int SendEx(
		PRIO_BUF pData,
		ULONG DataBufferCount,
		PRIO_BUF pLocalAddress,
		PRIO_BUF pRemoteAddress,
		PRIO_BUF pControlContext,
		PRIO_BUF pFlags,
		DWORD Flags,
		PVOID RequestContext
		)
	{
		return rio_extensions.RIOSendEx(
			rq,
			pData,
			DataBufferCount,
			pLocalAddress,
			pRemoteAddress,
			pControlContext,
			pFlags,
			Flags,
			RequestContext
			);
	}
public:
	~TRioSocketQueue() {
		// OS cleanup via closesocket
	}
};

///////////////////////////////////////////////////
class TRioSocketQueueTcp : public TRioSocketQueue {
	TRioSocketQueueTcp(unsigned long depth, RIO_CQ cq, int type = SOCK_STREAM, int protocol = IPPROTO_TCP) : 
		TRioSocketQueue(type, protocol, depth, cq) {
	}
};

///////////////////////////////////////////////////
class TRioSocketQueueUdp : public TRioSocketQueue {
	TRioSocketQueueUdp(unsigned long depth, RIO_CQ cq, int type = SOCK_DGRAM, int protocol = IPPROTO_UDP) :
		TRioSocketQueue(type, protocol, depth, cq) {
	}
};
