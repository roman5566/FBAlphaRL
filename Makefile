# -----------------------------------------------------------------
# FB Alpha Retro Loader Makefile (CaptainCPS-X, 2013)
# -----------------------------------------------------------------
SDK_VERSION			?=	3_4_0
CELL_SDK			?=	/usr/local/cell
CELL_MK_DIR 		?= $(CELL_SDK)/samples/mk

MAKE_FSELF 			?= $(CELL_SDK)/host-win32/bin/make_fself.exe
MAKE_FSELF_NPDRM 	?= $(CELL_SDK)/host-win32/bin/make_fself_npdrm.exe
MAKE_PACKAGE_NPDRM 	?= $(CELL_SDK)/host-win32/bin/make_package_npdrm.exe
PSN_PKG_NPDRM 		?= $(CURDIR)/bin/psn_package_npdrm

APP_NAME			?=	FB_ALPHA_RL
APP_VER				?=	1.02a
DATE 				?=	$(shell date +"%Y%m%d")

RARCH_PATH			=	retroarch
FBARL_PATH			=	$(CURDIR)
FBA_LIBRETRO_PATH	=	libretro/svn-current/trunk

MAKE_SELF = bin/make_self.exe

MAKE_SELF_NPDRM	= scetool.exe \
--sce-type=SELF \
--compress-data=TRUE \
--skip-sections=FALSE \
--key-revision=04 \
--self-ctrl-flags=4000000000000000000000000000000000000000000000000000000000000002 \
--self-auth-id=1010000001000003 \
--self-app-version=0001000000000000 \
--self-add-shdrs=TRUE \
--self-vendor-id=01000002 \
--self-type=NPDRM \
--self-fw-version=0003004000000000 \
--np-license-type=FREE \
--np-content-id=FBARAL-FBAL00123_00-0000000000000000 \
--np-app-type=EXEC \
--np-real-fname="EBOOT.BIN" \
--np-klicensee=72F990788F9CFF745725F08E4C128387 \
--np-add-sig=FALSE \
--encrypt

# -----------------------------------------------------------------

ifneq ($(LINUX),1)
ifneq ($(WIN32),1)
export CELL=1
export COLLADA_DOM_PLATFORM=CELL
endif
endif

PPU_CWARNFLAGS		?= $(CWARNFLAGS_L1)
PPU_CXXWARNFLAGS	?= $(CXXWARNFLAGS_L1)

include $(CELL_MK_DIR)/sdk.makedef.mk

## PSGL specific
PPU_LDLIBDIR		+= -L$(PPU_PSGL_LIBDIR) 

## FW specific
CELL_FW_DIR			?=$(CELL_SDK)/samples/fw

PPU_LDLIBDIR		+= -L$(CELL_FW_DIR) -Lsource/zlib

ifeq ($(CELL_PSGL_VERSION),gcmhud)
GRAPHICS_SAMPLE_CORE_LIBS = -lPSGLFX -lPSGL -lPSGLU -lgcm_hud -lgcm_pm \
							-lsysmodule_stub -lusbd_stub -lio_stub -lm \
							-lstdc++ -lsysutil_stub -lfs_stub -lsheap_stub \
							-ldbgfont -lresc_stub -lperf
else
GRAPHICS_SAMPLE_CORE_LIBS = -lPSGLFX -lPSGL -lPSGLU -lm -lusbd_stub -lfs_stub \
							-lio_stub -lsysutil_stub -ldbgfont -lresc_stub \
							-lgcm_cmd -lgcm_sys_stub -lsysmodule_stub -lperf
endif

PPU_INCDIRS			+= -I$(CELL_FW_DIR)/include

# build everything without exceptions so you can link with fw
PPU_CXXSTDFLAGS 	+= -DDATA_IN_HOME_DIR
GCC_PPU_CXXSTDFLAGS += -fno-exceptions
GCC_PPU_LDFLAGS     += -fno-exceptions

## PSGL samples specific
COLLADA_DOM_DIR		= $(CELL_SDK)/samples/COLLADA_DOM

# where to build the graphics data to
CELL_GRAPHICS_DATA_DIR	= $(CELL_DATA_DIR)/graphics

ifeq ($(NO_OPTIMIZATIONS),1)
PPU_OPTIMIZE_LV = 
endif 

PPU_CSTDFLAGS 	+= -std=gnu99

# --------------------------------------------------------------

PPU_INCDIRS		+= 	-I$(CELL_FW_DIR)/include \
					-Iinclude \
					-Isource \
					-Isource/miniz \
					-Isource/rarch \
					-Isource/zlib \
					-Isource/ftp
					
PPU_LIBS		+= $(CELL_FW_DIR)/libfw.a
PPU_LDLIBS		:= $(GRAPHICS_SAMPLE_CORE_LIBS) \
					-lz \
					-lsysutil_screenshot_stub \
					-lnet_stub \
					-lnetctl_stub

PPU_CPPFLAGS	+= -DPSGL -DHAVE_ZLIB_DEFLATE -DMINIZ_NO_ZLIB_COMPATIBLE_NAMES
PPU_CXXSTDFLAGS	+= -fno-exceptions

