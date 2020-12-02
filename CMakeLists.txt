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

add_library (TCPClientServerAPI STATIC "src/Socket.cpp" "src/ServerSocket.cpp" "src/ClientSocket.cpp" "src/SocketException.cpp")
if(WIN32)
	target_link_libraries(TCPClientServerAPI PRIVATE wsock32 ws2_32)
endif()
target_include_directories(TCPClientServerAPI PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/" "${CMAKE_CURRENT_SOURCE_DIR}/src/") 

add_executable (TCPServer "ServerMain.cpp")
target_link_libraries(TCPServer PRIVATE TCPClientServerAPI)
target_include_directories(TCPServer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/") 

add_executable (TCPClient "ClientMain.cpp")
target_link_libraries(TCPClient PRIVATE TCPClientServerAPI)
target_include_directories(TCPClient PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/") 

# TODO: Add tests and install targets if needed.
