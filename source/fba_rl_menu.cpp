// =======================================================================
// UI / MENUS

#include "main.h"

void c_fbaRL::InitMainMenu()
{
	main_menu = new c_Menu(7);

	main_menu->AddItem((char*)"GAME LIST");
	main_menu->AddItem((char*)"OPTIONS");
	main_menu->AddItem((char*)"LOAD \"FB Alpha Core\" MOD");
	main_menu->AddItem((char*)"LOAD \"Iris Manager\"");
	main_menu->AddItem((char*)"LOAD \"multiMAN\"");
	main_menu->AddItem((char*)"EXIT");
}

void c_fbaRL::EndMainMenu()
{
	SAFE_DELETE(main_menu)
}

void c_fbaRL::InitOptionsMenu()
{
	options_menu = new c_Menu(7);

	options_menu->AddItem((char*)"AUTO ASPECT RATIO");
	options_menu->AddItem((char*)"AUTO CREATE ALL INPUT .CFG");
	options_menu->AddItem((char*)"ALTERNATE MENU KEY COMBO");
	options_menu->AddItem((char*)"USE NEO-GEO UNI-BIOS");
	options_menu->AddItem((char*)"DISPLAY MISSING GAMES");
	
	options_menu->AddItem((char*)"FILTER [CAPCOM MISC]");
	options_menu->AddItem((char*)"FILTER [CAVE]");
	options_menu->AddItem((char*)"FILTER [CPS1]");
	options_menu->AddItem((char*)"FILTER [CPS2]");
	options_menu->AddItem((char*)"FILTER [CPS3]");
	options_menu->AddItem((char*)"FILTER [DATA EAST]");
	options_menu->AddItem((char*)"FILTER [GALAXIAN]");
	options_menu->AddItem((char*)"FILTER [IREM]");
	options_menu->AddItem((char*)"FILTER [KANEKO]");
	options_menu->AddItem((char*)"FILTER [KONAMI]");
	options_menu->AddItem((char*)"FILTER [NEO GEO]");
	options_menu->AddItem((char*)"FILTER [PAC-MAN]");
	options_menu->AddItem((char*)"FILTER [PGM]");
	options_menu->AddItem((char*)"FILTER [PSIKYO]");
	options_menu->AddItem((char*)"FILTER [SEGA]");
	options_menu->AddItem((char*)"FILTER [SETA]");
	options_menu->AddItem((char*)"FILTER [TAITO]");
	options_menu->AddItem((char*)"FILTER [TECHNOS]");
	options_menu->AddItem((char*)"FILTER [TOAPLAN]");
	options_menu->AddItem((char*)"FILTER [MISC PRE 90\'s]");
	options_menu->AddItem((char*)"FILTER [MISC POST 90\'s]");

	options_menu->AddItem((char*)"ROMs PATH #1");
	options_menu->AddItem((char*)"ROMs PATH #2");
	options_menu->AddItem((char*)"ROMs PATH #3");
	options_menu->AddItem((char*)"ROMs PATH #4");
	options_menu->AddItem((char*)"ROMs PATH #5");
	options_menu->AddItem((char*)"ROMs PATH #6");
	options_menu->AddItem((char*)"ROMs PATH #7");
	options_menu->AddItem((char*)"ROMs PATH #8");
	options_menu->AddItem((char*)"ROMs PATH #9");
	options_menu->AddItem((char*)"ROMs PATH #10");
	options_menu->AddItem((char*)"ROMs PATH #11");
	options_menu->AddItem((char*)"ROMs PATH #12");
	
	options_menu->AddItem((char*)"[CAPCOM MISC] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[CAVE] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[CPS1] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[CPS2] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[CPS3] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[DATA EAST] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[GALAXIAN] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[IREM] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[KANEKO] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[KONAMI] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[NEO GEO] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[PAC-MAN] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[PGM] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[PSIKYO] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[SEGA] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[SETA] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[TAITO] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[TECHNOS] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[TOAPLAN] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[MISC PRE 90\'s] INPUT .CFG PATH");
	options_menu->AddItem((char*)"[MISC POST 90's] INPUT .CFG PATH");
}

void c_fbaRL::EndOptionsMenu()
{
	SAFE_DELETE(options_menu)
}

void c_fbaRL::InitZipInfoMenu()
{
	if(nFilteredGames < 1) return;

	zipinfo_menu = new c_Menu(34);

	uint32_t		i;
	mz_bool			bZipStatus;
	//size_t			uncomp_size;
	mz_zip_archive	zip_archive;

	// Now try to open the archive.
	memset(&zip_archive, 0, sizeof(zip_archive));
	bZipStatus = mz_zip_reader_init_file(&zip_archive, fgames[nSelectedGame]->path, 0);
	if (!bZipStatus)
	{
		//FILE* fp = fopen("/dev_hdd0/game/FBAL00123/USRDIR/zip_error_log.txt", "w");
		//if(fp) {
		//	fprintf(fp, "mz_zip_reader_init_file() failed!\n");
		//	fclose(fp);
		//	fp = NULL;
		//}

		SAFE_DELETE(zipinfo_menu)

		return;
	}

	// Get and print information about each file in the archive.
	for (i = 0; i < mz_zip_reader_get_num_files(&zip_archive); i++)
	{
		mz_zip_archive_file_stat file_stat;
		if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
		{
			//FILE* fp = fopen("/dev_hdd0/game/FBAL00123/USRDIR/zip_error_log.txt", "w");
			//if(fp) {
			//	fprintf(fp, "mz_zip_reader_file_stat() failed!\n");
			//	fclose(fp);
			//	fp = NULL;
			//}
			mz_zip_reader_end(&zip_archive);

			SAFE_DELETE(zipinfo_menu)

			return;
		}

		char* pszTmp = NULL;
		pszTmp = (char*)malloc(1024);
		memset(pszTmp, 0, 1024);

		sprintf(pszTmp, " [ %s ] [SIZE: %u bytes] [CRC32: %08X] \n", 
			file_stat.m_filename,
			(uint32_t)file_stat.m_uncomp_size, 
			//(uint32_t)file_stat.m_comp_size,
			(uint32_t)file_stat.m_crc32
		);

		zipinfo_menu->AddItem((char*)pszTmp);

		SAFE_FREE(pszTmp)
	}
}

void c_fbaRL::EndZipInfoMenu()
{
	SAFE_DELETE(zipinfo_menu)
}
