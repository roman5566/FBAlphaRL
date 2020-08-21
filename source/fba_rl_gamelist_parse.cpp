// =======================================================================
// GAMELIST (PARSING)

#include "main.h"
#include <sysutil/msg.h>
#include <vector>
#include <string>
#include <algorithm>
#include <lv2/sysfs.h>
#include <sys/dirent.h>
#include <sys/stat.h>
#include "sqlite.h"
#include "misc.h"
#include <sys/systime.h>
#include <sysutil/sysutil.h>


#define DIR_SKIP_DOT			1			//dont include "."
#define DIR_SKIP_DOTDOT			2			//dont include ".."
#define DIR_FILES				4			//include files
#define DIR_DIRS				0x10		//include directories
#define DIR_SKIP_HIDDEN			0x20		//skip entries starting with "." besides dot and dotdot
#define DIR_NO_DOT_AND_DOTDOT	( DIR_SKIP_DOT | DIR_SKIP_DOTDOT )


#define _GETTAG_STR_VALUE(_opentag, _closetag, _outstr)	\
	fgets(pszLine, 2048, fp);							\
	pch = strstr(pszLine, _opentag);					\
	if(pch)	{											\
		char* pszOption =  NULL;						\
		pszOption = (char*)malloc(2048+1);				\
		memset(pszOption, 0, 2048+1);					\
		strncpy(pszOption, pch+(strlen(_opentag)), 2048);\
		pch = strstr(pszOption, _closetag);				\
		pszOption[(int)(pch-pszOption)] = 0;			\
		strcpy(_outstr, pszOption);						\
		SAFE_FREE(pszOption)							\
	}

#define _GETTAG_INT_VALUE(_opentag, _closetag, _outint)	\
	fgets(pszLine, 2048, fp);							\
	pch = strstr(pszLine, _opentag);					\
	if(pch)	{											\
		char* pszOption =  NULL;						\
		pszOption = (char*)malloc(2048+1);				\
		memset(pszOption, 0, 2048+1);					\
		strncpy(pszOption, pch+(strlen(_opentag)), 2048);\
		pch = strstr(pszOption, _closetag);				\
		pszOption[(int)(pch-pszOption)] = 0;			\
		sscanf(pszOption, "%"PRIu64, _outint);			\
		SAFE_FREE(pszOption)							\
	}

#define _GETTAG_BOOL_VALUE(_opentag, _closetag, _outbool)\
	fgets(pszLine, 2048, fp);							\
	pch = strstr(pszLine, _opentag);					\
	if(pch)	{											\
		char* pszOption =  NULL;						\
		pszOption = (char*)malloc(2048+1);				\
		memset(pszOption, 0, 2048+1);					\
		strncpy(pszOption, pch+(strlen(_opentag)), 2048);\
		pch = strstr(pszOption, _closetag);				\
		pszOption[(int)(pch-pszOption)] = 0;			\
		if(strcmp(pszOption, "yes")==0) {_outbool = true;}	\
		if(strcmp(pszOption, "no")==0)	{_outbool = false;}	\
		SAFE_FREE(pszOption)							\
	}

