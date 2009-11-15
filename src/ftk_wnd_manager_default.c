/*
 * File: ftk_wnd_manager.c    
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   a simple window manager. 
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
 * 2009-10-03 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "ftk_log.h"
#include "ftk_globals.h"
#include "ftk_status_panel.h"
#include "ftk_source_primary.h"
#include "ftk_wnd_manager_default.h"

#define FTK_MAX_GLOBAL_LISTENER 12

typedef struct _PrivInfo
{
	int top;
	FtkMainLoop* main_loop;
	FtkSource*   primary_source;
	FtkWidget*   grab_widget;
	FtkWidget*   focus_widget;
	FtkWidget*   windows[FTK_MAX_WINDOWS];

	void* global_listeners_ctx[FTK_MAX_GLOBAL_LISTENER];
	FtkListener global_listeners[FTK_MAX_GLOBAL_LISTENER];
}PrivInfo;

static Ret  ftk_wnd_manager_default_grab(FtkWndManager* thiz, FtkWidget* window)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	priv->grab_widget = window;

	return RET_OK;
}

static Ret  ftk_wnd_manager_default_ungrab(FtkWndManager* thiz, FtkWidget* window)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL, RET_FAIL);

	if(priv->grab_widget == window)
	{
		priv->grab_widget  = NULL;
	}

	return RET_OK;
}

static Ret ftk_wnd_manager_default_emit_top_wnd_changed(FtkWndManager* thiz)
{
	int i = 0;
	DECL_PRIV(thiz, priv);
	FtkWidget* win = NULL;
	FtkEvent event = {.type = FTK_EVT_TOP_WND_CHANGED};

	/*find the topest app/dialog window*/
	for(i = priv->top - 1; i >=0; i--)
	{
		win = priv->windows[i];
		if((ftk_widget_type(win) == FTK_WINDOW || ftk_widget_type(win) == FTK_DIALOG)
			&& ftk_widget_is_visible(win))
		{
			break;
		}
		else
		{
			win = NULL;
		}
	}

	event.widget = win;
	ftk_wnd_manager_queue_event(thiz, &event);

	return RET_OK;
}

static Ret  ftk_wnd_manager_default_add(FtkWndManager* thiz, FtkWidget* window)
{
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && window != NULL, RET_FAIL);
	return_val_if_fail((priv->top+1) < FTK_MAX_WINDOWS, RET_FAIL);

	priv->windows[priv->top++] = window;
	/*XXX: we assume panel is added as first window*/
	switch(ftk_widget_type(window))
	{
		case FTK_WINDOW:
		{
			y = FTK_STATUS_PANEL_HEIGHT;
			w = ftk_display_width(ftk_default_display());
			h = ftk_display_height(ftk_default_display()) - FTK_STATUS_PANEL_HEIGHT;

			break;
		}
		case FTK_DIALOG:
		{
			x = FTK_DIALOG_MARGIN;
			y = ftk_widget_top(window) + FTK_STATUS_PANEL_HEIGHT;
			w = ftk_display_width(ftk_default_display()) - FTK_DIALOG_MARGIN * 2; 
			h = ftk_widget_height(window);

			break;
		}
		case FTK_STATUS_PANEL:
		{
			w = ftk_display_width(ftk_default_display());
			h = FTK_STATUS_PANEL_HEIGHT;

			break;
		}
		case FTK_MENU_PANEL:
		{
			w = ftk_display_width(ftk_default_display());
			h = ftk_widget_height(window);
			x = 0;
			y = ftk_display_height(ftk_default_display()) - h;

			break;
		}
		default:break;
	}
	
	ftk_widget_move_resize(window, x, y, w, h);
	ftk_logd("type=%d %d %d %d %d\n", ftk_widget_type(window), x, y, w, h);

	return RET_OK;
}

static Ret  ftk_wnd_manager_default_remove(FtkWndManager* thiz, FtkWidget* window)
{
	int i = 0;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && window != NULL && priv->top > 0, RET_FAIL);

	if(priv->grab_widget == window)
	{
		priv->grab_widget = NULL;
	}
	
	if(priv->focus_widget == window)
	{
		priv->focus_widget = NULL;
	}

	if(priv->windows[priv->top - 1] == window)
	{
		priv->top--;
		priv->windows[priv->top] = NULL;
	}
	else
	{
		for(i = 0; i < priv->top; i++)
		{
			if(priv->windows[i] == window)
			{
				for(; i < priv->top; i++)
				{
					priv->windows[i] = priv->windows[i+1];
				}
				priv->top--;
				break;
			}
		}
	}

	ftk_wnd_manager_update(thiz);
	ftk_wnd_manager_default_emit_top_wnd_changed(thiz);

	return RET_OK;
}

