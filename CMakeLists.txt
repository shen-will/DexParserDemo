cmake_minimum_required(VERSION 3.14)
project(DexParseDemo C)

set(CMAKE_C_STANDARD 99)




add_executable(DexParseDemo main.c DexReader.c DexDump.c
        include/DexFile.h include/DexReader.h include/DexDump.h include/Helper.h Helper.c)