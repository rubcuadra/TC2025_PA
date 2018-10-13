hellomake: 
	gcc -c -o fatal_error.o fatal_error.c 
	gcc -c -o get_pi.o get_pi.c 
	gcc -Wall -o client pi_client.c fatal_error.o get_pi.o
	gcc -Wall -o server pi_server.c fatal_error.o get_pi.o -lpthread 
	rm fatal_error.o get_pi.o
