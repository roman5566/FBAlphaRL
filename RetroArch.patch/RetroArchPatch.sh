#!/bin/sh -e


CURRENTPATH=`pwd`

cat ../RetroArch.patch/configuration.h.patch >> configuration.h
cat ../RetroArch.patch/configuration.c.patch >> configuration.c
dos2unix retroarch.c
sed 's/config_set_defaults(&p_rarch->g_extern)/config_set_defaults_PS3()/' retroarch.c > retroarch.c.mod
sed 's/(!frontend_driver_set_fork(FRONTEND_FORK_RESTART))/(!frontend_driver_set_fork(FRONTEND_FORK_BACK_RETROLOADER))/' retroarch.c.mod > retroarch.c
sed ':a;N;$!ba;s/if (!command_event_save_core_config(p_rarch,\n                  settings->paths.directory_menu_config))\n            return false;/if (!frontend_driver_set_fork(FRONTEND_FORK_RESTART))\n            return false;\n        command_event(CMD_EVENT_QUIT, NULL);/' retroarch.c > retroarch.c.mod
cat retroarch.c.mod > retroarch.c
sed 's/FRONTEND_FORK_RESTART/FRONTEND_FORK_RESTART, FRONTEND_FORK_BACK_RETROLOADER/' frontend/frontend_driver.h > frontend/frontend_driver.h.mod
cat frontend/frontend_driver.h.mod > frontend/frontend_driver.h
dos2unix frontend/drivers/platform_ps3.c
sed ':a;N;$!ba;s/case FRONTEND_FORK_NONE:/case FRONTEND_FORK_BACK_RETROLOADER:\n         ps3_fork_mode  = FRONTEND_FORK_BACK_RETROLOADER;\n		 break;\n	  case FRONTEND_FORK_NONE:/' frontend/drivers/platform_ps3.c > frontend/drivers/platform_ps3.c.new
sed 's+#include <lv2/process.h>++' frontend/drivers/platform_ps3.c.new > frontend/drivers/platform_ps3.c
sed ':a;N;$!ba;s+frontend_ps3_exec(s, should_load_game);+if (ps3_fork_mode == FRONTEND_FORK_BACK_RETROLOADER)\n        frontend_ps3_exec("/dev_hdd0/game/FBNE00123/USRDIR/RELOAD.SELF", false);\n   else\n        frontend_ps3_exec(s, should_load_game);+' frontend/drivers/platform_ps3.c > frontend/drivers/platform_ps3.c.new
sed ':a;N;$!ba;s/frontend_ps3_exec_exitspawn(path,\n            NULL, NULL);/const char * const spawn2_argv[] = {"gamesList", NULL };\n	  if (strstr(path,"RELOAD.SELF"))\n           frontend_ps3_exec_exitspawn(path, (const char** const)spawn2_argv, NULL);\n	  else\n           frontend_ps3_exec_exitspawn(path, NULL, NULL);/' frontend/drivers/platform_ps3.c.new > frontend/drivers/platform_ps3.c
#cat frontend/drivers/platform_ps3.c.new > frontend/drivers/platform_ps3.c
sed 's/Reset to Defaults/Reset retroarch.cfg to Default/' intl/msg_hash_us.h > intl/msg_hash_us.h.mod
sed 's/Reset the current configuration to default values./Reset configuration file to default values./' intl/msg_hash_us.h.mod > intl/msg_hash_us.h
sed 's/Save New Configuration/Reload retroarch.cfg and Restart/' intl/msg_hash_us.h > intl/msg_hash_us.h.mod
sed 's/"Restart RetroArch"/"Back to FB Neo Plus RL"/' intl/msg_hash_us.h.mod > intl/msg_hash_us.h
sed 's/"Restart the program."/"Back to Retro Loader."/' intl/msg_hash_us.h > intl/msg_hash_us.h.mod
cat intl/msg_hash_us.h.mod > intl/msg_hash_us.h
#rm -f frontend/drivers/platform_ps3.c
#mv frontend/drivers/platform_ps3.c.new frontend/drivers/platform_ps3.c
cd menu
dos2unix.exe menu_displaylist.c
cat ../../RetroArch.patch/menu_displaylist.c.patch | /h/cygwin64/bin/patch
#cat ../../RetroArch.patch/menu_setting.c.patch | /h/cygwin64/bin/patch


echo "Patching done."
