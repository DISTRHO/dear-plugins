#!/usr/bin/make -f
# Makefile for DISTRHO Plugins
# SPDX-License-Identifier: ISC

# --------------------------------------------------------------
# Project name, used for binaries

NAME = TextEditor

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	PluginDSP.cpp

FILES_UI = \
	PluginUI.cpp \
	../../dpf-widgets/opengl/DearImGui.cpp \
	../../dpf-widgets/opengl/DearImGuiColorTextEditor.cpp

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

BUILD_CXX_FLAGS += -DIMGUI_DISABLE_DEBUG_TOOLS
BUILD_CXX_FLAGS += -DIMGUI_DISABLE_DEFAULT_SHELL_FUNCTIONS
BUILD_CXX_FLAGS += -DIMGUI_DISABLE_DEMO_WINDOWS
BUILD_CXX_FLAGS += -I../../dpf-widgets/opengl

# --------------------------------------------------------------
# Enable all possible plugin types

all: au clap dssi jack lv2_sep vst2 vst3

# --------------------------------------------------------------
