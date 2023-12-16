################################################################################
# Makefile for blob-study
# Author: J. Ian Lindsay
# Date:   2023.12.16
#
# This makefile builds an X11 program for visualizing binary data.
################################################################################

# Build configuration
EXECUTABLE_NAME    = blob-study
CXX_STANDARD       = gnu++17
OPTIMIZATION       = -O2

CONFIG_C3P_CRYPTO = mbedtls
C3P_CONF  = -DCONFIG_C3P_CBOR
C3P_CONF += -DCONFIG_C3P_IMG_SUPPORT


###########################################################################
# Environmental awareness...
###########################################################################
SHELL              = /bin/bash
export CXX         = $(shell which g++)
export AR          = $(shell which ar)
export SZ          = $(shell which size)
export MAKE        = $(shell which make)
export BUILD_ROOT  = $(shell pwd)
export OUTPUT_PATH = $(BUILD_ROOT)/build/


###########################################################################
# Source files, includes, and linker directives...
###########################################################################
# TODO: C3P really needs its own portable Makefile...
INCLUDES  = -Ilib/CppPotpourri/src -Ilib/Platform -Ilib -Isrc

CXX_SRCS  = src/*.cpp
CXX_SRCS += lib/CppPotpourri/src/*.cpp
CXX_SRCS += lib/CppPotpourri/src/TimerTools/*.cpp
CXX_SRCS += lib/CppPotpourri/src/Identity/*.cpp
CXX_SRCS += lib/CppPotpourri/src/cbor-cpp/*.cpp
CXX_SRCS += lib/CppPotpourri/src/C3PValue/*.cpp
CXX_SRCS += lib/CppPotpourri/src/Pipes/BufferAccepter/*.cpp
CXX_SRCS += lib/CppPotpourri/src/Storage/*.cpp
CXX_SRCS += lib/CppPotpourri/src/Storage/RecordTypes/*.cpp
CXX_SRCS += lib/CppPotpourri/src/Image/*.cpp
CXX_SRCS += lib/CppPotpourri/src/Image/GfxUI/*.cpp
CXX_SRCS += lib/CppPotpourri/src/Image/ImageUtils/*.cpp
CXX_SRCS += lib/Platform/src/GUI/X11/*.cpp
CXX_SRCS += lib/Platform/src/Linux.cpp
CXX_SRCS += lib/Platform/src/LinuxStdIO.cpp
#CXX_SRCS += lib/ManuvrPlatforms/src/LinuxStorage.cpp

# Libraries to link against.
LIBS	= -L$(OUTPUT_PATH) -lstdc++ -lm -lpthread -lX11


###########################################################################
# Paramter packaging and error-checking...
###########################################################################
CXXFLAGS  = $(OPTIMIZATION) -std=$(CXX_STANDARD)
CXXFLAGS += -Wl,--gc-sections
CXXFLAGS += -fsingle-precision-constant -fno-rtti -fno-exceptions
CXXFLAGS += -Wdouble-promotion -Wall
CXXFLAGS += $(INCLUDES)
CXXFLAGS += $(C3P_CONF)

OBJS = $(C_SRCS:.c=.o) $(CXX_SRCS:.cpp=.o)


###########################################################################
# Rules for building the firmware follow...
###########################################################################

default:	$(EXECUTABLE_NAME)

builddir:
	mkdir -p $(OUTPUT_PATH)

debug:  $(EXECUTABLE_NAME).o
	$(CXX) $(CXXFLAGS) -ggdb -g -pg -o $(EXECUTABLE_NAME) *.o $(LIBS)

$(EXECUTABLE_NAME):	builddir $(EXECUTABLE_NAME).o $(LIBS_TO_BUILD)
	$(CXX) $(CXXFLAGS) -o $(EXECUTABLE_NAME) *.o $(LIBS)

$(EXECUTABLE_NAME).o: builddir
	$(CXX) $(CXXFLAGS) $(CFLAGS) -c $(CXX_SRCS) -fno-exceptions

clean:
	rm -rf $(OUTPUT_PATH)
	rm -f $(EXECUTABLE_NAME) *.o *~
