/*
events.c - SDL event system handlers
Copyright (C) 2015-2017 a1batross

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/
#if !XASH_DEDICATED
#include <ctype.h>

#include "keydefs.h"

#include <MSFS\MSFS.h>
#include <MSFS\Legacy\gauges.h>
#include <MSFS/MSFS_WindowsTypes.h>
#include <SimConnect.h>

//#include "stm32h7xx_hal.h"
//#include "stm32h747i_discovery.h"

//#include "../MiniFB.h"

//extern struct mfb_window* window;


#define JOYSTATES 6

static unsigned char joystate[JOYSTATES];
static const char* cmds[JOYSTATES] =
{
		"jump",
		"back",
		"left",
		"right",
		"forward",
		"attack\n+use\n-use"
};


static char input_rcv_buf_aaaa[256];
extern char serial_rcv_buf[];
extern volatile int rcv_buf_rdy;

extern volatile int input_event_count;

static int mouse_x, relx;
static int mouse_y, rely;




HANDLE g_hSimConnect;


static enum GROUP_ID {
	GROUP0,
};

static enum INPUT_ID {
	INPUT0,
};

typedef struct KeyEvent
{
	const char* fs_name;
	unsigned char game_key;
} KeyEvent;

static KeyEvent event_list[] =
{
	{"Backspace", K_BACKSPACE},
	/*
	{"VK_ENTER", K_ENTER},
	{"VK_Enter", K_ENTER},
	*/
	{"Enter", K_ENTER},
	{"Tab", K_TAB},
	/*
	{"Return", K_ENTER},
	{"VK_RETURN", K_ENTER},
	{"VK_Return", K_ENTER},
	*/
	{"VK_LSHIFT", K_SHIFT},
	{"VK_LCONTROL", K_CTRL},
	{"VK_NUMPAD7", K_ESCAPE},
	{"Space", K_SPACE},
	{"VK_UP", K_UPARROW},	
	{"VK_DOWN", K_DOWNARROW},
	{"VK_RIGHT", K_RIGHTARROW},
	{"VK_LEFT", K_LEFTARROW},
	{"VK_NUMPAD5", '~'} // Console key
	/*
	{"VK_NUMPAD5", KEY_FIRE},
	{"VK_NUMPAD0", KEY_ENTER},
	{"VK_NUMPAD1", KEY_USE},
	*/
};
const size_t event_count = sizeof(event_list) / sizeof(event_list[0]);
const size_t key_event_offset = 0x1000;

extern "C" void Key_Event(int key, int down);
extern "C" void CL_CharEvent(int key);
extern "C" void IN_MouseEvent(int key, int down);
extern "C" int Con_Visible(void);

void CALLBACK dispatchMessage(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext)
{
	switch (pData->dwID)
	{
	case SIMCONNECT_RECV_ID_EVENT:
	{
		SIMCONNECT_RECV_EVENT* evt = (SIMCONNECT_RECV_EVENT*)pData;

		if (evt->uEventID < event_count)
		{
			if (Con_Visible() && evt->dwData)
			{
				if (event_list[evt->uEventID].game_key == K_SPACE)	
					CL_CharEvent(' ');
				else if (event_list[evt->uEventID].game_key == K_BACKSPACE)
					Key_Event(event_list[evt->uEventID].game_key, evt->dwData);
				else if (event_list[evt->uEventID].game_key == K_ENTER)
					Key_Event(event_list[evt->uEventID].game_key, evt->dwData);
				else
					Key_Event(event_list[evt->uEventID].game_key, evt->dwData);
			}
			else
				Key_Event(event_list[evt->uEventID].game_key, evt->dwData);
			fprintf(stderr, "Event ID: %d (%d), %d\n", evt->uEventID, event_list[evt->uEventID].game_key, evt->dwData);
		}
		else if (evt->uEventID >= key_event_offset)
		{
			char base_c = evt->uEventID - key_event_offset;
			if (isupper(base_c))
				break;
			char c = tolower(evt->uEventID - key_event_offset);

			if (!Con_Visible())
				Key_Event(c, evt->dwData);
			if (evt->dwData && Con_Visible())
				CL_CharEvent(c);
			fprintf(stderr, "Event text: %c, %d, %d\n", base_c, evt->dwData, Con_Visible());
		}
		break;
	}

	default:
		break;
	}
}