PPU_SRCS		= 	source/miniz/miniz.cpp \
					source/main.cpp \
					source/input.cpp \
					source/misc.cpp \
					source/burn_drivers.cpp \
					source/fba_rl.cpp \
					source/fba_rl_burndrv.cpp \
					source/fba_rl_dialogs.cpp \
					source/fba_rl_filebrowser.cpp \
					source/fba_rl_gamelist_filter.cpp \
					source/fba_rl_gamelist_parse.cpp \
					source/fba_rl_input.cpp \
					source/fba_rl_menu.cpp \
					source/fba_rl_render.cpp \
					source/fba_rl_section.cpp \
					source/fba_rl_utility.cpp \
					source/ini.cpp \
					source/cfg.cpp \
					source/rarch/image.cpp \
					source/rarch/rpng.cpp \
					source/ftp/ftp.cpp \
					source/ftp/ftpcmd.cpp \
					source/ftp/functions.cpp

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

include $(CELL_MK_DIR)/sdk.target.mk

_cleanup: clean
	@echo "|"
	@echo "---------------------------------------------------------------------------"
	@echo "Cleaning all cores (please wait)..."
	@echo "---------------------------------------------------------------------------"
	@echo "|"
	@cd $(FBA_LIBRETRO_PATH); make --no-print-directory -f makefile.libretro clean; cd ..
	@rm -fr $(FBA_LIBRETRO_PATH)/fb_alpha_libretro_ps3.a
	@cd $(RARCH_PATH); make --no-print-directory -f Makefile.ps3.salamander clean; cd ..
	@cd $(RARCH_PATH); make --no-print-directory -f Makefile.ps3.rgl clean; cd ..
	@cd $(RARCH_PATH); make --no-print-directory -f Makefile.ps3 clean; cd ..
	@rm -fr $(RARCH_PATH)/libretro_ps3.a
	@rm -fr $(OBJS_DIR)
	
_libretro:
	@echo "|"
	@echo "---------------------------------------------------------------------------"
	@echo "Building FB Alpha libretro core..."
	@echo "---------------------------------------------------------------------------"
	@echo "|"
#	make -f ./makefile.libretro clean
	@cd $(FBA_LIBRETRO_PATH); make --no-print-directory -f makefile.libretro -j4 platform=ps3; cd ..
	@rm -fr $(RARCH_PATH)/libretro_ps3.a
	@cp -fr $(FBA_LIBRETRO_PATH)/fb_alpha_libretro_ps3.a $(RARCH_PATH)/libretro_ps3.a
	
_retroarch:
	@echo "|"
	@echo "---------------------------------------------------------------------------"
	@echo "Building RetroArch modified core..."
	@echo "---------------------------------------------------------------------------"
	@echo "|"
	@cd $(RARCH_PATH); make --no-print-directory -f Makefile.ps3.salamander; cd ..
	@cd $(RARCH_PATH); make --no-print-directory -f Makefile.ps3.rgl; cd ..
	@cd $(RARCH_PATH); make --no-print-directory -f Makefile.ps3 clean; cd ..
	@cd $(RARCH_PATH); make --no-print-directory -f Makefile.ps3; cd ..
	@rm -fr release/PS3_GAME/USRDIR/cores/fb_alpha.SELF release_upd/PS3_GAME/USRDIR/cores/fb_alpha.SELF
	@$(MAKE_SELF) $(RARCH_PATH)/retroarch_ps3.elf release/PS3_GAME/USRDIR/cores/fb_alpha.SELF
	@cp -fr release/PS3_GAME/USRDIR/cores/fb_alpha.SELF release_upd/PS3_GAME/USRDIR/cores/fb_alpha.SELF
	@echo "|"
	@echo "---------------------------------------------------------------------------"
	@echo "Building FB Alpha Retro Loader..."
	@echo "---------------------------------------------------------------------------"
	@echo "|"
	
fbarl: _libretro _retroarch $(PPU_TARGET)
	@$(PPU_STRIP) -s $(PPU_TARGET) -o $(OBJS_DIR)/$(PPU_TARGET)
	@rm $(PPU_TARGET)
	@rm -fr release/PS3_GAME/USRDIR/RELOAD.SELF release_upd/PS3_GAME/USRDIR/RELOAD.SELF
	@$(MAKE_SELF) objs/EBOOT.ELF release/PS3_GAME/USRDIR/RELOAD.SELF
	@cp -fr release/PS3_GAME/USRDIR/RELOAD.SELF release_upd/PS3_GAME/USRDIR/RELOAD.SELF
	@cd bin; $(MAKE_SELF_NPDRM) ../objs/EBOOT.ELF ../release/PS3_GAME/USRDIR/EBOOT.BIN; cd ..
	@cp -fr release/PS3_GAME/USRDIR/EBOOT.BIN release_upd/PS3_GAME/USRDIR/EBOOT.BIN	
	@echo "Done!"

pkg:
	@echo "|"
	@echo "---------------------------------------------------------------------------"
	@echo "Preparing BASE PKG..."
	@echo "---------------------------------------------------------------------------"
	@echo "|"
	@$(PSN_PKG_NPDRM) release/package.conf release/PS3_GAME/ >nul 2>&1
	@mv *.pkg release/$(APP_NAME)_$(APP_VER)_[$(DATE)]_[CEX_34X_4XX]_[BASE].pkg
	@echo "Done!"

pkg_upd:
	@echo "|"
	@echo "---------------------------------------------------------------------------"
	@echo "Preparing PKG..."
	@echo "---------------------------------------------------------------------------"
	@echo "|"
	@$(PSN_PKG_NPDRM) release_upd/package.conf release_upd/PS3_GAME/ >nul 2>&1
	@mv *.pkg release_upd/$(APP_NAME)_$(APP_VER)_[$(DATE)]_[CEX_34X_4XX]_[UPD].pkg
	@echo "Done!"

