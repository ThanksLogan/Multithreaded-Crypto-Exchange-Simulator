cmake_minimum_required(VERSION 3.23)
project(A4_v6)

set(CMAKE_CXX_STANDARD 11)

add_executable(A4_v6 main.cpp log.cpp producer.cpp consumer.cpp producer.h consumer.h log.h cryptoexchange.h broker.h)
