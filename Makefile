CC=gcc
CFLAGS=-c
LDFLAGS=-lseccomp
SOURCES=cgroup.c mount.c main.c seccomp.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

