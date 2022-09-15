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
#include "mstardict.hpp"

#include <espeak/speak_lib.h>

enum {
    DISPNAME_COLUMN,
    NAME_COLUMN,
    N_COLUMNS
};

enum {
    ENGINE_NONE = 0,
    ENGINE_ESPEAK,
    ENGINE_REALPEOPLE
};

enum {
    GENDER_NONE = 0,
    GENDER_MALE,
    GENDER_FEMALE
};

static TtsVoice voices[] = {
    {"afrikaans",		N_("Afrikaans")},
    {"bosnian",			N_("Bosnian")},
    {"catalan",			N_("Catalan")},
    {"czech",			N_("Czech")},
    {"welsh-test",		N_("Welsh")},
    {"danish",			N_("Danish")},
    {"german",			N_("German")},
    {"greek",			N_("Greek")},
    {"en-scottish",		N_("English (Scottish)")},
    {"english",			N_("English")},
    {"lancashire",		N_("English (Lancashire)")},
    {"english_rp",		N_("English (Received Pronunciation)")},
    {"english_wmids",		N_("English (West Midlands)")},
    {"english-us",		N_("English (American)")},
    {"en-westindies",		N_("English (Westindies)")},
    {"esperanto",		N_("Esperanto")},
    {"spanish",			N_("Spanish")},
    {"spanish-latin-american",	N_("Spanish (Latin American)")},
    {"finnish",			N_("Finnish")},
    {"french",			N_("French")},
    {"french (Belgium)",	N_("French (Belgium)")},
    {"greek-ancient",		N_("Greek (Ancient)")},
    {"hindi",			N_("Hindi")},
    {"croatian",		N_("Croatian")},
    {"hungarian",		N_("Hungarian")},
    {"armenian",		N_("Armenian")},
    {"armenian-west",		N_("Armenian (West)")},
    {"indonesian-test",		N_("Indonesian")},
    {"icelandic-test",		N_("Icelandic")},
    {"italian",			N_("Italian")},
    {"lojban",			N_("Lojban")},
    {"kurdish",			N_("Kurdish")},
    {"latin",			N_("Latin")},
    {"latvian",			N_("Latvian")},
    {"macedonian-test",		N_("Macedonian")},
    {"dutch-test",		N_("Dutch")},
    {"norwegian",		N_("Norwegian")},
    {"papiamento-test",		N_("Papiamento")},
    {"polish",			N_("Polish")},
    {"brazil",			N_("Brazil")},
    {"portugal",		N_("Portugal")},
    {"romanian",		N_("Romanian")},
    {"russian_test",		N_("Russian")},
    {"slovak",			N_("Slovak")},
    {"albanian",		N_("Albanian")},
    {"serbian",			N_("Serbian")},
    {"swedish",			N_("Swedish")},
    {"swahihi-test",		N_("Swahihi")},
    {"tamil",			N_("Tamil")},
    {"turkish",			N_("Turkish")},
    {"vietnam",			N_("Vietnamese")},
    {"Mandarin",		N_("Chinese (Mandarin)")},
    {"cantonese",		N_("Chinese (Cantonese)")},
    {"akan-test",		N_("Akam")},
    {"amharic-test",		N_("Amharic")},
    {"azerbaijani-test",	N_("Azerbaijani")},
    {"bulgarian-test",		N_("Bulgarian")},
    {"dari-test",		N_("Dari")},
    {"divehi-test",		N_("Divehi")},
    {"georgian-test",		N_("Georgian")},
    {"haitian",			N_("Haitian")},
    {"kannada",			N_("Kannada")},
    {"kinyarwanda-test",	N_("Kinyarwanda")},
    {"malayalam",		N_("Mlayalam")},
    {"nahuatl - clasical",	N_("Nahuatl (Clasical)")},
    {"nepali-test",		N_("Nepali")},
    {"northern-sotho",		N_("Northern Sotho")},
    {"punjabi-test",		N_("Punjabi")},
    {"setswana-test",		N_("Setswana")},
    {"sinhala",			N_("Sinhala")},
    {"slovenian-test",		N_("Slovenian")},
    {"swahili-test",		N_("Swahili")},
    {"telugu",			N_("Telugu")},
    {"urdu-test",		N_("Urdu")},
    {"wolof-test",		N_("Wolof")},
    {"default", 		N_("Default")},
    {NULL,			NULL}
};


