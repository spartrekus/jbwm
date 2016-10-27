// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#include "keys.h"
#include "client.h"
#include "config.h"
#include "JBWMEnv.h"
#include "log.h"
#include "max.h"
#include "screen.h"
#include "snap.h"
#include "title_bar.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
static struct {
	uint16_t grab_mask, mod_mask;
} jbwm_keys_data = { .grab_mask = JBWM_KEYMASK_GRAB,
	.mod_mask = JBWM_KEYMASK_MOD };
void jbwm_set_grab_mask(const uint16_t mask)
{
	jbwm_keys_data.grab_mask = mask;
}
void jbwm_set_mod_mask(const uint16_t mask)
{
	jbwm_keys_data.mod_mask = mask;
}
void jbwm_grab_window_keys(const jbwm_window_t win)
{
	jbwm_grab_button(win, jbwm_keys_data.grab_mask, AnyButton);
}
__attribute__((nonnull(1)))
static void point(struct JBWMClient * restrict c, const int16_t x,
	const int16_t y)
{
	XRaiseWindow(jbwm.d, c->parent);
	XWarpPointer(jbwm.d, None, c->window, 0, 0, 0, 0, x, y);
}
__attribute__((nonnull(1)))
static void commit_key_move(struct JBWMClient * restrict c)
{
	jbwm_snap_border(c);
	jbwm_move_resize(c);
	point(c, 1, 1);
}
struct KeyMoveFlags {
	bool horz:1, pos:1, mod:1;
};
__attribute__((nonnull(1)))
static void key_move(struct JBWMClient * restrict c,
	const struct KeyMoveFlags f)
{
	const int8_t d = f.pos ? JBWM_RESIZE_INCREMENT
		: -JBWM_RESIZE_INCREMENT;
	XSizeHints * restrict s = &c->size;
	int * wh = f.horz ? &s->width : &s->height;
	int * xy = f.horz ? &s->x : &s->y;
	*(f.mod && (*wh > JBWM_RESIZE_INCREMENT<<1)
		&& !c->opt.shaped && !c->opt.no_resize ? wh : xy) += d;
	commit_key_move(c);
}
__attribute__((nonnull(1)))
static void handle_key_move(struct JBWMClient * restrict c,
	const KeySym k, const bool mod)
{
	/* These operations invalid when fullscreen.  */
	if (c->opt.fullscreen)
		return;
	struct KeyMoveFlags f = {.mod = mod, .pos = true};
	switch (k) {
	case JBWM_KEY_LEFT:
		f.pos = false;
	case JBWM_KEY_RIGHT:
		f.horz = true;
		break;
	case JBWM_KEY_UP:
		f.pos = false;
	}
	key_move(c, f);
}
static void toggle_maximize(struct JBWMClient * restrict c)
{
	const struct JBWMClientOptions o = c->opt;
	// Honor !MWM_FUNC_MAXIMIZE
	// Maximizing shaped windows is buggy, so return.
	if (o.shaped || o.no_max || o.fullscreen)
		return;
	if (o.max_horz && o.max_vert) {
		jbwm_set_not_horz(c);
		jbwm_set_not_vert(c);
	} else {
		jbwm_set_horz(c);
		jbwm_set_vert(c);
	}
}
__attribute__((nonnull(2)))
static void handle_client_key_event(const bool mod,
	struct JBWMClient * restrict c, const KeySym key)
{
	JBWM_LOG("handle_client_key_event: %d", (int)key);
	if (c->opt.fullscreen) {
		// only allow exiting from fullscreen
		if (key == JBWM_KEY_FS) {
			jbwm_set_not_fullscreen(c);
		}
		return; // prevent other operations while fullscreen
	}
	switch (key) {
	case JBWM_KEY_LEFT:
	case JBWM_KEY_RIGHT:
	case JBWM_KEY_UP:
	case JBWM_KEY_DOWN:
		handle_key_move(c, key, mod);
		break;
	case JBWM_KEY_KILL:
		jbwm_send_wm_delete(c);
		break;
	case JBWM_KEY_LOWER:
	case JBWM_KEY_ALTLOWER:
		XLowerWindow(jbwm.d, c->parent);
		break;
	case JBWM_KEY_RAISE:
		XRaiseWindow(jbwm.d, c->parent);
		break;
	case JBWM_KEY_FS:
		jbwm_set_fullscreen(c);
		break;
	case JBWM_KEY_MAX:
		toggle_maximize(c);
		break;
	case JBWM_KEY_MAX_H:
		(c->opt.max_horz ? jbwm_set_not_horz : jbwm_set_horz)(c);
		break;
	case JBWM_KEY_MAX_V:
		(c->opt.max_vert ? jbwm_set_not_vert : jbwm_set_vert)(c);
		break;
	case JBWM_KEY_STICK:
		jbwm_toggle_sticky(c);
		break;
	case JBWM_KEY_MOVE:
		jbwm_drag(c, false);
		break;
	case JBWM_KEY_SHADE:
		jbwm_toggle_shade(c);
		break;
	}
}
static struct JBWMClient * get_next_on_vdesk(void)
{
	struct JBWMClient *c = jbwm.current;
	do {
		if (c) {
			c = c->next;
			if (!c && !jbwm.current)
				break;
		}
		if (!c) c = jbwm.head;
		if (!c || (c == jbwm.current))
			break;
	} while (c->vdesk != c->screen->vdesk);
	return c;
}
static void next(void)
{
	struct JBWMClient * c = get_next_on_vdesk();
	if (!c)
		return;
	jbwm_restore_client(c);
	jbwm_select_client(c);
	point(c, 0, 0);
	point(c, c->size.width, c->size.height);
}
static void cond_client_to_desk(struct JBWMClient * c, struct JBWMScreen * s,
	const uint8_t d, const bool mod)
{
	mod && c ? jbwm_set_client_vdesk(c, d) : jbwm_set_vdesk(s, d);
}
static void start_terminal(void)
{
	if (fork() == 0) {
		char * t = getenv(JBWM_ENV_TERM);
		execlp(t, t, (char*)NULL);
	}
}
void jbwm_handle_key_event(XKeyEvent * e)
{
	JBWM_LOG("jbwm_handle_key_event");
	const KeySym key = XLookupKeysym(e, 0);
	struct JBWMClient *c = jbwm.current;
	struct JBWMScreen *s = c ? c->screen : jbwm.s;
	struct {
		uint8_t vdesk:4;
		bool mod:1;
		bool zero:1;
	} opt = {s->vdesk, e->state & jbwm_keys_data.mod_mask, 0};
	switch (key) {
	case JBWM_KEY_NEW:
		start_terminal();
		break;
	case JBWM_KEY_QUIT:
		exit(0);
	case JBWM_KEY_NEXT:
		next();
		break;
	case XK_0:
		opt.zero = true;
	case XK_1: case XK_2: case XK_3: case XK_4: case XK_5: case XK_6:
	case XK_7: case XK_8: case XK_9:
		// First desktop 0, per wm-spec
		cond_client_to_desk(c, s, opt.zero
			? 10 : key - XK_1, opt.mod);
		break;
	case JBWM_KEY_PREVDESK:
		cond_client_to_desk(c, s, s->vdesk - 1, opt.mod);
		break;
	case JBWM_KEY_NEXTDESK:
		cond_client_to_desk(c, s, s->vdesk + 1, opt.mod);
		break;
	default:
		if (c)
			handle_client_key_event(opt.mod, c, key);
	}
}
__attribute__((nonnull(1,2)))
static void grab(struct JBWMScreen * restrict s, KeySym * restrict ks,
	const uint32_t mask)
{
	for (; *ks; ++ks)
		XGrabKey(jbwm.d, XKeysymToKeycode(jbwm.d, *ks),
			 jbwm_keys_data.grab_mask | mask, s->root, true,
			 GrabModeAsync, GrabModeAsync);
}
void jbwm_grab_screen_keys(struct JBWMScreen * restrict s)
{
	grab(s, (KeySym[]){JBWM_KEYS_TO_GRAB}, 0);
	grab(s, (KeySym[]){JBWM_ALT_KEYS_TO_GRAB}, jbwm_keys_data.mod_mask);
}
