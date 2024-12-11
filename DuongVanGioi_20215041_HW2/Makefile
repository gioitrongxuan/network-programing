
# Tên file thực thi
TARGET = lookup

# Trình biên dịch
CC = gcc

# Các cờ trình biên dịch
CFLAGS = -Wall -g

# Các file nguồn và file header
SRC = lookup.c
OBJ = lookup.o

# Luật mặc định để biên dịch chương trình
all: $(TARGET)

# Biên dịch file thực thi từ các file đối tượng
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Biên dịch file đối tượng từ file nguồn
lookup.o: lookup.c 
	$(CC) $(CFLAGS) -c lookup.c

# Luật clean để xóa các file tạm sau khi biên dịch
clean:
	rm -f $(TARGET) $(OBJ)