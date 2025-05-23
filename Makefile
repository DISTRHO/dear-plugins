#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# needed by some plugins
export USE_WEB_VIEW = true

include dpf/Makefile.base.mk

all: dgl plugins gen

# --------------------------------------------------------------

dgl:
ifeq ($(HAVE_OPENGL),true)
	$(MAKE) -C dpf/dgl opengl
endif

plugins: dgl
	$(MAKE) all -C plugins/ImGuiDemo
	$(MAKE) all -C plugins/TextEditor
	$(MAKE) all -C plugins/WebBrowser

ifneq ($(CROSS_COMPILING),true)
gen: plugins dpf/utils/lv2_ttl_generator
	@$(CURDIR)/dpf/utils/generate-ttl.sh

dpf/utils/lv2_ttl_generator:
	$(MAKE) -C dpf/utils/lv2-ttl-generator
else
gen:
endif

# --------------------------------------------------------------

clean:
	$(MAKE) clean -C dpf/dgl
	$(MAKE) clean -C dpf/utils/lv2-ttl-generator
	$(MAKE) clean -C plugins/ImGuiDemo
	$(MAKE) clean -C plugins/TextEditor
	$(MAKE) clean -C plugins/WebBrowser
	rm -rf bin build dpf-widgets/opengl/*.d dpf-widgets/opengl/*.o

# --------------------------------------------------------------

.PHONY: plugins
