/*
 * File: ftk_display_sigma.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   sigma implemented display.
 *
 * Copyright (c) 2009  Li XianJing <xianjimli@hotmail.com>
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * History:
 * ================================================================
 * 2009-12-21 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "ftk_log.h"
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "ftk_display_fb.h"

typedef struct _PrivInfo
{
	struct FB fb;
	FtkBitmapCopyFromData copy_from_data;
	FtkBitmapCopyToData   copy_to_data;
}PrivInfo;

static Ret ftk_display_fb_update(FtkDisplay* thiz, FtkBitmap* bitmap, FtkRect* rect, int xoffset, int yoffset)
{
	DECL_PRIV(thiz, priv);
	int display_width  = fb_width(&priv->fb);
	int display_height = fb_height(&priv->fb);

	ftk_logd("%s: ox=%d oy=%d (%d %d %d %d)\n", __func__, xoffset, yoffset, 
		rect->x, rect->y, rect->width, rect->height);
	return priv->copy_to_data(bitmap, rect, 
		priv->fb.bits, xoffset, yoffset, display_width, display_height); 
}

static int ftk_display_fb_width(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);

	return priv->fb.vi.xres;
}

static int ftk_display_fb_height(FtkDisplay* thiz)
{
	DECL_PRIV(thiz, priv);

	return priv->fb.vi.yres;
}

static Ret ftk_display_fb_snap(FtkDisplay* thiz, size_t x, size_t y, FtkBitmap* bitmap)
{
	FtkRect rect = {0};
	DECL_PRIV(thiz, priv);
	int w = ftk_display_width(thiz);
	int h = ftk_display_height(thiz);
	int bw = ftk_bitmap_width(bitmap);
	int bh = ftk_bitmap_height(bitmap);
	
	rect.x = x;
	rect.y = y;
	rect.width = bw;
	rect.height = bh;

	return priv->copy_from_data(bitmap, priv->fb.bits, w, h, &rect);
}

static void ftk_display_fb_destroy(FtkDisplay* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		fb_close(&priv->fb);
		FTK_ZFREE(thiz, sizeof(FtkDisplay) + sizeof(PrivInfo));
	}

	return;
}

FtkDisplay* ftk_display_fb_create(const char* filename)
{
	FtkDisplay* thiz = NULL;
	return_val_if_fail(filename != NULL, NULL);

	thiz = (FtkDisplay*)FTK_ZALLOC(sizeof(FtkDisplay) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		if(fb_open(&priv->fb, filename) == 0)
		{
			thiz->update   = ftk_display_fb_update;
			thiz->width    = ftk_display_fb_width;
			thiz->height   = ftk_display_fb_height;
			thiz->snap     = ftk_display_fb_snap;
			thiz->destroy  = ftk_display_fb_destroy;
	
			if(priv->fb.vi.bits_per_pixel == 16)
			{
				priv->copy_to_data   = ftk_bitmap_copy_to_data_rgb565;
				priv->copy_from_data = ftk_bitmap_copy_from_data_rgb565;
			}
			else if(priv->fb.vi.bits_per_pixel == 24)
			{
				priv->copy_to_data   = ftk_bitmap_copy_to_data_bgr24;
				priv->copy_from_data = ftk_bitmap_copy_from_data_bgr24;
			}
			else if(priv->fb.vi.bits_per_pixel == 32)
			{
				priv->copy_to_data   = ftk_bitmap_copy_to_data_bgra32;
				priv->copy_from_data = ftk_bitmap_copy_from_data_bgra32;
			}
			else
			{
				assert(!"not supported framebuffer format.");
			}
		}
		else
		{
			ftk_logd("%s: open %s failed.\n", __func__, filename);
			FTK_ZFREE(thiz, sizeof(*thiz) + sizeof(PrivInfo));
		}
	}
		
	return thiz;
}
