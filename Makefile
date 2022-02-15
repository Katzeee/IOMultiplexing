.PHONY: clean run
CC =gcc
RM = rm
SRCS = $(wildcard *.c)
EXES = $(patsubst %.c, %, $(SRCS))

%:%.c
	$(CC) -g -o $@ $^

$(EXES):$(SRCS)
	$(CC) -g -o  $@ $^

client:
	$(CC) -g -o client client.c
select:
	$(CC) -g -o select select.c
clean:
	rm select
run: select
	./select 5005
