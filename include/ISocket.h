#pragma once

#include <cstdint>

class ISocket
{
public:
	explicit ISocket() = default;
	virtual ~ISocket() = default;

	virtual const uint32_t& getPort() const = 0;
	virtual const size_t& getBufferSize() const = 0;
 	virtual const char* getIP_str() const = 0;
};