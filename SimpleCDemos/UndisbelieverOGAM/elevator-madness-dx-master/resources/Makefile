
# MetaSprite tiles
TILES = $(patsubst %.pcx,%.4bpp,$(wildcard metasprites/*.pcx))
PALETTES =  $(patsubst %.4bpp,%.clr,$(TILES))


.PHONY: ALL

ALL: $(TILES) $(PALETTES)

%.4bpp %.clr: %.pcx
	pcx2snes -n -s8 -c16 -o16 $*
	mv $*.pic $*.4bpp

.PHONY: clean
clean:
	$(RM)  $(TILES) $(PALETTES)


