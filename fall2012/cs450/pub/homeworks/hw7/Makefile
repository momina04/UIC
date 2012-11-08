all: collect plot pdf

pdf:
	pdflatex hw7.tex
	pdflatex hw7.tex
# twice to get references right the first time

plot:
	scripts/post-process
	scripts/onewayplot

collect:
	scripts/collect

clean:
	rm -f logs/* data/raw.txt data/post-processed.txt *.aux *.pdf *.ps *.log
