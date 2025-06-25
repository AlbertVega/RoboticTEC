.PHONY: all server client node clean

all: server client node

server:
	cd server && gcc server.c node_manager.c ServerUtils/utils.c -o server -lpthread library/liblibrary.a -Ilibrary

client:
	cd client && gcc client.c -o client

node:
	cd nodes && gcc node.c word_counter.c -o node

clean:
	cd server && rm -f server
	cd client && rm -f client
	cd nodes && rm -f node