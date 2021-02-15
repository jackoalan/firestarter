/*---[ logread.h ]----------------------------------------------------
 * Copyright (C) 2000 Tomas Junnonen (majix@sci.fi)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Functions for reading from the syslog file
 *--------------------------------------------------------------------*/

#ifndef _FIRESTARTER_LOGREAD
#define _FIRESTARTER_LOGREAD

#include <config.h>
#include <gtk/gtk.h>
#include "firestarter.h"

#define FILE_BUF 4096

void open_logfile (char *logpath);

void logread_async_read_callback (GObject *source_object,
                                  GAsyncResult *res,
                                  gpointer user_data);

Hit *parse_log_line (gchar *line);

typedef struct _Parse Parse;
struct _Parse
{
	gchar *buffer;
	GPatternSpec *pattern;
	int half_line;
	gsize size;
	gsize bytes_read;
	GFileInputStream *handle;
	gboolean continuous;
};

#endif
