/*
vid_sdl.c - SDL vid component
Copyright (C) 2018 a1batross

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

#include "common.h"
#include "client.h"
#include "mod_local.h"
#include "input.h"
#include "vid_common.h"


static vidmode_t vidmodes[] = {{.width = 640, .height = 480, .desc = "640x480"}};
static int num_vidmodes = 1;
struct mfb_window* window;

uint8_t* half_life_fb;

qboolean SW_CreateBuffer( int width, int height, uint *stride, uint *bpp, uint *r, uint *g, uint *b )
{
	if (half_life_fb)
		free(half_life_fb);
	half_life_fb = calloc(width * height, 4);

	*stride = width;

#if 0
	*bpp = 4;
	/*
	*r = 0x00FF0000;
	*g = 0x0000FF00;
	*b = 0x000000FF;
	*/
	*b = 31;
	*g = 63 << 5;
	*r = 31 << 11;


#else // RGB565
	*bpp = 2;
	*b = 31;
	*g = 63 << 5;
	*r = 31 << 11;
	// LTDC_PIXEL_FORMAT_L8

#endif

	return true;
}

void *SW_LockBuffer( void )
{
	return half_life_fb;
}

void SW_UnlockBuffer( void )
{

}

int R_MaxVideoModes( void )
{
	return num_vidmodes;
}

vidmode_t *R_GetVideoMode( int num )
{
	if (num < num_vidmodes)
		return &vidmodes[num];
	return NULL;
}

static void R_InitVideoModes( void )
{
}

static void R_FreeVideoModes( void )
{
}


/*
=================
GL_GetProcAddress
=================
*/
void *GL_GetProcAddress( const char *name )
{
	return NULL;
}

/*
===============
GL_UpdateSwapInterval
===============
*/
void GL_UpdateSwapInterval( void )
{
}

/*
=================
GL_DeleteContext

always return false
=================
*/
qboolean GL_DeleteContext( void )
{
	return false;
}

/*
=================
GL_CreateContext
=================
*/
qboolean GL_CreateContext( void )
{
	return true;
}

/*
=================
GL_UpdateContext
=================
*/
qboolean GL_UpdateContext( void )
{
	return true;
}

void VID_SaveWindowSize( int width, int height )
{
	int render_w = width, render_h = height;
	uint rotate = vid_rotate->value;

	R_SaveVideoMode( width, height, render_w, render_h );
}

static qboolean VID_SetScreenResolution( int width, int height )
{
	return true;
}

void VID_RestoreScreenResolution( void )
{
}
/*
static void keyboard(struct mfb_window* window, mfb_key key, mfb_key_mod mod, bool isPressed) {
	Key_Event(key, isPressed);
}

static void char_input(struct mfb_window* window, unsigned int charCode) {
	CL_CharEvent(charCode);
}

static void mouse_btn(struct mfb_window* window, mfb_mouse_button button, mfb_key_mod mod, bool isPressed) {
	IN_MouseEvent(button, isPressed);
}

// Use wisely this event. It can be sent too often
static void mouse_move(struct mfb_window* window, int x, int y) {

}

// Mouse wheel
static void mouse_scroll(struct mfb_window* window, mfb_key_mod mod, float deltaX, float deltaY) {
	IN_MWheelEvent((int)deltaY);
}
*/
/*
=================
VID_CreateWindow
=================
*/
qboolean VID_CreateWindow( int width, int height, qboolean fullscreen )
{
	/*
	window = mfb_open_ex("Half-Life", width, height, fullscreen ? WF_FULLSCREEN : 0);
	if (!window)
		return false;

	mfb_set_keyboard_callback(window, keyboard);
	mfb_set_char_input_callback(window, char_input);
	mfb_set_mouse_button_callback(window, mouse_btn);
	mfb_set_mouse_move_callback(window, mouse_move);
	mfb_set_mouse_scroll_callback(window, mouse_scroll);
*/
	return true;
}

/*
=================
VID_DestroyWindow
=================
*/
void VID_DestroyWindow( void )
{
	//mfb_close(window);
}

/*
==================
GL_SetupAttributes
==================
*/
static void GL_SetupAttributes( void )
{
}

void GL_SwapBuffers( void )
{
}

int GL_SetAttribute( int attr, int val )
{
	return 0;
}

int GL_GetAttribute( int attr, int *val )
{
	return 0;
}
/*
==================
R_Init_Video
==================
*/
qboolean R_Init_Video( const int type )
{
	string safe;
	qboolean retval;
	refState.desktopBitsPixel = 16;

	glw_state.software = true;

	if( !(retval = VID_SetMode()) )
	{
		return retval;
	}

	R_InitVideoModes();

	host.renderinfo_changed = false;

	return true;
}

rserr_t R_ChangeDisplaySettings( int width, int height, qboolean fullscreen )
{
	return rserr_ok;
}

/*
==================
VID_SetMode

Set the described video mode
==================
*/
qboolean VID_SetMode( void )
{
	return true;
}

/*
==================
R_Free_Video
==================
*/
void R_Free_Video( void )
{
	GL_DeleteContext ();

	VID_DestroyWindow ();

	R_FreeVideoModes();

	ref.dllFuncs.GL_ClearExtensions();
}

#endif // XASH_DEDICATED
