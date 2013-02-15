// ---------------------------------------------------------------------
// FB Alpha Retro Loader (CaptainCPS-X, 2013)
// ---------------------------------------------------------------------
#include "main.h"

void CapApp::InputFrameStart()
{
	npad_read();
}

void CapApp::InputFrameEnd()
{
	for(int x = 0; x < 16; x++)	{
		mIsButtPressed[x] = buttPressedNow[x];
	}
}

int CapApp::InputInit()
{
	// Init GamePad
	int ret = 0;

	ret = cellSysmoduleLoadModule( CELL_SYSMODULE_IO );
	if ( ret != CELL_OK ) {
		//FILE* fp = fopen("/dev_hdd0/game/PSGL00123/USRDIR/inputlog.txt", "w");
		//if(fp) fprintf(fp, "input module failed \n");
		//if(fp) fclose(fp);
		return ret;
	}

	ret = cellPadInit( 7 );
	if ( ret != CELL_PAD_OK ) {
		//FILE* fp = fopen("/dev_hdd0/game/PSGL00123/USRDIR/inputlog.txt", "w");
		//if(fp) fprintf(fp, "pad init failed \n");
		//if(fp) fclose(fp);
		return ret;
	}

	return 0;
}

#define GET_PAD_D1(BT_X, CELL_PAD_CTRL_X)	\
	if(pPad->button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_X)	\
	{	\
		buttPressedNow[BT_X] = true;	\
	} else {	\
		buttPressedNow[BT_X] = false;	\
	}

#define GET_PAD_D2(BT_X, CELL_PAD_CTRL_X)	\
	if(pPad->button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_X)	\
	{	\
		buttPressedNow[BT_X] = true;	\
	} else {	\
		buttPressedNow[BT_X] = false;	\
	}


int CapApp::npad_read()
{
	int32_t nRet = CELL_PAD_OK;

	// Get gamepads information
	CellPadInfo2 gpad_info;
	nRet = cellPadGetInfo2(&gpad_info);

	if(nRet != CELL_PAD_OK) {
		// error
		return 0;
	}

	// Check if we have 1 - 7 controllers connected
	if(gpad_info.now_connect >= 1 && gpad_info.now_connect <= 7)
	{
		// Use controller 1 for operation (for now)
		if(gpad_info.port_status[0] & CELL_PAD_STATUS_CONNECTED)
		{
			// Controller 1 is connected

			// Get Buttons data [pad 1]
			CellPadData* pPad = NULL;
			pPad = (CellPadData*)malloc(sizeof(CellPadData));

			if(cellPadGetData(0, pPad) == CELL_PAD_OK)
			{
				if(pPad->len == 0) return 0;

				GET_PAD_D1(BT_LEFT	, CELL_PAD_CTRL_LEFT);
				GET_PAD_D1(BT_RIGHT	, CELL_PAD_CTRL_RIGHT);
				GET_PAD_D1(BT_DOWN	, CELL_PAD_CTRL_DOWN);
				GET_PAD_D1(BT_UP	, CELL_PAD_CTRL_UP);
				GET_PAD_D1(BT_START	, CELL_PAD_CTRL_START);
				GET_PAD_D1(BT_R3	, CELL_PAD_CTRL_R3);
				GET_PAD_D1(BT_L3	, CELL_PAD_CTRL_L3);
				GET_PAD_D1(BT_SELECT, CELL_PAD_CTRL_SELECT);

				GET_PAD_D2(BT_SQUARE	, CELL_PAD_CTRL_SQUARE);
				GET_PAD_D2(BT_CROSS		, CELL_PAD_CTRL_CROSS);
				GET_PAD_D2(BT_CIRCLE	, CELL_PAD_CTRL_CIRCLE);
				GET_PAD_D2(BT_TRIANGLE	, CELL_PAD_CTRL_TRIANGLE);
				GET_PAD_D2(BT_R1		, CELL_PAD_CTRL_R1);
				GET_PAD_D2(BT_L1		, CELL_PAD_CTRL_L1);
				GET_PAD_D2(BT_R2		, CELL_PAD_CTRL_R2);
				GET_PAD_D2(BT_L2		, CELL_PAD_CTRL_L2);

				// analog controls

				mValRStickX = (uint32_t)pPad->button[CELL_PAD_BTN_OFFSET_ANALOG_RIGHT_X];
				mValRStickY = (uint32_t)pPad->button[CELL_PAD_BTN_OFFSET_ANALOG_RIGHT_Y];
				mValLStickX = (uint32_t)pPad->button[CELL_PAD_BTN_OFFSET_ANALOG_LEFT_X];
				mValLStickY = (uint32_t)pPad->button[CELL_PAD_BTN_OFFSET_ANALOG_LEFT_Y];
			}
		}

	} else {
		// todo: report user to connect a controller 
	}

	return 1;
}

void CapApp::InputExit()
{
	cellPadEnd();
}