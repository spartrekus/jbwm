// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_BUTTON_EVENT_H
#define JBWM_BUTTON_EVENT_H
#include <X11/Xlib.h>
struct JBWMClient;
void jbwm_handle_button_event(XButtonEvent * e,
	struct JBWMClient * restrict c) __attribute__((nonnull));
#endif /* JBWM_BUTTON_EVENT_H */
