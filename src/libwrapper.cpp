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

#include <glib.h>
#include <glib/gi18n.h>

#include "lib/getuint32.h"

#include "libwrapper.hpp"
#include "mstardict.hpp"

static
string parse_data(const gchar *data,
		  const gchar *oword)
{
    if (!data)
	return "";

    string mark;
    guint32 data_size, sec_size = 0;
    const gchar *p = data;
    data_size = get_uint32(data);
    p += sizeof(guint32);
    size_t iPlugin;
    size_t nPlugins = pMStarDict->oPlugins->ParseDataPlugins.nplugins();
    unsigned int parsed_size;
    ParseResult parse_result;

    while (guint32(p - data) < data_size) {
	for (iPlugin = 0; iPlugin < nPlugins; iPlugin++) {
	    parse_result.clear();
	    if (pMStarDict->oPlugins->ParseDataPlugins.parse(iPlugin, p, &parsed_size, parse_result, oword)) {
		p += parsed_size;
		break;
	    }
	}
	if (iPlugin != nPlugins) {
	    for (std::list<ParseResultItem>::iterator it = parse_result.item_list.begin(); it != parse_result.item_list.end(); ++it) {
		switch (it->type) {
		    case ParseResultItemType_mark:
			g_debug("ParseResultItemType_mark");
			mark += it->mark->pango;
			break;
		    case ParseResultItemType_link:
			mark += it->mark->pango;
			break;
		    case ParseResultItemType_res:
		    {
			g_debug("ParseResultItemType_res");
			bool loaded = false;
			if (it->res->type == "image") {
			} else if (it->res->type == "sound") {
			} else if (it->res->type == "video") {
			} else {
			}
			if (!loaded) {
			    mark += "<span foreground=\"red\">";
			    gchar *m_str = g_markup_escape_text(it->res->key.c_str(), -1);
			    mark += m_str;
			    g_free(m_str);
			    mark += "</span>";
			}
			break;
		    }
		    case ParseResultItemType_widget:
			g_debug("ParseResultItemType_widget");
			break;
		    default:
			g_debug("ParseResultItemType_default");
			break;
		}
	    }
	    parse_result.clear();
	    continue;
	}

	switch (*p) {
	case 'm':
	case 'l':		//need more work...
	    p++;
	    sec_size = strlen(p);
	    if (sec_size) {
		gchar *m_str = g_markup_escape_text(p, sec_size);
		mark += m_str;
		g_free(m_str);
	    }
	    sec_size++;
	    break;
	case 'g':
	    p++;
	    sec_size = strlen(p);
	    if (sec_size) {
		mark += p;
	    }
	    sec_size++;
	    break;
	case 'x':
	    p++;
	    sec_size = strlen(p) + 1;
	    mark += _("XDXF data parsing plug-in is not found!");
	    break;
	case 'k':
	    p++;
	    sec_size = strlen(p) + 1;
	    mark += _("PowerWord data parsing plug-in is not found!");
	    break;
	case 'w':
	    p++;
	    sec_size = strlen(p) + 1;
	    mark += _("Wiki data parsing plug-in is not found!");
	    break;
	case 'h':
	    p++;
	    sec_size = strlen(p) + 1;
	    mark += _("HTML data parsing plug-in is not found!");
	    break;
	case 'n':
	    p++;
	    sec_size = strlen(p) + 1;
	    mark += _("WordNet data parsing plug-in is not found!");
	    break;
	case 't':
	    p++;
	    sec_size = strlen(p);
	    if (sec_size) {
		mark += "[<span foreground=\"blue\">";
		gchar *m_str = g_markup_escape_text(p, sec_size);
		mark += m_str;
		g_free(m_str);
		mark += "</span>]";
	    }
	    sec_size++;
	    break;
	case 'y':
	    p++;
	    sec_size = strlen(p);
	    if (sec_size) {
		mark += "[<span foreground=\"red\">";
		gchar *m_str = g_markup_escape_text(p, sec_size);
		mark += m_str;
		g_free(m_str);
		mark += "</span>]";
	    }
	    sec_size++;
	    break;
	case 'W':
	    p++;
	    sec_size = g_ntohl(get_uint32(p));
	    //enbale sound button.
	    sec_size += sizeof(guint32);
	    break;
	case 'P':
	    {
		p++;
		sec_size = g_ntohl(get_uint32(p));
		if (sec_size) {
		    GdkPixbufLoader *loader = gdk_pixbuf_loader_new();
		    gdk_pixbuf_loader_write(loader, (const guchar *) (p + sizeof(guint32)), sec_size, NULL);
		    gdk_pixbuf_loader_close(loader, NULL);
		    GdkPixbuf *pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
		    if (pixbuf) {
//			append_and_mark_orig_word(mark, real_oword, LinksPosList());
//			mark.clear();
//			append_pixbuf(pixbuf);
		    } else {
		        mark += _("<span foreground=\"red\">[Load image error!]</span>");
		    }
		    g_object_unref(loader);
		} else {
		    mark += _("<span foreground=\"red\">[Missing Image]</span>");
		}
		sec_size += sizeof(guint32);
	    }
	    break;
	default:
	    if (g_ascii_isupper(*p)) {
		p++;
		sec_size = g_ntohl(get_uint32(p));
		sec_size += sizeof(guint32);
	    } else {
		p++;
		sec_size = strlen(p) + 1;
	    }
	    mark += _("Unknown data type");
	    break;
	}
	p += sec_size;
    }
    return mark;
}

