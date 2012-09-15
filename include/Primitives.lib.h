#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <map>

////////////////////////////////
inline void Verify(bool check) {
	if(!check)
		::DebugBreak();
}

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
};

/////////////////////////////////////////////////////
class TMessage : std::map<std::string, std::string> {
};
