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

#include <list>

#include <glib.h>
#include <glib/gi18n.h>
#include <glib/gstdio.h>

#include <gtk/gtk.h>
#include <hildon/hildon.h>

#include "conf.hpp"
#include "libwrapper.hpp"
#include "mstardict.hpp"
#include "dictmngr.hpp"

#define DEFAULT_DICT_DIR "/home/user/MyDocs/mstardict"
#define STARDICT_DICT_DIR "/home/user/.stardict/dic"

enum {
    BOOKNAME_DICT_INFO_COLUMN,
    FILENAME_DICT_INFO_COLUMN,
    N_DICT_INFO_COLUMNS
};

class GetAllDictList {
  public:
    GetAllDictList(std::list < std::string > &dict_all_list_):dict_all_list(dict_all_list_) {
    } void operator() (const std::string & url, bool disable) {
	dict_all_list.push_back(url);
    }
  private:
    std::list < std::string > &dict_all_list;
};

DictMngr::DictMngr(MStarDict *mStarDict)
{
    oStarDict = mStarDict;

    /* check if default dictionary exists */
    if (!g_file_test(DEFAULT_DICT_DIR, GFileTest(G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))) {
	    if (g_mkdir(DEFAULT_DICT_DIR, S_IRWXU)==-1)
		g_warning("Cannot create directory %s.", DEFAULT_DICT_DIR);
    }
}

DictMngr::~DictMngr()
{
}

void
DictMngr::CreateDictMngrDialog()
{
    GtkWidget *dialog, *selector;
    GtkCellRenderer *renderer;
    HildonTouchSelectorColumn *column;
    GtkTreeModel *tree_model;
    GtkTreeIter iter;
    gboolean iter_valid = TRUE;
    std::list < std::string > all_dict_list;
    std::list < std::string > selected_dict_list;
    GtkListStore *dict_list = NULL;

    dict_list = gtk_list_store_new(N_DICT_INFO_COLUMNS,
				   G_TYPE_STRING,	/* bookname */
				   G_TYPE_STRING);	/* filename */

    /* create dialog */
    dialog = gtk_dialog_new();
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_title(GTK_WINDOW(dialog), _("Dictionaries"));
    gtk_dialog_add_button(GTK_DIALOG(dialog), "OK", GTK_RESPONSE_ACCEPT);
    gtk_window_set_default_size(GTK_WINDOW(dialog), -1, 400);

    /* dictionary selector */
    selector = hildon_touch_selector_new();
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), selector);

    renderer = gtk_cell_renderer_text_new();
    g_object_set(G_OBJECT(renderer),
		 "xpad", 10,
		 "ellipsize", PANGO_ELLIPSIZE_END,
		 "ellipsize-set", TRUE,
		 NULL);

    column =
	hildon_touch_selector_append_column(HILDON_TOUCH_SELECTOR
					    (selector),
					    GTK_TREE_MODEL(dict_list),
					    renderer, "text", BOOKNAME_DICT_INFO_COLUMN, NULL);
    hildon_touch_selector_column_set_text_column(column, 0);

    /* fill list with all available dictionaries */
    GetAllDictionaryList(all_dict_list);
    for (std::list < std::string >::iterator i = all_dict_list.begin();
	 i != all_dict_list.end(); ++i) {
	DictInfo dictinfo;

	dictinfo.load_from_ifo_file(i->c_str(), 0);
	gtk_list_store_append(dict_list, &iter);
	gtk_list_store_set(dict_list, &iter,
			   BOOKNAME_DICT_INFO_COLUMN,
			   dictinfo.bookname.c_str(), FILENAME_DICT_INFO_COLUMN, i->c_str(), -1);
    }
    g_object_unref(dict_list);

    /* set selector mode to multiple */
    hildon_touch_selector_set_column_selection_mode(HILDON_TOUCH_SELECTOR
						    (selector),
						    HILDON_TOUCH_SELECTOR_SELECTION_MODE_MULTIPLE);
    hildon_touch_selector_unselect_all(HILDON_TOUCH_SELECTOR(selector), BOOKNAME_DICT_INFO_COLUMN);

    /* select all load dictionaries */
    tree_model =
	hildon_touch_selector_get_model(HILDON_TOUCH_SELECTOR(selector), BOOKNAME_DICT_INFO_COLUMN);
    for (iter_valid = gtk_tree_model_get_iter_first(tree_model, &iter);
	 iter_valid; iter_valid = gtk_tree_model_iter_next(tree_model, &iter)) {
	const gchar *bookname;

	gtk_tree_model_get(tree_model, &iter, BOOKNAME_DICT_INFO_COLUMN, &bookname, -1);
	for (size_t iLib = 0; iLib < oStarDict->oLibs->query_dictmask.size(); iLib++) {
	    if (!strcmp(oStarDict->oLibs->dict_name(iLib).c_str(), bookname)) {
		hildon_touch_selector_select_iter(HILDON_TOUCH_SELECTOR
						  (selector),
						  BOOKNAME_DICT_INFO_COLUMN, &iter, FALSE);
		break;
	    }
	}
    }

    /* show dialog */
    gtk_widget_show_all(GTK_WIDGET(dialog));

    /* run the dialog */
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
	GList *selected_dicts = NULL;

	selected_dicts =
	    hildon_touch_selector_get_selected_rows(HILDON_TOUCH_SELECTOR
						    (selector), BOOKNAME_DICT_INFO_COLUMN);
	if (selected_dicts) {
	    GList *dict = selected_dicts;
	    const gchar *filename;

	    while (dict) {
		gtk_tree_model_get_iter(GTK_TREE_MODEL(tree_model), &iter,
					(GtkTreePath *) (dict->data));
		gtk_tree_model_get(GTK_TREE_MODEL(tree_model), &iter,
				   FILENAME_DICT_INFO_COLUMN, &filename, -1);
		selected_dict_list.push_back(std::string(filename));
		dict = dict->next;
	    }
	    g_list_foreach(selected_dicts, (GFunc) gtk_tree_path_free, NULL);
	    g_list_free(selected_dicts);
	}

	if (oStarDict->oConf->SetStringList("/apps/maemo/mstardict/dict_list", selected_dict_list)) {
	    /* reload dictionaries */
	    ReLoadDictionaries(selected_dict_list);
	}
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void
DictMngr::GetAllDictionaryList(std::list < std::string > &dict_list)
{
    strlist_t dicts_dir_list;
    strlist_t order_list;
    strlist_t disable_list;

    /* default mstardict dictionary directory */
    dicts_dir_list.push_back(std::string(DEFAULT_DICT_DIR));

    /* stardict dictionary directory */
    dicts_dir_list.push_back(std::string(STARDICT_DICT_DIR));
    for_each_file(dicts_dir_list, ".ifo", order_list, disable_list, GetAllDictList(dict_list));
}

