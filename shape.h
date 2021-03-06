// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_SHAPE_H
#define JBWM_SHAPE_H
struct JBWMClient;
void jbwm_new_shaped_client(struct JBWMClient * c);
void jbwm_set_shape(struct JBWMClient * c);
#endif//!JBWM_SHAPE_H
