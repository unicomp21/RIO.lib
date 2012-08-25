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
	void Init(SOCKET socket) {
		Verify(NULL != socket);

		DWORD dwBytes = 0;
		int check = 0;

		GUID GUID_WSAID_MULTIPLE_RIO = WSAID_MULTIPLE_RIO;
		check = WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GUID_WSAID_MULTIPLE_RIO,
			sizeof(GUID_WSAID_MULTIPLE_RIO), &function_table, sizeof (function_table), &dwBytes, NULL, NULL);
		Verify(SOCKET_ERROR != check);
	}
};
