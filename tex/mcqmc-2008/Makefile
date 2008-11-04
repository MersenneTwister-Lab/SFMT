#for GNU make

FILES = mcqmc2008.tex \
	fig-dsfmt.pdf \
	fig-dsfmt.bb \
	ieee.pdf \
	ieee.bb \
	svmult.cls \
	sfmt-kanren.bib \
	mcqmc2008.pdf

.PHONY : zip
zip: $(FILES)
	zip mcqmc2008.zip ${FILES}

clean:
	rm -f *.o *~
