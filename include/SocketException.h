#pragma once

#include <cwchar>
#include <exception>

class SocketException : public std::exception
{
public:
	SocketException() = delete;
	SocketException(const wchar_t* exception, ...);
	~SocketException();

	const char* what() const throw () override;	 // just notice to use wwhat() method
	const wchar_t* wwhat() const throw();
private:
	const wchar_t* m_Exception = NULL;
};