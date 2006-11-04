//                       YaST2-GTK                                //
// YaST webpage - http://developer.novell.com/wiki/index.php/YaST //

/* YGtkMenuButton is a button that displays a widget when pressed.
   This widget can either be of type GtkMenu or another, like a
   GtkCalendar and we'll do the proper "emulation".
*/

#ifndef YGTK_POPUP_WINDOW_H
#define YGTK_POPUP_WINDOW_H

#include <gtk/gtkwindow.h>
G_BEGIN_DECLS

#define YGTK_TYPE_POPUP_WINDOW            (ygtk_popup_window_get_type ())
#define YGTK_POPUP_WINDOW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                          YGTK_TYPE_POPUP_WINDOW, YGtkPopupWindow))
#define YGTK_POPUP_WINDOW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  \
                                          YGTK_TYPE_POPUP_WINDOW, YGtkPopupWindowClass))
#define IS_YGTK_POPUP_WINDOW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                          YGTK_TYPE_POPUP_WINDOW))
#define IS_YGTK_POPUP_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  \
                                          YGTK_TYPE_POPUP_WINDOW))
#define YGTK_POPUP_WINDOW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  \
                                          YGTK_TYPE_POPUP_WINDOW, YGtkPopupWindowClass))

typedef struct _YGtkPopupWindow       YGtkPopupWindow;
typedef struct _YGtkPopupWindowClass  YGtkPopupWindowClass;

struct _YGtkPopupWindow
{
	GtkWindow window;
};

struct _YGtkPopupWindowClass
{
	GtkWindowClass parent_class;
};

// don't forget to use gtk_widget_show() on the child!
GtkWidget* ygtk_popup_window_new (GtkWidget *child);
GType ygtk_popup_window_get_type (void) G_GNUC_CONST;

void ygtk_popup_window_popup (GtkWidget *widget, gint x, gint y, guint activate_time);

G_END_DECLS
#endif /*YGTK_POPUP_WINDOW_H*/

#ifndef YGTK_MENU_BUTTON_H
#define YGTK_MENU_BUTTON_H

#include <gtk/gtktogglebutton.h>
#include <gtk/gtkmenu.h>
G_BEGIN_DECLS

#define YGTK_TYPE_MENU_BUTTON            (ygtk_menu_button_get_type ())
#define YGTK_MENU_BUTTON(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                          YGTK_TYPE_MENU_BUTTON, YGtkMenuButton))
#define YGTK_MENU_BUTTON_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  \
                                          YGTK_TYPE_MENU_BUTTON, YGtkMenuButtonClass))
#define IS_YGTK_MENU_BUTTON(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                          YGTK_TYPE_MENU_BUTTON))
#define IS_YGTK_MENU_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  \
                                          YGTK_TYPE_MENU_BUTTON))
#define YGTK_MENU_BUTTON_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  \
                                          YGTK_TYPE_MENU_BUTTON, YGtkMenuButtonClass))

typedef struct _YGtkMenuButton       YGtkMenuButton;
typedef struct _YGtkMenuButtonClass  YGtkMenuButtonClass;

struct _YGtkMenuButton
{
	GtkToggleButton button;

	GtkWidget *label, *popup;
};

struct _YGtkMenuButtonClass
{
	GtkToggleButtonClass parent_class;
};

GtkWidget* ygtk_menu_button_new();
GType ygtk_menu_button_get_type (void) G_GNUC_CONST;

void ygtk_menu_button_set_label (YGtkMenuButton *button, const gchar *label);

/* Popup must be either a GtkMenu or a YGtkPopupWindow. */
// You may hide your popup "manually" with gtk_widget_hide() on it
void ygtk_menu_button_set_popup (YGtkMenuButton *button, GtkWidget *popup);

G_END_DECLS
#endif /*YGTK_MENU_BUTTON_H*/
