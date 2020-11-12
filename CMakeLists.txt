# CMakeList.txt : CMake project for TCPClientServer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("TCPClientServer")

if(WIN32)
	find_library(WSOCK32_LIBRARY wsock32)
	find_library(WS2_32_LIBRARY ws2_32)
endif()

# Add source to this project's executable.
add_executable (TCPClientServer "ServerMain.cpp" "SocketVal.h" "ClientSocket.h" "ServerSocket.h" "ClientSocket.cpp" "ServerSocket.cpp")
if(WIN32)
	target_link_libraries(TCPClientServer wsock32 ws2_32)
endif()

# TODO: Add tests and install targets if needed.
