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

#include <gconf/gconf.h>
#include <gconf/gconf-client.h>

class Conf {
  private:
    GConfClient *gconf_client;

  public:
    Conf();
    ~Conf();

    bool GetBool(const gchar *key,
		 bool *val);
    bool SetBool(const gchar *key,
		 bool val);
    bool GetInt(const gchar *key,
		int *val);
    bool SetInt(const gchar *key,
		int val);
    bool GetString(const gchar *key,
		   gchar **val);
    bool SetString(const gchar *key,
		   const gchar *val);
    bool GetStringList(const gchar *key,
		       std::list < std::string > &list);
    bool SetStringList(const gchar *key,
		       std::list < std::string > &list);
};
