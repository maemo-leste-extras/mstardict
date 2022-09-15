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

#include <gtk/gtk.h>
#include <hildon/hildon.h>
#include <libosso.h>

#include "lib/pluginmanager.h"

extern MStarDict *pMStarDict;

class Conf;
class DictMngr;
class TransWin;
class Library;
class MStarDict;
class PrefsDlg;
class Tts;

class MStarDict {
  private:
    osso_context_t *osso_context;

    GtkWidget *window;
    GtkWidget *label_widget;
    GtkWidget *results_widget;
    GtkWidget *search_entry;
    GtkWidget *results_view;
    GtkWidget *results_view_scroll;

    GtkListStore *results_list;

    static gboolean onResultsViewSelectionChanged(GtkTreeSelection *selection,
						  MStarDict *mStarDict);
    static gboolean onSearchEntryChanged(GtkEditable *editable,
					 MStarDict *mStarDict);
    static gboolean onSearchClearClicked(GtkButton *button,
					 MStarDict *mStarDict);
    static gboolean onDictionariesMenuItemClicked(GtkButton *button,
						  MStarDict *mStarDict);
    static gboolean onDownloadDictionariesMenuItemClicked(GtkButton *button,
							  MStarDict *mStarDict);
    static gboolean onPreferencesMenuItemClicked(GtkButton *button,
						 MStarDict *mStarDict);
    static gboolean onQuitMenuItemClicked(GtkButton *button,
					  MStarDict *mStarDict);
    static gboolean onLookupProgressDialogResponse(GtkDialog *dialog,
						   gint response_id,
						   bool *cancel);
    static gboolean onMainWindowKeyPressEvent(GtkWidget *window,
					      GdkEventKey *event,
					      MStarDict *mStarDict);

  public:
     MStarDict();
    ~MStarDict();

    Conf *oConf;
    DictMngr *oDict;
    TransWin *oTransWin;
    Library *oLibs;
    StarDictPlugins *oPlugins;
    PrefsDlg *oPrefs;
    Tts *oTts;

    GtkWidget *CreateLookupProgressDialog(bool *cancel);
    void DestroyLookupProgressDialog(GtkWidget *dialog);
    void CreateMainWindow();
    GtkWidget *GetMainWindow();
    GtkWidget *CreateSearchBar();
    void CreateMainMenu();

    void SearchWord();

    void ResultsListClear();
    void ResultsListInsertLast(const gchar *word);
    void ResultsReScroll();
    void ResultsUnselectAll(GtkSelectionMode mode);
    void ShowNoResults(bool bNoResults);
    void ShowNoDictionary(bool bNoDictionary);
    void ShowProgressIndicator(bool bShow);
    void GrabFocus();
};
