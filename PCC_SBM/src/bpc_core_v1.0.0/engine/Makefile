ROOT_DIR = ..

SUBDIRS = \
	tests \

SOURCES += \
	CommonReal.cpp \
	EngineConfiguration.cpp \
	Errno.cpp \
	SelectorFactory.cpp \
	ServerSocketFactory.cpp \
	SocketFactory.cpp \
	ThreadListenerFactory.cpp \
	Truth.cpp \
	WeaverFactory.cpp \
	main.cpp \

TARGET = engine.elf

include $(ROOT_DIR)/Rules.mk

vpath %.cpp $(COMMON_REAL_DIR)
vpath %.cpp $(ENGINE_REAL_DIR)

#
# Local Variables:
# compile-command: "make -C .. "
# tags-file-name: "../TAGS"
# c-basic-offset: 4
# End:
#