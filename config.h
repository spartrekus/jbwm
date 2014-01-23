/* Copyright 2007, Jeffrey E. Bedard */
/*
 * This file is ment to hold configuration values and to resolve
 * configuration dependencies.
 */

#ifndef ARWM_CONFIG_H
#define ARWM_CONFIG_H

/* Default fonts */
#ifdef USE_XFT
/*#define DEF_FONT "Sans Bold 10"*/
#define DEF_FONT "Luxi Sans-11:Bold"
#else /* ! USE_XFT */
/* #define DEF_FONT "-*-helvetica-*-r-*-*-*-100-*-*-*-*-*-*" */
/*#define DEF_FONT "9x15"*/
#define DEF_FONT "fixed"
#define FONT_Y_OFFSET 4
#if 0
#define DEF_FONT "8x13bold"
#endif /* 0 */
#endif /* USE_XFT */

/* Default colors */
#define DEF_FG          "#eef"
#define DEF_BG          "#444"
#define TITLEBAR_BG		0x40, 0x40, 0x40
#define TITLEBAR_RESIZE_BG	0x70, 0x70, 0xA0
#define TITLEBAR_CLOSE_BG	0xA0, 0x70, 0x80
#define TITLEBAR_SHADE_BG	0x70, 0xA0, 0x80

/* #define GRAB_MASK Mod1Mask */
#define GRAB_MASK Mod4Mask
#define LOCK_MASK LockMask
#define MOD_MASK ControlMask

#define ARWM_BORDER_WIDTH 1
#define ARWM_SNAP_DISTANCE 4

#define DEF_FC          "#ccf"
#define SPACE           3
#ifdef DEBIAN
#define DEF_TERM        "x-terminal-emulator"
#else
#define DEF_TERM        "rxvt"
#endif
#define XLOCK_CMD	"slock"

#include "macros.h"

/* Sanity checks: */
#if defined(USE_XPM) && defined(USE_XBM)
#undef USE_XBM /* Prefer USE_XPM */
#endif /* USE_XPM && USE_XBM */

/* Values.  */
#ifdef USE_XFT
#define TITLE_FONT_HEIGHT arwm.X.font->height
#else /* ! USE_XFT */
#define TITLE_FONT_HEIGHT (arwm.X.font->ascent+arwm.X.font->descent)
#endif /* USE_XFT */
/* Ensure that the title text fits within the titlebar.  */
#ifdef USE_TBAR
#define TITLEBAR_HEIGHT MAX(22,(TITLE_FONT_HEIGHT+ARWM_BORDER_WIDTH))
#else
#define TITLEBAR_HEIGHT 8
#endif

#define BUTTON_WIDTH (TITLEBAR_HEIGHT-ARWM_BORDER_WIDTH)
#define ARWM_RESIZE_INCREMENT TITLEBAR_HEIGHT

#define ARWM_DEFAULT_SNAP 8

#endif /* ARWM_CONFIG_H */