/*static void DummyCallbackFunction(msgButton buttonType,void *usrData)
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

int c_fbaRL::ParseGameListCache()
{
	//printf("START ParseGameListCache\n");
	bProcessingGames = true;
    int row = 0;
    char key[KEY_MAX_LENGTH];
    bool flag_available;
    char* str = NULL;
    int ret = 0;

    // New Parse List Chace
    hashmap_map *gamesmap, *drvmap;
    gamesmap =  (hashmap_map *) app.gamesMap;
    drvmap = (hashmap_map *) app.drvMap;
    FBA_DRV *fba_drv;
    FBA_GAMES *fba_games;
    nTotalGames = gamesmap->size;

    if (nTotalGames == 0) {

                bProcessingGames = false;
                return 0;
    }

    int i, j;
    //nMissingGames = 0;
    for (i=0; i < drvmap->table_size; i++)
    {
        if (drvmap->data[i].data != NULL) {
                fba_drv = (FBA_DRV *)drvmap->data[i].data;
                games[row] = new c_game(row);
                //1//snprintf(games[row]->title, 256, "%s", fba_drv->szTitle);
                snprintf(games[row]->name, 128, "%s", fba_drv->szName);		// title
                snprintf(games[row]->sysmask, 32, "%s", fba_drv->szSystemFilter);	// sys filter
                snprintf(games[row]->company, 96, "%s", fba_drv->szCompany);	// sys filter
                snprintf(games[row]->year, 5, "%s", fba_drv->szYear);	// sys filter
                snprintf(games[row]->system, 32, "%s", fba_drv->szSystem);	// sys filter
                snprintf(games[row]->subsystem, 32, "%s", fba_drv->szSubSystem);	// sys filter
                snprintf(games[row]->resolution, 10, "%s", fba_drv->szResolution);	// sys filter
                snprintf(games[row]->aspectratio, 7, "%s", fba_drv->szAspectRatio);	// sys filter
                //fbaRL->games[row]->parent_id = sqlite3_column_int(stmt, COL08);
                games[row]->players = fba_drv->nMaxPlayers;
                games[row]->def_core_id = fba_drv->nDefCoreID;

                games[row]->nSize = i;				// Hasp map position
                games[row]->GameID = fba_drv->nGameID;
                games[row]->bAvailable = fba_drv->isAvailable;
                games[row]->isClone = fba_drv->isClone;
                games[row]->core_id = 0;
                if (fba_drv->isClone)
                    snprintf(games[row]->parent_name, 128,  "%s", fba_drv->szParent);

                games[row]->isFavorite = fba_drv->isFavorite;

                if (!fba_drv->isAvailable) {
                        //nMissingGames++;
                        row++;

                }
                else {
                    flag_available = false;
                    for (j=1; j<=TOTAL_CORES ; j++) {
                            snprintf(key, KEY_MAX_LENGTH, "%s%s%d", fba_drv->szSystem, fba_drv->szName, j);
                            ret = hashmap_get(gamesmap, key, (void**)(&fba_games));
                            if (ret >= 0) {
                                if (flag_available) {
                                    games[row] = new c_game(row);
                                    //1//snprintf(games[row]->title, 256, "%s", fba_drv->szTitle);
                                    snprintf(games[row]->name, 128, "%s", fba_drv->szName);		// title
                                    snprintf(games[row]->sysmask, 32, "%s", fba_drv->szSystemFilter);	// sys filter
                                    snprintf(games[row]->company, 96, "%s", fba_drv->szCompany);	// sys filter
                                    snprintf(games[row]->year, 5, "%s", fba_drv->szYear);	// sys filter
                                    snprintf(games[row]->system, 32, "%s", fba_drv->szSystem);	// sys filter
                                    snprintf(games[row]->subsystem, 32, "%s", fba_drv->szSubSystem);	// sys filter
                                    snprintf(games[row]->resolution, 10, "%s", fba_drv->szResolution);	// sys filter
                                    snprintf(games[row]->aspectratio, 7, "%s", fba_drv->szAspectRatio);	// sys filter
                                    //fbaRL->games[row]->parent_id = sqlite3_column_int(stmt, COL08);
                                    games[row]->players = fba_drv->nMaxPlayers;
                                    games[row]->def_core_id = fba_drv->nDefCoreID;
                                    games[row]->core_id = j;
                                    games[row]->nSize = i;				// hash map position
                                    games[row]->GameID = fba_drv->nGameID;
                                    games[row]->bAvailable = fba_drv->isAvailable;
                                    games[row]->isClone = fba_drv->isClone;
                                    if (fba_drv->isClone)
                                            snprintf(games[row]->parent_name, 128,  "%s", fba_drv->szParent);
                                    games[row]->isFavorite = fba_drv->isFavorite;
                                }
                                else
                                    games[row]->core_id = j;
                                flag_available = true;
                                //snprintf(fbaRL->games[row]->path, 256, "%s", fba_games->szPath);
                                str = strrchr(fba_games->szPath,'/');
                                if (str != NULL)
                                        str++;
                                sprintf(fbaRL->games[row]->zipname, "%s", str);
                                row++;
                            }

                    }


                }






        }

	}
	app.nTotalGamesWithDup = row;
	return 1;

    //OLD PART TO BE REMOVED
    //End New Parse List Chace
    /*
	//check if there is availables games
	int rc = 0;
	sqlite3 *mdb;
	char buf[512];
	snprintf(buf, sizeof(buf), "/dev_hdd0/game/FBNE00123/USRDIR/retro.db");

    //mdb = db_open_cpp(buf);
    rc = sqlite3_open_v2(buf, &mdb,
			   SQLITE_OPEN_READWRITE,
		       NULL);
    if(mdb == NULL) {
            printf("Open DB failed!\n");
            return 1;
    }
    sqlite3_stmt *stmt;


            snprintf(buf, sizeof(buf), "SELECT count(game_id) FROM games_available");
            rc = sqlite3_prepare_v2(mdb, buf, -1, &stmt, 0);
            if( rc != SQLITE_OK ) {
                    printf("Query failed!\n");
                    return 1;
            }
            rc = sqlite3_step(stmt);
            nTotalGames = sqlite3_column_int(stmt, 0);
            //printf("Games available: %d\n", nTotalGames);
            sqlite3_finalize(stmt);
            if (nTotalGames == 0) {

                rc = sqlite3_close(mdb);
                if (rc)
                    printf("CLOSE PROBLEM: count(game_id) %d\n",rc);
                //InitFilterList();
                bProcessingGames = false;
                return 0;
            }


    snprintf(buf, sizeof(buf), "select g.id COL00, g.title COL01, g.order_id  COL02, ga.path COL03, "
                "s.sysmask COL04, g.cloneof COL05, g.aspectratio COL06, g.name COL07, "
                "g.publisher COL08, g.year COL09, g.subsystem COL10, g.players COL11, g.resolution COL12, "
                "g.aspectratio COL13, g.def_core_id COL14 from games g inner join systems s "
                "on g.subsystem = s.name left join games_available ga on g.id = ga.game_id "
                //"left join games_favorite fa on g.order_id = fa.ord_game_id order by order_id");
                "order by order_id");
    rc = sqlite3_prepare_v2(mdb, buf, -1, &stmt, 0);
    if( rc != SQLITE_OK ) {
            printf("Query failed!\n");
            sqlite3_finalize(stmt);
            sqlite3_close(mdb);
            return 1;
    }
    rc = sqlite3_step(stmt);
    row = 0;
    nTotalGames = 0;

    nMissingGames = 0;
    while (rc == SQLITE_ROW) {
                fbaRL->games[row] = new c_game(row);
                sprintf(fbaRL->games[row]->title, "%s", (char *)sqlite3_column_text(stmt, COL01));
                sprintf(fbaRL->games[row]->name, "%s", (char *)sqlite3_column_text(stmt, COL07));		// title
                sprintf(fbaRL->games[row]->sysmask, "%s", (char *)sqlite3_column_text(stmt, COL04));	// sys filter
                snprintf(fbaRL->games[row]->company, 256, "%s", (char *)sqlite3_column_text(stmt, COL08) == NULL ? "" : (char *)sqlite3_column_text(stmt, COL08));	// sys filter
                snprintf(fbaRL->games[row]->year, 5, "%s", (char *)sqlite3_column_text(stmt, COL09));	// sys filter
                snprintf(fbaRL->games[row]->system, 256, "%s", (char *)sqlite3_column_text(stmt, COL10));	// sys filter
                snprintf(fbaRL->games[row]->resolution, 10, "%s", (char *)sqlite3_column_text(stmt, COL12));	// sys filter
                snprintf(fbaRL->games[row]->aspectratio, 7, "%s", (char *)sqlite3_column_text(stmt, COL13));	// sys filter
                //fbaRL->games[row]->parent_id = sqlite3_column_int(stmt, COL08);
                fbaRL->games[row]->players = sqlite3_column_int(stmt, COL11);
                fbaRL->games[row]->def_core_id = sqlite3_column_int(stmt, COL14);

                fbaRL->games[row]->nSize = 0;				// Size in bytes
                fbaRL->games[row]->GameID = sqlite3_column_int(stmt, COL02) - 1;
                if ((char *)sqlite3_column_text(stmt, COL03) == NULL) {
                        nMissingGames++;
                        fbaRL->games[row]->bAvailable = false;
                }
                else {
                    fbaRL->games[row]->bAvailable = true;
                    nTotalGames++;
                    snprintf(fbaRL->games[row]->path, 256, "%s", (char *)sqlite3_column_text(stmt, COL03));
                    str = strrchr((char *)sqlite3_column_text(stmt, COL03),'/');
                    if (str != NULL)
                            //str = ++str;
                            str++;
                    sprintf(fbaRL->games[row]->zipname, "%s", str);

                }
                if (sqlite3_column_text(stmt, COL05) != NULL) { // is a clone?
                        fbaRL->games[row]->isClone = true;
                        snprintf(fbaRL->games[row]->parent_name, 128, "%s", (char *)sqlite3_column_text(stmt, COL05));

                }
                else
                        fbaRL->games[row]->isClone = false;

                //if ((char *)sqlite3_column_text(stmt, COL14) == NULL)
                fbaRL->games[row]->isFavorite = false;
                //else
                        //fbaRL->games[row]->isFavorite = true;

                row++;

                rc = sqlite3_step(stmt);

    }
    app.nTotalGamesWithDup = row;
    sqlite3_finalize(stmt);
    rc = sqlite3_close(mdb);

    if (rc)
        printf("CLOSE PROBLEM:END ParseGameListCache %d\n",rc);
//    printf("END ParseGameListCache\n");
    //InitFilterList();

    snprintf(buf, sizeof(buf), "/dev_hdd0/game/FBNE00123/USRDIR/retro_local.db");

    //mdb = db_open_cpp(buf);
    rc = sqlite3_open_v2(buf, &mdb,
			   SQLITE_OPEN_READONLY,
		       NULL);
    if(mdb == NULL) {
            printf("Open DB failed!\n");
            return 1;
    }

    snprintf(buf, sizeof(buf), "SELECT ord_game_id COL00 FROM games_favorite");
    rc = sqlite3_prepare_v2(mdb, buf, -1, &stmt, 0);
    if( rc != SQLITE_OK ) {
            printf("Query failed!\n");
            sqlite3_finalize(stmt);
            sqlite3_close(mdb);
            return 1;
    }
    rc = sqlite3_step(stmt);
    row = 0;

    while (rc == SQLITE_ROW) {
        row = sqlite3_column_int(stmt, COL00);
        fbaRL->games[row]->isFavorite = true;
        rc = sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    rc = sqlite3_close(mdb);

    if (rc)
        printf("CLOSE PROBLEM:END ParseGameListCache %d\n",rc);
    return 1;
*/
}

