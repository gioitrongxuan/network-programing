CC = gcc
CFLAGS = -Wall -g

# Client
CLIENT_TARGET = client/client
CLIENT_OBJ = client/client.o client/auth_form.o

# Server
SERVER_TARGET = server/server
SERVER_OBJ = server/server.o server/network.o server/user.o server/net_lookup.o server/check_valid.o

all: $(CLIENT_TARGET) $(SERVER_TARGET)

$(CLIENT_TARGET): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(SERVER_TARGET): $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Biên dịch các file .o cho client
client/%.o: client/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Biên dịch các file .o cho server
server/%.o: server/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(CLIENT_TARGET) $(CLIENT_OBJ) $(SERVER_TARGET) $(SERVER_OBJ)
