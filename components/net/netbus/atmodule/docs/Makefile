SPHINXBUILD ?= sphinx-build
LANGUAGE ?= en
SOURCEDIR = $(LANGUAGE)
BUILDDIR = build/$(LANGUAGE)

help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" -c . $(SPHINXOPTS)

.PHONY: help Makefile
%: Makefile
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" -c . -D language=$(LANGUAGE) $(SPHINXOPTS)
