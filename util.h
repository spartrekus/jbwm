// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_UTIL_H
#define JBWM_UTIL_H
#include <X11/Xlib.h>
#include <stdint.h>
#include "pixel_t.h"
void jbwm_error(const char * restrict msg) __attribute__((noreturn));
void jbwm_set_property(Display * d, const Window win,
	const Atom property, const Atom type,
	void * data, uint16_t size);
jbwm_pixel_t jbwm_get_pixel(Display * dpy, const uint8_t screen,
	const char * name);
// Free the result with XFree
void * jbwm_get_property(Display * dpy, const Window w,
	Atom property, uint16_t * num_items)
	__attribute__((warn_unused_result,nonnull));
void jbwm_grab_button(Display * d, const Window w,
	const unsigned int mask) __attribute__((nonnull(1)));
#ifdef DEBUG
void jbwm_print_atom(Display * d, const Atom a, const char * src,
	const uint16_t line);
#else//!DEBUG
#define jbwm_print_atom(d, a, s, l)
#endif//DEBUG
#endif//!JBWM_UTIL_H
