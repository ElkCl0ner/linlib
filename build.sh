#!/bin/bash

clang++ -std=c++17 -Wall -O2 main.cpp src/*.cpp -Iinclude -lpthread

