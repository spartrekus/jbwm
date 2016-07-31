// Copyright 2016, Jeffrey E. Bedard <jefbed@gmail.com>
#include "ewmh_state.h"

#include "client.h"
#include "ewmh.h"
#include "jbwmenv.h"
#include "log.h"
#include "max.h"
#include "screen.h"
#include "util.h"

#include <X11/Xatom.h>

// Remove specified atom from WM_STATE
void ewmh_remove_state(const Window w, const Atom state)
{
	uint16_t n;
	Atom *a = get_property(w, ewmh[WM_STATE], &n);

	if (a) {
		const unsigned long nitems = n;

		while (n--) { // decrement here to prevent offset error
			if (a[n] == state)
				a[n] = 0;
		}

		XPROP(w, ewmh[WM_STATE], XA_ATOM, a, nitems);
		XFree(a);
	}
}

static bool ewmh_get_state(const Window w, const Atom state)
{
	uint16_t n;
	Atom *a = get_property(w, ewmh[WM_STATE], &n);
	bool found = false;
	if (a) {
		while (n--) // prevent offset error
			if ((found = (a[n] == state)))
				break;
		XFree(a);
	}
	return found;
}

void ewmh_add_state(const Window w, Atom state)
{
	XChangeProperty(jbwm.d, w, ewmh[WM_STATE],
		XA_ATOM, 32, PropModePrepend,
		(unsigned char *)&state, 1);
}

/*      Reference, per wm-spec:
  window  = the respective client window
  message_type = _NET_WM_STATE
  format = 32
  data.l[0] = the action, as listed below
        _NET_WM_STATE_REMOVE        0    remove/unset property
        _NET_WM_STATE_ADD           1    add/set property
        _NET_WM_STATE_TOGGLE        2    toggle property
  data.l[1] = first property to alter
  data.l[2] = second property to alter
  data.l[3] = source indication
  other data.l[] elements = 0 */

static void set_state(Client * restrict c,
	const bool add, const AtomIndex t)
{
	if (!c)
		return;
	switch(t) {
	case WM_STATE_FULLSCREEN:
		add?set_fullscreen(c):unset_fullscreen(c);
		break;
	case WM_STATE_STICKY:
		c->opt.sticky=add;
		break;
	case WM_STATE_ABOVE:
		if(add) XRaiseWindow(jbwm.d, c->parent);
		else XLowerWindow(jbwm.d, c->parent);
		break;
	case WM_STATE_BELOW:
		if(add) XLowerWindow(jbwm.d, c->parent);
		else XRaiseWindow(jbwm.d, c->parent);
		break;
	case WM_STATE_HIDDEN:
		LOG("HIDDEN");
		break;
	case WM_STATE_MAXIMIZED_VERT:
		add?set_vert(c):unset_vert(c);
		break;
	case WM_STATE_MAXIMIZED_HORZ:
		add?set_horz(c):unset_horz(c);
		break;
	default:
		break;
	}
}

__attribute__((nonnull(1,3)))
static void check_state(XClientMessageEvent * e,	// event data
			const AtomIndex t,	// state to test
			Client *c)
{
	const Atom state = ewmh[t];
	// 2 atoms can be set at once
	long * l = &e->data.l[0];
	const bool set = l[1] == (long)state || l[2] == (long)state;
	if(!set) return;
	switch (e->data.l[0]) {
	default:
	case 0:	// remove
		set_state(c, false, t);
		ewmh_remove_state(e->window, state);
		break;

	case 1:	// add
		set_state(c, true, t);
		ewmh_add_state(e->window, state);
		break;

	case 2:{	// toggle
			const bool add = !ewmh_get_state(e->window, state);
			set_state(c, add, t);
			(add ? ewmh_add_state : ewmh_remove_state)
				(e->window, state);
		}
	}
}

__attribute__((nonnull(1,2)))
static void handle_wm_state_changes(XClientMessageEvent * restrict e,
	Client * restrict c)
{
	check_state(e, WM_STATE_ABOVE, c);
	check_state(e, WM_STATE_BELOW, c);
	check_state(e, WM_STATE_FULLSCREEN, c);
	check_state(e, WM_STATE_MAXIMIZED_HORZ, c);
	check_state(e, WM_STATE_MAXIMIZED_VERT, c);
	check_state(e, WM_STATE_STICKY, c);
}

static bool client_specific_message(XClientMessageEvent * restrict e,
	Client * restrict c, const Atom t)
{
	if (t == ewmh[WM_DESKTOP])
		client_to_vdesk(c, e->data.l[0]);
	// If user moves window (client-side titlebars):
	else if (t == ewmh[WM_MOVERESIZE]) {
		XRaiseWindow(jbwm.d, c->parent);
		drag(c);
	} else if (t == ewmh[WM_STATE])
		handle_wm_state_changes(e, c);
	else if (t == ewmh[ACTIVE_WINDOW])
		select_client(c);
	else if (t == ewmh[CLOSE_WINDOW])
		send_wm_delete(c);
	else
		  return false;
	return true;
}

void ewmh_client_message(XClientMessageEvent * restrict e,
	Client * restrict c)
{
	const Atom t = e->message_type;
#ifdef EWMH_DEBUG
	dprintf(STDERR_FILENO, "----CLIENTMESSAGE----");
	print_atom(t, __FILE__, __LINE__);
	print_atom(e->data.l[0], __FILE__, __LINE__);
	print_atom(e->data.l[1], __FILE__, __LINE__);
	print_atom(e->data.l[2], __FILE__, __LINE__);
	print_atom(e->data.l[3], __FILE__, __LINE__);
#endif//EWMH_DEBUG
	ScreenInfo *s = c ? c->screen : jbwm.s;
	if(c && client_specific_message(e, c, t))
		  return;
	if (t == ewmh[CURRENT_DESKTOP])
		  switch_vdesk(s, e->data.l[0]);
	// If something else moves the window:
	else if (t == ewmh[MOVERESIZE_WINDOW]) {
		const uint8_t src = (e->data.l[0] >> 12) & 3;
		if (src != 2)
			  return;
		const int vm = (e->data.l[0] >> 8) & 0x0f;
		XConfigureWindow(e->display, e->window,
			vm, &(XWindowChanges){
			.x = e->data.l[1], .y = e->data.l[2],
			.width = e->data.l[3],
			.height = e->data.l[4]});
	}
}


