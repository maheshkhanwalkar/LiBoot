SHELL := /bin/bash

.PHONY: image qemu

all:
	$(MAKE) -C deps/edk2 	

clean: 
	$(MAKE) -C deps/edk2 clean
	$(MAKE) -C image clean

image:
	$(MAKE) -C image

qemu: 
	$(MAKE) -C image qemu
