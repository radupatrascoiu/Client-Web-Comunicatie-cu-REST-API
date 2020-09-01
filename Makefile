CC = g++ -std=c++11 -Wall -Wextra -g

client: client.o helpers.o
	$(CC) $^ -o $@

client.o: client.cpp
	$(CC) $(CFLAGS) $< -c

helpers.o: helpers.cpp
	$(CC) $(CFLAGS) $< -c

run: client
	./client

clean:
	rm -f *.o client
