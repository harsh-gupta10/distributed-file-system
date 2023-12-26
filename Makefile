CC = gcc
CFLAGS = 

SRC = naming_Server.c bookkeeping.c LRUcaching.c Triefinal.c
OBJ = $(SRC:.c=.o)

TARGET = nm

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
