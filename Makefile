all:
	g++ -o server src/server.cpp
	g++ -o client src/client.cpp

client:
	g++ -o client src/client.cpp
	./client

server:
	g++ -o server src/server.cpp
	./server

clean:
	rm -f server client