Tts::Tts(MStarDict *mStarDict)
{
    int rate = 0;
    bool tts_enabled = false;
    int engine = ENGINE_NONE;
    gchar *language = NULL;
    int gender = GENDER_NONE;

    oStarDict = mStarDict;
    Enabled = false;

    /* initialize espeak */
    rate = espeak_Initialize(AUDIO_OUTPUT_PLAYBACK, 512, NULL, 0);

    /* check if tts is enabled */
    if (!oStarDict->oConf->GetBool("/apps/maemo/mstardict/tts_enabled", &tts_enabled)) {
	tts_enabled = true;
    }
    Enable(tts_enabled);

    /* read configured engine */
    if (!oStarDict->oConf->GetInt("/apps/maemo/mstardict/tts_engine", &engine)) {
	engine = ENGINE_ESPEAK;
    }

    /* read configured language */
    if (!oStarDict->oConf->GetString("/apps/maemo/mstardict/tts_language", &language)) {
	language = g_strdup("english");
    }

    /* read configured gender */
    if (!oStarDict->oConf->GetInt("/apps/maemo/mstardict/tts_gender", &gender)) {
	gender = GENDER_MALE;
    }
    SetVoice(engine, language, gender);

    if (language) {
	g_free(language);
    }
}

Tts::~Tts()
{
    /* deinitialize espeak */
    espeak_Terminate();
}

void
Tts::Enable(bool bEnable)
{
    Enabled = bEnable;
}

bool
Tts::IsEnabled()
{
    return Enabled;
}

void
Tts::SetVoice(int engine, const gchar *language, int gender)
{
    Engine = engine;

    espeak_VOICE voice;

    memset(&voice, 0, sizeof(espeak_VOICE));
    voice.name = language;
    voice.gender = gender;

    espeak_SetVoiceByProperties(&voice);
}

void
Tts::SayText(const gchar *sText)
{
    if (Engine == ENGINE_ESPEAK) {
        espeak_Synth(sText, strlen(sText) + 1, 0, POS_CHARACTER, 0, espeakCHARS_UTF8, NULL, NULL);
        return;
    } else if (Engine == ENGINE_REALPEOPLE) {
	gchar *lower = g_utf8_strdown(sText, -1);
	if (!lower)
	    return;
	
	gchar *cmd = g_strdup_printf("paplay /home/user/MyDocs/mstardict/WyabdcRealPeopleTTS/%c/%s.wav", lower[0], lower);
	if (!cmd)
	    return;
	
	system(cmd);
	
	g_free(lower);
	g_free(cmd);
    } else {
	/* unknown engine */
    }
}

GtkListStore *
Tts::GetVoicesList()
{
    const espeak_VOICE **espeak_voices;
    GtkListStore *list_store;
    GtkTreeIter iter;
    size_t i = 0;

    list_store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING);

    espeak_voices = espeak_ListVoices(NULL);
    while (espeak_voices[i]) {
	gchar *disp_name = NULL;

	for (int j = 0; voices[j].name != NULL; j++) {
	    if (!strcmp(espeak_voices[i]->name, voices[j].name)) {
		disp_name = g_strdup(_(voices[j].disp_name));
		break;
	    }
	}

	if (disp_name == NULL)
	    disp_name = g_strdup(espeak_voices[i]->name);

	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store,
			   &iter,
			   DISPNAME_COLUMN, disp_name,
			   NAME_COLUMN, espeak_voices[i]->name,
			   -1);

	if (disp_name)
	    g_free(disp_name);

	i++;
    }
    return list_store;
}

gboolean
Tts::onTtsEnableButtonClicked(GtkButton *button,
			      Tts *oTts)
{
    if (hildon_check_button_get_active(HILDON_CHECK_BUTTON(button))) {
	gtk_widget_set_sensitive(GTK_WIDGET(oTts->engine_espeak_button), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(oTts->engine_realpeople_button), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(oTts->gender_male_button), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(oTts->gender_female_button), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(oTts->language_button), TRUE);
    } else {
	gtk_widget_set_sensitive(GTK_WIDGET(oTts->engine_espeak_button), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(oTts->engine_realpeople_button), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(oTts->gender_male_button), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(oTts->gender_female_button), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(oTts->language_button), FALSE);
    }
    return true;
}

