# ------------------------------------------------
# FB Alpha Retro Loader Makefile (CaptainCPS-X, 2013)
# ------------------------------------------------
ifneq ($(LINUX),1)
ifneq ($(WIN32),1)
export CELL=1
export COLLADA_DOM_PLATFORM=CELL
endif
endif

SDK_VERSION			=3_4_0
CELL_SDK			?=/usr/local/cell
CELL_MK_DIR 		?= $(CELL_SDK)/samples/mk
PPU_CWARNFLAGS		?= $(CWARNFLAGS_L1)
PPU_CXXWARNFLAGS	?= $(CXXWARNFLAGS_L1)

include $(CELL_MK_DIR)/sdk.makedef.mk

## PSGL specific
PPU_LDLIBDIR		+= -L$(PPU_PSGL_LIBDIR) 

## FW specific
CELL_FW_DIR			?=$(CELL_SDK)/samples/fw

PPU_LDLIBDIR		+= -L$(CELL_FW_DIR) -Lsource/zlib

ifeq ($(CELL_PSGL_VERSION),gcmhud)
GRAPHICS_SAMPLE_CORE_LIBS = -lPSGLFX -lPSGL -lPSGLU -lgcm_hud -lgcm_pm -lsysmodule_stub -lusbd_stub -lio_stub -lm -lstdc++ -lsysutil_stub -lfs_stub -lsheap_stub -ldbgfont -lresc_stub -lperf
else
GRAPHICS_SAMPLE_CORE_LIBS = -lPSGLFX -lPSGL -lPSGLU -lm -lusbd_stub -lfs_stub -lio_stub -lsysutil_stub -ldbgfont -lresc_stub -lgcm_cmd -lgcm_sys_stub -lsysmodule_stub -lperf
endif

PPU_INCDIRS		+= -I$(CELL_FW_DIR)/include

# build everything without exceptions so you can link with fw
PPU_CXXSTDFLAGS += -DDATA_IN_HOME_DIR
GCC_PPU_CXXSTDFLAGS += -fno-exceptions
GCC_PPU_LDFLAGS     += -fno-exceptions

## PSGL samples specific
COLLADA_DOM_DIR=$(CELL_SDK)/samples/COLLADA_DOM

# where to build the graphics data to
CELL_GRAPHICS_DATA_DIR=$(CELL_DATA_DIR)/graphics

ifeq ($(NO_OPTIMIZATIONS),1)
PPU_OPTIMIZE_LV = 
endif 

PPU_CSTDFLAGS += -std=gnu99

# --------------------------------------------------------------

PPU_INCDIRS		+= -I$(CELL_FW_DIR)/include -Iinclude -Isource/ -Isource/rarch -Isource/zlib
PPU_LIBS		+= $(CELL_FW_DIR)/libfw.a

PPU_CPPFLAGS	+= -DPSGL -DHAVE_ZLIB_DEFLATE -DMINIZ_NO_ZLIB_COMPATIBLE_NAMES
PPU_CXXSTDFLAGS	+= -fno-exceptions

PPU_SRCS		= 	source/main.cpp \
					source/input.cpp \
					source/misc.cpp \
					source/fba_rl.cpp \
					source/ini.cpp \
					source/cfg.cpp \
					source/rarch/image.cpp \
					source/rarch/rpng.cpp

PPU_SRCS		+=	source/zlib/adler32.c \
					source/zlib/compress.c \
					source/zlib/crc32.c \
					source/zlib/deflate.c \
					source/zlib/gzclose.c \
					source/zlib/gzlib.c \
					source/zlib/gzread.c \
					source/zlib/gzwrite.c \
					source/zlib/infback.c \
					source/zlib/inffast.c \
					source/zlib/inflate.c \
					source/zlib/inftrees.c \
					source/zlib/trees.c \
					source/zlib/uncompr.c \
					source/zlib/zutil.c
					
PPU_TARGET		:= EBOOT.ELF
PPU_LDLIBS		:= $(GRAPHICS_SAMPLE_CORE_LIBS) -lz -lsysutil_screenshot_stub

