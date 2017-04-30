// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
//#undef DEBUG
#include "screen.h"
#include <X11/Xatom.h> // keep
#include <X11/Xutil.h>
#include "client.h"
#include "config.h"
#include "display.h"
#include "ewmh_state.h" // keep
#include "ewmh.h" // keep
#include "font.h"
#include "log.h"
#include "mwm.h"
#include "shape.h"
#include "title_bar.h"
#include "util.h" // keep
#include "wm_state.h"
static struct JBWMScreen * screens;
struct JBWMScreen * jbwm_get_screens(void)
{
	return screens;
}
void jbwm_set_screens(struct JBWMScreen * restrict s)
{
	screens = s;
}
extern inline Window jbwm_get_root(struct JBWMClient * restrict c);
extern inline struct JBWMScreen * jbwm_get_screen(struct JBWMClient
	* restrict c);
__attribute__((nonnull))
static void jbwm_configure_client(Display * d,
	struct JBWMClient * restrict c)
{
	const Window w = c->window;
	struct JBWMRectangle * restrict g = &c->size;
	XSendEvent(d, w, true, StructureNotifyMask, (XEvent
		*) &(XConfigureEvent){.x = g->x, .y = g->y, .width = g->width,
		.height = g->height, .type = ConfigureNotify, .event = w,
		.window = w, .above = c->parent, .override_redirect = true,
		.border_width = c->border});
}
void jbwm_move_resize(struct JBWMClient * restrict c)
{
	JBWM_LOG("jbwm_move_resize");
	struct JBWMClientOptions * restrict o = &c->opt;
	const uint8_t offset = o->no_title_bar || o->fullscreen
		? 0 : jbwm_get_font_height();
	if(offset) { // Leave braces in case title bar support was disabled.
		jbwm_handle_mwm_hints(c);
		jbwm_update_title_bar(c);
	} // Skip shaped and fullscreen clients.
	Display * d = jbwm_get_display();
	{ //  * g scope
		struct JBWMRectangle * restrict g = &c->size;
		XMoveResizeWindow(d, c->parent, g->x, g->y - offset,
			g->width, g->height + offset);
		XMoveResizeWindow(d, c->window, 0, offset,
			g->width, g->height);
	}
	jbwm_set_shape(c);
	jbwm_configure_client(d, c);
}
static void showing(struct JBWMClient * restrict c,
	int (* mapping)(Display *, Window),
	const int8_t wm_state)
{
	mapping(jbwm_get_display(), c->parent);
	jbwm_set_wm_state(c, wm_state);
}
void jbwm_hide_client(struct JBWMClient * restrict c)
{
	showing(c, XUnmapWindow, IconicState);
}
void jbwm_restore_client(struct JBWMClient * restrict c)
{
	showing(c, XMapWindow, NormalState);
}
static void check_visibility(struct JBWMScreen * s,
	struct JBWMClient * restrict c, const uint8_t v)
{
	if (c->screen != s->screen)
		return;
	if (c->vdesk == v || c->opt.sticky) {
		c->vdesk = v; // allow moving windows by sticking
		jbwm_restore_client(c);
	} else
		jbwm_hide_client(c);
}
uint8_t jbwm_set_vdesk(struct JBWMScreen * s, uint8_t v)
{
	JBWM_LOG("jbwm_set_vdesk");
	if (v == s->vdesk || v > JBWM_MAX_DESKTOPS)
		return s->vdesk;
	for (struct JBWMClient * restrict c = jbwm_get_head_client();
		c; c = c->next)
		check_visibility(s, c, v);
	s->vdesk = v;
#ifdef JBWM_USE_EWMH
	jbwm_set_property(jbwm_get_display(), s->root,
		jbwm_ewmh_get_atom(JBWM_EWMH_CURRENT_DESKTOP),
		XA_CARDINAL, &v, 1);
#endif//JBWM_USE_EWMH
	return s->vdesk;
}