gboolean
Tts::onTtsEngineButtonClicked(GtkWidget *button,
			      Tts *oTts)
{
    GtkWidget *button2;

    if (button != oTts->engine_realpeople_button)
	button2 = oTts->engine_realpeople_button;
    else
	button2 = oTts->engine_espeak_button;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button2), FALSE);
    else
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button2), TRUE);
    
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(oTts->engine_espeak_button))) {
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(oTts->engine_realpeople_button), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(oTts->gender_male_button), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(oTts->gender_female_button), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(oTts->language_button), TRUE);
    } else {
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(oTts->engine_realpeople_button), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(oTts->gender_male_button), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(oTts->gender_female_button), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(oTts->language_button), FALSE);
    }

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(oTts->engine_realpeople_button))) {
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(oTts->engine_espeak_button), FALSE);
    } else {
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(oTts->engine_espeak_button), TRUE);
    }
    
    return true;
}

gboolean
Tts::onTtsGenderButtonClicked(GtkToggleButton *button1,
			      GtkToggleButton *button2)
{
    if (gtk_toggle_button_get_active(button1))
	gtk_toggle_button_set_active(button2, FALSE);
    else
	gtk_toggle_button_set_active(button2, TRUE);
    return true;
}

GtkWidget *
Tts::CreateTtsConfWidget()
{
    GtkWidget *frame, *vbox, *engine_hbox, *gender_hbox;
    GtkWidget *selector;
    HildonTouchSelectorColumn *column;
    GtkListStore *voices_list;

    frame = gtk_frame_new(_("Text-To-Speech"));

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(frame), vbox);

    /* enable tts button */
    enable_button = hildon_check_button_new(HILDON_SIZE_FINGER_HEIGHT);
    gtk_button_set_label(GTK_BUTTON(enable_button), _("Enable"));
    gtk_box_pack_start(GTK_BOX(vbox), enable_button, TRUE, TRUE, 0);

    /* engine selection */
    engine_hbox = gtk_hbox_new(TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), engine_hbox, TRUE, TRUE, 0);

    engine_espeak_button = hildon_gtk_toggle_button_new(HILDON_SIZE_FINGER_HEIGHT);
    gtk_button_set_label(GTK_BUTTON(engine_espeak_button), _("eSpeak"));
    gtk_box_pack_start(GTK_BOX(engine_hbox), engine_espeak_button, TRUE, TRUE, 0);

    engine_realpeople_button = hildon_gtk_toggle_button_new(HILDON_SIZE_FINGER_HEIGHT);
    gtk_button_set_label(GTK_BUTTON(engine_realpeople_button), _("Real People"));
    gtk_box_pack_start(GTK_BOX(engine_hbox), engine_realpeople_button, TRUE, TRUE, 0);

    /* gender selection */
    gender_hbox = gtk_hbox_new(TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), gender_hbox, TRUE, TRUE, 0);

    gender_male_button = hildon_gtk_toggle_button_new(HILDON_SIZE_FINGER_HEIGHT);
    gtk_button_set_label(GTK_BUTTON(gender_male_button), _("Male"));
    gtk_box_pack_start(GTK_BOX(gender_hbox), gender_male_button, TRUE, TRUE, 0);

    gender_female_button = hildon_gtk_toggle_button_new(HILDON_SIZE_FINGER_HEIGHT);
    gtk_button_set_label(GTK_BUTTON(gender_female_button), _("Female"));
    gtk_box_pack_start(GTK_BOX(gender_hbox), gender_female_button, TRUE, TRUE, 0);

    /* language picker button */
    language_button = hildon_picker_button_new(HILDON_SIZE_FINGER_HEIGHT,
					       HILDON_BUTTON_ARRANGEMENT_VERTICAL);
    hildon_button_set_title(HILDON_BUTTON(language_button), _("Language"));
    hildon_button_set_alignment(HILDON_BUTTON(language_button), 0.0, 0.5, 1.0, 0.0);
    hildon_button_set_title_alignment(HILDON_BUTTON(language_button), 0.0, 0.5);
    gtk_box_pack_start(GTK_BOX(vbox), language_button, TRUE, TRUE, 0);

    /* get list of voices */
    voices_list = GetVoicesList();

    /* sort list of voices */
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(voices_list),
					 DISPNAME_COLUMN, GTK_SORT_ASCENDING);

    /* voices selector */
    selector = hildon_touch_selector_new();
    column = hildon_touch_selector_append_text_column(HILDON_TOUCH_SELECTOR(selector),
						      GTK_TREE_MODEL(voices_list), TRUE);
    g_object_set(G_OBJECT(column), "text-column", DISPNAME_COLUMN, NULL);

    hildon_picker_button_set_selector(HILDON_PICKER_BUTTON(language_button),
				      HILDON_TOUCH_SELECTOR(selector));

    g_signal_connect(enable_button, "clicked", G_CALLBACK(onTtsEnableButtonClicked), this);
    g_signal_connect(engine_espeak_button, "clicked", G_CALLBACK(onTtsEngineButtonClicked), this);
    g_signal_connect(engine_realpeople_button, "clicked", G_CALLBACK(onTtsEngineButtonClicked), this);
    g_signal_connect(gender_male_button, "clicked", G_CALLBACK(onTtsGenderButtonClicked), gender_female_button);
    g_signal_connect(gender_female_button, "clicked", G_CALLBACK(onTtsGenderButtonClicked), gender_male_button);

    return frame;
}

