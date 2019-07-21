all:parser

parser::parser.cpp
	g++ $^ -o $@ -std=c++11 -lboost_filesystem -lboost_system

.PHONY:clean
clean:
	rm all

