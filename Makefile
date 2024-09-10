all:
	g++ -o server src/server.cpp
	g++ -o client src/client.cpp

client:
	for i in {1..50}; do ./client; done

server:
	g++ -o server src/server.cpp

clean:
	rm -f server client