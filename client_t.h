
#ifndef ARWM_CLIENT_STRUCT_H
#define ARWM_CLIENT_STRUCT_H

/* client structure */

typedef struct Client Client;
struct Client
{
	Window window;
	Window parent;
	ScreenInfo *screen;
#ifdef USE_CMAP
	Colormap cmap;
#endif /* USE_CMAP */
	int ignore_unmap;

	XRectangle geometry;
	ubyte border;
	XRectangle old_geometry;	/* used when maximising */

	unsigned int exposed_width;

	unsigned int min_width, min_height;
	unsigned int max_width, max_height;
	int width_inc, height_inc;
	int base_width, base_height;
	ubyte old_border;
	byte vdesk;
	int shade_height;

	Atom window_type;

#define AR_CLIENT_IS_STICKY 		(1<<0)
/* Set when client needs to be removed: */
#define AR_CLIENT_REMOVE 		(1<<1)
#define AR_CLIENT_ACTIVE 		(1<<2)
#define AR_CLIENT_SHADED 		(1<<3)
#define AR_CLIENT_DONT_MANAGE 		(1<<4)
#define AR_CLIENT_DONT_USE_TITLEBAR 	(1<<5)
#define AR_CLIENT_BLACK_BORDER 		(1<<6)
#define AR_CLIENT_SHAPED 		(1<<7)

	unsigned int flags;

	/*
	 * This is stored within the structure to allow for one-time
	 * assignment and, thus, faster send_config calls.
	 */
	XConfigureEvent ce;
	Client *next;

#ifdef USE_TBAR
	Window info_window;
#endif
	Window close_button;
	Window resize_handle_right;
};

/* This is to initialize the above CE.  */
void initialize_client_ce(Client * c);

#define is_sticky(c) (c->flags & AR_CLIENT_IS_STICKY)
#define add_sticky(c) c->flags |= AR_CLIENT_IS_STICKY
#define remove_sticky(c) c->flags &= ~AR_CLIENT_IS_STICKY
#define toggle_sticky(c) c->flags ^= AR_CLIENT_IS_STICKY

#endif /* ARWM_CLIENT_STRUCT_H */
