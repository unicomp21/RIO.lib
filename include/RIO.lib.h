#pragma once

#include "IOCP.lib.h"
#include <set>
#include <hash_map>

//////////////////////////////////////
class TRioSocketTcp : public TSocket {
	TRioSocketTcp() : TSocket(SOCK_STREAM, IPPROTO_TCP, WSA_FLAG_REGISTERED_IO) {
	}
};

//////////////////////////////////////
class TRioSocketUdp : public TSocket {
	TRioSocketUdp() : TSocket(SOCK_DGRAM, IPPROTO_UDP, WSA_FLAG_REGISTERED_IO) {
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

/////////////////////////////
class TRioRingBufferManager {
	friend class TRioSocketQueue;
private:
	TRioRingBufferManager() { }
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
	TRioRingBufferManager(SOCKET socket, DWORD block_count, DWORD block_size = 1024) : 
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
	~TRioRingBufferManager() {
		rio_extensions.RIODeregisterBuffer(bufferid);
	}
};

/////////////////////////
class TRioBufferManager {
	friend class TRioSocketQueue;
	friend class TBuffer;
public:
	class TBuffer : public RIO_BUF {
	private:
		int block_id;
	private:
		TRioBufferManager *rioBufferManager;
	private:
		TBuffer() { }
	public:
		TBuffer(int block_id, TRioBufferManager *rioBufferManager) : 
			block_id(block_id), rioBufferManager(rioBufferManager) { }
	public:
		int get_block_id() { return block_id; }
	public:
		~TBuffer() { rioBufferManager->free_blocks.insert(block_id); }
	};
private:
	TRioBufferManager() { }
private:
	TRioExtensions rio_extensions;
private:
	std::vector<CHAR> parent_buffer;
private:
	RIO_BUFFERID bufferid;
public:
	operator RIO_BUFFERID() { return bufferid; }
private:
	DWORD block_size;
private:
	DWORD block_count;
private:
	std::set<int> free_blocks;
private:
	TRioBufferManager(SOCKET socket, DWORD block_count, DWORD block_size = 1024) : 
		parent_buffer(block_size * block_count), bufferid(RIO_INVALID_BUFFERID),
		block_size(block_size),	block_count(block_count)
	{
		rio_extensions.Init(socket);
		bufferid = rio_extensions.RIORegisterBuffer(
			&parent_buffer[0], static_cast<DWORD>(block_size * block_count));
		Verify(RIO_INVALID_BUFFERID != bufferid);
		for(DWORD i = 0; i < block_count; i++)
			free_blocks.insert(i);
	}
public:
	DWORD get_block_size() { return block_size; }
public:
	std::shared_ptr<TBuffer> Alloc() {
		Verify(free_blocks.size() > 0);
		int block_id = *free_blocks.begin();
		free_blocks.erase(block_id);
		return std::shared_ptr<TBuffer>(new TBuffer(block_id, this));
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
public:
	operator RIO_CQ() { return cq; }
public:
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

//////////////////////////////////
class TRioCompletionQueueEvented {
private:
	std::shared_ptr<TRioCompletionQueue> completion_queue;
public:
	std::shared_ptr<TRioCompletionQueue> completions() {
		return completion_queue;
	}
public:
	TEvent completions_waiting;
public:
	TRioCompletionQueueEvented(SOCKET socket, DWORD queue_size) {
		completion_queue = std::shared_ptr<TRioCompletionQueue>(
			new TRioCompletionQueue(socket, queue_size, completions_waiting));
	}
};

///////////////////////
class TRioSocketQueue {
private:
	ISocketPtr socket;
private:
	TRioExtensions rio_extensions;
private:
	RIO_RQ rq;
public:
	TRioSocketQueue(int type, int protocol, unsigned long depth, RIO_CQ completion_queue) : rq(NULL) {
		socket = ISocketPtr(new TSocket(type, protocol, WSA_FLAG_REGISTERED_IO));
		rio_extensions.Init(*socket);
		rq = rio_extensions.RIOCreateRequestQueue(
			depth, 1, depth, 1, completion_queue, completion_queue, NULL);
		Verify(RIO_INVALID_RQ != rq);
	}
public:
	operator SOCKET() { return *socket; }
public:
	std::shared_ptr<TRioRingBufferManager> CreateRingBufferManager(
		DWORD block_count, DWORD block_size = 1024) {
		std::shared_ptr<TRioRingBufferManager> buffer_manager =
			std::shared_ptr<TRioRingBufferManager>(
			new TRioRingBufferManager(*socket, block_count, block_size));
		return buffer_manager;
	}
public:
	std::shared_ptr<TRioCompletionQueue> CreateCompletionQueue(
		DWORD queue_size, HANDLE hEvent)
	{
		std::shared_ptr<TRioCompletionQueue> completion_queue =
			std::shared_ptr<TRioCompletionQueue>(new TRioCompletionQueue(*socket, queue_size, hEvent));
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
public:
	TRioSocketQueueTcp(unsigned long depth, RIO_CQ cq,
		int type = SOCK_STREAM, int protocol = IPPROTO_TCP) : 
		TRioSocketQueue(type, protocol, depth, cq) {
	}
};

///////////////////////////////////////////////////
class TRioSocketQueueUdp : public TRioSocketQueue {
public:
	TRioSocketQueueUdp(unsigned long depth, RIO_CQ cq,
		int type = SOCK_DGRAM, int protocol = IPPROTO_UDP) :
		TRioSocketQueue(type, protocol, depth, cq) {
	}
};

///////////////////
class TRioSession {
public:
	UUID id;
private:
	enum { socket_queue_depth = 16 };
private:
	TRioSocketQueueTcp socket_queue;
public:
	TRioSession(std::shared_ptr<TRioCompletionQueue> completion_queue) : 
		socket_queue(socket_queue_depth, *completion_queue) { }
public:
	void SendMessage(UUID session_id, TMessage &message) {
		//todo
	}
};
typedef std::shared_ptr<TRioSession> TRioSessionPtr;

////////////////////
class IRecvMessage {
public:
	virtual void RecvMessage(TMessage &message) = 0;
};

//////////////////////
class ISessionNotify {
public:
	virtual void SessionOpened(std::string session_id) = 0;
public:
	virtual void SessionClosed(std::string session_id) = 0;
};

///////////////////////////////////////////////
class ICloseSessionCallback : ICompletionResult
{ };

//////////////////////////////////////////////
class ISendMessageCallback : ICompletionResult
{ };

//////////////////////////////////////////
class IConnectCallback : ICompletionResult
{ };

//////////////////////////
class TRioSessionManager {
private:
	std::hash_map<std::string /*uuid*/, TRioSessionPtr> sessions;
private:
	TRioSessionManager() { }
private:
	IRecvMessage *iRecvMessage;
private:
	IIOCPEventedPtr iocp;
private:
	std::shared_ptr<TRioCompletionQueueEvented> rio_cq;
private:
	int max_completion_queue;
public:
	TRioSessionManager(IRecvMessage *iRecvMessage, ISessionNotify *iSessionNotify,
		IIOCPEventedPtr iocp, int max_completion_queue = 1024 * 1024) :
		iRecvMessage(iRecvMessage), iocp(iocp), max_completion_queue(max_completion_queue)
	{
		Verify(NULL != iRecvMessage);
	}
private:
	void InitCompletionQueue(SOCKET socket) {
		if(!rio_cq) {
			rio_cq = std::shared_ptr<TRioCompletionQueueEvented>(
				new TRioCompletionQueueEvented(socket, max_completion_queue));
		}
	}
private:
	friend class TSessionListenerEx;
	class TSessionListenerEx : public TListenerEx {
	public:
		TSessionListenerEx() { ::__debugbreak(); }
	private:
		TRioSessionManager *rioSessionManager;
	public:
		TSessionListenerEx(TRioSessionManager *rioSessionManager, 
			std::string intfc, short port, int depth) :
			TListenerEx(intfc, port, depth), rioSessionManager(rioSessionManager) { }
	private:
		void TListenerEx::Accepted(BOOL status, ISocketPtr socket) {
			Verify(TRUE == status);
			rioSessionManager->NewServerSession(socket);
		}
	};
	typedef std::shared_ptr<TSessionListenerEx> TSessionListenerExPtr;
	std::hash_map<std::string, TSessionListenerExPtr> listeners;
private:
	void NewServerSession(ISocketPtr socket) {
		//todo
	}
public:
	void Listen(std::string intfc, short port, int accept_depth) {
		std::stringstream key;
		key << intfc << ":" << port;
		Verify(listeners.end() == listeners.find(key.str()));
		TSessionListenerExPtr listener = TSessionListenerExPtr(
			new TSessionListenerEx(this, intfc, port, accept_depth));
		 listeners[key.str()] = listener;
		 InitCompletionQueue(*listener);
	}
private:
	class TSessionClientEx : public TClientEx {
	public:
		TSessionClientEx() { ::__debugbreak(); }
	private:
		TRioSessionManager *rioSessionManager;
	public:
		TSessionClientEx(TRioSessionManager *rioSessionManager, TIOCP &iocp, 
			std::string intfc, std::string remote,  short port) : 
			TClientEx(iocp, intfc, remote, port), 
			rioSessionManager(rioSessionManager) { }
	private:
		void TClientEx::Connected(BOOL status, ISocketPtr socket) {
			Verify(TRUE == status);
			rioSessionManager->NewClientSession(socket);
		}
	};
	typedef std::shared_ptr<TSessionClientEx> TSessionClientExPtr;
private:
	void NewClientSession(ISocketPtr socket) {
		//todo
	}
public:
	void Connect(std::string intfc, std::string remote_address, 
		short remote_port, IConnectCallback *iConnectCallback) 
	{
		std::stringstream key;
		key << remote_address << ":" << remote_port;
		Verify(listeners.end() == listeners.find(key.str()));
		TSessionClientExPtr client = TSessionClientExPtr(
			new TSessionClientEx(this, iocp, intfc, remote_address, remote_port));
		InitCompletionQueue(*client);
	}
public:
	void SendMesage(std::string session_id, TMessage &message,
		ISendMessageCallback *iSendMessageCallback) {
		//todo
	}
public:
	void CloseSession(std::string session_id, ICloseSessionCallback *iCloseSessionCallback) {
		//todo
	}
public:
	void RecvMessageCallback(IRecvMessage *iRecvMessage) {
		Verify(NULL != iRecvMessage);
		this->iRecvMessage = iRecvMessage;
	}
};
