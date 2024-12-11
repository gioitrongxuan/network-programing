# Tên file thực thi
TARGET = main

# Trình biên dịch
CC = gcc

# Các cờ trình biên dịch
CFLAGS = -Wall -g

# Các file nguồn và file header
SRC = main.c
OBJ = main.o

# Luật mặc định để biên dịch chương trình
all: $(TARGET)

# Biên dịch file thực thi từ các file đối tượng
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Biên dịch file đối tượng từ file nguồn
main.o: main.c user.h
	$(CC) $(CFLAGS) -c main.c

# Luật clean để xóa các file tạm sau khi biên dịch
clean:
	rm -f $(TARGET) $(OBJ)
