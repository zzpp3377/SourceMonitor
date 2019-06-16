#!/bin/sh
g++ -std=c++11 -o example example.cpp -lSrcMntr -pthread  #compile using dynamic library
#g++ -std=c++11 -o example example.cpp /usr/lib/libSrcMntr.a /usr/lib/x86_64-linux-gnu/libpcap.a -pthread #compile using static library