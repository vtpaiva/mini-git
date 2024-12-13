all:
	mkdir -p local
	mkdir -p repos
	g++ -o server src/server.cpp src/utils.cpp
	g++ -o client src/client.cpp src/utils.cpp

client:
	g++ -o client src/client.cpp
	./client

server:
	g++ -o server src/server.cpp
	./server

clean:
	rm -f server client