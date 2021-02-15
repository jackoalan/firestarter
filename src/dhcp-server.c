/*---[ dhcp-server.c ]------------------------------------------------
 * Copyright (C) 2004 Tomas Junnonen (majix@sci.fi)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Functions related to running and configuring the system DHCP server
 *--------------------------------------------------------------------*/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "dhcp-server.h"
#include "preferences.h"
#include "util.h"

#define DHCPD "/usr/sbin/dhcpd"
#define DHCPD_CONFIG "/etc/dhcpd.conf"
#define DHCP3_CONFIG "/etc/dhcp3/dhcpd.conf"
#define DHCPD_LOCK_FILE "/var/lock/subsys/dhcpd"

#define DEFAULT_LEASE_TIME "21600"
#define MAX_LEASE_TIME "43200"
#define IP_FORWARDING "off"

#define DHCPD_CONF_TEMPLATE \
	"\n" \
	"subnet ", network, " netmask ", subnet_mask, " {\n" \
	"\toption routers ", router, ";\n" \
	"\toption subnet-mask ", subnet_mask, ";\n" \
	"\toption domain-name-servers ", nameserver, ";\n" \
	"\toption ip-forwarding ", IP_FORWARDING, ";\n" \
	"\trange dynamic-bootp ", lowest_ip, " ", highest_ip, ";\n" \
	"\tdefault-lease-time ", DEFAULT_LEASE_TIME, ";\n" \
	"\tmax-lease-time ", MAX_LEASE_TIME, ";\n" \
	"}\n"
	

gboolean
dhcp_server_exists (void)
{
	return g_file_test (DHCPD, G_FILE_TEST_EXISTS);
}

gboolean
dhcp_server_is_running (void)
{
	return g_file_test (DHCPD_LOCK_FILE, G_FILE_TEST_EXISTS);
}

gboolean
dhcp_server_configuration_exists (void)
{
	return g_file_test (DHCPD_CONFIG, G_FILE_TEST_EXISTS) ||
	       g_file_test (DHCP3_CONFIG, G_FILE_TEST_EXISTS);
}


/* [ calculate_network ]
 * Caluclate the network address given an ip address and subnet mask
 */
static gchar *
calculate_network (char *ip, gchar *subnet)
{
	struct in_addr networkb, ipb, subnetb;

	inet_aton (ip, &ipb);
	inet_aton (subnet, &subnetb);

	networkb.s_addr = ipb.s_addr & subnetb.s_addr;

	return inet_ntoa (networkb);
}

/* [ dhcp_server_create_configuration ]
 * Write the dhcp daemon configuration file
 */
void
dhcp_server_create_configuration (void)
{
	gchar *int_if, *lowest_ip, *highest_ip, *nameserver, *router, *subnet_mask, *network;
	gchar *configuration;
	GIOChannel *out;
	GError *error = NULL;
	gboolean dhcp3_in_use, debian_in_use;

	/* Load DHCP user configurable settings */
	int_if = preferences_get_string (PREFS_FW_INT_IF);
	lowest_ip = preferences_get_string (PREFS_FW_DHCP_LOWEST_IP);
	highest_ip = preferences_get_string (PREFS_FW_DHCP_HIGHEST_IP);
	nameserver = preferences_get_string (PREFS_FW_DHCP_NAMESERVER);

	/* Load automatically determined settings */
	router = get_ip_of_interface (int_if);
	subnet_mask = get_subnet_of_interface (int_if);
	network = calculate_network (router, subnet_mask);
	
	/* Merge the settings with the config file template */
	dhcp3_in_use = g_file_test ("/etc/dhcp3", G_FILE_TEST_EXISTS);
	debian_in_use = g_file_test ("/etc/debian_version", G_FILE_TEST_EXISTS);

	configuration = g_strconcat (
		"# DHCP configuration generated by Firestarter\n", NULL);

	/* Debian doesn't suport ddns unless the dhcp3 package is used */
	if (dhcp3_in_use || !debian_in_use) {
		configuration = g_strconcat (configuration, 
			"ddns-update-style interim;\n" \
			"ignore client-updates;\n", NULL);
	}
	configuration = g_strconcat (configuration,
		DHCPD_CONF_TEMPLATE, NULL);

	if (dhcp3_in_use)
		out = g_io_channel_new_file (DHCP3_CONFIG, "w+", &error);
	else
		out = g_io_channel_new_file (DHCPD_CONFIG, "w+", &error);
	if (out == NULL) {
		gchar *error_message = g_strconcat (
			_("Failed to open DHCP server configuration file for writing: "),
			error->message, NULL);
		show_error (error_message);
		g_free (error_message);
	} else {
		if (g_io_channel_write_chars (out, configuration, -1, NULL, &error) == G_IO_STATUS_ERROR) {
			gchar *error_message = g_strconcat (
				_("Failed to write to DHCP server configuration file: "),
				error->message, NULL);
			show_error (error_message);
			g_free (error_message);
		}
		g_free (configuration);
		g_io_channel_shutdown (out, TRUE, NULL);
	}
}
