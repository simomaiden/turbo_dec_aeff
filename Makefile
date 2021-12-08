CC = gcc
CFLAGS = -Wall
LFLAGS = -lm

SRCDIR = ./src
OBJDIR = ./obj
BINDIR = ./bin

all :: $(BINDIR)/main

$(BINDIR)/main :: $(OBJDIR)/main.o $(OBJDIR)/log.o $(OBJDIR)/mem.o $(OBJDIR)/tasks.o $(OBJDIR)/result.o $(OBJDIR)/param.o
	$(CC) $(CFLAGS) $? -o $@ $(LFLAGS)

$(OBJDIR)/main.o :: $(SRCDIR)/main.c $(SRCDIR)/log.h
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/log.o :: $(SRCDIR)/log.c $(SRCDIR)/log.h $(SRCDIR)/param.h
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/mem.o :: $(SRCDIR)/mem.c $(SRCDIR)/mem.h $(SRCDIR)/param.h
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/tasks.o :: $(SRCDIR)/tasks.c $(SRCDIR)/tasks.h $(SRCDIR)/param.h $(SRCDIR)/log.h $(SRCDIR)/mem.h
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/result.o :: $(SRCDIR)/result.c $(SRCDIR)/result.h $(SRCDIR)/param.h
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/param.o :: $(SRCDIR)/param.c $(SRCDIR)/param.h
	$(CC) $(CFLAGS) -o $@ -c $<

clean ::
# Linux
#	rm -f $(BINDIR)/main $(OBJDIR)/*.o
# Windows
	del /f .\bin\main.exe
	del /f /s .\obj\*.o