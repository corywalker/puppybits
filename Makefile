
SUBDIRS=tools lib boot src/*/

all clean:
	for d in $(SUBDIRS) ; do ( cd $$d ; $(MAKE) $@ ) ; done

eclean: clean
	-find . -name '*~' -print0 | xargs -0 rm