void
Tts::TtsConfWidgetLoadConf()
{
    HildonTouchSelector *selector;
    GtkTreeIter iter;
    int engine = ENGINE_NONE;
    int gender = GENDER_NONE;
    gchar *language = NULL;
    GtkTreeModel *model;
    gboolean iter_valid = TRUE;

    if (IsEnabled()) {
	hildon_check_button_set_active(HILDON_CHECK_BUTTON(enable_button), TRUE);
	gtk_widget_set_sensitive(language_button, TRUE);
    } else {
	hildon_check_button_set_active(HILDON_CHECK_BUTTON(enable_button), FALSE);
	gtk_widget_set_sensitive(language_button, FALSE);
    }

    if (oStarDict->oConf->GetInt("/apps/maemo/mstardict/tts_engine", &engine)) {
	if (engine == ENGINE_ESPEAK)
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(engine_espeak_button), TRUE);
	else
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(engine_realpeople_button), TRUE);
    } else {
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(engine_espeak_button), TRUE);
    }

    if (oStarDict->oConf->GetInt("/apps/maemo/mstardict/tts_gender", &gender)) {
	if (gender == GENDER_MALE)
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gender_male_button), TRUE);
	else
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gender_female_button), TRUE);
    } else {
	    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gender_male_button), TRUE);
    }

    if (oStarDict->oConf->GetString("/apps/maemo/mstardict/tts_language", &language)) {
	if (language) {
	    selector = hildon_picker_button_get_selector(HILDON_PICKER_BUTTON(language_button));
	    model = hildon_touch_selector_get_model(HILDON_TOUCH_SELECTOR(selector), DISPNAME_COLUMN);
	    for (iter_valid = gtk_tree_model_get_iter_first(model, &iter); iter_valid; iter_valid = gtk_tree_model_iter_next(model, &iter)) {
		const gchar *tmp;
		gtk_tree_model_get(model, &iter, NAME_COLUMN, &tmp, -1);
		if (strcmp(tmp, language) == 0) {
		    hildon_touch_selector_select_iter(HILDON_TOUCH_SELECTOR(selector), DISPNAME_COLUMN, &iter, FALSE);
		    break;
		}
	    }
	    g_free(language);
	}
    } else {
	hildon_picker_button_set_active (HILDON_PICKER_BUTTON (language_button), -1);
    }
}

void
Tts::TtsConfWidgetSaveConf()
{
    bool enabled = false;
    HildonTouchSelector *selector;
    GtkTreeIter iter;
    int engine = ENGINE_NONE;
    int gender = GENDER_NONE;
    const gchar *language = NULL;

    enabled = hildon_check_button_get_active(HILDON_CHECK_BUTTON(enable_button));
    if (oStarDict->oConf->SetBool("/apps/maemo/mstardict/tts_enabled", enabled))
	Enable(enabled);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(engine_espeak_button)))
	engine = ENGINE_ESPEAK;
    else
	engine = ENGINE_REALPEOPLE;
    oStarDict->oConf->SetInt("/apps/maemo/mstardict/tts_engine", engine);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gender_male_button)))
	gender = GENDER_MALE;
    else
	gender = GENDER_FEMALE;
    oStarDict->oConf->SetInt("/apps/maemo/mstardict/tts_gender", gender);

    if (engine)
	Engine = engine;

    if (hildon_picker_button_get_active(HILDON_PICKER_BUTTON(language_button)) > -1) {
	selector = hildon_picker_button_get_selector(HILDON_PICKER_BUTTON(language_button));
	if (hildon_touch_selector_get_selected(selector, DISPNAME_COLUMN, &iter)) {
	    gtk_tree_model_get(hildon_touch_selector_get_model(selector, DISPNAME_COLUMN), &iter, NAME_COLUMN, &language, -1);

	    /* fixme convert back disp_name */
	    if (oStarDict->oConf->SetString("/apps/maemo/mstardict/tts_language", language))
		SetVoice(engine, language, gender);
	}
    }
}
