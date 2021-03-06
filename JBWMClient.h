// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_CLIENT_STRUCT_H
#define JBWM_CLIENT_STRUCT_H
#include <X11/Xlib.h>
#include "JBWMClientOptions.h"
#include "JBWMClientTitleBar.h"
#include "JBWMRectangle.h"
struct JBWMScreen;
struct JBWMClient {
	struct JBWMClient * next;
	Display * display;
	struct JBWMClientTitleBar tb;
	Colormap cmap;
	Window window, parent;
	struct JBWMClientOptions opt;
	struct JBWMRectangle size, old_size, before_fullscreen;
	uint16_t szsum; // used to check size changes for shape setting
	uint8_t screen;
	uint8_t vdesk;
	int8_t ignore_unmap;
	int8_t __pad[1];
};
#endif /* JBWM_CLIENT_STRUCT_H */