void
DictMngr::LoadDictionaries()
{
    std::list < std::string > dict_list;

    if (!oStarDict->oConf->GetStringList("/apps/maemo/mstardict/dict_list", dict_list)) {
	GetAllDictionaryList(dict_list);
	oStarDict->oConf->SetStringList("/apps/maemo/mstardict/dict_list", dict_list);
    }

    oStarDict->oLibs->load(dict_list);
    oStarDict->oLibs->query_dictmask.clear();
    for (std::list < std::string >::iterator i = dict_list.begin(); i != dict_list.end(); ++i) {
	size_t iLib;
	if (oStarDict->oLibs->find_lib_by_filename(i->c_str(), iLib)) {
	    InstantDictIndex instance_dict_index;
	    instance_dict_index.type = InstantDictType_LOCAL;
	    instance_dict_index.index = iLib;
	    oStarDict->oLibs->query_dictmask.push_back(instance_dict_index);
	}
    }

    if (oStarDict->oLibs->iCurrentIndex)
	g_free(oStarDict->oLibs->iCurrentIndex);
    oStarDict->oLibs->iCurrentIndex =
	(CurrentIndex *) g_malloc(sizeof(CurrentIndex) * oStarDict->oLibs->query_dictmask.size());

    if (oStarDict->oLibs->query_dictmask.empty())
	oStarDict->ShowNoDictionary(true);
}

void
DictMngr::ReLoadDictionaries(std::list < std::string > &dict_list)
{
    oStarDict->oLibs->reload(dict_list, 0, 0);
    oStarDict->oLibs->query_dictmask.clear();
    for (std::list < std::string >::iterator i = dict_list.begin(); i != dict_list.end(); ++i) {
	size_t iLib;
	if (oStarDict->oLibs->find_lib_by_filename(i->c_str(), iLib)) {
	    InstantDictIndex instance_dict_index;
	    instance_dict_index.type = InstantDictType_LOCAL;
	    instance_dict_index.index = iLib;
	    oStarDict->oLibs->query_dictmask.push_back(instance_dict_index);
	}
    }

    if (oStarDict->oLibs->iCurrentIndex)
	g_free(oStarDict->oLibs->iCurrentIndex);
    oStarDict->oLibs->iCurrentIndex =
	(CurrentIndex *) g_malloc(sizeof(CurrentIndex) * oStarDict->oLibs->query_dictmask.size());

    if (oStarDict->oLibs->query_dictmask.empty())
	oStarDict->ShowNoDictionary(true);
}
