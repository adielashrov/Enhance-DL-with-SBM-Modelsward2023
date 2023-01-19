ROOT_DIR = ..

LOCAL_INCLUDES += \
	$(ENGINE_DIR) \

SOURCES += \
	CommonReal.cpp \
	EngineConfiguration.cpp \
	EngineFactory.cpp \
	Errno.cpp \
	SelectorFactory.cpp \
	ServerSocketFactory.cpp \
	SocketFactory.cpp \
	ThreadListenerFactory.cpp \
	Truth.cpp \
	WeaverFactory.cpp \
	main.cpp \

TARGET = sayGoodMorning.elf

include $(ROOT_DIR)/Rules.mk

vpath %.cpp $(COMMON_REAL_DIR)
vpath %.cpp $(ENGINE_REAL_DIR)
vpath %.cpp $(ENGINE_DIR)

#
# Local Variables:
# compile-command: "make -C .. "
# tags-file-name: "../TAGS"
# c-basic-offset: 4
# End:
#