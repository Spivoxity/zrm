TARGETS = zrm.ps zrm.ps.gz zrm.pdf zrm.tex.gz zrm.sty.gz

all: $(TARGETS)

# Don't reduce for CRC or printing on A4/letter
MAG = 1000

# Reduce by 1/1.2 = 0.83333 for final-size printing
# MAG = 833

zrm.dvi: zrm.sty

clean:
	rm -f zrm.dvi zrm.log $(TARGETS) tmp.ps missfont.log

%.pdf: %.dvi
	dvips -Ppdf -t a4 -x $(MAG) -o tmp.ps $<
	ps2pdf tmp.ps
	mv tmp.pdf $@
	@rm tmp.ps

%.gz: %
	gzip -c $< >$@

%.dvi: %.tex
	latex $<

%.ps: %.dvi
	dvips -x $(MAG) -o $@ $<
