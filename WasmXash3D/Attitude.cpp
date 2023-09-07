// Copyright (c) Asobo Studio, All rights reserved. www.asobostudio.com

#include <MSFS\MSFS.h>
#include "MSFS\MSFS_Render.h"
#include "MSFS\Render\nanovg.h"
#include <MSFS\Legacy\gauges.h>

#include <wasi/api.h>

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <map>

#include <unistd.h>


//#include "doom/doomgeneric.h"

#ifdef _MSC_VER
#define snprintf _snprintf_s
#elif !defined(__MINGW32__)
#include <iconv.h>
#endif

struct sAttitudeVars
{
	ENUM m_eDegrees;
	ENUM m_eAttitudeIndicatorPitchDegrees;
	ENUM m_eAttitudeIndicatorBankDegrees;
	int m_iFont;
	int m_nanovg_fb;
};

sAttitudeVars g_AttitudeVars;
std::map < FsContext, NVGcontext*> g_AttitudeNVGcontext;

extern "C" int hl_runframe(void);
extern "C" uint8_t * half_life_fb;
extern "C" double simulation_time;
NVGcontext* cur_nvgctx;

void doom_draw(NVGcontext* nvgctx, sGaugeDrawData* p_draw_data)
{
	int ret = hl_runframe();
	/*
	doomgeneric_Tick();

	for (int i = 0; i < DOOMGENERIC_RESX * DOOMGENERIC_RESY; ++i)
	{
		DG_ScreenBuffer[i] |= 0xFF000000;
	}

	*/
	nvgUpdateImage(nvgctx, g_AttitudeVars.m_nanovg_fb, (unsigned char*)half_life_fb);

	//g_NetworkGetInfo.m_iImage = nvgCreateImage(nvgctx, g_NetworkGetInfo.imagePath.c_str(), 0);

	NVGpaint imgPaint = nvgImagePattern(nvgctx, 0, 0, p_draw_data->winWidth, p_draw_data->winHeight, 0, g_AttitudeVars.m_nanovg_fb, 1);

	nvgBeginPath(nvgctx);
	nvgRect(nvgctx, 0, 0, p_draw_data->winWidth, p_draw_data->winHeight);
	nvgFillPaint(nvgctx, imgPaint);
	nvgFill(nvgctx);
}

int first_frame = 1;

extern "C" int hl_fb_width;
extern "C" int hl_fb_height;
extern "C" int start_hl(int argc, char** argv);

static bool hl_initialized = false;

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <wasi/libc.h>
#include <wasi/libc-find-relpath.h>