int c_fbaRL::SaveGameListCache()
{

	return 0;
}

int c_fbaRL::ParseGameList()
{
	//printf("ParseGameList START\n");
	bProcessingGames = true;
	fbaRL->nStatus = STATUS_ROMSCAN_START;
	//fbaRL->nStatus = STATUS_ROMSCAN_DLG;
	return 1;
}

void c_fbaRL::RefreshGameList()
{
	//printf("EndGameLIST\n");
	EndGameList();
	EndFilterList();
	//printf("InitGameLIST\n");
	InitGameList();
	//printf("InitFILTER start\n");
	InitFilterList();
	//printf("nBurnSelected: %d\n", nBurnSelected);
}

void c_fbaRL::InitGameList()
{
	//printf("InitGameList START\n");

    nTotalGames = 0;
	//nMissingGames = 0;

	bProcessingGames = true;
    //printf("Pre malloc games\n");
	games = (c_game**)malloc(sizeof(c_game) * MAX_GAMES);
	//printf("Post malloc games\n");
	if (games == NULL) {
        printf("Exit memory fail\n");
        exit(0);
	}
	//printf("Malloc games OK\n");
	//memset(games, 0, sizeof(c_game) * MAX_GAMES);

//    printf("Init games 1\n");
	// Check for FBA.GAMELIST.CACHE.DAT...
	if(!ParseGameListCache())
	{
		// Not found, rescan ROMs
//		printf("Init games 2\n");
		ParseGameList();
//		printf("Init games 3\n");
	}
	//InitFilterList();


}

