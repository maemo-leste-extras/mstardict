/*
 *  MStarDict - International dictionary for Maemo.
 *  Copyright (C) 2010 Roman Moravcik
 *
 *  base on code of stardict:
 *  Copyright (C) 2003-2007 Hu Zheng <huzheng_001@163.com>
 *
 *  based on code of sdcv:
 *  Copyright (C) 2005-2006 Evgeniy <dushistov@mail.ru>
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

#include <string>
#include <vector>
#include <memory>
#include <list>

#include <glib.h>
#include <glib/gi18n.h>

#include <gtk/gtk.h>
#include <hildon/hildon.h>

#include "conf.hpp"
#include "tts.hpp"
#include "prefsdlg.hpp"
#include "mstardict.hpp"

#define _HL(str) dgettext("hildon-libs",str)

PrefsDlg::PrefsDlg(MStarDict *mStarDict)
{
    oStarDict = mStarDict;
}

PrefsDlg::~PrefsDlg()
{
}

void
PrefsDlg::CreatePrefsDialog()
{
    GtkWidget *dialog, *vbox, *tts_conf;

    /* create configuration dialog */
    dialog = gtk_dialog_new();
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(oStarDict->GetMainWindow()));
    gtk_window_set_title(GTK_WINDOW(dialog), _("Preferences"));
    gtk_dialog_add_button(GTK_DIALOG(dialog), _HL("wdgt_bd_save"), GTK_RESPONSE_ACCEPT);

    /* main vbox */
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), vbox);

    /* tts configuration widget */
    tts_conf = oStarDict->oTts->CreateTtsConfWidget();
    gtk_box_pack_start(GTK_BOX(vbox), tts_conf, TRUE, TRUE, 0);

    /* load tts configuration */
    oStarDict->oTts->TtsConfWidgetLoadConf();

    /* run the dialog */
    gtk_widget_show_all(GTK_WIDGET(dialog));
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
	/* save tts configuration */
	oStarDict->oTts->TtsConfWidgetSaveConf();
    }

    gtk_widget_destroy(GTK_WIDGET(dialog));
}
