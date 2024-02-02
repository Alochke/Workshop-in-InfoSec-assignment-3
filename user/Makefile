CFLAGS = -Wall

SRC = list.c main.c rule_table_parser.c
OBJ = $(SRC:.c=.o)
EXECUTABLE = main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	gcc $(CFLAGS) $^ -o3 $@

%.o: %.c
	gcc $(CFLAGS) -c $<

clean:
	rm $(EXECUTABLE) $(OBJ)