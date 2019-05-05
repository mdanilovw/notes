cmake_minimum_required(VERSION 3.5.1)
project(notes)

set(CMAKE_CXX_STANDARD 11)

include_directories( ./include ./lib/boost/boost_1_63_0 ./lib/cryptopp )
file(GLOB SOURCES "src/*.cpp")

set ( PROJECT_LINK_LIBS libcryptopp.a libboost_date_time.a libboost_serialization.a )
link_directories( ./lib/boost/linux/x64/stage/lib ./lib/cryptopp/linux/x64 )

add_executable(notes ${SOURCES})
target_link_libraries(notes ${PROJECT_LINK_LIBS} )


