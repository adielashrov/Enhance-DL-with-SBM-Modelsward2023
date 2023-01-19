#
# Places
#

TOOLS_DIR	= $(ROOT_DIR)/tools
CXXTEST_DIR 	= $(TOOLS_DIR)/cxxtest

COMMON_DIR 	= $(ROOT_DIR)/common
COMMON_MOCK_DIR = $(COMMON_DIR)/Mock
COMMON_REAL_DIR = $(COMMON_DIR)/Real
COMMON_TEST_DIR = $(COMMON_DIR)/tests

ENGINE_DIR 	= $(ROOT_DIR)/engine
ENGINE_MOCK_DIR = $(ENGINE_DIR)/Mock
ENGINE_REAL_DIR = $(ENGINE_DIR)/Real
ENGINE_TEST_DIR = $(ENGINE_DIR)/tests


PCC_DIR_BIN = $(ROOT_DIR)/../pcc/monitor_interval

PCC_OBJECTS_FROM_BIN += \
	$(PCC_DIR_BIN)/pcc_mi.o

# $(info PCC_DIR_BIN = $(PCC_DIR_BIN))
# $(info PCC_OBJECTS_FROM_BIN = $(PCC_OBJECTS_FROM_BIN))

# FILES = $(shell ls $(PCC_OBJECTS_FROM_BIN))
# $(info FILES = $(FILES))


#
# Utilities
#

COMPILE = g++
LINK 	= g++
RM	= rm
GFIND 	= find
ETAGS	= etags
GREP 	= grep
XARGS	= xargs

PERL 	= perl
TESTGEN = $(CXXTEST_DIR)/cxxtestgen.pl
TAR	= tar
UNZIP	= unzip

#
# Unzipping
#

%.unzipped: %.tar.bz2
	$(TAR) xjvf $<
	touch $@

%.unzipped: %.zip
	$(UNZIP) $<
	touch $@

#
# Compiling C/C++
#

LOCAL_INCLUDES += \
	. \
	$(COMMON_DIR) \
	$(CXXTEST_DIR) \

CFLAGS += \
	-MMD \
	-Wall \
	-Wextra \
	-Wno-narrowing \
	-Wno-ignored-qualifiers \
	-std=c++11\
	-g \
	-O0 \
	-ggdb3 \


%.obj: %.cpp
	$(COMPILE) -c -o $@ $< $(CFLAGS) $(addprefix -I, $(LOCAL_INCLUDES))

%.obj: %.cxx
	$(COMPILE) -c -o $@ $< $(CFLAGS) $(addprefix -I, $(LOCAL_INCLUDES))

#
# Linking C/C++
#

SYSTEM_LIBRARIES += \
	pthread

#
# Compiling Elf Files
#

ifneq ($(TARGET),)

DEPS = $(SOURCES:%.cpp=%.d)

OBJECTS = $(SOURCES:%.cpp=%.obj)

%.elf: $(OBJECTS) 
	$(LINK) -o $@ $^ $(PCC_OBJECTS_FROM_BIN) $(addprefix -l, $(SYSTEM_LIBRARIES))

.PRECIOUS: %.obj

endif

#
# Cxx Rules
#

ifneq ($(TEST_TARGET),)

TEST_FILES = $(wildcard Test_*.h)
OBJECTS = $(SOURCES:%.cpp=%.obj)

CXX_SOURCES = $(addprefix cxxtest_, $(TEST_FILES:%.h=%.cxx))
TEST_OBJECTS = $(CXX_SOURCES:%.cxx=%.obj) $(OBJECTS) runner.obj

DEPS = $(SOURCES:%.cpp=%.d) $(CXX_SOURCES:%.cxx=%.d)

cxxtest_%.cxx: %.h
	$(TESTGEN) --part --have-eh --abort-on-fail -o $@ $^

runner.cxx:
	$(TESTGEN) --root --have-eh --abort-on-fail --error-printer -o $@

%.tests: $(TEST_OBJECTS)
	$(LINK) -o $@ $^ $(PCC_OBJECTS_FROM_BIN) $(addprefix -l, $(SYSTEM_LIBRARIES))

.PRECIOUS: %.cxx %.obj

endif

#
# Recursive Make
#

all: $(SUBDIRS:%=%.all) $(TARGET) $(TEST_TARGET)

clean: $(SUBDIRS:%=%.clean) clean_directory

%.all:
	$(MAKE) -C $* all

%.clean:
	$(MAKE) -C $* clean

clean_directory:
	$(RM) -f *~ *.cxx *.obj *.d $(TARGET) $(TEST_TARGET)

ifneq ($(DEPS),)
-include $(DEPS)
endif

#
# Local Variables:
# compile-command: "make -C . "
# tags-file-name: "./TAGS"
# c-basic-offset: 4
# End:
#
