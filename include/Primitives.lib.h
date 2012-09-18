#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <map>
#include <vector>
#include <sstream>

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

///////////////
class TNumber {
private:
	enum { reserve_capacity = 128 };
public:
	TNumber() : out_writer(reserve_capacity) {
		out_buffer.reserve(reserve_capacity);
	}
private:
	std::stringstream in_reader;
private:
	std::string in_buffer;
public:
	size_t /*end_offset*/ get(std::vector<char> &buffer, size_t start_offset, __int64 *num) {
		in_buffer.clear();
		Verify(start_offset < buffer.size());
		size_t len = buffer[start_offset] - '0';
		start_offset++;
		in_buffer.insert(0, buffer.at(start_offset), buffer.at(start_offset + len));
		int val = 0;
		in_reader.str(in_buffer);
		in_reader >> val;
		return val;
	}
private:
	std::stringstream out_writer;
private:
	std::string out_buffer;
public:
	size_t /*end_offset*/ append(std::vector<char> &buffer, __int64 val) {
		out_writer.str("");
		out_writer << val;
		Verify(out_writer.str().length() >= 9);
		out_buffer.clear();
		out_buffer += static_cast<char>(val);
		out_buffer += out_writer.str();
		buffer.insert(buffer.end(), out_buffer.begin(), out_buffer.end());
	}
};

/////////////////////////////////////////////////////
class TMessage : std::map<std::string, std::string> {
};
