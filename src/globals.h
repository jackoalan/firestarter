/*---[ globals.h ]----------------------------------------------------
 * Copyright (C) 2002 Tomas Junnonen (majix@sci.fi)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Global structs and variables
 *--------------------------------------------------------------------*/
#ifndef _FIRESTARTER_GLOBLAS
#define _FIRESTARTER_GLOBALS

#include <config.h>
#include <gtk/gtk.h>

#include <libintl.h>
#define _(String) gettext (String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

#define GNOME_PAD 8
#define GNOME_PAD_SMALL 4
#define GNOME_PAD_BIG 12

typedef struct
{
	GtkWidget     *window;   /* Main window for Firestarter */
	GtkWidget     *ruleview; /* TreeView for rules */
	GtkTooltips   *ttips;    /* Tool tips widget */
} FirestarterApp;

extern FirestarterApp Firestarter;

extern gboolean NETFILTER;
extern gboolean CONSOLE;

#endif
