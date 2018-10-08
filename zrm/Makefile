# Needs LaTex 2.09

LATEX = pdflatex

all: zrm.pdf

zrm.pdf: zrm.tex
	echo \\relax >glossary.tex
	echo \\relax >symdex.tex
	echo \\relax >index.tex
	$(LATEX) zrm
	tools/index  zrm.idx index.tex
	tools/symdex zrm.sdx symdex.tex
	tools/gloss  zrm.glo glossary.tex
	$(LATEX) zrm
	$(LATEX) zrm

clean:
	rm -f zrm.aux zrm.pdf zrm.glo zrm.idx zrm.log zrm.sdx zrm.toc
