// jbwm - Minimalist Window Manager for X
// Copyright 2008-2016, Jeffrey E. Bedard <jefbed@gmail.com> 
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.

#ifndef SCREENINFO_H
#define SCREENINFO_H

typedef struct ScreenInfo
{
	uint8_t screen, vdesk;
	Dim size;
	Window root;
#ifdef EWMH
	Window supporting;
#endif//EWMH
	GC gc;
	XColor fg, bg, fc;
} ScreenInfo;

#endif /* not SCREENINFO_H */
