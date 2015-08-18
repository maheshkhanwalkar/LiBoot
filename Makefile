.PHONY: image

all:  
	$(MAKE) -C deps/edk2

clean: 
	$(MAKE) -C deps/edk2 clean

image:
	$(MAKE) -C image
