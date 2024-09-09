all:
	g++ -o server src/server.cpp
	g++ -o client src/client.cpp

client:
	g++ -o client src/client.cpp

server:
	g++ -o server src/server.cpp

clean:
	rm -f server client