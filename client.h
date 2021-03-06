// jbwm - Minimalist Window Manager for X
// Copyright 2008-2017, Jeffrey E. Bedard <jefbed@gmail.com>
// Copyright 1999-2015, Ciaran Anscomb <jbwm@6809.org.uk>
// See README for license and other details.
#ifndef JBWM_CLIENT_H
#define JBWM_CLIENT_H
#include <stdint.h>
#include <X11/Xlib.h>
struct JBWMClient;
// Free client and destroy its windows and properties.
void jbwm_client_free(struct JBWMClient * c);
// Search for the JBWMClient matching the specified window
struct JBWMClient * jbwm_get_client(Window w) __attribute__((pure));
// Get the client with input focus.
struct JBWMClient * jbwm_get_current_client(void);
// Get the head of the JBWMClient linked list
struct JBWMClient * jbwm_get_head_client(void);
void jbwm_hide_client(const struct JBWMClient * restrict c);
/* Set c as the new head client, linking its next element
   to the previous head */
void jbwm_prepend_client(struct JBWMClient * restrict c);
// Relink c's linked list to exclude c
void jbwm_relink_client_list(struct JBWMClient * restrict c);
// Move the client to the specified virtual desktop
void jbwm_set_client_vdesk(struct JBWMClient * restrict c,
	const uint8_t desktop);
// Assign c as the current client
void jbwm_set_current_client(struct JBWMClient * restrict c);
void jbwm_toggle_sticky(struct JBWMClient * restrict c);
void jbwm_restore_client(const struct JBWMClient * restrict c);
#endif//!JBWM_CLIENT_H
