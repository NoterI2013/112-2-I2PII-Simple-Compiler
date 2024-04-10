cmake_minimum_required(VERSION 3.26)
project(112_2_I2PII_Assembler C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(112_2_I2PII_Assembler
        codeGen.c
        codeGen.h
        lex.c
        lex.h
        main.c
        parser.c
        parser.h)