void Platform_InitEvents(void)
{

	HRESULT hr = SimConnect_Open(&g_hSimConnect, "HL_GAUGE", (HWND)0, 0, 0, 0);

	for (unsigned ev = 0; ev < event_count; ++ev)
	{
		hr = SimConnect_MapClientEventToSimEvent(g_hSimConnect, ev);

		hr = SimConnect_MapInputEventToClientEvent(g_hSimConnect, INPUT0, event_list[ev].fs_name, ev, 1, ev, 0, true);

		hr = SimConnect_AddClientEventToNotificationGroup(g_hSimConnect, GROUP0, ev, true);
	}

	// register text input events
	for (unsigned char c = 0; c <= 127; ++c)
	{
		if (!isprint(c))
			continue;

		const uint32_t ev = key_event_offset + c;
		hr = SimConnect_MapClientEventToSimEvent(g_hSimConnect, ev);

		char name_buf[2];
		name_buf[0] = c;
		name_buf[1] = '\0';
		hr = SimConnect_MapInputEventToClientEvent(g_hSimConnect, INPUT0, name_buf, ev, 1, ev, 0, true);

		hr = SimConnect_AddClientEventToNotificationGroup(g_hSimConnect, GROUP0, ev, true);
	}

	hr = SimConnect_SetNotificationGroupPriority(g_hSimConnect, GROUP0, SIMCONNECT_GROUP_PRIORITY_HIGHEST);
	hr = SimConnect_SetInputGroupState(g_hSimConnect, INPUT0, SIMCONNECT_STATE_ON);


	hr = SimConnect_CallDispatch(g_hSimConnect, dispatchMessage, 0);
}


static bool left_click_pressed = false;
static bool right_click_pressed = false;
static bool left_click_released = false;
static bool right_click_released= false;
static float offset_x, offset_y;
static bool right_button_down = false;

static int events_initialized = 0;
static int mousecoords_init = 0;

extern "C"
{

	MSFS_CALLBACK void Attitude_mouse_callback(float fX, float fY, unsigned int iFlags)
	{
		switch (iFlags)
		{
		case MOUSE_LEFTDRAG:
			left_click_pressed = 1;
			break;
		case MOUSE_LEFTRELEASE:
			left_click_released = 1;
			break;

		case MOUSE_RIGHTDRAG:
			right_click_pressed = 1;
			right_button_down = 1;
			break;
		case MOUSE_RIGHTRELEASE:
			right_click_released = 1;
			right_button_down = 0;
			break;
		}

		// right clicks seem to sometimes break the coordinates
		if (!right_button_down)
		{
			if (mousecoords_init)
			{
				relx = mouse_x - fX;
				rely = mouse_y - fY;
			}
			mouse_x = fX;
			mouse_y = fY;
		}
		mousecoords_init = 1;
		//fprintf(stderr, "Mouse event: %f %f, off %f, %f\n", mouse_x, mouse_y, offset_x, offset_y);
		//fprintf(stderr, "Rel: %d, %d\n", relx, rely);
	}


	void Platform_RunEvents(void)
	{
		if (!events_initialized)
		{
			Platform_InitEvents();
			events_initialized = 1;
		}

		SimConnect_CallDispatch(g_hSimConnect, dispatchMessage, nullptr);

		if (left_click_pressed)
			IN_MouseEvent(0, 1);
		if (left_click_released)
			IN_MouseEvent(0, 0);
		if (right_click_pressed)
			IN_MouseEvent(1, 1);
		if (right_click_released)
			IN_MouseEvent(1, 0);

		left_click_pressed = left_click_released 
			= right_click_pressed = right_click_released = 0;
	}

	void* Platform_GetNativeObject(const char* name)
	{
		return 0; // SDL don't have it
	}

	/*
	========================
	Platform_PreCreateMove

	this should disable mouse look on client when m_ignore enabled
	TODO: kill mouse in win32 clients too
	========================
	*/
	void Platform_PreCreateMove(void)
	{
	}

	void Platform_MouseMove(float* x, float* y)
	{
		float factor = -16;

		*x = relx * factor;
		*y = rely * factor;
		relx = rely = 0;
	}

	void Platform_GetMousePos(int* x, int* y)
	{
		*x = mouse_x;
		*y = mouse_y;
	}

}

#endif //  defined( XASH_SDL ) && !XASH_DEDICATED