void
Library::ListWords(CurrentIndex *iIndex)
{
    CurrentIndex *iCurrent = (CurrentIndex *) g_memdup(iIndex,
						       sizeof(CurrentIndex) *
						       query_dictmask.size());

    oStarDict->ResultsListClear();

    int iWordCount = 0;
    const gchar *poCurrentWord = poGetCurrentWord(iIndex, query_dictmask, 0);
    if (poCurrentWord) {
	oStarDict->ResultsListInsertLast(poCurrentWord);
	iWordCount++;

	while (iWordCount < 30 && (poCurrentWord = poGetNextWord(NULL, iIndex, query_dictmask, 0))) {
	    oStarDict->ResultsListInsertLast(poCurrentWord);
	    iWordCount++;
	}
    }
    oStarDict->ResultsReScroll();

    if (iCurrent)
	g_free(iCurrent);
}

bool
Library::BuildResultData(std::vector < InstantDictIndex > &dictmask,
			 const char *sWord,
			 CurrentIndex *iIndex,
			 int iLib,
			 GList **result_data)
{
    int iRealLib;
    bool bFound = false, bLookupWord = false, bLookupSynonymWord = false;
    gint nWord = 0, count = 0, i = 0, j = 0;
    glong iWordIdx;
    struct SearchResult *result;

    iRealLib = dictmask[iLib].index;

    bLookupWord = LookupWord(sWord, iIndex[iLib].idx, iIndex[iLib].idx_suggest, iRealLib, 0);
    if (!bLookupWord)
	bLookupWord =
	    LookupSimilarWord(sWord, iIndex[iLib].idx, iIndex[iLib].idx_suggest, iRealLib, 0);
    if (!bLookupWord)
	bLookupWord =
	    SimpleLookupWord(sWord, iIndex[iLib].idx, iIndex[iLib].idx_suggest, iRealLib, 0);

    bLookupSynonymWord =
	LookupSynonymWord(sWord, iIndex[iLib].synidx, iIndex[iLib].synidx_suggest, iRealLib, 0);
    if (!bLookupSynonymWord)
	bLookupSynonymWord =
	    LookupSynonymSimilarWord(sWord, iIndex[iLib].synidx,
				     iIndex[iLib].synidx_suggest, iRealLib, 0);
    if (!bLookupSynonymWord)
	bLookupSynonymWord =
	    SimpleLookupSynonymWord(sWord, iIndex[iLib].synidx,
				    iIndex[iLib].synidx_suggest, iRealLib, 0);

    if (bLookupWord || bLookupSynonymWord) {
	if (bLookupWord)
	    nWord++;

	if (bLookupSynonymWord)
	    nWord += GetOrigWordCount(iIndex[iLib].synidx, iRealLib, false);

	if (bLookupWord) {
	    count = GetOrigWordCount(iIndex[iLib].idx, iRealLib, true);
	    for (i = 0; i < count; i++) {
		result = (SearchResult *) g_malloc(sizeof(struct SearchResult));
		result->bookname = g_strdup(dict_name(iLib).c_str());
		result->exp = g_strdup(poGetOrigWord(iIndex[iLib].idx, iRealLib));
		result->def = g_strdup(parse_data(poGetOrigWordData(iIndex[iLib].idx + i, iRealLib), poGetOrigWord(iIndex[iLib].idx, iRealLib)).c_str());
		*result_data = g_list_append(*result_data, result);
	    }
	    i = 1;
	} else {
	    i = 0;
	}
	for (j = 0; i < nWord; i++, j++) {
	    iWordIdx = poGetOrigSynonymWordIdx(iIndex[iLib].synidx + j, iRealLib);
	    result = (SearchResult *) g_malloc(sizeof(struct SearchResult));
	    result->bookname = g_strdup(dict_name(iLib).c_str());
	    result->exp = g_strdup(poGetOrigWord(iWordIdx, iRealLib));
	    result->def = g_strdup(parse_data(poGetOrigWordData(iWordIdx, iRealLib), poGetOrigWord(iWordIdx, iRealLib)).c_str());
	    *result_data = g_list_append(*result_data, result);
	}

	bFound = true;
    }

    return bFound;
}

void
Library::FreeResultData(GList *result_data)
{
}

