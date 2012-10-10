# CXX = arm-none-linux-gnueabi-gcc

TARGET = brain

CXXFLAGS = -O2 -g -Wall -fmessage-length=0 -std=c++0x \
	-DLOGLEVEL=0 \
	-I.

OBJS =	brain.o \
	cfg/Config.o \
	misc/Logger.o \
	spine/Spine.o \
	spine/Eye.o \
	HUD/HUD.o \
	HUD/Window.o \
	client/Handler.o

ifeq ($(ARCH),arm)
LIBS = -L/home/nazgee/Workspace/eclipse/libosock-dev/output-arm/lib
LIBS+= -L/home/nazgee/Workspace/sources/libmodbus/output/lib/
CXX = /home/nazgee/Workspace/angstrom/setup-scripts/build/tmp-angstrom_v2012_05-eglibc/sysroots/x86_64-linux/usr/bin/armv7a-angstrom-linux-gnueabi/arm-angstrom-linux-gnueabi-g++
CXXFLAGS += -I/home/nazgee/Workspace/sources/libmodbus/output/include/
CXXFLAGS += -I/home/nazgee/Workspace/eclipse/libosock-dev/output-arm/include/
else
CXXFLAGS += $(shell pkg-config --cflags libmodbus)
endif

LIBS += -lncursesw -lmodbus -losock-1.0 -lrt -lboost_thread-mt


$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
