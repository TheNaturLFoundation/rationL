if HAVE_DOXYGEN
directory = $(top_srcdir)/docs/man/man3

man_MANS = $(directory)

$(directory): doxyfile.stamp

#TODO add an if pdflatex to build latex

doxyfile.stamp: Doxyfile
	$(DOXYGEN) $^
	echo Timestamp > $@

CLEANFILES = doxyfile.stamp

all-local: doxyfile.stamp
clean-local:
	-rm -rf Doxyfile Makefile
	-rm -rf $(top_srcdir)/docs/man
	-rm -rf $(top_srcdir)/docs/html
	-rm -rf $(top_srcdir)/docs/latex
endif
