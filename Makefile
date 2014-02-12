include make.env

all clean:
	cd compat && make $@
	cd src && make $@

