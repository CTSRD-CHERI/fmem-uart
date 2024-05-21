# Based on the Makefile from https://github.com/CTSRD-CHERI/cheri-bgas-fuse-devfs

OBJDIR = obj

SRC = fmem-uart.c
OUTPT = $(OBJDIR)/fmem-uart


CFLAGS = -O3 -Wall
LINKFLAGS = 

all: $(OUTPT)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OUTPT): $(SRC) $(OBJDIR)
	$(CC) -L $(OBJDIR) -I $(CURDIR) -o $@ $^ $(CFLAGS) $(LINKFLAGS)

.PHONY: clean

clean:
	rm -f $(OUTPT)
	rm -rf $(OBJDIR)
