// =======================================================================
// DIALOGS

#include "main.h"

void DlgCallbackFunction(int buttonType, void */*userData*/)
{
	switch( buttonType ) 
	{
	case CELL_MSGDIALOG_BUTTON_YES:
	case CELL_MSGDIALOG_BUTTON_NO:		
	case CELL_MSGDIALOG_BUTTON_ESCAPE:
	case CELL_MSGDIALOG_BUTTON_NONE:
		break;

	default:
		break;
	}
}

void ExitDlgCallbackFunction(int buttonType, void *userData)
{
	int nDialog = (int)userData;

	switch( buttonType ) 
	{
		case CELL_MSGDIALOG_BUTTON_YES:
		{
			if(nDialog == STATUS_ROMSCAN_DLG)
			{
				cellMsgDialogAbort();

				iniWrite();			// save settings

				// delete old cache
				cellFsUnlink("/dev_hdd0/game/FBAL00123/USRDIR/FBA.GAMELIST.CACHE.DAT");
				
				fbaRL->RefreshGameList();	// Refresh gamelist
				break;
			}

			if(nDialog == STATUS_EXIT_MMAN)
			{
				cellMsgDialogAbort();

				iniWrite(); // save settings

				app.onShutdown();

				char path[] = "/dev_hdd0/game/BLES80608/USRDIR/RELOAD.SELF";
				if(fbaRL->FileExist(path)) 
				{ 
					sys_game_process_exitspawn(path, NULL, NULL, NULL, 0, 1000, SYS_PROCESS_PRIMARY_STACK_SIZE_1M);
				}
				fbaRL->EndMainMenu();
				break;
			}

			if(nDialog == STATUS_EXIT_IMANAGER)
			{
				cellMsgDialogAbort();

				iniWrite(); // save settings

				app.onShutdown();

				char path[] = "/dev_hdd0/game/IMANAGER4/USRDIR/iris_manager.self";
				if(fbaRL->FileExist(path)) 
				{ 
					sys_game_process_exitspawn(path, NULL, NULL, NULL, 0, 1000, SYS_PROCESS_PRIMARY_STACK_SIZE_1M);
				}
				fbaRL->EndMainMenu();
				break;
			}

			if(nDialog == STATUS_EXIT_FBARL_RA)
			{
				cellMsgDialogAbort();

				iniWrite(); // save settings

				app.onShutdown();

				char fba_rl_path[]	= "/dev_hdd0/game/FBAL00123/USRDIR/RELOAD.SELF";
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
						(char*)(g_opt_bUseAltMenuKeyCombo ? "yes" : "no")
					);
				}
				fbaRL->EndMainMenu();
				break;
			}

			if(nDialog == STATUS_EXIT)
			{
				cellMsgDialogAbort();

				iniWrite(); // save settings
				app.onShutdown();
				exit(0);
				fbaRL->EndMainMenu();
				break;
			}
			break;
		}

		case CELL_MSGDIALOG_BUTTON_NO:	
		case CELL_MSGDIALOG_BUTTON_ESCAPE:
		case CELL_MSGDIALOG_BUTTON_NONE:
			break;

		default:
			break;
	}
}

void c_fbaRL::DlgDisplayFrame()
{
	switch(nStatus)
	{
		case STATUS_ROMPATH_CHANGED:
		{
			//::cellMsgDialogOpen2(
			//	CELL_MSGDIALOG_TYPE_SE_TYPE_NORMAL, 
			//	"Remeber to rescan ROMs after you finish configuring all paths,\nyou can do so at the Game List section.", 
			//	DlgCallbackFunction, (void*)STATUS_ROMPATH_CHANGED, NULL
			//);

			// Force Re-Scan
			// delete old cache
			cellFsUnlink("/dev_hdd0/game/FBAL00123/USRDIR/FBA.GAMELIST.CACHE.DAT");
			break;		
		}

		case STATUS_ROMSCAN_DLG:
		{
			::cellMsgDialogOpen2(
				CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO|
				CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NO, 
				"Do you want to Rescan all configured paths for ROM(s) ?", 
				ExitDlgCallbackFunction, (void*)STATUS_ROMSCAN_DLG, NULL
			);
			break;
		}

		case STATUS_MISSING_GAME_DLG:
		{
			::cellMsgDialogOpen2(
				CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO|
				CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NO, 
				"Error: There was a problem locating the specified game, if you had it on a USB Storage Device, \n"
				"make sure it is properly connected.\n\n"
				"Do you want to Re-Scan all configured paths for ROM(s) ?",
				ExitDlgCallbackFunction, (void*)STATUS_ROMSCAN_DLG, NULL
			);
			break;
		}

		case STATUS_CORRUPT_APP_DLG:
		{
			::cellMsgDialogOpen2(
				CELL_MSGDIALOG_TYPE_SE_TYPE_NORMAL
				|CELL_MSGDIALOG_TYPE_BUTTON_TYPE_OK
				|CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_ON
				|CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_OK, 
				"Error: FB Alpha RL installation is corrupted, please re-install it.", 
				ExitDlgCallbackFunction, (void*)STATUS_EXIT, NULL
			);

			break;
		}

		case STATUS_EXIT:
		{
			::cellMsgDialogOpen2(
				CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO|
				CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NO, 
				"Do you want to exit FB Alpha RL and return to XMB ?", 
				ExitDlgCallbackFunction, (void*)STATUS_EXIT, NULL
			);
			break;
		}

		case STATUS_EXIT_FBARL_RA:
		{
			::cellMsgDialogOpen2(
				CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO|
				CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NO, 
				"Do you want to exit and load FB Alpha RetroArch (modified core / fb_alpha.SELF) ?",
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

		case STATUS_UPDATEPREVIEW:
		{
			UpdateBurnSelected(fgames[nSelectedGame]->zipname);
			UpdatePreviewImage();
			break;
		}

		case STATUS_RESETPREVIEW:
		{
			ResetPreviewImage();
			break;
		}

		case STATUS_ROMSCAN_START:
		{
			char szMsg[256] = "";
			sprintf(szMsg, "Scanning ROM(s), please wait...\n\nNotice: If you have MAME complete romset, be patient, the process could take a few minutes.");

			::cellMsgDialogOpen2(
				CELL_MSGDIALOG_TYPE_SE_TYPE_NORMAL
				|CELL_MSGDIALOG_TYPE_BUTTON_TYPE_NONE
				|CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_ON
				|CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NONE
				|CELL_MSGDIALOG_TYPE_PROGRESSBAR_SINGLE,
				szMsg,
				DlgCallbackFunction, NULL, NULL
			);

			sys_ppu_thread_t thread_id;

			sys_ppu_thread_create(
				&thread_id,
				_ScanROMs,						// callback function
				0x1337,							// arg
				0,								// priority
				0x300000,							// stack size
				SYS_PPU_THREAD_CREATE_JOINABLE, //
				"ROMScan"						// name
			);
			break;
		}

		case STATUS_ROMSCAN_END:
		{
			cellMsgDialogAbort();
			SaveGameListCache();
			fbaRL->InitFilterList();
			break;
		}
	}

	nStatus = STATUS_NORMAL;
}