# variables -----------------------------------------------------
FPSHADER_ELFS     =  $(patsubst %.cg, %.cgelf, $(FPSHADER_SRCS))
VPSHADER_ELFS     =  $(patsubst %.cg, %.cgelf, $(VPSHADER_SRCS))
PSGL_CGNV2ELF     =  $(PSGL_TOOLS_DIR)/cgnv2elf

# piggy back deletion of our .cgelf files into the cleanup system
# by using the user-clean rule instead of the clean-cg rule
CLEANFILES        += $(FPSHADER_ELFS) $(VPSHADER_ELFS)
TARGET            += $(FPSHADER_ELFS) $(VPSHADER_ELFS)
# rules ---------------------------------------------------------
include $(CELL_MK_DIR)/sdk.target.mk

$(FPSHADER_ELFS): %.cgelf: %.fpo
	$(PSGL_CGNV2ELF) $< $@

$(VPSHADER_ELFS): %.cgelf: %.vpo
	$(PSGL_CGNV2ELF) $< $@
# ---------------------------------------------------------------

MAKE_FSELF 			= $(CELL_SDK)/host-win32/bin/make_fself.exe
MAKE_FSELF_NPDRM 	= $(CELL_SDK)/host-win32/bin/make_fself_npdrm.exe
MAKE_PACKAGE_NPDRM 	= $(CELL_SDK)/host-win32/bin/make_package_npdrm.exe

APP_NAME		=FB_ALPHA_RL
APP_VER			=1.01a
DATE 			=$(shell date +"%Y%m%d")

PSN_PKG_NPDRM 	= bin/psn_package_npdrm

SELF_APP_VER	=0001000000000000
CONTENT_ID		=FBARAL-FBAL00123_00-0000000000000000

MAKE_SELF_NPDRM_340 = scetool.exe \
	--sce-type=SELF \
	--compress-data=TRUE \
	--skip-sections=FALSE \
	--key-revision=0001 \
	--self-ctrl-flags=4000000000000000000000000000000000000000000000000000000000000002 \
	--self-auth-id=1010000001000003 \
	--self-app-version=$(SELF_APP_VER) \
	--self-add-shdrs=TRUE \
	--self-vendor-id=01000002 \
	--self-type=NPDRM \
	--self-fw-version=0003004000000000 \
	--np-license-type=FREE \
	--np-content-id=$(CONTENT_ID) \
	--np-app-type=EXEC \
	--np-real-fname="EBOOT.BIN" \
	--np-klicensee=72F990788F9CFF745725F08E4C128387 \
	--np-add-sig=FALSE \
	--encrypt

MAKE_SELF_NPDRM_355 = scetool.exe \
	--sce-type=SELF \
	--compress-data=TRUE \
	--skip-sections=FALSE \
	--key-revision=0001 \
	--self-auth-id=1010000001000003 \
	--self-app-version=$(SELF_APP_VER) \
	--self-add-shdrs=TRUE \
	--self-vendor-id=01000002 \
	--self-type=NPDRM \
	--self-fw-version=0003005500000000 \
	--np-license-type=FREE \
	--np-content-id=$(CONTENT_ID) \
	--np-app-type=EXEC \
	--np-real-fname="EBOOT.BIN" \
	--np-klicensee=72F990788F9CFF745725F08E4C128387 \
	--np-add-sig=FALSE \
	--encrypt
	
MAKE_RELOAD_FSELF_340 = scetool.exe \
	--sce-type=SELF \
	--compress-data=TRUE \
	--skip-sections=FALSE \
	--key-revision=0001 \
	--self-auth-id=1010000001000003 \
	--self-app-version=$(SELF_APP_VER) \
	--self-vendor-id=01000002 \
	--self-type=APP \
	--self-fw-version=0003004000000000 \
	--encrypt

MAKE_RELOAD_FSELF_355 = scetool.exe \
	--sce-type=SELF \
	--compress-data=TRUE \
	--skip-sections=FALSE \
	--key-revision=0001 \
	--self-auth-id=1010000001000003 \
	--self-app-version=$(SELF_APP_VER) \
	--self-vendor-id=01000002 \
	--self-type=APP \
	--self-fw-version=0003005500000000 \
	--encrypt	
	
