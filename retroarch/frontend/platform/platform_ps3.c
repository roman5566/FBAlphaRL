/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2013 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2013 - Daniel De Matteis
 * 
 *  RetroArch is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RetroArch.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/process.h>

#include "../../ps3/sdk_defines.h"
#include "../../ps3/ps3_input.h"

#include "../../console/rarch_console.h"
#include "../../conf/config_file.h"
#include "../../general.h"
#include "../../file.h"

#ifdef _FBARL_
#define EMULATOR_CONTENT_DIR "FBAL00123"
bool g_bUseAltMenuKeyCombo = true;
#else
#define EMULATOR_CONTENT_DIR "SSNE10000"
#endif

#ifndef __PSL1GHT__
#define NP_POOL_SIZE (128*1024)
static uint8_t np_pool[NP_POOL_SIZE];
#endif

//TODO - not sure if stack size needs to be lower for Salamander
#ifdef IS_SALAMANDER
SYS_PROCESS_PARAM(1001, 0x100000)
#else
SYS_PROCESS_PARAM(1001, 0x200000)
#endif
   
#ifdef HAVE_MULTIMAN
#ifdef _FBARL_
#define MULTIMAN_SELF_FILE "/dev_hdd0/game/FBAL00123/USRDIR/RELOAD.SELF"
#else
#define MULTIMAN_SELF_FILE "/dev_hdd0/game/BLES80608/USRDIR/RELOAD.SELF"
#endif
#endif

#ifdef IS_SALAMANDER
#include <netex/net.h>
#include <np.h>
#include <np/drm.h>
#include <cell/pad.h>
#include <cell/sysmodule.h>

static void find_and_set_first_file(void)
{
   //Last fallback - we'll need to start the first executable file 
   // we can find in the RetroArch cores directory

   char first_file[PATH_MAX];
   find_first_libretro_core(first_file, sizeof(first_file), default_paths.core_dir, "SELF");

   if(first_file)
   {
      snprintf(default_paths.libretro_path, sizeof(default_paths.libretro_path), 
            "%s/%s", default_paths.core_dir, first_file);
      RARCH_LOG("libretro_path now set to: %s.\n", default_paths.libretro_path);
   }
   else
      RARCH_ERR("Failed last fallback - RetroArch Salamander will exit.\n");
}

static void salamander_init_settings(void)
{
   CellPadData pad_data;
   cellPadInit(7);

   cellPadGetData(0, &pad_data);

   if(pad_data.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_TRIANGLE)
   {
      //override path, boot first executable in cores directory
      RARCH_LOG("Fallback - Will boot first executable in RetroArch cores/ directory.\n");
      find_and_set_first_file();
   }
   else
   {
      //normal executable loading path
      char tmp_str[PATH_MAX];
      bool config_file_exists = false;

      if (path_file_exists(default_paths.config_path))
         config_file_exists = true;

      //try to find CORE executable
      char core_executable[1024];
#ifdef _FBARL_
	  snprintf(core_executable, sizeof(core_executable), "%s/fb_alpha.SELF", default_paths.core_dir);
#else
      snprintf(core_executable, sizeof(core_executable), "%s/CORE.SELF", default_paths.core_dir);
#endif
      if(path_file_exists(core_executable))
      {
         //Start CORE executable
         snprintf(default_paths.libretro_path, sizeof(default_paths.libretro_path), core_executable);
         RARCH_LOG("Start [%s].\n", default_paths.libretro_path);
      }
      else
      {
         if (config_file_exists)
         {
            config_file_t * conf = config_file_new(default_paths.config_path);
            config_get_array(conf, "libretro_path", tmp_str, sizeof(tmp_str));
            config_file_free(conf);
            snprintf(default_paths.libretro_path, sizeof(default_paths.libretro_path), tmp_str);
         }

         if (!config_file_exists || !strcmp(default_paths.libretro_path, ""))
            find_and_set_first_file();
         else
            RARCH_LOG("Start [%s] found in retroarch.cfg.\n", default_paths.libretro_path);

         if (!config_file_exists)
         {
            config_file_t *new_conf = config_file_new(NULL);
            config_set_string(new_conf, "libretro_path", default_paths.libretro_path);
            config_file_write(new_conf, default_paths.config_path);
            config_file_free(new_conf);
         }
      }
   }

   cellPadEnd();

}

