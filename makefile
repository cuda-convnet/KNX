#compiles entire KNX project

SUBDIRS = Src/KDK/ Src/DataManager/ Src/Executor/ Src/ModuleManager/ Src/Parser/ Src/RTE/
BITVRS=32
BINPATH=$(shell pwd)/bin

KLIB=-I../../../KNX_Libraries/headers -L../../../KNX_Libraries/_bin/$(BITVRS) -lKNX_Library

export BITVRS
export BINPATH
export KLIB

subdirs:
	for dir in $(SUBDIRS); do \
	 $(MAKE) -C $$dir; \
	done

.PHONY: run
run:
	bin/KNX

.PHONY: dbg
dbg:
	bin/KNX -d -v

.PHONY: clean
clean:
	for dir in $(SUBDIRS); do \
	 $(MAKE) clean -C $$dir; \
	done
	 rm -f $@ bin/KNX
	 rm -f $@ bin/*.so