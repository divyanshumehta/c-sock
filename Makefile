PORT = 9000
CFLAGS = -pthread
all: server client

server: server.c
	g++ -g -pthread -o $@ $<

client: client.c
	g++ -g -pthread -o $@ $<

runserver: server
	./$< $(PORT)

runclient: client
	./$< 127.0.0.1 $(PORT)