bool
Library::SimpleLookup(const gchar *sWord,
		      CurrentIndex *piIndex)
{
    CurrentIndex *iIndex;
    GList *results = NULL;
    bool bFound = false;

    if (!piIndex)
	iIndex = (CurrentIndex *) g_malloc(sizeof(CurrentIndex) * query_dictmask.size());
    else
	iIndex = piIndex;

    for (size_t iLib = 0; iLib < query_dictmask.size(); iLib++) {
	if (BuildResultData(query_dictmask, sWord, iIndex, iLib, &results))
	    bFound = true;
    }

    FreeResultData(results);

    if (!piIndex)
	g_free(iIndex);

    return bFound;
}

bool
Library::LookupWithFuzzy(const gchar *sWord)
{
    static const int MAX_FUZZY_MATCH_ITEM = 100;
    gchar *fuzzy_reslist[MAX_FUZZY_MATCH_ITEM];
    bool bFound = false;

    oStarDict->ResultsListClear();

    bFound = Libs::LookupWithFuzzy(sWord, fuzzy_reslist, MAX_FUZZY_MATCH_ITEM, query_dictmask);
    if (bFound) {
	SimpleLookup(fuzzy_reslist[0], iCurrentIndex);

	for (int i = 0; i < MAX_FUZZY_MATCH_ITEM && fuzzy_reslist[i]; i++) {
	    oStarDict->ResultsListInsertLast(fuzzy_reslist[i]);
	    g_free(fuzzy_reslist[i]);
	}
	oStarDict->ResultsReScroll();
    }

    return bFound;
}

bool
Library::LookupWithRule(const gchar *sWord)
{
    gint iMatchCount = 0;
    bool bFound = false;
    gchar **ppMatchWord =
	(gchar **) g_malloc(sizeof(gchar *) * (MAX_MATCH_ITEM_PER_LIB) * query_dictmask.size());

    oStarDict->ResultsListClear();

    iMatchCount = Libs::LookupWithRule(sWord, ppMatchWord, query_dictmask);
    if (iMatchCount) {
	for (gint i = 0; i < iMatchCount; i++)
	    oStarDict->ResultsListInsertLast(ppMatchWord[i]);

	SimpleLookup(ppMatchWord[0], iCurrentIndex);
	oStarDict->ResultsReScroll();

	for (gint i = 0; i < iMatchCount; i++)
	    g_free(ppMatchWord[i]);

	bFound = true;
    }
    g_free(ppMatchWord);
    return bFound;
}

bool
Library::LookupWithRegex(const gchar *sWord)
{
    gint iMatchCount = 0;
    bool bFound = false;
    gchar **ppMatchWord =
	(gchar **) g_malloc(sizeof(gchar *) * (MAX_MATCH_ITEM_PER_LIB) * query_dictmask.size());

    oStarDict->ResultsListClear();

    iMatchCount = Libs::LookupWithRegex(sWord, ppMatchWord, query_dictmask);
    if (iMatchCount) {
	for (gint i = 0; i < iMatchCount; i++)
	    oStarDict->ResultsListInsertLast(ppMatchWord[i]);

	SimpleLookup(ppMatchWord[0], iCurrentIndex);
	oStarDict->ResultsReScroll();

	for (gint i = 0; i < iMatchCount; i++)
	    g_free(ppMatchWord[i]);

	bFound = true;
    }
    g_free(ppMatchWord);
    return bFound;
}

static void
LookupProgressDialogUpdate(gpointer data,
			   double fraction)
{
    GtkWidget *dialog = GTK_WIDGET(data);
    GtkWidget *progress;

    progress = GTK_WIDGET(g_object_get_data(G_OBJECT(dialog), "progress_bar"));
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), fraction);

    while (gtk_events_pending())
	gtk_main_iteration();
}

bool
Library::LookupData(const gchar *sWord)
{
    GtkWidget *dialog;
    bool cancel = false;
    bool bFound = false;

    std::vector < std::vector < gchar * > > reslist(query_dictmask.size());


    oStarDict->ResultsListClear();
    oStarDict->ShowProgressIndicator(true);
    dialog = oStarDict->CreateLookupProgressDialog(&cancel);

    bFound = Libs::LookupData(sWord, &reslist[0], LookupProgressDialogUpdate, (gpointer) dialog, &cancel, query_dictmask);
    if (bFound) {
	for (size_t iLib = 0; iLib < query_dictmask.size(); iLib++) {
	    if (!reslist[iLib].empty()) {
		SimpleLookup(reslist[iLib][0], iCurrentIndex);

		for (std::vector < gchar *>::iterator i = reslist[iLib].begin();
		     i != reslist[iLib].end(); ++i) {
		    oStarDict->ResultsListInsertLast(*i);
		}
		break;
	    }
	}
	oStarDict->ResultsReScroll();
    }
    oStarDict->ShowProgressIndicator(false);
    oStarDict->DestroyLookupProgressDialog(dialog);
    return bFound;
}

Library::Library(MStarDict *mStarDict):Libs(NULL, FALSE, 0, 0)
{
    oStarDict = mStarDict;
    iCurrentIndex = NULL;
}

Library::~Library()
{
    if (iCurrentIndex)
	g_free(iCurrentIndex);
}
