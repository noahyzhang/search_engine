FLAG=-std=c++11 -lboost_filesystem -lboost_system -I /home/zy/project/search_engine/cppjieba/include -ljsoncpp 

.PHONY:all
all:test libsearcher.so

test:test.cc searcher.h searcher.cc 
	g++ test.cc searcher.cc -o test $(FLAG)


libsearcher.so:searcher.h searcher.cc 
	g++ searcher.cc -shared -fPIC -o libsearcher.so $(FLAG)
