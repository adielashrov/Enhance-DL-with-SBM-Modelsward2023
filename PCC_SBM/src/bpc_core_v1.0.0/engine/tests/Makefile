ROOT_DIR = ../..

TEST_TARGET = EngineTests.tests

LOCAL_INCLUDES += \
	.. \
	$(COMMON_TEST_DIR) \

SOURCES += \
	CommonMock.cpp \
	EngineConfiguration.cpp \
	EngineFactory.cpp \
	Errno.cpp \
	ProcessRunnerFactory.cpp \
	SelectorFactory.cpp \
	ServerSocketFactory.cpp \
	SocketFactory.cpp \
	TestLog.cpp \
	ThreadListenerFactory.cpp \
	Truth.cpp \
	WeaverFactory.cpp \

all:
	./$(TEST_TARGET)

include $(ROOT_DIR)/Rules.mk

vpath %.cpp $(COMMON_MOCK_DIR)
vpath %.cpp $(COMMON_TEST_DIR)
vpath %.cpp $(ENGINE_DIR)
vpath %.cpp $(ENGINE_MOCK_DIR)

#
# Local Variables:
# compile-command: "make -C ../.. "
# tags-file-name: "../../TAGS"
# c-basic-offset: 4
# End:
#