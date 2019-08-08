.SUFFIXES: .m4 .html

.m4.html:
	m4 -P $*.m4 >$*.html
	chmod 644 $*.html

HTML_FILES=using_m4.html

default: $(HTML_FILES)

all: default 

*.html: stdlib.m4

clean:
	rm -rf *~ $(HTML_FILES)

