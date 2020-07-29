// =======================================================================
// DIALOGS

#include "main.h"
#include <sysutil/msg.h>
#include <lv2/sysfs.h>
#include <sysutil/sysutil.h>
#include <sys/process.h>



static vs32 dialog_action = 0;
static int dialog = 0;

/*
static void DlgCallbackFunction(msgButton buttonType,void *usrData)
{
	vs32 *p_action = (vs32 *)usrData;
	switch(buttonType) {
		case MSG_DIALOG_BTN_OK:
			*p_action = 1;
			break;
		case MSG_DIALOG_BTN_NO:
		case MSG_DIALOG_BTN_ESCAPE:
			*p_action = 2;
			break;
		case MSG_DIALOG_BTN_NONE:
			*p_action = -1;
			break;
		default:
			break;
	}
}*/

static void dialog_handler(msgButton button,void *usrData) {

	int nDialog =  *((int*)(usrData));
	//printf("Button: %d - %d\n", button,nDialog);

    //printf("Dialog: button: %d - Dialog: %d\n", (int)button, nDialog);
	switch( button )
	{
		case MSG_DIALOG_BTN_YES:
		{
			if (nDialog == STATUS_MISSING_CORE_2 || nDialog == STATUS_MISSING_CORE_4) {
                msgDialogAbort();
                break;
			}

			if(nDialog == STATUS_ROMSCAN_DLG)
			{
				msgDialogAbort();
				/////cellMsgDialogAbort();

				iniWrite();			// save settings

				// delete old cache
				sqlite3 *mdb;
				char buf[256];
				int rc;
				char *errmsg;
				snprintf(buf, sizeof(buf), "/dev_hdd0/game/FBNE00123/USRDIR/retro.db");
                rc = sqlite3_open_v2(buf, &mdb,
                               SQLITE_OPEN_READWRITE,
                               NULL);
                if(mdb == NULL) {
                        printf("Open DB failed!\n");
                        return ;
                    }

                //sqlite3_stmt *stmt;
                snprintf(buf, sizeof(buf), "DELETE FROM games_available");
                rc = sqlite3_exec(mdb, buf, NULL, NULL, &errmsg);
                        if(rc) {
                            printf( "DELETE games_available failed - %s: failed -- %s\n",
                              buf, errmsg);
                            sqlite3_free(errmsg);
                            sqlite3_close(mdb);
                        }

                //printf("\nOld CACHE deleted\n");
                rc = sqlite3_close(mdb);
                if (rc)
                    printf("CLOSE PROBLEM POST DELETE: %d\n",rc);

				//sysFsUnlink("/dev_hdd0/game/FBNE00123/USRDIR/FBA.GAMELIST.CACHE.DAT");

				////cellFsUnlink("/dev_hdd0/game/FBNE00123/USRDIR/FBA.GAMELIST.CACHE.DAT");

				fbaRL->RefreshGameList();	// Refresh gamelist
				break;
			}

			if(nDialog == STATUS_EXIT_MMAN)
			{
				msgDialogAbort();


				char path[] = "/dev_hdd0/game/BLES80608/USRDIR/RELOAD.SELF";
				if(fileExist(path))
				{
					iniWrite(); // save settings

					app.onShutdown();
					sysProcessExitSpawn2( path, NULL, NULL, NULL, 0, 1001, SYS_PROCESS_SPAWN_STACK_SIZE_1M );
				}
				//fbaRL->EndMainMenu();
				break;
			}


			if(nDialog == STATUS_EXIT_IMANAGER)
			{
				msgDialogAbort();


				char path[] = "/dev_hdd0/game/IRISMAN00/USRDIR/RELOAD.SELF";
				if(fileExist(path))
				{
					iniWrite(); // save settings

					app.onShutdown();
					sysProcessExitSpawn2( path, NULL, NULL, NULL, 0, 1001, SYS_PROCESS_SPAWN_STACK_SIZE_1M );
				}
				//fbaRL->EndMainMenu();
				break;
			}

			 /*////
			if(nDialog == STATUS_EXIT_FBARL_RA)
			{
				cellMsgDialogAbort();

				iniWrite(); // save settings

				app.onShutdown();

				char fba_rl_path[]	= "/dev_hdd0/game/FBNE00123/USRDIR/RELOAD.SELF";
				char rom_path[]		= "DUMMY_ARG";
				char preset_cfg[]	= "DUMMY_ARG";
				char aspect_ratio[]	= "DUMMY_ARG";

				if(fbaRL->FileExist(fba_rl_path))
				{
					fbaRL->LaunchFBACore(
						(char*)rom_path,
						(char*)fba_rl_path,
						(char*)preset_cfg,
						(char*)aspect_ratio,
						(char*)(g_opt_bUseAltMenuKeyCombo ? "yes" : "no"),
						(char*)(g_opt_bUseUNIBIOS ? "yes" : "no")  //CRYSTAL
					);
				}
				fbaRL->EndMainMenu();
				break;
			}
            */
			if(nDialog == STATUS_EXIT)
			{
				////cellMsgDialogAbort();
                //msgDialogAbort();
				iniWrite(); // save settings
				app.onShutdown();

				fbaRL->EndMainMenu();
				exit(0);
				break;
			}
			break;
		}

		case MSG_DIALOG_BTN_NO:
		    if (nDialog == STATUS_ROMSCAN_DLG)
                fbaRL->ResetPreviewImage();
		case MSG_DIALOG_BTN_ESCAPE:
		case MSG_DIALOG_BTN_NONE:

			break;

		default:

			break;
	}

	dialog_action = 1;
}

