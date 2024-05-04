g++ -std=c++11 -c token.cpp lexer.cpp parser.cpp main.cpp
g++ -o my_program token.o lexer.o parser.o main.o
./my_program