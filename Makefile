#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(PSL1GHT)),)
$(error "Please set PSL1GHT in your environment. export PSL1GHT=<path>")
endif
ifeq ($(strip $(PORTLIBS)),)
$(error "Please set PORTLIBS in your environment.")
endif

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
INCLUDES	:=	include
PKGFILES	:=	$(CURDIR)/pkgfiles

TITLE		:=	FB NEO RL Plus
APPID		:=	FBNE00123
CONTENTID	:=	FBNEOO-$(APPID)_00-0000000000000000
ICON0		:=	ICON0.PNG
ICON1		:=	ICON1.PAM
PIC1		:=	PIC1.PNG
SFOXML		:=	sfo.xml
APP_VER		:=	2.10

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

CFLAGS		=	-O2 -Wall -mcpu=cell $(MACHDEP) $(INCLUDE)
CXXFLAGS	=	$(CFLAGS)

LDFLAGS		=	$(MACHDEP) -Wl,-Map,$(notdir $@).map

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:=	-lgcm_sys -lrsx -lcairo -lsysutil -lio -laudioplayer -lmpg123 -logg \
			-lm -lnetctl -lnet -lfreetype -lzip -lz -lpixman-1 \
			-lrt -llv2 -lsysmodule -lpng -lpngdec -ljpgdec -lps3sqlite -lsysfs -lspu_sound -laudio -I$(PORTLIBS)/modules/lib/spu_soundmodule.bin.a

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

export BUILDDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.bin)))
PNGFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.png)))
JPGFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.jpg)))
TTFFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.ttf)))
VCGFILES	:=	$(foreach dir,$(SHADERS),$(notdir $(wildcard $(dir)/*.vcg)))
FCGFILES	:=	$(foreach dir,$(SHADERS),$(notdir $(wildcard $(dir)/*.fcg)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES	:=	$(addsuffix .o,$(BINFILES)) \
			$(addsuffix .o,$(TTFFILES)) \
			$(addsuffix .o,$(VPOFILES)) \
			$(addsuffix .o,$(FPOFILES)) \
			$(addsuffix .o,$(PNGFILES)) \
			$(addsuffix .o,$(JPGFILES)) \
					$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) \
					$(sFILES:.s=.o) $(SFILES:.S=.o)

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	-I$(PORTLIBS)/include/freetype2 \
			$(foreach dir,$(INCLUDES), -I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					$(LIBPSL1GHT_INC) \
					-I$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib) \
					$(LIBPSL1GHT_LIB) -L$(PORTLIBS)/lib \

export OUTPUT	:=	$(CURDIR)/$(TARGET)

.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) *.elf *.self *.pkg *~ */*~

#---------------------------------------------------------------------------------
run:
	ps3load $(OUTPUT).self

#---------------------------------------------------------------------------------
pkg: npdrm
	$(VERB) echo building pkg ... $(notdir $@)
	$(VERB) cp -f $(CURDIR)/../EBOOT.BIN $(CURDIR)/../pkg/USRDIR/EBOOT.BIN
	$(VERB) cp -f $(CURDIR)/../$(TARGET).self $(CURDIR)/../pkg/USRDIR/RELOAD.SELF
	$(VERB) $(SFO) --title "$(TITLE)" --appid "$(APPID)" -f $(SFOXML) $(CURDIR)/../pkg/PARAM.SFO
	$(VERB) $(PKG) --contentid $(CONTENTID) $(CURDIR)/../pkg/ $(TARGET)_$(APP_VER).pkg >> /dev/null
#---------------------------------------------------------------------------------

pkgsnes: npdrm
	$(VERB) echo building SNES upgrade pkg ... $(notdir $@)
	$(VERB) $(SFO) --title "$(TITLE)" --appid "$(APPID)" -f $(SFOXML) $(CURDIR)/../pkgSnes/PARAM.SFO
	$(VERB) $(PKG) --contentid $(CONTENTID) $(CURDIR)/../pkgSnes/ $(TARGET)_$(APP_VER)_snes_upgrade.pkg >> /dev/null
#---------------------------------------------------------------------------------

pkgmd: npdrm
	$(VERB) echo building Mega Drive upgrade pkg ... $(notdir $@)
	$(VERB) $(SFO) --title "$(TITLE)" --appid "$(APPID)" -f $(SFOXML) $(CURDIR)/../pkgMD/PARAM.SFO
	$(VERB) $(PKG) --contentid $(CONTENTID) $(CURDIR)/../pkgMD/ $(TARGET)_$(APP_VER)_megadrive_upgrade.pkg >> /dev/null
#---------------------------------------------------------------------------------

pkgAmiga: npdrm
	$(VERB) echo building Amiga upgrade pkg ... $(notdir $@)
	$(VERB) $(SFO) --title "$(TITLE)" --appid "$(APPID)" -f $(SFOXML) $(CURDIR)/../pkgAmiga/PARAM.SFO
	$(VERB) $(PKG) --contentid $(CONTENTID) $(CURDIR)/../pkgMD/ $(TARGET)_$(APP_VER)_amiga_upgrade.pkg >> /dev/null
#---------------------------------------------------------------------------------

npdrm: $(BUILD)
	@$(SELF_NPDRM) $(SCETOOL_FLAGS) --np-content-id=$(CONTENTID) --encrypt $(BUILDDIR)/$(basename $(notdir $(OUTPUT))).elf $(BUILDDIR)/../EBOOT.BIN
	$(VERB) cp -f $(CURDIR)/$(TARGET).self $(CURDIR)/RELOAD.SELF
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
%.ttf.o	:	%.ttf
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

#---------------------------------------------------------------------------------
%.jpg.o	:	%.jpg
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)
#---------------------------------------------------------------------------------
%.png.o	:	%.png
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
