CC = gcc
CFLAGS = -Wall

SRC = list.c main.c rule_table_parser.c
OBJ = $(SRC:.c=.o)
EXECUTABLE = main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) -o3 $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o3 $@ $<

clean:
	rm -f $(EXECUTABLE) $(OBJ)