// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_EWMH_H
#define JBWM_EWMH_H
#ifdef JBWM_USE_EWMH
#include "JBWMClient.h"
jbwm_atom_t jbwm_ewmh_get_atom(const uint8_t index);
void jbwm_ewmh_update_client_list(Display * restrict d);
void jbwm_ewmh_set_allowed_actions(Display * restrict d,
	const jbwm_window_t w);
void jbwm_ewmh_init_screen(Display * restrict d,
	struct JBWMScreen * restrict s) __attribute__((nonnull));
void jbwm_ewmh_init(Display * restrict d);
#if 0
enum JBWMAtomIndex {
	SUPPORTED, CURRENT_DESKTOP, NUMBER_OF_DESKTOPS, DESKTOP_VIEWPORT,
	DESKTOP_GEOMETRY, SUPPORTING_WM_CHECK, ACTIVE_WINDOW,
	MOVERESIZE_WINDOW, CLOSE_WINDOW, CLIENT_LIST,
	VIRTUAL_ROOTS, CLIENT_LIST_STACKING, FRAME_EXTENTS,
	// Atoms with "_NET_WM" prefix:
	WM_ALLOWED_ACTIONS, WM_NAME, WM_DESKTOP, WM_MOVERESIZE, WM_PID,
	WM_WINDOW_TYPE,
	// Atoms with "_NET_WM_ACTION" prefix:
	WM_ACTION_MOVE, WM_ACTION_RESIZE, WM_ACTION_CLOSE,
	WM_ACTION_SHADE, WM_ACTION_FULLSCREEN,
	WM_ACTION_CHANGE_DESKTOP, WM_ACTION_ABOVE, WM_ACTION_BELOW,
	WM_ACTION_MAXIMIZE_HORZ, WM_ACTION_MAXIMIZE_VERT,
	// Atoms with "_NET_WM_STATE" prefix:
	WM_STATE, WM_STATE_STICKY, WM_STATE_MAXIMIZED_VERT,
	WM_STATE_MAXIMIZED_HORZ, WM_STATE_SHADED, WM_STATE_HIDDEN,
	WM_STATE_FULLSCREEN, WM_STATE_ABOVE, WM_STATE_BELOW,
	WM_STATE_FOCUSED,
	// The following entry must be last:
	JBWM_EWMH_ATOMS_COUNT
};
#endif//0
enum JBWMAtomIndex {
	JBWM_EWMH_SUPPORTED,
	JBWM_EWMH_CURRENT_DESKTOP,
	JBWM_EWMH_NUMBER_OF_DESKTOPS,
	JBWM_EWMH_DESKTOP_VIEWPORT,
	JBWM_EWMH_DESKTOP_GEOMETRY,
	JBWM_EWMH_SUPPORTING_WM_CHECK,
	JBWM_EWMH_ACTIVE_WINDOW,
	JBWM_EWMH_MOVERESIZE_WINDOW,
	JBWM_EWMH_CLOSE_WINDOW,
	JBWM_EWMH_CLIENT_LIST,
	JBWM_EWMH_VIRTUAL_ROOTS,
	JBWM_EWMH_CLIENT_LIST_STACKING,
	JBWM_EWMH_FRAME_EXTENTS,
	JBWM_EWMH_WM_ALLOWED_ACTIONS,
	JBWM_EWMH_WM_NAME,
	JBWM_EWMH_WM_DESKTOP,
	JBWM_EWMH_WM_MOVERESIZE,
	JBWM_EWMH_WM_PID,
	JBWM_EWMH_WM_WINDOW_TYPE,
	JBWM_EWMH_WM_ACTION_MOVE,
	JBWM_EWMH_WM_ACTION_RESIZE,
	JBWM_EWMH_WM_ACTION_CLOSE,
	JBWM_EWMH_WM_ACTION_SHADE,
	JBWM_EWMH_WM_ACTION_FULLSCREEN,
	JBWM_EWMH_WM_ACTION_CHANGE_DESKTOP,
	JBWM_EWMH_WM_ACTION_ABOVE,
	JBWM_EWMH_WM_ACTION_BELOW,
	JBWM_EWMH_WM_ACTION_MAXIMIZE_HORZ,
	JBWM_EWMH_WM_ACTION_MAXIMIZE_VERT,
	JBWM_EWMH_WM_STATE,
	JBWM_EWMH_WM_STATE_STICKY,
	JBWM_EWMH_WM_STATE_MAXIMIZED_VERT,
	JBWM_EWMH_WM_STATE_MAXIMIZED_HORZ,
	JBWM_EWMH_WM_STATE_SHADED,
	JBWM_EWMH_WM_STATE_HIDDEN,
	JBWM_EWMH_WM_STATE_FULLSCREEN,
	JBWM_EWMH_WM_STATE_ABOVE,
	JBWM_EWMH_WM_STATE_BELOW,
	JBWM_EWMH_WM_STATE_FOCUSED,
	// The following entry must be last:
	JBWM_EWMH_ATOMS_COUNT
};

extern Atom ewmh[];
#else//!JBWM_USE_EWMH
#define jbwm_ewmh_update_client_list(d)
#define jbwm_ewmh_set_allowed_actions(d, w)
#define jbwm_ewmh_init_screen(d, s)
#define jbwm_ewmh_init(d)
#endif//JBWM_USE_EWMH
#endif//!JBWM_EWMH_H
