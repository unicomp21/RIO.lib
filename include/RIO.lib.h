#pragma once

#include "IOCP.lib.h"

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
	BOOL RioReceive(
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
	BOOL RioReceiveEx(
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
