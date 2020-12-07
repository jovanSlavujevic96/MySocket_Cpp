#include "SocketException.h"

#define DEFAULT_WSTRING_SIZE 4096u
#define WHAT_METHOD_MESSAGE "This is just disclaimer to use wwhat() instead of what() method."

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

SocketException::SocketException(const wchar_t* exception, ...) :
	m_Exception { (const wchar_t*)malloc(DEFAULT_WSTRING_SIZE) }
{
	if (m_Exception == (const wchar_t*)NULL || exception == (const wchar_t*)NULL)
	{
		return;
	}
	va_list args;
	va_start(args, exception);
	size_t ExceptionAddr = reinterpret_cast<size_t>(m_Exception);
	wchar_t* ExceptionPtr = reinterpret_cast<wchar_t*>(ExceptionAddr);
	int rc = vswprintf(ExceptionPtr, DEFAULT_WSTRING_SIZE / sizeof(wchar_t), exception, args) * sizeof(wchar_t);
	if (rc > 0 && rc < DEFAULT_WSTRING_SIZE)
	{
		ExceptionPtr = (wchar_t*)realloc((void*)m_Exception, rc);
	}
	va_end(args);
}

SocketException::~SocketException()
{
	if (m_Exception != NULL)
	{
		free((void*)m_Exception);
	}
}

const char* SocketException::what() const throw ()
{
	return WHAT_METHOD_MESSAGE;
}

const wchar_t* SocketException::wwhat() const throw()
{
	return this->m_Exception;
}