#!/bin/bash
# Approximate command for direct application compilation
g++ -std=c++11 *.cpp -I ./lib/crypto++/ -I ./lib/boost_1_63_0 ./lib/crypto++/libcryptopp.a ./lib/boost_1_63_0/stage/lib/libboost_date_time.a ./lib/boost_1_63_0/stage/lib/libboost_serialization.a -o notes