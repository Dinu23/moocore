# ============================================================================
#  mooarchives - Multi-objective Archive Library (libmoocore only)
# ============================================================================

PROJECT	  := mooarchives
VERSION	  := 1.0.0
DEBUG		?= 1
VARIANT	  ?= 4

# Directories
SRCDIR	   := src
INCDIR	   := include
LIBDIR	   := lib
OBJDIR	   := build
EXTERNAL_DIR := external

# libmoocore configuration
LIBMOOCORE_DIR := $(EXTERNAL_DIR)/libmoocore
LIBMOOCORE_LIB := $(LIBMOOCORE_DIR)/libmoocore.so

# Library targets
STATIC_LIB   := $(LIBDIR)/lib$(PROJECT).a
SHARED_LIB   := $(LIBDIR)/lib$(PROJECT).so.$(VERSION)
SHARED_LINK  := $(LIBDIR)/lib$(PROJECT).so

# Installation configuration
PREFIX ?= /usr/local
LIBDIR_INSTALL := $(PREFIX)/lib
INCLUDEDIR_INSTALL := $(PREFIX)/include
PKGCONFIGDIR := $(LIBDIR_INSTALL)/pkgconfig

# --------------------------------------------------------------------------
# Source files
# --------------------------------------------------------------------------

# Find existing source files
EXISTING_CORE_SRCS := $(wildcard $(SRCDIR)/core/*.cpp) $(wildcard $(SRCDIR)/core/*.c)
EXISTING_ARCHIVE_SRCS := $(wildcard $(SRCDIR)/archivers/*.cpp)

# All library sources
LIB_SRCS := $(EXISTING_CORE_SRCS) $(EXISTING_ARCHIVE_SRCS)

# Library objects
LIB_OBJS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(filter %.cpp,$(LIB_SRCS))) \
            $(patsubst %.c,$(OBJDIR)/%.o,$(filter %.c,$(LIB_SRCS)))

# --------------------------------------------------------------------------
# Compiler configuration
# --------------------------------------------------------------------------
CC		   ?= gcc
CXX		  ?= g++
AR		   ?= ar
RM		   := rm -f

# Include paths
CPPFLAGS	 := -I$(INCDIR) \
                -I$(INCDIR)/archiver \
                -I$(INCDIR)/archiver/utils \
                -I$(INCDIR)/archiver/archives \
                -I$(LIBMOOCORE_DIR)

CPPFLAGS	 += -DVERSION='"$(VERSION)"' -DVARIANT=$(VARIANT) -DDEBUG=$(DEBUG)
CPPFLAGS	 += -DUSE_LIBMOOCORE=1

# Linker flags for libmoocore
LDLIBS	   := -L$(LIBMOOCORE_DIR) -lmoocore -Wl,-rpath,$(LIBMOOCORE_DIR) -lm

# Compiler flags (no sanitizers to avoid linking issues)
COMMON_FLAGS := -Wall -Wextra -Wpedantic -fPIC
CFLAGS	   := $(COMMON_FLAGS)
CXXFLAGS	 := $(COMMON_FLAGS) -std=c++11

ifeq ($(DEBUG),0)
CFLAGS   += -O2 -DNDEBUG
CXXFLAGS += -O2 -DNDEBUG
else
CFLAGS   += -g3 -DMALLOC_CHECK_=3
CXXFLAGS += -g3 -DMALLOC_CHECK_=3
endif

# --------------------------------------------------------------------------
# Build rules
# --------------------------------------------------------------------------
.PHONY: all lib static shared clean help config check-sources build-libmoocore install uninstall

all: lib

lib: check-libmoocore check-sources static shared

static: $(STATIC_LIB)

shared: $(SHARED_LIB) $(SHARED_LINK)

# Check what source files exist
check-sources:
    @echo "Checking available source files..."
    @echo "Core sources found:"
    @for src in $(EXISTING_CORE_SRCS); do echo "  ✓ $$src"; done
    @echo "Archive sources found:"
    @for src in $(EXISTING_ARCHIVE_SRCS); do echo "  ✓ $$src"; done
    @echo "Total sources: $(words $(LIB_SRCS))"

# ============================================================================
# Installation targets (libraries and headers only)
# ============================================================================

install: install-libmoocore install-mooarchives install-headers install-pkgconfig
    @echo ""
    @echo "==================================="
    @echo "Installation completed successfully!"
    @echo "==================================="
    @echo ""
    @echo "Libraries and headers installed:"
    @echo "  ✓ libmoocore.so       -> $(LIBDIR_INSTALL)/"
    @echo "  ✓ libmooarchives.*    -> $(LIBDIR_INSTALL)/"
    @echo "  ✓ Headers             -> $(INCLUDEDIR_INSTALL)/"
    @echo "  ✓ pkg-config files    -> $(PKGCONFIGDIR)/"
    @echo ""
    @echo "You can now use in your projects:"
    @echo "  #include <mooarchiver.hpp>          // C++ interface"
    @echo "  #include <hv.h>                     // libmoocore functions"
    @echo ""
    @echo "Compile projects with:"
    @echo "  g++ myfile.cpp \$$(pkg-config --cflags --libs mooarchives)"
    @echo "  g++ myfile.cpp -lmooarchives -lmoocore"
    @echo ""

install-libmoocore: $(LIBMOOCORE_LIB)
    @echo "Installing libmoocore..."
    @mkdir -p $(LIBDIR_INSTALL)
    @cp $(LIBMOOCORE_LIB) $(LIBDIR_INSTALL)/
    @if [ -f "$(LIBMOOCORE_DIR)/libmoocorepp.so" ]; then \
        cp $(LIBMOOCORE_DIR)/libmoocorepp.so $(LIBDIR_INSTALL)/; \
    fi
    @ldconfig 2>/dev/null || echo "Note: ldconfig not run (may need root)"
    @echo "✓ libmoocore installed"

install-mooarchives: $(STATIC_LIB) $(SHARED_LIB)
    @echo "Installing mooarchives libraries..."
    @mkdir -p $(LIBDIR_INSTALL)
    @cp $(STATIC_LIB) $(LIBDIR_INSTALL)/
    @cp $(SHARED_LIB) $(LIBDIR_INSTALL)/lib$(PROJECT).so.$(VERSION)
    @cd $(LIBDIR_INSTALL) && ln -sf lib$(PROJECT).so.$(VERSION) lib$(PROJECT).so.1
    @cd $(LIBDIR_INSTALL) && ln -sf lib$(PROJECT).so.1 lib$(PROJECT).so
    @ldconfig 2>/dev/null || echo "Note: ldconfig not run (may need root)"
    @echo "✓ mooarchives libraries installed"

install-headers:
    @echo "Installing headers..."
    @mkdir -p $(INCLUDEDIR_INSTALL)
    @cp -r $(INCDIR)/* $(INCLUDEDIR_INSTALL)/
    @mkdir -p $(INCLUDEDIR_INSTALL)/moocore
    @cp $(LIBMOOCORE_DIR)/*.h $(INCLUDEDIR_INSTALL)/moocore/ 2>/dev/null || true
    @echo "✓ Headers installed"

install-pkgconfig: install-pkgconfig-moocore install-pkgconfig-mooarchives

install-pkgconfig-moocore:
    @echo "Creating pkg-config for libmoocore..."
    @mkdir -p $(PKGCONFIGDIR)
    @echo "prefix=$(PREFIX)" > $(PKGCONFIGDIR)/moocore.pc
    @echo "exec_prefix=\$${prefix}" >> $(PKGCONFIGDIR)/moocore.pc
    @echo "libdir=\$${exec_prefix}/lib" >> $(PKGCONFIGDIR)/moocore.pc
    @echo "includedir=\$${prefix}/include" >> $(PKGCONFIGDIR)/moocore.pc
    @echo "" >> $(PKGCONFIGDIR)/moocore.pc
    @echo "Name: moocore" >> $(PKGCONFIGDIR)/moocore.pc
    @echo "Description: Multi-Objective Optimization Core Library" >> $(PKGCONFIGDIR)/moocore.pc
    @echo "Version: 1.0.0" >> $(PKGCONFIGDIR)/moocore.pc
    @echo "Libs: -L\$${libdir} -lmoocore -lm" >> $(PKGCONFIGDIR)/moocore.pc
    @echo "Cflags: -I\$${includedir}/moocore" >> $(PKGCONFIGDIR)/moocore.pc
    @echo "✓ moocore.pc created"

install-pkgconfig-mooarchives:
    @echo "Creating pkg-config for mooarchives..."
    @mkdir -p $(PKGCONFIGDIR)
    @echo "prefix=$(PREFIX)" > $(PKGCONFIGDIR)/mooarchives.pc
    @echo "exec_prefix=\$${prefix}" >> $(PKGCONFIGDIR)/mooarchives.pc
    @echo "libdir=\$${exec_prefix}/lib" >> $(PKGCONFIGDIR)/mooarchives.pc
    @echo "includedir=\$${prefix}/include" >> $(PKGCONFIGDIR)/mooarchives.pc
    @echo "" >> $(PKGCONFIGDIR)/mooarchives.pc
    @echo "Name: mooarchives" >> $(PKGCONFIGDIR)/mooarchives.pc
    @echo "Description: Multi-Objective Archive Library" >> $(PKGCONFIGDIR)/mooarchives.pc
    @echo "Version: $(VERSION)" >> $(PKGCONFIGDIR)/mooarchives.pc
    @echo "Requires: moocore" >> $(PKGCONFIGDIR)/mooarchives.pc
    @echo "Libs: -L\$${libdir} -lmooarchives" >> $(PKGCONFIGDIR)/mooarchives.pc
    @echo "Cflags: -I\$${includedir}" >> $(PKGCONFIGDIR)/mooarchives.pc
    @echo "✓ mooarchives.pc created"

# User-local installation (no sudo required)
install-user: 
    @echo "Installing libraries and headers to user directory..."
    @$(MAKE) install PREFIX=$$HOME/.local
    @echo ""
    @echo "Add to your ~/.bashrc:"
    @echo "export PKG_CONFIG_PATH=\"\$$HOME/.local/lib/pkgconfig:\$$PKG_CONFIG_PATH\""
    @echo ""
    @echo "Then reload: source ~/.bashrc"

uninstall:
    @echo "Uninstalling mooarchives and moocore libraries..."
    @rm -f $(LIBDIR_INSTALL)/libmooarchives.*
    @rm -f $(LIBDIR_INSTALL)/libmoocore.*
    @rm -f $(LIBDIR_INSTALL)/libmoocorepp.*
    @rm -rf $(INCLUDEDIR_INSTALL)/archiver
    @rm -rf $(INCLUDEDIR_INSTALL)/moocore
    @rm -f $(INCLUDEDIR_INSTALL)/mooarchiver.hpp
    @rm -f $(PKGCONFIGDIR)/mooarchives.pc
    @rm -f $(PKGCONFIGDIR)/moocore.pc
    @ldconfig 2>/dev/null || echo "Note: ldconfig not run (may need root)"
    @echo "✓ Uninstall complete"

help:
    @echo "mooarchives - Multi-objective Archive Library"
    @echo ""
    @echo "Build targets:"
    @echo "  all               - Build library (default)"
    @echo "  static            - Build static library"
    @echo "  shared            - Build shared library"
    @echo ""
    @echo "Installation targets:"
    @echo "  install           - Install system-wide (requires sudo)"
    @echo "  install-user      - Install to ~/.local (no sudo needed)"
    @echo "  uninstall         - Remove installed files"
    @echo ""
    @echo "Utility targets:"
    @echo "  build-libmoocore  - Build libmoocore shared library"
    @echo "  clean             - Clean build files"
    @echo "  clean-all         - Clean including libmoocore"
    @echo "  config            - Show configuration"
    @echo "  check-sources     - List found source files"
    @echo ""
    @echo "Usage examples:"
    @echo "  make                     # Build libraries"
    @echo "  sudo make install       # Install libraries system-wide"
    @echo "  make install-user       # Install to user directory"

# Check and build libmoocore if needed
check-libmoocore: $(LIBMOOCORE_LIB)

$(LIBMOOCORE_LIB): build-libmoocore

build-libmoocore:
    @echo "Checking libmoocore..."
    @if [ ! -e "$(LIBMOOCORE_DIR)" ]; then \
        echo "Error: libmoocore directory not found at $(LIBMOOCORE_DIR)"; \
        echo "Please ensure the libmoocore submodule is properly initialized"; \
        exit 1; \
    fi
    @if [ ! -f "$(LIBMOOCORE_DIR)/hv.h" ]; then \
        echo "Error: libmoocore headers not found"; \
        exit 1; \
    fi
    @if [ ! -f "$(LIBMOOCORE_LIB)" ]; then \
        echo "Building libmoocore shared library (without sanitizers)..."; \
        cd $(LIBMOOCORE_DIR) && $(MAKE) clean; \
        cd $(LIBMOOCORE_DIR) && $(MAKE) shlibs CC=$(CC) CXX=$(CXX) \
            DEBUG=0 SANITIZERS="" OPT_CFLAGS="-O2 -DNDEBUG" \
            CFLAGS="-fPIC" CXXFLAGS="-fPIC"; \
        if [ ! -f "$(LIBMOOCORE_LIB)" ]; then \
            echo "shlibs target failed, trying alternative approach..."; \
            cd $(LIBMOOCORE_DIR) && $(MAKE) $(SHLIB) CC=$(CC) CXX=$(CXX) \
                DEBUG=0 SANITIZERS="" OPT_CFLAGS="-O2 -DNDEBUG" \
                CFLAGS="-fPIC" CXXFLAGS="-fPIC"; \
        fi; \
        if [ ! -f "$(LIBMOOCORE_LIB)" ]; then \
            echo "Error: Failed to build libmoocore shared library"; \
            echo "Available files in $(LIBMOOCORE_DIR):"; \
            ls -la $(LIBMOOCORE_DIR)/lib*.so* || ls -la $(LIBMOOCORE_DIR)/; \
            exit 1; \
        fi; \
        echo "✓ libmoocore.so built successfully"; \
    else \
        echo "✓ libmoocore.so already exists"; \
    fi

# Object file compilation
$(OBJDIR)/%.o: %.cpp $(LIBMOOCORE_LIB)
    @mkdir -p $(dir $@)
    @echo "Compiling C++: $< -> $@"
    $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: %.c $(LIBMOOCORE_LIB)
    @mkdir -p $(dir $@)
    @echo "Compiling C: $< -> $@"
    $(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

# Static library
$(STATIC_LIB): $(LIB_OBJS)
    @echo "Creating directory: $(LIBDIR)"
    @mkdir -p $(LIBDIR)
    @echo "Creating static library: $@"
    @if [ -z "$(LIB_OBJS)" ]; then \
        echo "Error: No object files to create library"; \
        exit 1; \
    fi
    @echo "Objects to include: $(LIB_OBJS)"
    $(AR) rcs $@ $(LIB_OBJS)
    @echo "✓ Static library created successfully"

# Shared library
$(SHARED_LIB): $(LIB_OBJS) $(LIBMOOCORE_LIB)
    @echo "Creating directory: $(LIBDIR)"
    @mkdir -p $(LIBDIR)
    @echo "Creating shared library: $@"
    @echo "Using linker flags: $(LDLIBS)"
    $(CXX) $(CXXFLAGS) -shared -Wl,-soname,lib$(PROJECT).so.1 \
        -o $@ $(LIB_OBJS) $(LDLIBS)
    @echo "✓ Shared library created successfully"

$(SHARED_LINK): $(SHARED_LIB)
    @cd $(LIBDIR) && ln -sf lib$(PROJECT).so.$(VERSION) lib$(PROJECT).so
    @echo "✓ Shared library symlink created"

# Configuration display
config:
    @echo "Configuration:"
    @echo "  PROJECT=$(PROJECT)"
    @echo "  VERSION=$(VERSION)"
    @echo "  DEBUG=$(DEBUG)"
    @echo "  LIBMOOCORE_DIR=$(LIBMOOCORE_DIR)"
    @echo ""
    @echo "Directories:"
    @echo "  LIBDIR=$(LIBDIR)"
    @echo "  OBJDIR=$(OBJDIR)"
    @echo ""
    @echo "Targets:"
    @echo "  STATIC_LIB=$(STATIC_LIB)"
    @echo "  SHARED_LIB=$(SHARED_LIB)"
    @echo ""
    @echo "Installation paths:"
    @echo "  PREFIX=$(PREFIX)"
    @echo "  LIBDIR_INSTALL=$(LIBDIR_INSTALL)"
    @echo "  INCLUDEDIR_INSTALL=$(INCLUDEDIR_INSTALL)"
    @echo ""
    @echo "Found sources: $(words $(LIB_SRCS))"
    @echo "Generated objects: $(words $(LIB_OBJS))"

# Force rebuild libmoocore without sanitizers
rebuild-libmoocore:
    @echo "Force rebuilding libmoocore without sanitizers..."
    cd $(LIBMOOCORE_DIR) && $(MAKE) clean
    cd $(LIBMOOCORE_DIR) && $(MAKE) shlibs CC=$(CC) CXX=$(CXX) \
        DEBUG=0 SANITIZERS="" OPT_CFLAGS="-O2 -DNDEBUG" \
        MARCH_FLAGS="" WARN_CFLAGS="" \
        CFLAGS="-fPIC" CXXFLAGS="-fPIC"
    @echo "✓ libmoocore rebuilt successfully"

# Cleanup
clean:
    @echo "Cleaning build files..."
    $(RM) -r $(OBJDIR) $(LIBDIR)
    @echo "✓ Clean complete"

# Clean everything including libmoocore
clean-all: clean
    @echo "Cleaning libmoocore..."
    cd $(LIBMOOCORE_DIR) && $(MAKE) clean 2>/dev/null || true
    @echo "✓ Full clean complete"

# Dependencies
-include $(LIB_OBJS:.o=.d)