// ------------------------
// Callbacks
extern "C" {

	MSFS_CALLBACK bool Attitude_gauge_callback(FsContext ctx, int service_id, void* pData)
	{
		//write(2, "Bonjour!\n", 10);

		/*
		char buf[256];
		sprintf(buf, "WASM: spf Haiii %d\n", service_id);
		write(2, buf, 256);
		*/

		/*
		const char* abs_pref;
		char* rel_path = 0;
		size_t len = 0;
		int fd = __wasilibc_find_relpath("./", &abs_pref, &rel_path, len);

		sprintf(buf, "WASM: Post relpath: %d %s %S\n", fd, abs_pref, rel_path);;
		write(2, buf, 256);

		close(fd);
		*/

		switch (service_id)
		{
		case PANEL_SERVICE_PRE_INSTALL:
		{
			sGaugeInstallData* p_install_data = (sGaugeInstallData*)pData;
			// Width given in p_install_data->iSizeX
			// Height given in p_install_data->iSizeY
			g_AttitudeVars.m_eDegrees = get_units_enum("DEGREES");
			g_AttitudeVars.m_eAttitudeIndicatorPitchDegrees = get_aircraft_var_enum("ATTITUDE INDICATOR PITCH DEGREES");
			g_AttitudeVars.m_eAttitudeIndicatorBankDegrees = get_aircraft_var_enum("ATTITUDE INDICATOR BANK DEGREES");
			return true;
		}
		break;
		case PANEL_SERVICE_POST_INSTALL:
		{
			NVGparams params;
			params.userPtr = ctx;
			params.edgeAntiAlias = true;
			g_AttitudeNVGcontext[ctx] = nvgCreateInternal(&params);
			NVGcontext* nvgctx = g_AttitudeNVGcontext[ctx];
			g_AttitudeVars.m_iFont = nvgCreateFont(nvgctx, "sans", "./data/Roboto-Regular.ttf");

			return true;
		}
		break;
		case PANEL_SERVICE_PRE_DRAW:
		{
			//sprintf(buf, "WASM: HEEEERE %d\n", service_id);
			//write(2, buf, 256);

			sGaugeDrawData* p_draw_data = (sGaugeDrawData*)pData;

			simulation_time = p_draw_data->t;

			//sprintf(buf, "WASM: HEEEERE %x, SP %x\n", p_draw_data, __builtin_frame_address(0));
			//write(2, buf, 256);



			if (!hl_initialized)
			{
				hl_fb_width = p_draw_data->fbWidth;
				hl_fb_height = p_draw_data->fbHeight;

				NVGcontext* nvgctx = g_AttitudeNVGcontext[ctx];
				g_AttitudeVars.m_nanovg_fb = nvgCreateImageRGBA(nvgctx, hl_fb_width, hl_fb_height, NVG_IMAGE_NEAREST, (unsigned char*)half_life_fb);


				// "prime" the memory allocator with one initial huge allocation
				volatile void* ptr = malloc(128 * 1024 * 1024);
				free((void*)ptr);

				static const char* args[] = { "xash3d" };

				start_hl(sizeof(args) / sizeof(args[0]), (char**)args);

				hl_initialized = 1;
			}

			FLOAT64 fPitch = aircraft_varget(g_AttitudeVars.m_eAttitudeIndicatorPitchDegrees, g_AttitudeVars.m_eDegrees, 0);
			FLOAT64 fBank = aircraft_varget(g_AttitudeVars.m_eAttitudeIndicatorBankDegrees, g_AttitudeVars.m_eDegrees, 0);
			float fSize = sqrt(p_draw_data->winWidth * p_draw_data->winWidth + p_draw_data->winHeight * p_draw_data->winHeight) * 1.1f;
			float pxRatio = (float)p_draw_data->fbWidth / (float)p_draw_data->winWidth;
			NVGcontext* nvgctx = g_AttitudeNVGcontext[ctx];
			//printf("W: %d, H: %d, FB_W: %d, FB_H: %d\n", p_draw_data->winWidth, p_draw_data->winHeight, p_draw_data->fbWidth, p_draw_data->fbHeight);
			nvgBeginFrame(nvgctx, p_draw_data->winWidth, p_draw_data->winHeight, pxRatio);
			{
				cur_nvgctx = nvgctx;

				// Will be drawn over if doom loads as expected, otherwise will stay displayed as the gauge crashes
				if (first_frame)
				{
					nvgTextBounds(nvgctx, 0, 0, NULL, NULL, NULL);
					nvgFontSize(nvgctx, 40.0f);
					nvgFontFace(nvgctx, "sans");
					nvgFillColor(nvgctx, nvgRGBA(255, 0, 0, 255));
					nvgTextAlign(nvgctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
					nvgText(nvgctx, 0, 0, "Please make sure a .wad file is present in this package's work/ folder", NULL);
					first_frame = 0;
				}
				else
					doom_draw(nvgctx, p_draw_data);
			}
			nvgEndFrame(nvgctx);
			return true;
		}
		break;
		case PANEL_SERVICE_PRE_KILL:
		{
			NVGcontext* nvgctx = g_AttitudeNVGcontext[ctx];
			nvgDeleteInternal(nvgctx);
			g_AttitudeNVGcontext.erase(ctx);
			return true;
		}
		break;
		}
		return false;
	}

}
