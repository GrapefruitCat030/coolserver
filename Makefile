
CFLAG := -g3

.PHONY: server

server: test-server test-client

test-server: test-util.cpp Channel.cpp Epoll.cpp Socket.cpp Inetaddress.cpp test-server.cpp
	g++ $(CFLAG) test-util.cpp Channel.cpp Epoll.cpp Socket.cpp Inetaddress.cpp test-server.cpp -o test-server

test-client: test-util.cpp Socket.cpp Inetaddress.cpp test-client.cpp
	g++ $(CFLAG) test-util.cpp Socket.cpp Inetaddress.cpp test-client.cpp -o test-client

clean:
	rm -f ./test-server ./test-client
