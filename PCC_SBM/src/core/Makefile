CORE_DIR = ../bpc_core_v1.0.0

ENGINE_DIR 	= $(CORE_DIR)/engine
COMMON_DIR 	= $(CORE_DIR)/common
CORE_TOOLS_DIR	= $(CORE_DIR)/tools
CXXTEST_DIR 	= $(CORE_TOOLS_DIR)/cxxtest

SOURCES = $(wildcard ./*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)


LOCAL_INCLUDES += \
	. \
	$(ENGINE_DIR) \
	$(COMMON_DIR) \
	$(CXXTEST_DIR) \

CCFLAGS += -fPIC

SOURCES = $(wildcard ./*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(OBJECTS)

%.o: %.cpp
	$(CC) $(CCFLAGS) $< -c $(addprefix -I, $(LOCAL_INCLUDES))

clean:
	rm -f *.o *.so *.dylib *.a