#endif

#ifdef HAVE_SYSUTILS
static void callback_sysutil_exit(uint64_t status, uint64_t param, void *userdata)
{
   (void)param;
   (void)userdata;
   (void)status;

#ifndef IS_SALAMANDER
#ifdef HAVE_OSKUTIL
   oskutil_params *osk = &rgui->oskutil_handle;
#endif
   gl_t *gl = driver.video_data;

   switch (status)
   {
      case CELL_SYSUTIL_REQUEST_EXITGAME:
         gl->quitting = true;
         g_extern.lifecycle_mode_state &= ~((1ULL << MODE_MENU) | (1ULL << MODE_GAME));
         break;
#ifdef HAVE_OSKUTIL
      case CELL_SYSUTIL_OSKDIALOG_LOADED:
         break;
      case CELL_SYSUTIL_OSKDIALOG_INPUT_CANCELED:
         RARCH_LOG("CELL_SYSUTIL_OSKDIALOG_INPUT_CANCELED.\n");
         pOskAbort(); //fall-through
      case CELL_SYSUTIL_OSKDIALOG_FINISHED:
         if (status == CELL_SYSUTIL_OSKDIALOG_FINISHED)
            RARCH_LOG("CELL_SYSUTIL_OSKDIALOG_FINISHED.\n");

         pOskUnloadAsync(&osk->outputInfo);

         if (osk->outputInfo.result == CELL_OSKDIALOG_INPUT_FIELD_RESULT_OK)
         {
            RARCH_LOG("Setting MODE_OSK_ENTRY_SUCCESS.\n");
            g_extern.lifecycle_mode_state |= (1ULL << MODE_OSK_ENTRY_SUCCESS);
         }
         else
         {
            RARCH_LOG("Setting MODE_OSK_ENTRY_FAIL.\n");
            g_extern.lifecycle_mode_state |= (1ULL << MODE_OSK_ENTRY_FAIL);
         }

         osk->flags &= ~OSK_IN_USE;
         break;
      case CELL_SYSUTIL_OSKDIALOG_UNLOADED:
         RARCH_LOG("CELL_SYSUTIL_OSKDIALOG_UNLOADED.\n");
         sys_memory_container_destroy(osk->containerid);
         break;
#endif
   }
#endif
}
#endif

#ifdef _FBARL_
#define		FBARL_PATH "/dev_hdd0/game/FBAL00123/USRDIR/RELOAD.SELF"
float		g_fAspectRatio = 0.0f;
int			g_nAspectRatio = 0;
#define		GET_MACRO_NAME(TYPE) #TYPE
#endif

