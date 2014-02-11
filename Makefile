include make.env

all clean:
	make -C compat $@
	make -C src $@

