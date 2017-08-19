cmake_minimum_required(VERSION 2.8)
project( pfmioview )
find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( pfmioview main.cpp )
target_link_libraries( pfmioview ${OpenCV_LIBS} ${SYSTEM_LIBS})