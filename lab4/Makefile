#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = 
CFLAGS = -Wall
RESINC = 
LIBDIR = 
LIB = 
LDFLAGS = 

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = bin/Debug/lab4

OBJ_DEBUG = $(OBJDIR_DEBUG)/main.o $(OBJDIR_DEBUG)/__/spolks_lib/MyClient.o $(OBJDIR_DEBUG)/__/spolks_lib/MyFiles.o $(OBJDIR_DEBUG)/__/spolks_lib/MyServer.o $(OBJDIR_DEBUG)/__/spolks_lib/MySignals.o $(OBJDIR_DEBUG)/__/spolks_lib/MySockets.o

all: debug

clean: clean_debug

before_debug: 
	test -d bin/Debug || mkdir -p bin/Debug
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)
	test -d $(OBJDIR_DEBUG)/__/spolks_lib || mkdir -p $(OBJDIR_DEBUG)/__/spolks_lib

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/main.o: main.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c main.c -o $(OBJDIR_DEBUG)/main.o

$(OBJDIR_DEBUG)/__/spolks_lib/MyClient.o: ../spolks_lib/MyClient.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../spolks_lib/MyClient.c -o $(OBJDIR_DEBUG)/__/spolks_lib/MyClient.o

$(OBJDIR_DEBUG)/__/spolks_lib/MyFiles.o: ../spolks_lib/MyFiles.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../spolks_lib/MyFiles.c -o $(OBJDIR_DEBUG)/__/spolks_lib/MyFiles.o

$(OBJDIR_DEBUG)/__/spolks_lib/MyServer.o: ../spolks_lib/MyServer.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../spolks_lib/MyServer.c -o $(OBJDIR_DEBUG)/__/spolks_lib/MyServer.o

$(OBJDIR_DEBUG)/__/spolks_lib/MySignals.o: ../spolks_lib/MySignals.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../spolks_lib/MySignals.c -o $(OBJDIR_DEBUG)/__/spolks_lib/MySignals.o

$(OBJDIR_DEBUG)/__/spolks_lib/MySockets.o: ../spolks_lib/MySockets.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../spolks_lib/MySockets.c -o $(OBJDIR_DEBUG)/__/spolks_lib/MySockets.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf bin/Debug
	rm -rf $(OBJDIR_DEBUG)
	rm -rf $(OBJDIR_DEBUG)/__/spolks_lib

.PHONY: before_debug after_debug clean_debug

