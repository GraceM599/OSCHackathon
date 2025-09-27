cmake_minimum_required(VERSION 4.0)
project(CalendarReadingWriting)

set(CMAKE_CXX_STANDARD 20)

add_executable(OSCHackathon CalendarReadingWritingCode/main.cpp
        CalendarReadingWritingCode/Calendar.cpp
        CalendarReadingWritingCode/Calendar.h)
