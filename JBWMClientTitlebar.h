// Copyright 2017, Jeffrey E. Bedard
#ifndef JBWM_JBWMCLIENTTITLEBAR_H
#define JBWM_JBWMCLIENTTITLEBAR_H
#include <X11/X.h>
struct JBWMClientTitlebar {
	Window win, close, resize, shade, stick;
};
#endif//!JBWM_JBWMCLIENTTITLEBAR_H
