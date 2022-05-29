CC 	:= gcc
LD	:= $(CC)
CFLAGS	:= -Wall
LDLIB	:= -lcurl -ljson-c

export LD_LIBRARY_PATH=$(PWD)
run: src/main.c libkanji.so
	clear
	$(CC) $(CFLAGS) $(^) -o $(@)

debug: src/type.h.c
	clear
	$(CC) $(CFLAGS) $(^) -o $(@)
	
libkanji.so: src/kanji.c src/type.h
	clear
	$(CC) $(CFLAGS) $(<) -shared -o $(@) $(LDLIB)

make c:
	rm -f run
	clear