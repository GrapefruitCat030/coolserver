
CFLAG := -g3

.PHONY: server

server: test-server test-client

test-server: test-util.cpp test-server.cpp
	g++ $(CFLAG) test-util.cpp test-server.cpp -o test-server

test-client: test-util.cpp test-client.cpp
	g++ $(CFLAG) test-util.cpp test-client.cpp -o test-client

clean:
	rm -f ./test-server ./test-client
