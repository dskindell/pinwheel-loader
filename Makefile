ifeq ($(MAKEFILE_DIR),)
  ifeq ($(MAKEFILE_LIST),)
    $(warning MAKEFILE_LIST was empty!)
    export MAKEFILE_DIR := $(CURDIR)
  else
    export MAKEFILE_DIR := $(dir $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))
  endif
endif
export PROJECT_DIR   ?= $(realpath $(MAKEFILE_DIR))

PROJECT_NAME  := pinwheel-loader
$(PROJECT_NAME).interface_version = 0
$(PROJECT_NAME).major_version     = 0
$(PROJECT_NAME).minor_version     = 1
$(PROJECT_NAME).version           = $($(PROJECT_NAME).interface_version).$($(PROJECT_NAME).major_version).$($(PROJECT_NAME).minor_version)

CONFIGURATION            := $(if $(subst 0,,$(ndb)),Release,Debug)
PLATFORM                 ?= x64
BIN_DIRECTORY            := $(PROJECT_DIR)/bin
LIB_DIRECTORY            := $(PROJECT_DIR)/lib
SCRATCH_DIRECTORY        := $(PROJECT_DIR)/.scratch
CONFIG_BIN_DIRECTORY     := $(BIN_DIRECTORY)/$(CONFIGURATION)/$(PLATFORM)
CONFIG_LIB_DIRECTORY     := $(LIB_DIRECTORY)/$(CONFIGURATION)/$(PLATFORM)
CONFIG_SCRATCH_DIRECTORY := $(SCRATCH_DIRECTORY)/$(CONFIGURATION)/$(PLATFORM)

DEFAULT_GOAL := $(PROJECT_NAME)

$(PROJECT_NAME).pkg    := $(CONFIG_LIB_DIRECTORY)/$(PROJECT_NAME)-$($(PROJECT_NAME).version).$(PLATFORM).tar.gz
$(PROJECT_NAME).lib    := $(CONFIG_LIB_DIRECTORY)/lib$(PROJECT_NAME).a
$(PROJECT_NAME).so     := $(CONFIG_LIB_DIRECTORY)/lib$(PROJECT_NAME).so.$($(PROJECT_NAME).version)
$(PROJECT_NAME).soname := lib$(PROJECT_NAME).so.$($(PROJECT_NAME).interface_version)
$(PROJECT_NAME).objs   := $(patsubst %.c,$(CONFIG_SCRATCH_DIRECTORY)/%.o,$(wildcard src/*.c))
$(PROJECT_NAME).deps   := $(patsubst %.o,%.d,$($(PROJECT_NAME).objs))

LDFLAGS += -pthread
CFLAGS  += -Wall -Werror -fPIC -I$(PROJECT_DIR)/src
ifeq ($(CONFIGURATION),Release)
CFLAGS  += -DNDEBUG -O2
else
CFLAGS  += -D_DEBUG -O0
CFLAGS  += -g -fno-inline
endif

ifeq ($(PLATFORM),x64)
CFLAGS  += -m64
LDFLAGS += -m64
else
CFLAGS  += -m32
LDFLAGS += -m32
endif

$(PROJECT_NAME): $(PROJECT_NAME).lib $(PROJECT_NAME).so $(PROJECT_NAME).pkg

$(PROJECT_NAME).pkg: $($(PROJECT_NAME).pkg)

$(PROJECT_NAME).lib: $($(PROJECT_NAME).lib)

$(PROJECT_NAME).so: $($(PROJECT_NAME).so)

$($(PROJECT_NAME).pkg): $($(PROJECT_NAME).lib) $($(PROJECT_NAME).so)
	-rm -rf $(CONFIG_SCRATCH_DIRECTORY)/$(PROJECT_NAME)-$($(PROJECT_NAME).version)
	mkdir -p $(CONFIG_SCRATCH_DIRECTORY)/$(PROJECT_NAME)-$($(PROJECT_NAME).version)/docs
	cp -f $(PROJECT_DIR)/docs/changelog.txt $(CONFIG_SCRATCH_DIRECTORY)/$(PROJECT_NAME)-$($(PROJECT_NAME).version)/docs/
	cp -f $(PROJECT_DIR)/README.md $(CONFIG_SCRATCH_DIRECTORY)/$(PROJECT_NAME)-$($(PROJECT_NAME).version)/docs/
	mkdir -p $(CONFIG_SCRATCH_DIRECTORY)/$(PROJECT_NAME)-$($(PROJECT_NAME).version)/lib
	cp -f $(CONFIG_LIB_DIRECTORY)/* $(CONFIG_SCRATCH_DIRECTORY)/$(PROJECT_NAME)-$($(PROJECT_NAME).version)/lib/
	mkdir -p $(CONFIG_SCRATCH_DIRECTORY)/$(PROJECT_NAME)-$($(PROJECT_NAME).version)/include
	cp -f $(PROJECT_DIR)/src/*.h $(CONFIG_SCRATCH_DIRECTORY)/$(PROJECT_NAME)-$($(PROJECT_NAME).version)/include/
	tar -cz -C $(CONFIG_SCRATCH_DIRECTORY) $(patsubst %,--exclude '%' ,.svn *.user *.suo *.opensdf *.sdf) -f $@ \
	  $(PROJECT_NAME)-$($(PROJECT_NAME).version)

$($(PROJECT_NAME).lib): $($(PROJECT_NAME).objs)
	[ -d $(@D) ] || mkdir -p $(@D)
	$(AR) rc $@ $(filter %.o,$^)

$($(PROJECT_NAME).so): $($(PROJECT_NAME).objs)
	[ -d $(@D) ] || mkdir -p $(@D)
	$(CC) -shared -Wl,-soname,$($(PROJECT_NAME).soname) \
	  -o $@ $(LDFLAGS)
	ln -f -s $(@F) $(@D)/$($(PROJECT_NAME).soname)

$($(PROJECT_NAME).objs): $(CONFIG_SCRATCH_DIRECTORY)/%.o: %.c
	[ -d $(@D) ] || mkdir -p $(@D)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c $< -o $@

$($(PROJECT_NAME).deps): $(CONFIG_SCRATCH_DIRECTORY)/%.d: %.c
	[ -d $(@D) ] || mkdir -p $(@D)
	$(CPP) -MM -MG -MT "$(@D)/$(@F) $(CONFIG_SCRATCH_DIRECTORY)/$*.o" \
	  $(CPPFLAGS) $< -o $@

ifeq ($(strip $(foreach t,clean,$(findstring $(t),$(MAKECMDGOALS)))),)
include $($(PROJECT_NAME).deps)
endif

$($(PROJECT_NAME).pkg) \
$($(PROJECT_NAME).lib) \
$($(PROJECT_NAME).so) \
$($(PROJECT_NAME).objs) \
$($(PROJECT_NAME).deps): $(PROJECT_DIR)/Makefile

clean:
	rm -rf $(BIN_DIRECTORY) $(LIB_DIRECTORY) $(SCRATCH_DIRECTORY)