static void get_environment_settings(int argc, char *argv[], void *args)
{
   (void)args;
#ifndef IS_SALAMANDER
   g_extern.verbose = true;

#if defined(HAVE_LOGGER)
   logger_init();
#elif defined(HAVE_FILE_LOGGER)
   g_extern.log_file = fopen("/retroarch-log.txt", "w");
#endif
#endif

   int ret;
   unsigned int get_type;
   unsigned int get_attributes;
   CellGameContentSize size;
   char dirName[CELL_GAME_DIRNAME_SIZE];
   char contentInfoPath[PATH_MAX];

#ifdef HAVE_MULTIMAN

#ifdef _FBARL_
   if(argc > 1 )
   {
	   //strcpy(g_settings.libretro, "/dev_hdd0/game/FBAL00123/USRDIR/cores/fb_alpha.SELF");

	   // Check if launched from FB Alpha RL...
	   if(argc > 2)
	   {
		   if(strcmp(argv[2], FBARL_PATH) == 0) {
			   //strlcpy(default_paths.multiman_self_file, FBARL_PATH, sizeof(default_paths.multiman_self_file));
		   }

		   // 3rd argument used to load Game Input Preset on the fly
		   if(argc > 3) {
			   if(strstr(argv[3], ".cfg") != NULL) { 
				   FILE* fp = fopen(argv[3], "r");
				   if(fp) {
					   fclose(fp); fp = NULL;
					   strlcpy(g_extern.input_config_path, argv[3], sizeof(g_extern.input_config_path));
				   }
			   }
		   }

		   // 4th argument used to specify proper game Aspect Ratio (Ex. "4:3" would be 4:3)
		   if(argc > 4) 
		   {
			   //FILE* fp = fopen("/dev_hdd0/game/FBAL00123/USRDIR/debug_ar.txt","w");
			   
			   int x, y;
			   sscanf(argv[4], "%d:%d", &x, &y);

			   char ar_game[256] = { 0 };
			   sprintf(ar_game, "ASPECT_RATIO_%d_%d", x, y); 

			   char ar_macro[19][256] = { 
				   GET_MACRO_NAME(ASPECT_RATIO_4_3),
				   GET_MACRO_NAME(ASPECT_RATIO_16_9),
				   GET_MACRO_NAME(ASPECT_RATIO_16_10),
				   GET_MACRO_NAME(ASPECT_RATIO_16_15),
				   GET_MACRO_NAME(ASPECT_RATIO_1_1),
				   GET_MACRO_NAME(ASPECT_RATIO_2_1),
				   GET_MACRO_NAME(ASPECT_RATIO_3_2),
				   GET_MACRO_NAME(ASPECT_RATIO_3_4),
				   GET_MACRO_NAME(ASPECT_RATIO_4_1),
				   GET_MACRO_NAME(ASPECT_RATIO_4_4),
				   GET_MACRO_NAME(ASPECT_RATIO_5_4),
				   GET_MACRO_NAME(ASPECT_RATIO_6_5),
				   GET_MACRO_NAME(ASPECT_RATIO_7_9),
				   GET_MACRO_NAME(ASPECT_RATIO_8_3),
				   GET_MACRO_NAME(ASPECT_RATIO_8_7),
				   GET_MACRO_NAME(ASPECT_RATIO_19_12),
				   GET_MACRO_NAME(ASPECT_RATIO_19_14),
				   GET_MACRO_NAME(ASPECT_RATIO_30_17),
				   GET_MACRO_NAME(ASPECT_RATIO_32_9)
			   };

			   for(int i = 0; i < 19; i++) 
			   {
				   //if(fp) fprintf(fp, "checking...%s for %s (%d)\n", ar_macro[i], ar_game, i);
				   
				   if(strcmp(ar_game, ar_macro[i]) == 0) 
				   {
					   //if(fp) fprintf(fp, "got it...%s", ar_macro[i]);
					   
					   // i value is according to "enum aspect_ratio" in "rarch_console_video.h"
					   g_nAspectRatio = i;
					   g_fAspectRatio = (float)x / (float) y;
					   break;
				   }
			   }
			  
			   //if(fp) fclose(fp); fp = NULL;
		   }

		   // 5th argument used for Alternate RetroArch menu key combo
		   if(argc > 5) 
		   {
			   if(strstr(argv[5], "yes") != NULL) { 
				   g_bUseAltMenuKeyCombo = true;
			   } else {
				   g_bUseAltMenuKeyCombo = false;
			   }
		   }
	   }
      
	  g_extern.lifecycle_mode_state |= (1ULL << MODE_EXTLAUNCH_MULTIMAN);
      RARCH_LOG("Started from multiMAN, auto-game start enabled.\n");

   }
#else
   /* not launched from external launcher, set default path */
   // second param is multiMAN SELF file
   if(path_file_exists(argv[2]) && argc > 1 && (strcmp(argv[2], EMULATOR_CONTENT_DIR) == 0))
   {
      g_extern.lifecycle_mode_state |= (1ULL << MODE_EXTLAUNCH_MULTIMAN);
      RARCH_LOG("Started from multiMAN, auto-game start enabled.\n");
   }
#endif
   else
#endif
#ifndef IS_SALAMANDER
      RARCH_WARN("Started from Salamander, auto-game start disabled.\n");
#endif

   memset(&size, 0x00, sizeof(CellGameContentSize));

   ret = cellGameBootCheck(&get_type, &get_attributes, &size, dirName);
   if(ret < 0)
   {
      RARCH_ERR("cellGameBootCheck() Error: 0x%x.\n", ret);
   }
   else
   {
      RARCH_LOG("cellGameBootCheck() OK.\n");
      RARCH_LOG("Directory name: [%s].\n", dirName);
      RARCH_LOG(" HDD Free Size (in KB) = [%d] Size (in KB) = [%d] System Size (in KB) = [%d].\n", size.hddFreeSizeKB, size.sizeKB, size.sysSizeKB);

      switch(get_type)
      {
         case CELL_GAME_GAMETYPE_DISC:
            RARCH_LOG("RetroArch was launched on Optical Disc Drive.\n");
            break;
         case CELL_GAME_GAMETYPE_HDD:
            RARCH_LOG("RetroArch was launched on HDD.\n");
            break;
      }

      if((get_attributes & CELL_GAME_ATTRIBUTE_APP_HOME) == CELL_GAME_ATTRIBUTE_APP_HOME)
         RARCH_LOG("RetroArch was launched from host machine (APP_HOME).\n");

      ret = cellGameContentPermit(contentInfoPath, default_paths.port_dir);

#ifdef HAVE_MULTIMAN
      if (g_extern.lifecycle_mode_state & (1ULL << MODE_EXTLAUNCH_MULTIMAN))
      {
         snprintf(contentInfoPath, sizeof(contentInfoPath), "/dev_hdd0/game/%s", EMULATOR_CONTENT_DIR);
         snprintf(default_paths.port_dir, sizeof(default_paths.port_dir), "/dev_hdd0/game/%s/USRDIR", EMULATOR_CONTENT_DIR);
      }
#endif

      if(ret < 0)
      {
         RARCH_ERR("cellGameContentPermit() Error: 0x%x\n", ret);
      }
      else
      {
         RARCH_LOG("cellGameContentPermit() OK.\n");
         RARCH_LOG("contentInfoPath : [%s].\n", contentInfoPath);
         RARCH_LOG("usrDirPath : [%s].\n", default_paths.port_dir);
      }

      snprintf(default_paths.core_dir, sizeof(default_paths.core_dir), "%s/cores", default_paths.port_dir);
      snprintf(default_paths.savestate_dir, sizeof(default_paths.savestate_dir), "%s/savestates", default_paths.core_dir);
      snprintf(default_paths.filesystem_root_dir, sizeof(default_paths.filesystem_root_dir), "/");
      snprintf(default_paths.filebrowser_startup_dir, sizeof(default_paths.filebrowser_startup_dir), default_paths.filesystem_root_dir);
      snprintf(default_paths.sram_dir, sizeof(default_paths.sram_dir), "%s/savefiles", default_paths.core_dir);

      snprintf(default_paths.system_dir, sizeof(default_paths.system_dir), "%s/system", default_paths.core_dir);

      /* now we fill in all the variables */
      snprintf(default_paths.menu_border_file, sizeof(default_paths.menu_border_file), "%s/borders/Menu/main-menu_1080p.png", default_paths.core_dir);
      snprintf(default_paths.input_presets_dir, sizeof(default_paths.input_presets_dir), "%s/presets", default_paths.core_dir);
      snprintf(default_paths.border_dir, sizeof(default_paths.border_dir), "%s/borders", default_paths.core_dir);
#if defined(HAVE_CG) || defined(HAVE_GLSL)
      snprintf(g_settings.video.shader_dir, sizeof(g_settings.video.shader_dir), "%s/shaders", default_paths.core_dir);
#endif

#ifdef IS_SALAMANDER
      snprintf(default_paths.config_path, sizeof(default_paths.config_path), "%s/retroarch.cfg", default_paths.port_dir);
#else
      snprintf(g_extern.config_path, sizeof(g_extern.config_path), "%s/retroarch.cfg", default_paths.port_dir);
#endif
   }

#ifndef IS_SALAMANDER
   rarch_make_dir(default_paths.port_dir, "port_dir");
   rarch_make_dir(default_paths.system_dir, "system_dir");
   rarch_make_dir(default_paths.savestate_dir, "savestate_dir");
   rarch_make_dir(default_paths.sram_dir, "sram_dir");
   rarch_make_dir(default_paths.input_presets_dir, "input_presets_dir");

   config_load();

   rarch_get_environment_console();
#endif
}

