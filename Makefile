all: client server
client: client.c
		gcc -w -o client client.c
server: server.c
		gcc -w -o server server.c
clean:
		rm client server