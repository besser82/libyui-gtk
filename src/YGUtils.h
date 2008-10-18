/********************************************************************
 *           YaST2-GTK - http://en.opensuse.org/YaST2-GTK           *
 ********************************************************************/

#ifndef YGUTILS_H
#define YGUTILS_H

#include <string>
#include <list>
#include <gtk/gtktextview.h>
#include <gtk/gtkentry.h>

/* YGUtils.h/cc have some functionality that is shared between different parts
   of the code. */

namespace YGUtils
{
	/* Replaces Yast's '&' accelerator by Gnome's '_' (and proper escaping). */
	std::string mapKBAccel (const std::string &src);

	/* Adds filter support to a GtkEntry. */
	void setFilter (GtkEntry *entry, const std::string &validChars);

	/* Replaces every 'mouth' by 'food' in 'str'. */
	void replace (std::string &str, const char *mouth, int mouth_len, const char *food);

	/* Truncates the text with "..." past the given length.
	   pos: -1 = start, 0 = middle, 1 = end. Honors utf-8 characters. */
	std::string truncate (const std::string &str, int length, int pos);

	/* Escapes markup text (eg. changes '<' by '\<'). */
	void escapeMarkup (std::string &str);

	/* Adds functionality to scroll widgets to top or bottom. */
	void scrollWidget (GtkAdjustment *vadj, bool top);

	/* Returns the average width of the given number of characters in pixels. */
	int getCharsWidth (GtkWidget *widget, int chars_nb);
	int getCharsHeight (GtkWidget *widget, int chars_nb);

	/* Sets some widget font proprities. */
	void setWidgetFont (GtkWidget *widget, PangoStyle style, PangoWeight weight, double scale);

	/* Saves some code and standardizes the error. Returns NULL if failed.
	   Don't forget to g_object_unref it! */
	GdkPixbuf *loadPixbuf (const std::string &fileneme);

	/* Shifts colors in a GdkPixbuf. */
	GdkPixbuf *setOpacity (const GdkPixbuf *src, int opacity);

	/* Tries to make sense out of the string, applying some stock icon to the button. */
	bool setStockIcon (GtkWidget *button, const std::string &label,
	                   const char *fallbackIcon);
};

extern "C" {
	void ygutils_setWidgetFont (GtkWidget *widget, PangoStyle style, PangoWeight weight, double scale);

	void ygutils_setFilter (GtkEntry *entry, const char *validChars);

	void ygutils_scrollAdj (GtkAdjustment *vadj, gboolean top);

	gboolean ygutils_setStockIcon (GtkWidget *button, const char *label,
	                               const char *fallbackIcon);

	GdkPixbuf *ygutils_setOpacity (const GdkPixbuf *src, int opacity);
};

#endif // YGUTILS_H

