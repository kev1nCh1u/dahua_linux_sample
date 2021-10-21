CROSS_COMPILE = 
CC = $(CROSS_COMPILE)gcc
CPPFLAGS = -Wall -g
AR=ar

HOST_ARCH=$(shell uname -m)

ifeq ($(HOST_ARCH), x86_64) 
	CPPFLAGS += -m64
	SDK_LIB_SUBDIR=m64x86
else ifeq ($(HOST_ARCH), i686) 
	CPPFLAGS += -m32
	SDK_LIB_SUBDIR=m32x86
else
endif

TARGET = sample

INCLUDES = -I./include
LINKLIBS = -L./depends/$(SDK_LIB_SUBDIR) -lMVSDK -lrt -lpthread
LINKLIBS += -L../../../lib -lMVSDK
SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

$(TARGET):$(OBJECTS)
	$(CC) $(INCLUDES) $(CPPFLAGS) $(OBJECTS) $(LINKLIBS) -o $(TARGET)
	rm -f $(OBJECTS)
	
.c.o:
	$(CC) $(INCLUDES) -c $(CPPFLAGS) $< -o $@
	
.PHONY: clean
clean:
	rm -f $(OBJECTS) $(TARGET)

	