MAKE_SELF=make_fself_npdrm.exe
# -------------------------------------------------------------------

fba_340: $(PPU_TARGET)
	@$(PPU_STRIP) -s $(PPU_TARGET) -o $(OBJS_DIR)/$(PPU_TARGET)
	@cd bin/ && $(MAKE_RELOAD_FSELF_340) ../$(OBJS_DIR)/$(PPU_TARGET) ../release/PS3_GAME/USRDIR/RELOAD.SELF && cd ../
	@rm $(PPU_TARGET)
	@cd bin/ && $(MAKE_SELF_NPDRM_340) ../$(OBJS_DIR)/$(PPU_TARGET) ../release/PS3_GAME/USRDIR/EBOOT.BIN && cd ../
	@echo "Preparing PKG..."
	@$(PSN_PKG_NPDRM) release/package.conf release/PS3_GAME/ >nul 2>&1
	@mv *.pkg release/$(APP_NAME)_$(APP_VER)_[$(DATE)]_CEX340.pkg
	@echo "Done!"

fba_340_upd: $(PPU_TARGET)
	@$(PPU_STRIP) -s $(PPU_TARGET) -o $(OBJS_DIR)/$(PPU_TARGET)
	@cd bin/ && $(MAKE_RELOAD_FSELF_340) ../$(OBJS_DIR)/$(PPU_TARGET) ../release_upd/PS3_GAME/USRDIR/RELOAD.SELF && cd ../
	@rm $(PPU_TARGET)
	@cd bin/ && $(MAKE_SELF_NPDRM_340) ../$(OBJS_DIR)/$(PPU_TARGET) ../release_upd/PS3_GAME/USRDIR/EBOOT.BIN && cd ../
	@echo "Preparing PKG..."
	@$(PSN_PKG_NPDRM) release_upd/package.conf release_upd/PS3_GAME/ >nul 2>&1
	@mv *.pkg release_upd/$(APP_NAME)_$(APP_VER)_[$(DATE)]_CEX340_UPD.pkg
	@echo "Done!"
	
fba_355: $(PPU_TARGET)
	@$(PPU_STRIP) -s $(PPU_TARGET) -o $(OBJS_DIR)/$(PPU_TARGET)
	@cd bin/ && $(MAKE_RELOAD_FSELF_355) ../$(OBJS_DIR)/$(PPU_TARGET) ../release/PS3_GAME/USRDIR/RELOAD.SELF && cd ../
	@rm $(PPU_TARGET)
	@cd bin/ && $(MAKE_SELF_NPDRM_355) ../$(OBJS_DIR)/$(PPU_TARGET) ../release/PS3_GAME/USRDIR/EBOOT.BIN && cd ../
	@echo "Preparing PKG..."
	@$(PSN_PKG_NPDRM) release/package.conf release/PS3_GAME/ >nul 2>&1
	@mv *.pkg release/$(APP_NAME)_$(APP_VER)_[$(DATE)]_CEX355.pkg
	@echo "Done!"

fba_355_upd: $(PPU_TARGET)
	@$(PPU_STRIP) -s $(PPU_TARGET) -o $(OBJS_DIR)/$(PPU_TARGET)
	@cd bin/ && $(MAKE_RELOAD_FSELF_355) ../$(OBJS_DIR)/$(PPU_TARGET) ../release_upd/PS3_GAME/USRDIR/RELOAD.SELF && cd ../
	@rm $(PPU_TARGET)
	@cd bin/ && $(MAKE_SELF_NPDRM_355) ../$(OBJS_DIR)/$(PPU_TARGET) ../release_upd/PS3_GAME/USRDIR/EBOOT.BIN && cd ../
	@echo "Preparing PKG..."
	@$(PSN_PKG_NPDRM) release_upd/package.conf release_upd/PS3_GAME/ >nul 2>&1
	@mv *.pkg release_upd/$(APP_NAME)_$(APP_VER)_[$(DATE)]_CEX355_UPD.pkg
	@echo "Done!"