void c_fbaRL::EndGameList()
{
	bProcessingGames = true;
    //printf("Process to delete games array of %d\n", app.nTotalGamesWithDup);

	for(unsigned n = 0; n < app.nTotalGamesWithDup; n++)
	{
		if (games[n] != NULL) {
            //delete (games[n]);
            SAFE_DELETE(games[n])

            }
	}
	SAFE_FREE(games);
    app.remakegamesMaps();
    //printf("hashmap_free(app.gamesMap)\n");


    nTotalGames = 0;


	bProcessingGames = false;

	//EndFilterList();
}


void c_fbaRL::RomScan2() {
     int rc = 1;

    sqlite3 *mdb;
    char buf[256];
    char szMsg[256] = "";
    sprintf(szMsg, "Scanning ROM(s), please wait...\n\nNotice: If you have MAME complete romset, be patient, the process could take a few minutes.");
    vs32 progressbar_action = 0;
    char sql[256];
    //char pszFilePath[256];
    u64 tsec1, tnsec1, tsec2, tnsec2;
    hashmap_map *gamesmap, *drvmap;
    gamesmap =  (hashmap_map *) app.gamesMap;
    drvmap = (hashmap_map *) app.drvMap;
    FBA_DRV *fba_drv;
    FBA_GAMES *fba_games;

    msgDialogOpen2((msgType)(MSG_DIALOG_DOUBLE_PROGRESSBAR | MSG_DIALOG_DISABLE_CANCEL_ON | MSG_DIALOG_MUTE_ON),
                   szMsg, NULL, NULL, NULL);
	msgDialogProgressBarSetMsg(MSG_PROGRESSBAR_INDEX0,"");
	msgDialogProgressBarReset(MSG_PROGRESSBAR_INDEX0);
	msgDialogProgressBarSetMsg(MSG_PROGRESSBAR_INDEX1,"");
	msgDialogProgressBarReset(MSG_PROGRESSBAR_INDEX1);
	msgDialogProgressBarInc(MSG_PROGRESSBAR_INDEX0,0);
	msgDialogProgressBarInc(MSG_PROGRESSBAR_INDEX1,0);
    //waitFlip();
    app.Flip();


    snprintf(buf, sizeof(buf), "/dev_hdd0/game/FBNE00123/USRDIR/retro.db");

    rc = sqlite3_open_v2(buf, &mdb,
			   SQLITE_OPEN_READWRITE,
		       NULL);

    if(mdb == NULL) {
            printf("Open DB failed!\n");
            return ;
        }
    while(!progressbar_action) {

        sqlite3_stmt *stmt;

        snprintf(sql, sizeof(sql), "DELETE FROM games_available");
        rc = sqlite3_prepare_v2(mdb, sql, -1, &stmt, 0);
        if( rc != SQLITE_OK ) {
                printf("DELETE games_available failed!\n");
                return ;
        }
        rc = sqlite3_step(stmt);

        sqlite3_finalize(stmt);


        int coreid;
        char system[64];
        char *lower = NULL;
        std::vector<std::string> dirs;
        std::vector<std::string>::iterator it;
        uint32_t nPaths = sizeof(g_opt_szROMPaths) / 2048;

        size_t found ;

        int error;
        int nDrv = 0;
        char *errmsg;
        int barUp = 0;
        int barDown = 0;
        char msg[256] = "";
        char key[KEY_MAX_LENGTH];
        nTotalGames = 0;
        int dup = 0;

        uint32_t x;

        for(x = 0; x < (uint32_t) drvmap->table_size; x++) {  // Reset avilable
            if(drvmap->data[x].in_use == 1) {
                fba_drv = (FBA_DRV *)drvmap->data[x].data;
                fba_drv->isAvailable = false;
            }
        }

        for(x = 0; x < nPaths; x++) {
            usleep(1000);
            barUp++;
            msgDialogProgressBarReset(MSG_PROGRESSBAR_INDEX1);
            sprintf(msg,"%d / %d - Folders.", barUp, nPaths );
            msgDialogProgressBarSetMsg(MSG_PROGRESSBAR_INDEX0,msg);
            msgDialogProgressBarInc(MSG_PROGRESSBAR_INDEX0,(int)100/nPaths);

            if(dirExist(g_opt_szROMPaths[x])) {
                //printf("DIR: %s\n", g_opt_szROMPaths[x]);
                dirs.clear();
                dirs = readDir( g_opt_szROMPaths[x], DIR_NO_DOT_AND_DOTDOT | DIR_FILES);
                it = dirs.begin();
                barDown = 0;
                while( it < dirs.end()) {

                    barDown++;

                    found = (*it).find_last_of(".");
                    if (found > 0) {
                        sysGetCurrentTime(&tsec1, &tnsec1);
                        lower = toLowerCase(g_opt_szROMPaths[x],strlen(g_opt_szROMPaths[x]));
                        if (strstr(lower, "mame125")) {
                                coreid = 3;
                                snprintf(system, sizeof(system), "mame");
                        }
                        else
                                if (strstr(lower, "snes")) {
                                            coreid = 2;
                                            snprintf(system, sizeof(system), "snes");
                                }
                                else {
                                        if (strstr(lower, "megadriv")) {
                                            coreid = 4;
                                            snprintf(system, sizeof(system), "megadrive");
                                        }
                                        else {
                                            if (strstr(lower, "amiga")) {
                                                coreid = 5;
                                                snprintf(system, sizeof(system), "amiga");
                                            }
                                            else {
                                                if (strstr(lower, "neocd")) {
                                                    coreid = 1;
                                                    snprintf(system, sizeof(system), "neocd");
                                                }
                                                else {
                                                        coreid = 1;
                                                        snprintf(system, sizeof(system), "mame");
                                                    }
                                            }
                                        }
                                }


    //                    printf("GGGame: %s \n", (*it).substr(0,found).c_str());
                        free(lower);
                        lower = toLowerCase((char *)(*it).substr(0,found).c_str(), (*it).substr(0,found).length());
                        snprintf(key, KEY_MAX_LENGTH, "%s%s", system, lower);
                        free(lower);

                        if (((*it).substr((*it).find_last_of(".") + 1) != "bin" && (*it).substr((*it).find_last_of(".") + 1) != "BIN") || strcmp(system, "neocd") != 0)
                            if (hashmap_get(drvmap, key, (void**)(&fba_drv)) >= 0) {
                                    //printf("Trovato: %s\n", (*it).c_str());
                                    fba_games = (FBA_GAMES*) malloc(sizeof(FBA_GAMES));
                                    if (fba_games == NULL) {
                                                printf("Exit.... memory error malloc(sizeof(FBA_GAMES)\n");
                                                exit(0);
                                    }
                                    snprintf(fba_games->szPath, 256, "%s/%s",g_opt_szROMPaths[x], (*it).c_str());
                                    snprintf(fba_games->key_string, KEY_MAX_LENGTH, "%s%s%d",system, fba_drv->szName, coreid);
                                    error = hashmap_put(gamesmap, fba_games->key_string, fba_games);
                                    if (error == MAP_OK) {
                                        //printf("Error: %d - %s\n", error, fba_games->szPath);
                                        nTotalGames++;
                                    }
                                    else
                                        dup++;
                                    fba_drv->isAvailable = true;
                                    snprintf(sql, sizeof(sql), "INSERT INTO games_available (game_id, path, coreid) "
                                                "values(%d, '%s', %d)",fba_drv->nGameID , fba_games->szPath, coreid);
                                    rc = sqlite3_exec(mdb, sql, NULL, NULL, &errmsg);

                            }


                        if (barDown % 100 == 0) {
                            sysGetCurrentTime(&tsec2, &tnsec2);
                            sprintf(msg,"%d / %d - Files. Time: %f", barDown, static_cast<int>(dirs.size()), (double) (tsec2 - tsec1) +
                                            (tsec2 - tsec1) > 0 ? (double)((long)(1000000000L - tnsec2 + tnsec1))/1000000000L : (double)((long)(tnsec2 - tnsec1))/1000000000L);
                            msgDialogProgressBarSetMsg(MSG_PROGRESSBAR_INDEX1,msg);
                            msgDialogProgressBarInc(MSG_PROGRESSBAR_INDEX1,(int)100/static_cast<int>(dirs.size()));

                            waitFlip();
                            app.Flip();
                            sysUtilCheckCallback();
                        //usleep(1000);
                        }
                    }

                    ++it;


                }
                nDrv++;
            }


        }

        snprintf(sql, sizeof(sql), "select count(g.id) from games g left join games_available ga on g.id = ga.game_id");
        rc = sqlite3_prepare_v2(mdb, sql, -1, &stmt, 0);
        if( rc != SQLITE_OK ) {
                printf("Query failed!\n");
                sqlite3_finalize(stmt);
                sqlite3_close(mdb);
                return ;
        }
        rc = sqlite3_step(stmt);
        app.nTotalGamesWithDup = sqlite3_column_int(stmt, 0);
        app.nMissingGames = app.nTotalGamesWithDup - nTotalGames - dup;

        sqlite3_finalize(stmt);
        rc = sqlite3_close(mdb);
        //printf("nTotalGamesWithDup: %d - nTotalGames: %d - nMissingGames: %d\n", app.nTotalGamesWithDup, nTotalGames, app.nMissingGames);

        if (rc)
            printf("CLOSE PROBLEM:select count(*) %d\n",rc);

        break;
    }
    hashmap_free(app.drvMap);
    hashmap_free(app.gamesMap);
    app.drvMap = hashmap_new();
    app.gamesMap = hashmap_new();
    app.InitDB();
    msgDialogAbort();
    waitFlip();
    bProcessingGames = false;
    nStatus = STATUS_ROMSCAN_END;

    ParseGameListCache();

}



