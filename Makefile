.PHONY: clean

example: main.c carr.h
	$(CC) -O3 main.c -o $@

clean:
	$(RM) example
