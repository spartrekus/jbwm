// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef JBWM_UTIL_H
#define JBWM_UTIL_H

#include <stdint.h>
#include <X11/Xlib.h>

#define XPROP(win, prop, type, data, size) XChangeProperty(jbwm.dpy, win, \
		prop, type, 32, PropModeReplace, \
		(const unsigned char *)(data), size)

unsigned long pixel(const uint8_t screen, const char * restrict name);

#if defined(EWMH) || defined(MWM)
void *get_property(Window w, Atom property,
	unsigned long * restrict num_items);
#endif//EWMH||MWM

void jbwm_grab_button(const Window w, const unsigned int mask,
		 const unsigned int btn);

#ifdef DEBUG
void print_atom(const Atom a, const uint16_t line);
#endif//DEBUG

#endif//!JBWM_UTIL_H