CFLAGS = -Wall

SRC = log_list.c main.c rule_table.c logs.c
OBJ = $(SRC:.c=.o)
EXECUTABLE = main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	gcc $(CFLAGS) $^ -o $@
	rm $(OBJ)

%.o: %.c
	gcc $(CFLAGS) -c $<

clean:
	rm $(EXECUTABLE)