static void system_init(void)
{
// FBARL
#ifdef _FBARL_
   config_read_keybinds(g_extern.input_config_path);
#endif

#ifdef HAVE_SYSUTILS
   RARCH_LOG("Registering system utility callback...\n");
   cellSysutilRegisterCallback(0, callback_sysutil_exit, NULL);
#endif

#ifdef HAVE_SYSMODULES

#ifdef HAVE_FREETYPE
   cellSysmoduleLoadModule(CELL_SYSMODULE_FONT);
   cellSysmoduleLoadModule(CELL_SYSMODULE_FREETYPE);
   cellSysmoduleLoadModule(CELL_SYSMODULE_FONTFT);
#endif

   cellSysmoduleLoadModule(CELL_SYSMODULE_IO);
   cellSysmoduleLoadModule(CELL_SYSMODULE_FS);
#ifndef __PSL1GHT__
   cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_GAME);
#endif
#ifndef IS_SALAMANDER
#ifndef __PSL1GHT__
   cellSysmoduleLoadModule(CELL_SYSMODULE_AVCONF_EXT);
#endif
   cellSysmoduleLoadModule(CELL_SYSMODULE_PNGDEC);
   cellSysmoduleLoadModule(CELL_SYSMODULE_JPGDEC);
#endif
   cellSysmoduleLoadModule(CELL_SYSMODULE_NET);
   cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_NP);
