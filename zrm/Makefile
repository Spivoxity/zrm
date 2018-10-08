all: zrm.dvi

zrm.dvi: zrm.tex
	echo \\relax >glossary.tex
	echo \\relax >symdex.tex
	echo \\relax >index.tex
	latex zrm
	tools/index  zrm.idx index.tex
	tools/symdex zrm.sdx symdex.tex
	tools/gloss  zrm.glo glossary.tex
	latex zrm
	latex zrm

clean:
	rm -f zrm.{aux,dvi,glo,idx,log,sdx,toc}
