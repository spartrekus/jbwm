// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "title_bar.h"
#include <X11/Xatom.h>
#include <X11/Xft/Xft.h>
#include <string.h>
#include "JBWMScreen.h"
#include "config.h"
#include "display.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "font.h"
#include "jbwm.h"
#include "screen.h"
#include "select.h"
#include "util.h"
#include "wm_state.h"
static void set_shaded(struct JBWMClient * restrict c)
{
	c->old_size.height = c->size.height;
	c->size.height = -1;
	c->opt.shaded = true;
	jbwm_set_wm_state(c, IconicState);
	jbwm_ewmh_add_state(jbwm_get_display(), c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_SHADED));
	jbwm_select_client(c);
}
static void set_not_shaded(struct JBWMClient * restrict c)
{
	c->size.height = c->old_size.height;
	c->opt.shaded = false;
	jbwm_move_resize(c);
	jbwm_set_wm_state(c, NormalState);
	jbwm_ewmh_remove_state(jbwm_get_display(), c->window,
		jbwm_ewmh_get_atom(JBWM_EWMH_WM_STATE_SHADED));
}
void jbwm_toggle_shade(struct JBWMClient * restrict c)
{
	// Honor !MJBWM_EWMH_WM_FUNC_MINIMIZE
	if (c->opt.no_min || c->opt.fullscreen)
		return;
	// This implements window shading, a substitute for iconification.
	(c->opt.shaded ? set_not_shaded : set_shaded)(c);
	jbwm_update_title_bar(c);
}
static uint16_t mv(Display * d, const Window w, uint16_t x)
{
	if (w) {
		x -= jbwm_get_font_height();
		XMoveWindow(d, w, x, 0);
	}
	return x;
}
static void move_buttons(Display * d,
	struct JBWMClientTitlebar * t,
	const uint16_t width)
{
	mv(d, t->stick, mv(d, t->shade, mv(d, t->resize, width)));
}
static Window get_win(Display * d, const Window p,
	const jbwm_pixel_t bg)
{
	uint8_t h = jbwm_get_font_height();
	return XCreateSimpleWindow(d, p, 0, 0, h, h, 0, 0, bg);
}
static Window new_title_bar(struct JBWMClient * restrict c)
{
	const struct JBWMPixels * p = &jbwm_get_screen(c)->pixels;
	Display * d = jbwm_get_display();
	const Window t = c->tb.win = get_win(d, c->parent, p->bg);
	XSelectInput(d, t, ExposureMask);
	struct JBWMClientOptions * o = &c->opt;
	c->tb.close = o->no_close ? 0 : get_win(d, t, p->close);
	c->tb.resize = o->no_resize ? 0: get_win(d, t, p->resize);
	c->tb.shade = o->no_min ? 0 : get_win(d, t, p->shade);
	c->tb.stick = get_win(d, t, p->stick);
	XMapRaised(d, t);
	XMapSubwindows(d, t);
	jbwm_grab_button(d, t, 0, AnyButton);
	return t;
}
static void
draw_xft(struct JBWMClient * restrict c, const int16_t * restrict p,
	char * name, const size_t l)
{
	XftFont * f = jbwm_get_font();
	XGlyphInfo e;
	Display * d = jbwm_get_display();
	XftTextExtentsUtf8(d, f, (XftChar8 *) name, l, &e);
	const uint8_t s = c->screen;
	Visual *v = DefaultVisual(d, s);
	XftDraw *xd = XftDrawCreate(d, c->tb.win, v, c->cmap);
	XftColor color;
	XftColorAllocName(d, v, c->cmap, getenv(JBWM_ENV_FG), &color);
	/* Prevent the text from going over the resize button.  */
	const uint16_t max_width = c->size.width
		- 3 * jbwm_get_font_height();
	XftDrawStringUtf8(xd, &color, f, p[0], p[1],
		(XftChar8 *) name, e.width > max_width
		&& e.width > 0 ? l * max_width / e.width : l);
	XftDrawDestroy(xd);
	XftColorFree(d, v, c->cmap, &color);
}
// Free result with XFree if not NULL
__attribute__((pure))
static char * jbwm_get_title(Display * d, const Window w)
{
	XTextProperty tp;
	if (!XGetWMName(d, w, &tp))
		  return NULL;
	return (char *)tp.value;
}
static void draw_title(struct JBWMClient * restrict c)
{
	Display * d = jbwm_get_display();
	char * name = jbwm_get_title(d, c->window);
	if(!name)
		return; // No title could be loaded, abort
	const int16_t p[] = {jbwm_get_font_height() + 4,
		jbwm_get_font_ascent()};
	const size_t l = strlen(name);
	draw_xft(c, p, name, l);
	XFree(name);
}
static void remove_title_bar(struct JBWMClient * restrict c)
{
	++(c->ignore_unmap);
	XDestroyWindow(jbwm_get_display(), c->tb.win);
	c->tb.win = 0;
}
void jbwm_update_title_bar(struct JBWMClient * restrict c)
{
	if (c->opt.shaped)
		return;
	Window w = c->tb.win;
	if (c->opt.fullscreen && w) {
		remove_title_bar(c);
		return;
	}
	if (!w)
		w = new_title_bar(c);
	{ // * d scope
		Display * d = jbwm_get_display();
		/* Expand/Contract the title bar width as necessary:  */
		{ // width scope
			const uint16_t width = c->size.width;
			XResizeWindow(d, w, width, jbwm_get_font_height());
			move_buttons(d, &c->tb, width);
		}
		XClearWindow(d, w);
	}
	draw_title(c);
	if (c->opt.no_title_bar)
		remove_title_bar(c);
}