#endif

#ifndef __PSL1GHT__
   sys_net_initialize_network();
   sceNpInit(NP_POOL_SIZE, np_pool);
#endif

#ifndef IS_SALAMANDER
#if (CELL_SDK_VERSION > 0x340000) && !defined(__PSL1GHT__)
#ifdef HAVE_SYSMODULES
   cellSysmoduleLoadModule(CELL_SYSMODULE_SYSUTIL_SCREENSHOT);
#endif
#ifdef HAVE_SYSUTILS
   CellScreenShotSetParam screenshot_param = {0, 0, 0, 0};

   screenshot_param.photo_title = "RetroArch PS3";
   screenshot_param.game_title = "RetroArch PS3";
   cellScreenShotSetParameter (&screenshot_param);
   cellScreenShotEnable();
#endif
#ifdef HAVE_SYSUTILS
   //if (g_extern.lifecycle_mode_state & (1ULL << MODE_AUDIO_CUSTOM_BGM_ENABLE))
   cellSysutilEnableBgmPlayback();
#endif
#endif
#endif



// FBARL

}

static int system_process_args(int argc, char *argv[], void *args)
{
#ifndef IS_SALAMANDER
   if (argc > 1)
   {
      RARCH_LOG("Auto-start game %s.\n", argv[1]);
      strlcpy(g_extern.fullpath, argv[1], sizeof(g_extern.fullpath));
#ifdef _FBARL_
	  g_extern.lifecycle_mode_state &= ~(1ULL << MODE_GAME);
	  g_extern.lifecycle_mode_state |= (1ULL << MODE_EXITSPAWN);
	  g_extern.lifecycle_mode_state |= (1ULL << MODE_EXITSPAWN_START_GAME);
#endif
      return 1;
   }
#endif

   return 0;
}

