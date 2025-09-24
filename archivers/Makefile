# ============================================================================
#  Sequential Online Archiving of Objective Vectors (top-level Makefile)
# ============================================================================

PROJECT      := archiver
VERSION      := 1.1
DEBUG        ?= 1
VARIANT      ?= 4

CC           ?= gcc-13
CXX          ?= g++-13
AR           ?= ar
RM           ?= rm -f

BINDIR       := bin
OBJDIR       := build
TARGET       := $(BINDIR)/$(PROJECT)

# --------------------------------------------------------------------------
# Sources
# --------------------------------------------------------------------------
BASE_CPP_SRCS := archiver.cpp Archive.cpp Solution.cpp
BASE_C_SRCS   := mt.c rng.c

SHARED_SRCS  := hv.c avl.c 
SHARED_HDRS  := hv.h avl.h libmoocore-config.h

# --------------------------------------------------------------------------
# Path to libmoocore (symlink inside archiver -> ../c)
# --------------------------------------------------------------------------
LIBMOOCORE_DIR := libmoocore
LIBMOOCORE_SO  := $(LIBMOOCORE_DIR)/libmoocore.so
MOO_OUT        := $(BINDIR)/libmoocore.so

# --------------------------------------------------------------------------
# USE_LIBMOOCORE:
#   0 = use shared lib from libmoocore (default)
#   1 = use bundled .c files from archiver dir
# --------------------------------------------------------------------------
USE_LIBMOOCORE ?= 0

ifeq ($(USE_LIBMOOCORE),0)
    $(info Using shared library from $(LIBMOOCORE_DIR))
    SRCS := $(BASE_CPP_SRCS) $(BASE_C_SRCS)
    CPPFLAGS += -DUSE_SHARED_LIB=1 -DUSE_LIBMOOCORE_HEADERS=1
    CPPFLAGS += -I$(LIBMOOCORE_DIR) -I.
    LDLIBS   += -L$(BINDIR) -lmoocore -Wl,-rpath,$(BINDIR)
    EXTRA_DEPS := $(MOO_OUT)

else ifeq ($(USE_LIBMOOCORE),1)
    $(info Using bundled C sources from archiver dir)
    SRCS := $(BASE_CPP_SRCS) $(BASE_C_SRCS) $(SHARED_SRCS)
    CPPFLAGS += -DUSE_SHARED_LIB=0 -DUSE_LIBMOOCORE_HEADERS=0
    CPPFLAGS += -I. -I$(LIBMOOCORE_DIR)
    EXTRA_DEPS :=

else
    $(error Invalid USE_LIBMOOCORE value: $(USE_LIBMOOCORE))
endif

# --------------------------------------------------------------------------
# Objects (derived automatically from SRCS)
# --------------------------------------------------------------------------
OBJS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(filter %.cpp,$(SRCS))) \
        $(patsubst %.c,$(OBJDIR)/%.o,$(filter %.c,$(SRCS)))

# --------------------------------------------------------------------------
# Compiler flags
# --------------------------------------------------------------------------
CPPFLAGS    += -DVARIANT=$(VARIANT) -DDEBUG=$(DEBUG) -DVERSION='"$(VERSION)"'
CPPFLAGS    += -MMD -MP
COMMON_WARN := -Wall -Wextra -Wpedantic

ifeq ($(DEBUG),0)
  DEBUGFLAGS := -O2
else
  DEBUGFLAGS := -g3 -DMALLOC_CHECK_=3
endif

CFLAGS   += $(DEBUGFLAGS) $(COMMON_WARN)
CXXFLAGS += $(DEBUGFLAGS) $(COMMON_WARN)

# --------------------------------------------------------------------------
# Build rules
# --------------------------------------------------------------------------
.PHONY: all clean help print-sources

all: $(TARGET)

help:
	@echo "Targets: all, clean, print-sources"
	@echo "Vars: DEBUG=1|0 VARIANT=<int> USE_C_CODE=<0|1|2> CC/CXX BINDIR OBJDIR"
	@echo "  USE_C_CODE=0 : shared lib from libmoocore (default)"
	@echo "  USE_C_CODE=1 : compile sources from libmoocore/"
	@echo "  USE_C_CODE=2 : compile bundled hv.c/avl.c from archiver/"

print-sources:
	@echo "SRCS = $(SRCS)"
	@echo "OBJS = $(OBJS)"

$(BINDIR) $(OBJDIR):
	@mkdir -p $@

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling C++ file: $< -> $@"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "Compiling C file: $< -> $@"
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: $(LIBMOOCORE_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling C file from libmoocore: $< -> $@"
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

# Rule to ensure we have a shared library if using mode 0
$(MOO_OUT): | $(BINDIR)
	if [ ! -f "$(LIBMOOCORE_SO)" ]; then \
		echo "libmoocore.so not found. Building in $(LIBMOOCORE_DIR)"; \
		$(MAKE) -C $(LIBMOOCORE_DIR) shlibs || { echo "Failed to build libmoocore"; exit 1; }; \
	fi
	cp $(LIBMOOCORE_SO) $(MOO_OUT)

# Main executable
$(TARGET): $(OBJS) $(EXTRA_DEPS) | $(BINDIR)
	@echo "Linking target: $@"
	$(CXX) $(CXXFLAGS) $^ $(LDLIBS) -o $@

clean:
	@echo "Cleaning build directories: $(BINDIR) and $(OBJDIR)"
	$(RM) -r $(BINDIR) $(OBJDIR)

-include $(OBJS:.o=.d)
