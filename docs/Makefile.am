public: DoxyfilePublic
	$(DOXYGEN) $^

internal: DoxyfileInternal
	$(DOXYGEN) $^

clean-local:
	-rm -rf $(top_srcdir)/docs/public
	-rm -rf $(top_srcdir)/docs/internal

distclean-local:
	-rm DoxyfilePublic DoxyfileInternal Makefile