static Ret  ftk_wnd_manager_dispatch_globals(FtkWndManager* thiz, FtkEvent* event)
{
	int i = 0;
	Ret ret = RET_OK;
	DECL_PRIV(thiz, priv);

	for(i = 0; i < FTK_MAX_GLOBAL_LISTENER; i++)
	{
		if(priv->global_listeners[i] != NULL)
		{
			ret = priv->global_listeners[i](priv->global_listeners_ctx[i], event);
			if(ret == RET_REMOVE)
			{
				printf("%s: event is filtered by global listeners\n", __func__);
				return ret;
			}
		}
	}

	return ret;
}

static FtkWidget* ftk_wnd_manager_find_target(FtkWndManager* thiz, int x, int y)
{
	int i = 0;
	int top = 0;
	int left = 0;
	int w = 0;
	int h = 0;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && priv->top > 0, NULL);	

	i = priv->top;
	for(; i > 0; i--)
	{
		FtkWidget* win = priv->windows[i-1];
		if(!ftk_widget_is_visible(win))
		{
			continue;
		}
	
		top = ftk_widget_top_abs(win);
		left = ftk_widget_left_abs(win);
		w = ftk_widget_width(win);
		h = ftk_widget_height(win);

		if(x >= left && y >= top && x < (left  + w)	&& y < (top + h))
		{
			return win;
		}
	}

	return NULL;
}

static Ret  ftk_wnd_manager_default_dispatch_event(FtkWndManager* thiz, FtkEvent* event)
{
	DECL_PRIV(thiz, priv);
	FtkWidget* target = NULL;
	return_val_if_fail(thiz != NULL && event != NULL, RET_FAIL);

	if(ftk_wnd_manager_dispatch_globals(thiz, event) != RET_OK)
	{
		return RET_REMOVE;
	}
	
	switch(event->type)
	{
		case FTK_EVT_WND_DESTROY:
		{
			ftk_wnd_manager_default_remove(thiz, event->widget);	
			return RET_OK;
		}
		case FTK_EVT_HIDE:
		{
			ftk_wnd_manager_update(thiz);
			ftk_wnd_manager_default_emit_top_wnd_changed(thiz);
			return RET_OK;
		}
		case FTK_EVT_SHOW:
		{
			ftk_wnd_manager_default_emit_top_wnd_changed(thiz);
			return RET_OK;
		}
		default:break;
	}
	
	if((event->type == FTK_EVT_MOUSE_DOWN 
		|| event->type == FTK_EVT_MOUSE_UP
		|| event->type == FTK_EVT_MOUSE_MOVE) && priv->grab_widget == NULL)
	{
		int x = event->u.mouse.x;
		int y = event->u.mouse.y;
		
		target = ftk_wnd_manager_find_target(thiz, x, y);
		if(event->type == FTK_EVT_MOUSE_DOWN)
		{
			priv->focus_widget = target;
		}
	}
	else if(event->widget != NULL)
	{
		target = event->widget;
	}
	else if(priv->grab_widget != NULL)
	{
		target = priv->grab_widget;
	}
	else if(priv->focus_widget != NULL)
	{
		target = priv->focus_widget;
	}
	else if(priv->top > 0)
	{
		target = priv->windows[priv->top - 1];
	}

	if(target != NULL && !ftk_widget_is_insensitive(target))
	{
		ftk_widget_ref(target);
		ftk_widget_event(target, event);
		ftk_widget_unref(target);
	}

	return RET_OK;
}

static Ret  ftk_wnd_manager_default_update(FtkWndManager* thiz)
{
	int i = 0;
	FtkWidget* win = NULL;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && priv->top > 0, RET_FAIL);
	
	for(i = priv->top; i > 0; i--)
	{
		win = priv->windows[i - 1];
		if(ftk_widget_type(win) == FTK_WINDOW && ftk_widget_is_visible(win))
		{
			i--;
			break;
		}
	}
	
	for(; i < priv->top; i++)
	{
		win = priv->windows[i];

		if(ftk_widget_is_visible(win))
		{
			ftk_widget_paint(win);
		}
	}
	
	for(i = 0; i < priv->top; i++)
	{
		win = priv->windows[i];
		if((ftk_widget_type(win) == FTK_STATUS_PANEL || ftk_widget_type(win) == FTK_MENU_PANEL)
			&& ftk_widget_is_visible(win))
		{
			ftk_widget_paint(win);
		}
	}

	return RET_OK;
}

