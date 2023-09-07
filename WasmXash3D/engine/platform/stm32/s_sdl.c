/*
s_backend.c - sound hardware output
Copyright (C) 2009 Uncle Mike

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "common.h"
#include "platform/platform.h"

#include "sound.h"
#include "voice.h"


#define MAX_SAMPLE_COUNT 0x4000
#define AUDIO_BUFFER_SIZE            (1024*2*2)

/* Private typedef -----------------------------------------------------------*/
typedef enum {
  AUDIO_STATE_IDLE = 0,
  AUDIO_STATE_INIT,
  AUDIO_STATE_PLAYING,
  AUDIO_STATE_PAUSE
}AUDIO_PLAYBACK_StateTypeDef;

typedef enum {
  BUFFER_OFFSET_NONE = 0,
  BUFFER_OFFSET_HALF,
  BUFFER_OFFSET_FULL,
}BUFFER_StateTypeDef;

typedef struct {
  uint8_t buff[AUDIO_BUFFER_SIZE];
  BUFFER_StateTypeDef state;
}AUDIO_BufferTypeDef;

typedef enum {
  AUDIO_ERROR_NONE = 0,
  AUDIO_ERROR_NOTREADY,
  AUDIO_ERROR_IO,
  AUDIO_ERROR_EOF,
}AUDIO_ErrorTypeDef;


uint8_t AUDIO_Init()
{

	  return 0;
}

void AUDIO_FillBuffers(void* ptr, unsigned int bytes)
{

}

uint8_t AUDIO_Process(void)
{
	return 0;
}


/*
==================
SNDDMA_Init

Try to find a sound device to mix for.
Returns false if nothing is found.
==================
*/

qboolean SNDDMA_Init( void )
{

	return true;
}


/*
==============
SNDDMA_BeginPainting

Makes sure dma.buffer is valid
===============
*/
void SNDDMA_BeginPainting( void )
{
//	SDL_LockAudioDevice( sdl_dev );
}

/*
==============
SNDDMA_Submit

Send sound to device if buffer isn't really the dma buffer
Also unlocks the dsound buffer
===============
*/
void SNDDMA_Submit( void )
{
//	SDL_UnlockAudioDevice( sdl_dev );
}

/*
==============
SNDDMA_Shutdown

Reset the sound device for exiting
===============
*/
void SNDDMA_Shutdown( void )
{
}


/*
===========
SNDDMA_Activate
Called when the main window gains or loses focus.
The window have been destroyed and recreated
between a deactivate and an activate.
===========
*/
void SNDDMA_Activate( qboolean active )
{
}


/*
===========
VoiceCapture_Init
===========
*/
qboolean VoiceCapture_Init( void )
{
	return false;
}

/*
===========
VoiceCapture_Activate
===========
*/
qboolean VoiceCapture_Activate( qboolean activate )
{
	return true;
}

/*
===========
VoiceCapture_Lock
===========
*/
qboolean VoiceCapture_Lock( qboolean lock )
{
	return true;
}

/*
==========
VoiceCapture_Shutdown
==========
*/
void VoiceCapture_Shutdown( void )
{
}
