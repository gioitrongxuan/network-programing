CC = gcc
CFLAGS = -Wall -g

# Client
CLIENT_TARGET = client
CLIENT_OBJ = _client/client.o _client/auth_form.o

# Server
SERVER_TARGET = server
SERVER_OBJ = _server/server.o _server/network.o _server/user.o _server/net_lookup.o _server/check_valid.o

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
