noxChat: client.c server.c prompt.c chatroom.c
	gcc -o client client.c
	gcc -o server server.c
	gcc -o chatroom chatroom.c
	gcc -o prompt prompt.c