static Ret  ftk_wnd_manager_default_queue_event(FtkWndManager* thiz, FtkEvent* event)
{
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && event != NULL, RET_FAIL);
	return_val_if_fail(ftk_default_main_loop() != NULL, RET_FAIL);

	return ftk_source_queue_event(priv->primary_source, event);
}

static Ret  ftk_wnd_manager_default_add_global_listener(FtkWndManager* thiz, FtkListener listener, void* ctx)
{
	int i = 0;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && listener != NULL, RET_FAIL);

	for(i = 0; i < FTK_MAX_GLOBAL_LISTENER; i++)
	{
		if(priv->global_listeners[i] == NULL)
		{
			priv->global_listeners[i] = listener;
			priv->global_listeners_ctx[i] = ctx;

			return RET_OK;
		}
	}

	return RET_OUT_OF_SPACE;
}

static Ret  ftk_wnd_manager_default_remove_global_listener(FtkWndManager* thiz, FtkListener listener, void* ctx)
{
	int i = 0;
	DECL_PRIV(thiz, priv);
	return_val_if_fail(thiz != NULL && listener != NULL, RET_FAIL);

	for(i = 0; i < FTK_MAX_GLOBAL_LISTENER; i++)
	{
		if(priv->global_listeners[i] == listener && priv->global_listeners_ctx[i] == ctx)
		{
			priv->global_listeners[i] = NULL;
			priv->global_listeners_ctx[i] = NULL;
		}
	}

	return RET_OK;
}

static void ftk_wnd_manager_default_destroy(FtkWndManager* thiz)
{
	int i = 0;
	DECL_PRIV(thiz, priv);
	if(thiz != NULL)
	{
		int nr = priv->top;
		for(i = 0; i < nr; i++)
		{
			FtkWidget* win = priv->windows[0];
			ftk_wnd_manager_remove(thiz, win);
			ftk_widget_unref(win);
		}

		FTK_ZFREE(thiz, sizeof(FtkWndManager));
	}

	return;
}

FtkWndManager* ftk_wnd_manager_default_create(FtkMainLoop* main_loop)
{
	FtkWndManager* thiz = NULL;
	return_val_if_fail(main_loop != NULL, NULL);

	if((thiz = (FtkWndManager*)FTK_ZALLOC(sizeof(FtkWndManager) + sizeof(PrivInfo))) != NULL)
	{
		DECL_PRIV(thiz, priv);

		priv->main_loop = main_loop;
		priv->primary_source = ftk_source_primary_create(
			(FtkOnEvent)ftk_wnd_manager_default_dispatch_event, thiz);
		ftk_sources_manager_add(ftk_default_sources_manager(), priv->primary_source);

		thiz->grab   = ftk_wnd_manager_default_grab;
		thiz->ungrab = ftk_wnd_manager_default_ungrab;
		thiz->add    = ftk_wnd_manager_default_add;
		thiz->remove = ftk_wnd_manager_default_remove;
		thiz->update         = ftk_wnd_manager_default_update;
		thiz->queue_event    = ftk_wnd_manager_default_queue_event;
		thiz->dispatch_event         = ftk_wnd_manager_default_dispatch_event;
		thiz->add_global_listener    = ftk_wnd_manager_default_add_global_listener;
		thiz->remove_global_listener = ftk_wnd_manager_default_remove_global_listener;
		thiz->destroy                = ftk_wnd_manager_default_destroy;
	}

	return thiz;
}


#ifdef FTK_WND_MANAGER_TEST
int main(int argc, char* argv[])
{
	FtkWndManager* thiz = NULL;
	FtkMainLoop* main_loop = ftk_main_loop_create();
	
	thiz = ftk_wnd_manager_default_create(main_loop);

	ftk_wnd_manager_default_destroy(thiz);

	return 0;
}
#endif/*FTK_WND_MANAGER_TEST*/
