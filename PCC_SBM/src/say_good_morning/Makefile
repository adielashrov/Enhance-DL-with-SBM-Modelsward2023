ROOT_DIR = .

SUBDIRS = \
	$(CORE_DIR) \
	src \

all:
	@echo Done

#
# Tags
#

.PHONY: TAGS

FINDARGS = -iname Makefile \
		-or -iname \*.mk \
		-or -iname \*.h \
		-or -iname \*.cpp \
		-or -iname \*.c | \
		$(GREP) -v TestRunner | \
		$(XARGS) $(ETAGS)

TAGS:
	$(GFIND) core $(FINDARGS)
	$(GFIND) src $(FINDARGS) -a
	$(ETAGS) -a Makefile

include $(ROOT_DIR)/Rules.mk

#
# Local Variables:
# compile-command: "make -C . "
# tags-file-name: "./TAGS"
# c-basic-offset: 4
# End:
#