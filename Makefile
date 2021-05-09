#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(PSL1GHT)),)
$(error "Please set PSL1GHT in your environment. export PSL1GHT=<path>")
endif

ICON0		:=	$(CURDIR)/../pkg/ICON0.PNG
SFOXML		:=	$(CURDIR)/sfo.xml

SCETOOL_FLAGS	?=	--self-app-version=0001000000000000  --sce-type=SELF --compress-data=TRUE --self-add-shdrs=TRUE --skip-sections=FALSE --key-revision=1 \
					--self-auth-id=1010000001000003 --self-vendor-id=01000002 --self-fw-version=0003004000000000 \
					--self-ctrl-flags 4000000000000000000000000000000000000000000000000000000000000002 \
					--self-cap-flags 00000000000000000000000000000000000000000000007B0000000100000000

include $(PSL1GHT)/ppu_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET		:=	$(notdir $(CURDIR))
BUILD		:=	build
SOURCES		:=	source source/ftp
DATA		:=	data
SHADERS		:=	shaders
INCLUDES	:=	include
PKGFILES	:=	$(CURDIR)/../pkg

TITLE		:=	FB NEO RL Plus
APPID		:=	FBNE00123
CONTENTID	:=	UP0001-$(APPID)_00-0000000000000000
APP_VER		:=	2.30
TO_DAY		:=	$(shell date +%F)

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

CFLAGS		=	-Wall -mcpu=cell $(MACHDEP) $(INCLUDE) -O2 -D_APP_VER=$(APP_VER)
CXXFLAGS	=	$(CFLAGS)

LDFLAGS		=	$(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:=	-lsimdmath -lrsx -lgcm_sys -lio -lsysutil -lrt -llv2 -lm -lsysmodule -lpngdec \
			-lps3sqlite -lsysfs -lspu_sound -laudio -lspu_soundmodule -laudioplayer \
			-lfreetype -lmpg123 -logg -lnet -lnetctl -lzip -lz -lbdffnt

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir)) \
					$(foreach dir,$(SHADERS),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

export BUILDDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))
VCGFILES	:=	$(foreach dir,$(SHADERS),$(notdir $(wildcard $(dir)/*.vcg)))
FCGFILES	:=	$(foreach dir,$(SHADERS),$(notdir $(wildcard $(dir)/*.fcg)))

VPOFILES	:=	$(VCGFILES:.vcg=.vpo)
FPOFILES	:=	$(FCGFILES:.fcg=.fpo)

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
					$(addsuffix .o,$(VPOFILES)) \
					$(addsuffix .o,$(FPOFILES)) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) \
					$(sFILES:.s=.o) $(SFILES:.S=.o)

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES), -I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					$(LIBPSL1GHT_INC) -I$(PORTLIBS)/include/ -I$(PORTLIBS)/modules \
					-I$(PORTLIBS)/modules/include -I$(CURDIR)/$(BUILD) -I$(PORTLIBS)/include/freetype2

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
					$(LIBPSL1GHT_LIB) -L$(PORTLIBS)/lib -L$(PORTLIBS)/modules/lib

export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).*elf $(OUTPUT)*.pkg $(PKGFILES)/USRDIR/RELOAD.SELF $(PKGFILES)/USRDIR/EBOOT.BIN

#---------------------------------------------------------------------------------
run:
	ps3load $(OUTPUT).self

#---------------------------------------------------------------------------------
pkg1:	$(BUILD) $(OUTPUT).pkg
pkg2:	$(BUILD)
pkg3:
	@cp $(OUTPUT).self $(PKGFILES)/USRDIR/RELOAD.SELF
pkg4:
	@mv $(OUTPUT).pkg $(OUTPUT)_v$(APP_VER)_$(TO_DAY).pkg
pkg:	pkg2 pkg3 pkg1 pkg4

pkgdir:
	@echo $@
	@echo $(BUILDDIR)
	@echo $(CURDIR)
	$(VERB) $(PKG) --contentid $(CONTENTID) $(BUILDDIR)/ test.pkg
	$(VERB) cp test.pkg test.gnpdrm.pkg
	$(VERB) $(PACKAGE_FINALIZE) test.gnpdrm.pkg

#---------------------------------------------------------------------------------
else

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).self: $(OUTPUT).elf
$(OUTPUT).elf:	$(OFILES)

#---------------------------------------------------------------------------------
# This rule links in binary data with the .bin extension
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.vpo.o	:	%.vpo
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.fpo.o	:	%.fpo
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