static void system_deinit(void)
{
#ifndef IS_SALAMANDER

#if defined(HAVE_SYSMODULES)
#ifdef HAVE_FREETYPE
   /* Freetype font PRX */
   cellSysmoduleLoadModule(CELL_SYSMODULE_FONTFT);
   cellSysmoduleUnloadModule(CELL_SYSMODULE_FREETYPE);
   cellSysmoduleUnloadModule(CELL_SYSMODULE_FONT);
#endif

#ifndef __PSL1GHT__
   /* screenshot PRX */
   cellSysmoduleUnloadModule(CELL_SYSMODULE_SYSUTIL_SCREENSHOT);
#endif

   cellSysmoduleUnloadModule(CELL_SYSMODULE_JPGDEC);
   cellSysmoduleUnloadModule(CELL_SYSMODULE_PNGDEC);

#ifndef __PSL1GHT__
   /* system game utility PRX */
   cellSysmoduleUnloadModule(CELL_SYSMODULE_AVCONF_EXT);
   cellSysmoduleUnloadModule(CELL_SYSMODULE_SYSUTIL_GAME);
#endif

#endif

#endif
}

static void system_exec(const char *path, bool should_load_game);

static void system_exitspawn(void)
{
#ifdef HAVE_RARCH_EXEC

#ifdef IS_SALAMANDER
   system_exec(default_paths.libretro_path, false);

   cellSysmoduleUnloadModule(CELL_SYSMODULE_SYSUTIL_GAME);
   cellSysmoduleLoadModule(CELL_SYSMODULE_FS);
   cellSysmoduleLoadModule(CELL_SYSMODULE_IO);
#else
   char core_launch[256];
#ifdef HAVE_MULTIMAN 
   if (g_extern.lifecycle_mode_state & (1ULL << MODE_EXITSPAWN_MULTIMAN))
   {
      RARCH_LOG("Boot Multiman: %s.\n", MULTIMAN_SELF_FILE);
      strlcpy(core_launch, MULTIMAN_SELF_FILE, sizeof(core_launch));
   }
   else
#endif
   strlcpy(core_launch, g_settings.libretro, sizeof(core_launch));
   bool should_load_game = false;
   if (g_extern.lifecycle_mode_state & (1ULL << MODE_EXITSPAWN_START_GAME))
      should_load_game = true;

   system_exec(core_launch, should_load_game);
#endif

#endif

}

#include <stdio.h>

#include <cell/sysmodule.h>
#include <sys/process.h>
#include <sysutil/sysutil_common.h>
#include <netex/net.h>
#include <np.h>
#include <np/drm.h>

#include "../../retroarch_logger.h"

static void system_exec(const char *path, bool should_load_game)
{
   (void)should_load_game;

   RARCH_LOG("Attempt to load executable: [%s].\n", path);
   char spawn_data[256];
#ifndef IS_SALAMANDER
   char game_path[256];
   game_path[0] = '\0';
#endif

   for(unsigned int i = 0; i < sizeof(spawn_data); ++i)
      spawn_data[i] = i & 0xff;

#ifndef IS_SALAMANDER
   if (should_load_game)
      strlcpy(game_path, g_extern.fullpath, sizeof(game_path));
#endif

   const char * const spawn_argv[] = {
#ifndef IS_SALAMANDER
      game_path,
#endif
      NULL
   };

   SceNpDrmKey * k_licensee = NULL;
   int ret = sceNpDrmProcessExitSpawn2(k_licensee, path, (const char** const)spawn_argv, NULL, (sys_addr_t)spawn_data, 256, 1000, SYS_PROCESS_PRIMARY_STACK_SIZE_1M);

   if(ret <  0)
   {
      RARCH_WARN("SELF file is not of NPDRM type, trying another approach to boot it...\n");
      sys_game_process_exitspawn(path, (const char** const)spawn_argv, NULL, NULL, 0, 1000, SYS_PROCESS_PRIMARY_STACK_SIZE_1M);
   }

   sceNpTerm();
   sys_net_finalize_network();
   cellSysmoduleUnloadModule(CELL_SYSMODULE_SYSUTIL_NP);
   cellSysmoduleUnloadModule(CELL_SYSMODULE_NET);
}

const frontend_ctx_driver_t frontend_ctx_ps3 = {
   get_environment_settings,     /* get_environment_settings */
   system_init,                  /* init */
   system_deinit,                /* deinit */
   system_exitspawn,             /* exitspawn */
   system_process_args,          /* process_args */
   NULL,                         /* process_events */
   system_exec,                  /* exec */
   NULL,                         /* shutdown */
   "ps3",
};
