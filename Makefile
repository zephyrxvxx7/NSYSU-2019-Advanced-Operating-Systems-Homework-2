compiler = gcc
obj = server.o client.o

all: $(obj)
	$(compiler) -o server server.o
	$(compiler) -o client client.o

%.o: %.c
	$(compiler) -c $^ -o $@

.PHONY: clean
clean:
	rm *.o