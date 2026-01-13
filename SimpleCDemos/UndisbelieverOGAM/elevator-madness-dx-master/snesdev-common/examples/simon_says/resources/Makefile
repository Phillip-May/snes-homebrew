# Using a custom Makefile as pcx2snes doesn't work like I expected.

# Disable Builtin rules
.SUFFIXES:
.DELETE_ON_ERROR:
MAKEFLAGS += --no-builtin-rules

.PHONY: all
all: snesgss gamefield.map gamefield.clr gamefield.4bpp text.map text.clr text.4bpp

gamefield.map gamefield.clr gamefield.4bpp: gamefield.pcx
	pcx2snes -s8 -c16 -screen gamefield
	mv gamefield.pic gamefield.4bpp

text.map text.clr text.4bpp: text.pcx
	pcx2snes -n -s8 -c16 -o16 text
	mv text.pic text.4bpp


.PHONY: clean
clean:
	$(RM) gamefield.map gamefield.clr gamefield.4bpp

