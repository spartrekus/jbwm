/*
 * jbwm - Restructuring, optimization, and feature fork
 *        Copyright 2007-2015, Jeffrey E. Bedard <jefbed@gmail.com>
 * evilwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */

#ifndef _EWMH_H
#define _EWMH_H

void
ewmh_init();

typedef struct {
	// All Atoms with the "_NET" prefix enumerated here:
	Atom SUPPORTED, CURRENT_DESKTOP, NUMBER_OF_DESKTOPS, DESKTOP_VIEWPORT, 
		DESKTOP_GEOMETRY, SUPPORTING_WM_CHECK, ACTIVE_WINDOW,
		MOVERESIZE_WINDOW, CLOSE_WINDOW, CLIENT_LIST, RESTACK_WINDOW,
		REQUEST_FRAME_EXTENTS, VIRTUAL_ROOTS;

	// Atoms with "_NET_WM" prefix:
	Atom WM_ALLOWED_ACTIONS, WM_NAME, WM_DESKTOP, WM_MOVERESIZE, WM_PID,
		WM_MOVERESIZE_CANCEL, WM_WINDOW_TYPE, WM_STRUT_PARTIAL;

	// Atoms with "_NET_WM_ACTION" prefix:
	Atom WM_ACTION_MOVE, WM_ACTION_RESIZE, WM_ACTION_CLOSE, 
		WM_ACTION_SHADE, WM_ACTION_FULLSCREEN, 
		WM_ACTION_CHANGE_DESKTOP, WM_ACTION_ABOVE, WM_ACTION_BELOW,
		WM_ACTION_MAXIMIZE_HORZ, WM_ACTION_MAXIMIZE_VERT;

	// Atoms with "_NET_WM_STATE" prefix:
	Atom WM_STATE, WM_STATE_STICKY, WM_STATE_MAXIMIZED_VERT, 
		WM_STATE_MAXIMIZED_HORZ, WM_STATE_SHADED, WM_STATE_HIDDEN,
		WM_STATE_FULLSCREEN, WM_STATE_ABOVE, WM_STATE_BELOW,
		WM_STATE_DEMANDS_ATTENTION, WM_STATE_FOCUSED, 
		WM_STATE_SKIP_PAGER;

	Atom supported[50]; //42 taken so far.
	ubyte count;
} EWMHEnvironment;

extern EWMHEnvironment ewmh;

void
ewmh_update_client_list();

void
ewmh_client_message(XClientMessageEvent *e);

void
set_ewmh_allowed_actions(const Window w);

void
setup_ewmh_for_screen(ScreenInfo *s);

#endif//!_EWMH_H