/*
 * jbwm - Restructuring, optimization, and feature fork
 *        Copyright 2007-2015, Jeffrey E. Bedard <jefbed@gmail.com>
 * evilwm - Minimalist Window Manager for X Copyright (C) 1999-2006 Ciaran
 * Anscomb <jbwm@6809.org.uk> see README for license and other details.
 */

#include "jbwm.h"
#include <unistd.h>

#ifdef EWMH

EWMHEnvironment ewmh;

void
ewmh_init()
{
	/* Initializing these elements with the preprocessor shortcut
		below, and following a variable naming convention directly
		corresponding to the atom name, allows further programatic
		access to the atoms defined and helps prevent typographic
		errors.  */
#define EA(atom) ewmh.supported[ewmh.count++]=ewmh.atom=XA("_NET_"#atom)
	EA(SUPPORTED); // Must be first!!!
	EA(CURRENT_DESKTOP);
	EA(NUMBER_OF_DESKTOPS);
	EA(DESKTOP_VIEWPORT);
	EA(DESKTOP_GEOMETRY);
	EA(SUPPORTING_WM_CHECK);
	EA(ACTIVE_WINDOW);
	EA(MOVERESIZE_WINDOW);
	EA(CLOSE_WINDOW);
	EA(CLIENT_LIST);
	EA(RESTACK_WINDOW);
	EA(REQUEST_FRAME_EXTENTS);

	EA(WM_ALLOWED_ACTIONS);
	EA(WM_NAME);
	EA(WM_DESKTOP);
	EA(WM_MOVERESIZE);
	EA(WM_PID);
	EA(WM_MOVERESIZE_CANCEL);
	EA(WM_WINDOW_TYPE);
	EA(WM_STRUT_PARTIAL);

	EA(WM_ACTION_MOVE);
	EA(WM_ACTION_RESIZE);
	EA(WM_ACTION_CLOSE);
	EA(WM_ACTION_SHADE);
	EA(WM_ACTION_FULLSCREEN);
	EA(WM_ACTION_CHANGE_DESKTOP);
	EA(WM_ACTION_ABOVE);
	EA(WM_ACTION_BELOW);
	EA(WM_ACTION_MAXIMIZE_HORZ);
	EA(WM_ACTION_MAXIMIZE_VERT);

	EA(WM_STATE);
	EA(WM_STATE_STICKY);
	EA(WM_STATE_MAXIMIZED_VERT);
	EA(WM_STATE_MAXIMIZED_HORZ);
	EA(WM_STATE_SHADED);
	EA(WM_STATE_HIDDEN);
	EA(WM_STATE_FULLSCREEN);
	EA(WM_STATE_ABOVE);
	EA(WM_STATE_BELOW);
	EA(WM_STATE_DEMANDS_ATTENTION);
	EA(WM_STATE_FOCUSED);
}


#ifdef DEBUG
void
print_atom(const Atom a, const unsigned int line)
{
        char *an=XGetAtomName(D, a);
        fprintf(stderr, "\t%s:%d %s(%lu)\n", __FILE__, line, an, a);
        XFree(an);
}
#endif//DEBUG