void c_fbaRL::DlgDisplayFrame()
{
	switch(nStatus)
	{
		case STATUS_ROMPATH_CHANGED: // TODO!!!!!!
		{
			//::cellMsgDialogOpen2(
			//	CELL_MSGDIALOG_TYPE_SE_TYPE_NORMAL,
			//	"Remeber to rescan ROMs after you finish configuring all paths,\nyou can do so at the Game List section.",
			//	DlgCallbackFunction, (void*)STATUS_ROMPATH_CHANGED, NULL
			//);

			// Force Re-Scan
			// delete old cache
			//sysFsUnlink("/dev_hdd0/game/FBNE00123/USRDIR/FBA.GAMELIST.CACHE.DAT");
				sqlite3 *mdb;
				char buf[256];
				int rc;
				char *errmsg;
				snprintf(buf, sizeof(buf), "/dev_hdd0/game/FBNE00123/USRDIR/retro.db");
                rc = sqlite3_open_v2(buf, &mdb,
                               SQLITE_OPEN_READWRITE,
                               NULL);
                if(mdb == NULL) {
                        printf("Open DB failed!\n");
                        return ;
                    }

                //sqlite3_stmt *stmt;
                snprintf(buf, sizeof(buf), "DELETE FROM games_available");
                rc = sqlite3_exec(mdb, buf, NULL, NULL, &errmsg);
                        if(rc) {
                            printf( "DELETE games_available failed - %s: failed -- %s\n",
                              buf, errmsg);
                            sqlite3_free(errmsg);
                            sqlite3_close(mdb);
                        }


                rc = sqlite3_close(mdb);
                if (rc)
                    printf("CLOSE PROBLEM POST DELETE: %d\n",rc);

				//sysFsUnlink("/dev_hdd0/game/FBNE00123/USRDIR/FBA.GAMELIST.CACHE.DAT");
				//printf("\nOld CACHE deleted\n");
				nTotalGames = 0;
                break;
		}

		case STATUS_ROMSCAN_DLG:
		{
			//int i = STATUS_ROMSCAN_DLG;
            dialog = STATUS_ROMSCAN_DLG;
			//printf("Pre dialod %d\n",nStatus);
			msgDialogOpen2((msgType)((MSG_DIALOG_BTN_TYPE_YESNO | MSG_DIALOG_DEFAULT_CURSOR_NO)),
                  "Do you want to Rescan all configured paths for ROM(s) ?",
                  dialog_handler,(void *)&dialog,NULL);

            /*dialog_action = 0;
            while(dialog_action == 0) {
                    sysUtilCheckCallback();
                    waitFlip();
                    app.Flip();
            }
            msgDialogAbort();*/
//            ::cellMsgDialogOpen2(
//				CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO|
//				CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NO,
//				"Do you want to Rescan all configured paths for ROM(s) ?",
//				ExitDlgCallbackFunction, (void*)STATUS_ROMSCAN_DLG, NULL
//			);
            //printf("Post dialod %d\n",nStatus);
			break;
		}

        case STATUS_MISSING_CORE_2:
        {
            dialog = STATUS_MISSING_CORE_2;
            msgDialogOpen2((msgType)((MSG_DIALOG_BTN_TYPE_OK
                                |MSG_DIALOG_DISABLE_CANCEL_ON)),
                                "Core SNES9X is missing. Install correct upgrade pack.",dialog_handler,(void *)&dialog,NULL);
            break;
        }

        case STATUS_MISSING_CORE_4:
        {
            dialog = STATUS_MISSING_CORE_4;
            msgDialogOpen2((msgType)((MSG_DIALOG_BTN_TYPE_OK
                                |MSG_DIALOG_DISABLE_CANCEL_ON)),
                                "Core GENESIS PLUS GX is missing. Install correct upgrade pack.",dialog_handler,(void *)&dialog,NULL);
            break;
        }

        case STATUS_MISSING_CORE_5:
        {
            dialog = STATUS_MISSING_CORE_5;
            msgDialogOpen2((msgType)((MSG_DIALOG_BTN_TYPE_OK
                                |MSG_DIALOG_DISABLE_CANCEL_ON)),
                                "Core E-UAE is missing. Install correct upgrade pack.",dialog_handler,(void *)&dialog,NULL);
            break;
        }

		case STATUS_MISSING_GAME_DLG:
		{
			//int i = STATUS_ROMSCAN_DLG;
            dialog = STATUS_ROMSCAN_DLG;
			msgDialogOpen2((msgType)((MSG_DIALOG_BTN_TYPE_YESNO | MSG_DIALOG_DEFAULT_CURSOR_NO)),
                "Error: There was a problem locating the specified game, if you had it on a USB Storage Device, \n"
                "make sure it is properly connected.\n\n"
				"Do you want to Re-Scan all configured paths for ROM(s) ?",
                dialog_handler,(void *)&dialog,NULL);

            /*dialog_action = 0;
            while(dialog_action == 0) {
                    sysUtilCheckCallback();
                    //waitFlip();
                    app.Flip();
            }
            msgDialogAbort();*/
//            ::cellMsgDialogOpen2(
//				CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO|
//				CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NO,
//				"Error: There was a problem locating the specified game, if you had it on a USB Storage Device, \n"
//				"make sure it is properly connected.\n\n"
//				"Do you want to Re-Scan all configured paths for ROM(s) ?",
//				ExitDlgCallbackFunction, (void*)STATUS_ROMSCAN_DLG, NULL
//			);
			break;
		}


		case STATUS_CORRUPT_APP_DLG:
		{
			//int i = STATUS_EXIT;
            //dialog = STATUS_ROMSCAN_DLG;
            dialog = STATUS_EXIT;
			msgDialogOpen2((msgType)((MSG_DIALOG_NORMAL
                                |MSG_DIALOG_BTN_TYPE_OK
                                |MSG_DIALOG_DISABLE_CANCEL_ON)),
                                "Error: FB NEO RL installation is corrupted, please re-install it.",
                                dialog_handler,(void *)&dialog,NULL);

            /*dialog_action = 0;
            while(dialog_action == 0) {
                    sysUtilCheckCallback();
                    waitFlip();
                    app.Flip();

            }
            msgDialogAbort();*/
//            ::cellMsgDialogOpen2(
//				CELL_MSGDIALOG_TYPE_SE_TYPE_NORMAL
//				|CELL_MSGDIALOG_TYPE_BUTTON_TYPE_OK
//				|CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_ON
//				|CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_OK,
//				"Error: FB NEO RL installation is corrupted, please re-install it.",
//				ExitDlgCallbackFunction, (void*)STATUS_EXIT, NULL
//			);

			break;
		}

		case STATUS_EXIT:
		{
            //int i = STATUS_EXIT;
            dialog = STATUS_EXIT;
			msgDialogOpen2((msgType)((MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_YESNO | MSG_DIALOG_DISABLE_CANCEL_ON | MSG_DIALOG_DEFAULT_CURSOR_NO)),
                  "Do you want to exit FB NEO RL and return to XMB ?",
                  dialog_handler,(void *)&dialog,NULL);

			/*////::cellMsgDialogOpen2(
				CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO|
				CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NO,
				"Do you want to exit FB NEO RL and return to XMB ?",
				ExitDlgCallbackFunction, (void*)STATUS_EXIT, NULL
			);*/
			break;
		}

		case STATUS_EXIT_MMAN:
		{
			dialog = STATUS_EXIT_MMAN;
			msgDialogOpen2((msgType)((MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_YESNO | MSG_DIALOG_DISABLE_CANCEL_ON | MSG_DIALOG_DEFAULT_CURSOR_NO)),
				"Do you want to exit and load \"multiMAN\" ?",
				dialog_handler,(void *)&dialog,NULL);
			break;
		}

		case STATUS_EXIT_IMANAGER:
		{
			dialog = STATUS_EXIT_IMANAGER;
			msgDialogOpen2((msgType)((MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_YESNO | MSG_DIALOG_DISABLE_CANCEL_ON | MSG_DIALOG_DEFAULT_CURSOR_NO)),
				"Do you want to exit and load \"Iris Manager\" ?",
				dialog_handler,(void *)&dialog,NULL);
			break;
		}
        /*////
		case STATUS_EXIT_FBARL_RA:
		{
			::cellMsgDialogOpen2(
				CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO|
				CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NO,
				"Do you want to exit and load FB NEO RetroArch (modified core / fb_neo.SELF) ?",
				ExitDlgCallbackFunction, (void*)STATUS_EXIT_FBARL_RA, NULL
			);
			break;
		}

		case STATUS_EXIT_MMAN:
		{
			::cellMsgDialogOpen2(
				CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO|
				CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NO,
				"Do you want to exit and load \"multiMAN\" ?",
				ExitDlgCallbackFunction, (void*)STATUS_EXIT_MMAN, NULL
			);
			break;
		}

		case STATUS_EXIT_IMANAGER:
		{
			::cellMsgDialogOpen2(
				CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO|
				CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NO,
				"Do you want to exit and load \"Iris Manager\" ?",
				ExitDlgCallbackFunction, (void*)STATUS_EXIT_IMANAGER, NULL
			);
			break;
		}
        */
		case STATUS_UPDATEPREVIEW:
		{
			//printf("UPDATEPREVIEW nSelectedGame %d\n",nSelectedGame);
			//printf("fgames[nSelectedGame]->GameID %d\n",fgames[nSelectedGame]->GameID);
			nBurnSelected = fgames[nSelectedGame]->GameID;
			//UpdateBurnSelected(fgames[nSelectedGame]->zipname);
//			printf("AAA %s - %d - %d - BURNSELECTED = %d \n",
//                    fgames[nSelectedGame]->title, nBurnSelected, fgames[nSelectedGame]->nGame,
//                    nBurnSelected);
			UpdatePreviewImage();
			//printf("Dialog UpdatePreviewImage done\n");
			break;
		}

		case STATUS_RESETPREVIEW:
		{
			ResetPreviewImage();
			break;
		}

		case STATUS_ROMSCAN_START:
		{
			//printf("STATUS_ROMSCAN_START\n");
			RomScan2();

            nStatus = STATUS_ROMSCAN_END;
			break;
		}

		case STATUS_ROMSCAN_END:
		{
			//printf("STATUS_ROMSCAN_END\n");
			//msgDialogAbort();
			//cellMsgDialogAbort();
			//printf("SaveGameListCache\n");
			//SaveGameListCache();

			//printf("ENd filter list\n");
			fbaRL->EndFilterList();

			//printf("InitFilterList\n");
			fbaRL->InitFilterList();
			fbaRL->UpdatePreviewImage();
			//printf("Status normal\n");
			nStatus = STATUS_NORMAL;
			break;
		}
	}
    if (nStatus != STATUS_ROMSCAN_END && nStatus != STATUS_ROMSCAN_START )
            nStatus = STATUS_NORMAL;

}
