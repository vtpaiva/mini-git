all:
	g++ -o server server.cpp
	g++ -o client client.cpp

client:
	g++ -o client client.cpp

server:
	g++ -o server server.cpp

clean:
	rm -f server client
	
run:
	./exec