void
ewmh_client_message(XClientMessageEvent *e)
{
	const Atom t=e->message_type;
#ifdef DEBUG
        puts("-CLIENTMESSAGE-");
        print_atom(t, __LINE__);
        print_atom(e->data.l[0], __LINE__);
        print_atom(e->data.l[1], __LINE__);
        print_atom(e->data.l[2], __LINE__);
        print_atom(e->data.l[3], __LINE__);
        puts("---------------");
#endif//DEBUG
        ScreenInfo *s = jbwm.X.screens;
        if(t==ewmh.CURRENT_DESKTOP) switch_vdesk(s, e->data.l[0]);
        Client *c=find_client(e->window);
        if(t==ewmh.WM_DESKTOP && e->data.l[1]==2 && c) 
        {
                c->vdesk=e->data.l[0];
                if(c->vdesk==c->screen->vdesk || c->flags&JB_CLIENT_IS_STICKY)
                        unhide(c);
                else
                        hide(c);
		XChangeProperty(D, c->window, ewmh.WM_DESKTOP, XA_CARDINAL, 32,
			PropModeReplace, (unsigned char *)&(c->vdesk), 1);
                select_client(current);
        }
	else if(t==ewmh.DESKTOP_VIEWPORT)
	{
	}
	else if(t==ewmh.NUMBER_OF_DESKTOPS)
	{
		ubyte num=DESKTOPS;
		XPROP(jbwm.X.screens->root, ewmh.NUMBER_OF_DESKTOPS, 
			XA_CARDINAL, &num, 1);
	}
        else if(t==ewmh.ACTIVE_WINDOW && e->data.l[0]==2 && c) select_client(c);
	else if(t==ewmh.CLOSE_WINDOW && e->data.l[1]==2 && c) send_wm_delete(c);
        else if(t==ewmh.MOVERESIZE_WINDOW && c)
        {
                 /* Only do this if it came from direct user action */
                const int source_indication = (e->data.l[0] >> 12) & 3;
                if (source_indication == 2) 
                {
                        c->geometry.x=e->data.l[1];
                        c->geometry.y=e->data.l[2];
                        c->geometry.width=e->data.l[3];
                        c->geometry.height=e->data.l[4];
                        moveresize(c);
                }
       
        }
        else if(t==ewmh.WM_STATE)
        {
/* Reference, per wm-spec:
  window  = the respective client window
  message_type = _NET_WM_STATE
  format = 32
  data.l[0] = the action, as listed below
  data.l[1] = first property to alter
  data.l[2] = second property to alter
  data.l[3] = source indication:
        _NET_WM_STATE_REMOVE        0    remove/unset property 
        _NET_WM_STATE_ADD           1    add/set property 
        _NET_WM_STATE_TOGGLE        2    toggle property 
  other data.l[] elements = 0
*/     
                const Atom p1=(Atom)(e->data.l[1]);
		if(p1!=XA_PRIMARY) return;
                if(p1==ewmh.WM_STATE_ABOVE) XRaiseWindow(D, e->window);
                else if(p1==ewmh.WM_STATE_BELOW) XLowerWindow(D, e->window);
                else if(p1==ewmh.WM_STATE_FULLSCREEN&&c)
                        maximize(c);
        } 
}

void
set_ewmh_allowed_actions(const Window w)
{
	Atom a[]={ewmh.WM_ALLOWED_ACTIONS, ewmh.WM_ACTION_MOVE,
		ewmh.WM_ACTION_RESIZE, ewmh.WM_ACTION_CLOSE,
		ewmh.WM_ACTION_SHADE, ewmh.WM_ACTION_FULLSCREEN,
		ewmh.WM_ACTION_CHANGE_DESKTOP, ewmh.WM_ACTION_ABOVE,
		ewmh.WM_ACTION_BELOW, ewmh.WM_ACTION_MAXIMIZE_HORZ,
		ewmh.WM_ACTION_MAXIMIZE_VERT};
	XPROP(w, a[0], XA_ATOM, &a, sizeof(a)/sizeof(Atom));
}      

void
setup_ewmh_for_screen(ScreenInfo *s)
{      
	const Window r=s->root;
	XPROP(r, ewmh.supported[0], XA_ATOM, ewmh.supported, ewmh.count);
        const unsigned long workarea[4] = { 0, 0, DisplayWidth(D, s->screen),
                DisplayHeight(D, s->screen)};
        XPROP(r, ewmh.DESKTOP_VIEWPORT, XA_CARDINAL, &workarea[0], 2);
        XPROP(r, ewmh.DESKTOP_GEOMETRY, XA_CARDINAL, &workarea[2], 2);
        const unsigned long vdesk = s->vdesk;
 	XPROP(r, ewmh.CURRENT_DESKTOP, XA_CARDINAL, &vdesk, 1);
        const unsigned char n=1;
        XPROP(r, ewmh.NUMBER_OF_DESKTOPS, XA_CARDINAL, &n, 1);
        XPROP(r, ewmh.WM_NAME, XA_STRING, "jbwm", 4);
        s->supporting=XCreateSimpleWindow(D, s->root, 0, 0, 1, 1, 0, 0, 0);
	XPROP(r, ewmh.SUPPORTING_WM_CHECK, XA_WINDOW, &(s->supporting), 1);
	XPROP(s->supporting, ewmh.SUPPORTING_WM_CHECK, XA_WINDOW,
		&(s->supporting), 1);
        XPROP(s->supporting, ewmh.WM_NAME, XA_STRING, "jbwm", 4);
        const pid_t pid=getpid();
        XPROP(s->supporting, ewmh.WM_PID, XA_CARDINAL, &pid, 1);
	// Set this to the root window until we have some clients.  
        XPROP(r, ewmh.CLIENT_LIST, XA_WINDOW, &r, 1);
}      

#endif//EWMH
