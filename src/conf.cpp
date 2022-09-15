/*
 *  MStarDict - International dictionary for Maemo.
 *  Copyright (C) 2010 Roman Moravcik
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <list>
#include <string>

#include <gconf/gconf.h>
#include <gconf/gconf-client.h>

#include "conf.hpp"

Conf::Conf()
{
    /* get the default client */
    gconf_client = gconf_client_get_default();
}

Conf::~Conf()
{
}

bool Conf::GetBool(const gchar *key,
		   bool *val)
{
    GConfValue *value = NULL;

    if (!gconf_client)
	return false;

    value = gconf_client_get(gconf_client, key, NULL);
    if (value) {
	*val = gconf_value_get_bool(value);
	gconf_value_free(value);
    } else {
	return false;
    }
    return true;
}

bool Conf::SetBool(const gchar *key,
		   bool val)
{
    gboolean ret = false;

    if (!gconf_client)
	return false;

    ret = gconf_client_set_bool(gconf_client, key, val, NULL);
    return ret;
}

bool Conf::GetInt(const gchar *key,
		  int *val)
{
    GConfValue *value = NULL;

    if (!gconf_client)
	return false;

    value = gconf_client_get(gconf_client, key, NULL);
    if (value) {
	*val = gconf_value_get_int(value);
	gconf_value_free(value);
    } else {
	return false;
    }
    return true;
}

bool Conf::SetInt(const gchar *key,
		  int val)
{
    gboolean ret = false;

    if (!gconf_client)
	return false;

    ret = gconf_client_set_int(gconf_client, key, val, NULL);
    return ret;
}

bool Conf::GetString(const gchar *key,
		     gchar **val)
{
    GConfValue *value = NULL;
    const gchar *sValue = NULL;

    if (!gconf_client)
	return false;

    value = gconf_client_get(gconf_client, key, NULL);
    if (value) {
	sValue = gconf_value_get_string(value);

	if (*val)
	    g_free(*val);
	*val = g_strdup(sValue);

	gconf_value_free(value);
    } else {
	return false;
    }
    return true;
}

bool Conf::SetString(const gchar *key,
		     const gchar *val)
{
    gboolean ret = false;

    if (!gconf_client)
	return false;

    if (!val)
	return false;

    ret = gconf_client_set_string(gconf_client, key, val, NULL);
    return ret;
}

bool Conf::GetStringList(const gchar *key,
			 std::list < std::string > &list)
{
    GConfValue *value = NULL;
    GSList *slist = NULL;

    if (!gconf_client)
	return false;

    value = gconf_client_get(gconf_client, key, NULL);
    if (value) {
	slist = gconf_value_get_list(value);

	if (slist) {
	    GSList *entry = slist;

	    list.clear();
	    while (entry) {
		list.push_back(std::string(gconf_value_get_string((GConfValue *)entry->data)));
		entry = entry->next;
	    }
	}
	gconf_value_free(value);
    } else {
	return false;
    }
    return true;
}

bool Conf::SetStringList(const gchar *key,
			 std::list < std::string > &list)
{
    GSList *slist = NULL;
    gboolean ret = false;

    if (!gconf_client)
	return false;

    for (std::list < std::string >::iterator i = list.begin(); i != list.end(); ++i) {
	slist = g_slist_append(slist, (gpointer) i->c_str());
    }

    if (slist) {
	ret = gconf_client_set_list(gconf_client, key, GCONF_VALUE_STRING, slist, NULL);
	g_slist_free(slist);
    }
    return ret;
}
