export CC = g++

# Compile with gdb flags
export CCFLAGS = -g -std=c++0x -Wall -O0 -ggdb3

# Compile with release
#export CCFLAGS = -std=c++0x -Wall

export LDFLAGS = -Lcore -L/usr/lib/python3.7/config-3.7m-x86_64-linux-gnu/ -lpython3.7 -lstdc++ -lpthread -lm

LIB_SOURCES = $(wildcard core/*.cpp)
LIB_SOURCES += $(wildcard pcc/monitor_interval/*.cpp)
LIB_SOURCES += $(wildcard pcc/rate_control/*.cpp)
LIB_SOURCES += $(wildcard pcc/utility/*.cpp)
LIB_SOURCES += $(wildcard pcc/*.cpp)
LIB_OBJECTS = $(LIB_SOURCES:.cpp=.o)

BP_OBJECTS += \
	bpc_core_v1.0.0/engine/CommonReal.obj \
	bpc_core_v1.0.0/engine/EngineConfiguration.obj \
	bpc_core_v1.0.0/engine/Errno.obj \
	bpc_core_v1.0.0/engine/SelectorFactory.obj \
	bpc_core_v1.0.0/engine/ServerSocketFactory.obj \
	bpc_core_v1.0.0/engine/SocketFactory.obj \
	bpc_core_v1.0.0/engine/ThreadListenerFactory.obj \
	bpc_core_v1.0.0/engine/Truth.obj \
	bpc_core_v1.0.0/engine/WeaverFactory.obj \
	say_good_morning/src/EngineFactory.obj \


all:
	$(MAKE) -C pcc/monitor_interval all
	$(MAKE) -C pcc/rate_control all
	$(MAKE) -C pcc/utility all
	$(MAKE) -C pcc all
	$(MAKE) -C core all
	$(MAKE) -C app all
	$(CC) $(LIB_OBJECTS) $(CCFLAGS) -shared -o core/libudt.so $(LDFLAGS)
	# $(CC) app/pccserver.o $(CCFLAGS) -o app/pccserver $(LDFLAGS) -ludt
	# $(CC) app/pccclient.o $(CCFLAGS) -o app/pccclient $(LDFLAGS) -ludt
	$(CC) $(CCFLAGS) -o app/pccserver app/pccserver.o $(BP_OBJECTS) $(LDFLAGS) -ludt
	$(CC) $(CCFLAGS) -o app/pccclient app/pccclient.o $(BP_OBJECTS) $(LDFLAGS) -ludt

clean:
	$(MAKE) -C pcc/monitor_interval clean
	$(MAKE) -C pcc/rate_control clean
	$(MAKE) -C pcc/utility clean
	$(MAKE) -C pcc clean
	$(MAKE) -C core clean
	$(MAKE) -C app clean
