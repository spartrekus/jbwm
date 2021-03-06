// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_SNAP_H
#define JBWM_SNAP_H
struct JBWMClient;
void jbwm_snap_border(struct JBWMClient * restrict c)
	__attribute__((nonnull));
void jbwm_snap_client(struct JBWMClient * restrict c)
	__attribute__((nonnull));
#endif//JBWM_SNAP_H
