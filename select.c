// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "select.h"
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include "JBWMAtomIndex.h"
#include "client.h"
#include "ewmh.h"
#include "ewmh_state.h"
#include "screen.h"
#include "util.h"
#define EWMH_ATOM(a) jbwm_ewmh_get_atom(JBWM_EWMH_##a)
#define WM_STATE(a) EWMH_ATOM(WM_STATE_##a)
static inline jbwm_pixel_t get_bg(struct JBWMClient * c)
{
	return jbwm_get_screen(c)->pixels.bg;
}
static void set_state_not_focused(struct JBWMClient * c)
{
	XSetWindowBorder(c->display, c->parent, get_bg(c));
	jbwm_ewmh_remove_state(c->display, c->window, WM_STATE(FOCUSED));
}
static void unselect_current(struct JBWMClient * c)
{
	struct JBWMClient * current = jbwm_get_current_client();
	if (current && current != c)
		set_state_not_focused(current);
}
static void set_border(struct JBWMClient * c)
{
	struct JBWMPixels * restrict p = &jbwm_get_screen(c)->pixels;
	XSetWindowBorder(c->display, c->parent,
		c->opt.sticky ? p->fc : p->fg);
}
static void set_focused(struct JBWMClient * c)
{
	Display * d = c->display;
	XInstallColormap(d, c->cmap);
	const Window w = c->window;
	XSetInputFocus(d, w, RevertToPointerRoot, CurrentTime);
	jbwm_ewmh_add_state(d, w, WM_STATE(FOCUSED));
}
static void set_active_window(struct JBWMClient * c)
{
	/* Store the window id as a static variable here in case
	 * client c is freed before the X server handles the event.
	 * If the property is read after the client is freed, it will
	 * cause a segmentation fault.  */
	static Window w;
	w = c->window;
	jbwm_set_property(c->display, jbwm_get_client_root(c),
		EWMH_ATOM(ACTIVE_WINDOW), XA_WINDOW, &w, 1);
	jbwm_set_current_client(c);
}
void jbwm_select_client(struct JBWMClient * c)
{
	if (!c)
		return;
	unselect_current(c);
	set_border(c);
	set_focused(c);
	set_active_window(c);
}

