/********************************************************************
 *           YaST2-GTK - http://en.opensuse.org/YaST2-GTK           *
 ********************************************************************/
/*
  Textdomain "yast2-gtk"
 */

#define YUILogComponent "gtk"
#include "config.h"
#include <YPackageSelector.h>
#include <string.h>
#include "YGUI.h"
#include "YGUtils.h"
#include "YGi18n.h"
#include "YGDialog.h"

#include "ygtkwizard.h"
#include "ygtkfindentry.h"
#include "ygtkmenubutton.h"
#include "ygtkhtmlwrap.h"
#include "ygtkrichtext.h"
#include "ygtktreeview.h"
#include "ygtktooltip.h"
#include "ygtkpackageview.h"
#include "ygtkdetailview.h"
#include "ygtkdiskview.h"
#include "ygtknotebook.h"

// experiments:
extern bool search_entry_side, search_entry_top, dynamic_sidebar,
	expander_sidebar, flex_sidebar, grid_sidebar, layered_sidebar,
	layered_tabs_sidebar, startup_menu, big_icons_sidebar, icons_sidebar,
	categories_side,
	repositories_side, categories_top, repositories_top, status_side, status_top,
	status_tabs, status_tabs_as_actions,
	undo_side, undo_tab, undo_old_style, undo_log_all, undo_log_changed, undo_box,
	status_col,
	action_col, action_col_as_button, action_col_as_check, version_col,
	single_line_rows, details_start_hide, toolbar_top, toolbar_yast, arrange_by;

//** UI components -- split up for re-usability, but mostly for readability

static gboolean paint_white_expose_cb (GtkWidget *widget, GdkEventExpose *event)
{
	cairo_t *cr = gdk_cairo_create (widget->window);
	GdkColor white = { 0, 255 << 8, 255 << 8, 255 << 8 };
	gdk_cairo_set_source_color (cr, &white);
	cairo_rectangle (cr, event->area.x, event->area.y,
			         event->area.width, event->area.height);
	cairo_fill (cr);
	cairo_destroy (cr);
	return FALSE;
}

class UndoView
{
Ypp::PkgList m_changes;

public:
//	GtkWidget *getWidget() { return m_vbox; }

	UndoView()
	{
		Ypp::QueryBase *query = new Ypp::QueryProperty ("to-modify", true);
		m_changes = Ypp::PkgQuery (Ypp::Package::PACKAGE_TYPE, query);
#if 0
		GtkWidget *view = createView (listener);

		m_vbox = gtk_vbox_new (FALSE, 6);
		gtk_container_set_border_width (GTK_CONTAINER (m_vbox), 6);
		gtk_box_pack_start (GTK_BOX (m_vbox), view, TRUE, TRUE, 0);
#if 0
// FIXME: temporarily disable uncouple button
// issues: * too visible (also the labeling/icon are a bit off)
//         * undo window should not overlap (or at least placed at the side on start)
		GtkWidget *uncouple_button = gtk_button_new_with_label (_("Uncouple"));
		GtkWidget *icon = gtk_image_new_from_stock (GTK_STOCK_DISCONNECT, GTK_ICON_SIZE_BUTTON);
		gtk_button_set_image (GTK_BUTTON (uncouple_button), icon);
		GtkWidget *uncouple_align = gtk_alignment_new (1, .5, 0, 1);
		gtk_container_add (GTK_CONTAINER (uncouple_align), uncouple_button);
		gtk_widget_set_tooltip_text (uncouple_button, _("Open in new window"));
		g_signal_connect (G_OBJECT (uncouple_button), "clicked",
		                  G_CALLBACK (uncouple_clicked_cb), this);
		gtk_box_pack_start (GTK_BOX (m_vbox), uncouple_align, FALSE, TRUE, 0);
#endif
		gtk_widget_show_all (m_vbox);
		g_object_ref_sink (m_vbox);
#endif
	}

#if 0
	~UndoView()
	{ g_object_unref (m_vbox); }
#endif

	GtkWidget *createView (YGtkPackageView::Listener *listener, bool horizontal)
	{
		GtkWidget *main_box = horizontal ? gtk_hbox_new (TRUE, 6) : gtk_vbox_new (TRUE, 6);
		for (int i = 0; i < 3; i++) {
			const char *str = 0, *stock = 0;
			Ypp::QueryAnd *query = new Ypp::QueryAnd();
			const char *prop;
			switch (i) {
				case 0:
					str = _("To install:");
					stock = GTK_STOCK_ADD;
					query->add (new Ypp::QueryProperty ("to-install", true));
					query->add (new Ypp::QueryProperty ("is-installed", false));
					prop = "to-install";
					break;
				case 1:
					str = _("To upgrade:");
					stock = GTK_STOCK_GO_UP;
					query->add (new Ypp::QueryProperty ("to-install", true));
					query->add (new Ypp::QueryProperty ("is-installed", true));
					prop = "to-upgrade";
					break;
				case 2:
					str = _("To remove:");
					stock = GTK_STOCK_REMOVE;
					query->add (new Ypp::QueryProperty ("to-remove", true));
					prop = "to-remove";
					break;
				default: break;
			}
			Ypp::PkgQuery list (m_changes, query);

			GtkWidget *label_box, *icon, *label;
			label = gtk_label_new (str);
			gtk_misc_set_alignment (GTK_MISC (label), 0, .5);
			icon = gtk_image_new_from_stock (stock, GTK_ICON_SIZE_MENU);
			label_box = gtk_hbox_new (FALSE, 6);
			gtk_box_pack_start (GTK_BOX (label_box), icon, FALSE, TRUE, 0);
			gtk_box_pack_start (GTK_BOX (label_box), label, TRUE, TRUE, 0);
			YGtkPackageView *view = ygtk_package_view_new (FALSE);
			view->setVisible ("available-version", false);
/*			view->appendCheckColumn (checkCol);
			view->appendTextColumn (NULL, NAME_PROP, -1, true);*/
			view->setActivateAction (YGtkPackageView::UNDO_ACTION);
			view->setList (list, NULL);
			view->setListener (listener);
			gtk_scrolled_window_set_shadow_type (
				GTK_SCROLLED_WINDOW (view), GTK_SHADOW_IN);

			GtkWidget *box = gtk_vbox_new (FALSE, 6);
			gtk_box_pack_start (GTK_BOX (box), label_box, FALSE, TRUE, 0);
			gtk_box_pack_start (GTK_BOX (box), GTK_WIDGET (view), TRUE, TRUE, 0);
			gtk_box_pack_start (GTK_BOX (main_box), box, TRUE, TRUE, 0);
		}
		return main_box;
	}
#if 0
private:
	static void uncouple_clicked_cb (GtkButton *button, UndoView *pThis)
	{
		gtk_widget_hide (pThis->m_vbox);
		GtkWidget *dialog = gtk_dialog_new_with_buttons (_("Undo History"),
			YGDialog::currentWindow(), GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE, NULL);
		gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_CLOSE);
		gtk_window_set_default_size (GTK_WINDOW (dialog), 500, 450);
		gtk_container_set_border_width (GTK_CONTAINER (dialog), 6);
		g_signal_connect (G_OBJECT (dialog), "delete-event",
		                  G_CALLBACK (uncouple_delete_event_cb), pThis);
		g_signal_connect (G_OBJECT (dialog), "response",
		                  G_CALLBACK (close_response_cb), pThis);

		GtkWidget *view = pThis->createView (NULL, true);
		gtk_container_add (GTK_CONTAINER (gtk_dialog_get_content_area (GTK_DIALOG (dialog))), view);
		gtk_widget_show_all (dialog);
	}

	static gboolean uncouple_delete_event_cb (
		GtkWidget *widget, GdkEvent *event, UndoView *pThis)
	{
		gtk_widget_show (pThis->m_vbox);
		return FALSE;
	}

	static void close_response_cb (GtkDialog *dialog, gint response, UndoView *pThis)
	{
		gtk_widget_show (pThis->m_vbox);
		gtk_widget_destroy (GTK_WIDGET (dialog));
	}
#endif
};

static void small_button_style_set_cb (GtkWidget *button, GtkStyle *prev_style)
{
	// based on gedit
	int width, height;
	gtk_icon_size_lookup_for_settings (gtk_widget_get_settings (button),
		GTK_ICON_SIZE_MENU, &width, &height);
	gtk_widget_set_size_request (button, width+2, height+2);
}

static GtkWidget *create_small_button (const char *stock_icon, const char *tooltip)
{
	static bool first_time = true;
	if (first_time) {
		first_time = false;
		gtk_rc_parse_string (
			"style \"small-button-style\"\n"
			"{\n"
			"  GtkWidget::focus-padding = 0\n"
			"  GtkWidget::focus-line-width = 0\n"
			"  xthickness = 0\n"
			"  ythickness = 0\n"
			"}\n"
			"widget \"*.small-button\" style \"small-button-style\"");
	}

	GtkWidget *button = gtk_button_new();
	gtk_widget_set_name (button, "small-button");
	gtk_button_set_focus_on_click (GTK_BUTTON (button), FALSE);
	gtk_button_set_relief (GTK_BUTTON (button), GTK_RELIEF_NONE);
	gtk_widget_set_tooltip_text (button, tooltip);
	GtkWidget *image = gtk_image_new_from_stock (stock_icon, GTK_ICON_SIZE_MENU);
	gtk_button_set_image (GTK_BUTTON (button), image);
	g_signal_connect (G_OBJECT (button), "style-set",
	                  G_CALLBACK (small_button_style_set_cb), NULL);
	return button;
}

class ChangesPane : public Ypp::PkgList::Listener
{
	struct Entry {
		GtkWidget *m_box, *m_label, *m_button;
		GtkWidget *getWidget() { return m_box; }

		Entry (Ypp::Package *package)
		{
			m_label = gtk_label_new ("");
			gtk_misc_set_alignment (GTK_MISC (m_label), 0, 0.5);
			gtk_label_set_ellipsize (GTK_LABEL (m_label), PANGO_ELLIPSIZE_END);
			m_button = create_small_button (GTK_STOCK_UNDO, _("Undo"));
			m_box = gtk_hbox_new (FALSE, 6);
			gtk_box_pack_start (GTK_BOX (m_box), m_label, TRUE, TRUE, 0);
			gtk_box_pack_start (GTK_BOX (m_box), m_button, FALSE, TRUE, 0);
			gtk_widget_show_all (m_box);
			modified (package);
			g_signal_connect (G_OBJECT (m_button), "clicked",
			                  G_CALLBACK (undo_clicked_cb), package);
		}

		void modified (Ypp::Package *package)
		{
			const Ypp::Package::Version *version = 0;
			std::string text, action;
			if (package->toInstall (&version)) {
				if (package->isInstalled()) {
					if (version->cmp > 0)
						action = _("upgrade");
					else if (version->cmp < 0)
						action = _("downgrade");
					else
						action = _("re-install");
				}
				else if (package->type() == Ypp::Package::PATCH_TYPE)
					action = _("patch");
				else
					action = _("install");
			}
			else
				action = _("remove");
			text = action + " " + package->name();
			if (package->isAuto()) {
				text = "\t" + text;
				gtk_widget_hide (m_button);
			}
			else
				gtk_widget_show (m_button);
			gtk_label_set_text (GTK_LABEL (m_label), text.c_str());
			std::string tooltip = action + " " + package->name();
			if (version)
				tooltip += std::string (_("\nfrom")) + " <i>" + version->repo->name + "</i>";
			gtk_widget_set_tooltip_markup (m_label, tooltip.c_str());
		}

		static void undo_clicked_cb (GtkButton *button, Ypp::Package *package)
		{
			package->undo();
		}
	};

	struct LogEntry {
		GtkWidget *m_box, *m_label, *m_button;
		Ypp::Log::Entry::Action m_action;
		GtkWidget *getWidget() { return m_box; }

		LogEntry (Ypp::Package *package, Ypp::Log::Entry::Action action)
		: m_action (action)
		{
			std::string label = action == Ypp::Log::Entry::INSTALL_ACTION ?
				_("install") : _("remove");
			label += " " + package->name();
			m_label = gtk_label_new (label.c_str());
			gtk_misc_set_alignment (GTK_MISC (m_label), 0, 0.5);
			gtk_label_set_ellipsize (GTK_LABEL (m_label), PANGO_ELLIPSIZE_END);
			if (canUndo (package, action)) {
				m_button = create_small_button (GTK_STOCK_UNDO, _("Undo"));
				m_box = gtk_hbox_new (FALSE, 6);
				gtk_box_pack_start (GTK_BOX (m_box), m_label, TRUE, TRUE, 0);
				gtk_box_pack_start (GTK_BOX (m_box), m_button, FALSE, TRUE, 0);
				g_signal_connect (G_OBJECT (m_button), "clicked",
					              G_CALLBACK (undo_clicked_cb), package);
			}
			else
				m_box = m_label;
			gtk_widget_show_all (m_box);
		}

		static void undo_clicked_cb (GtkButton *button, Ypp::Package *package)
		{
#if 0
			if (m_action == Ypp::Log::Entry::INSTALL_ACTION)
				package->remove();
			else
				package->install (0);
#endif
			if (package->isInstalled())
				package->remove();
			else
				package->install (0);
			gtk_widget_hide (GTK_WIDGET (button));
		}

		static bool canUndo (Ypp::Package *package, Ypp::Log::Entry::Action action)
		{
			return (action == Ypp::Log::Entry::INSTALL_ACTION && package->isInstalled())
				|| (action == Ypp::Log::Entry::REMOVE_ACTION && !package->isInstalled());
		}
	};

GtkWidget *m_handle_box, *m_empty_label, *m_entries_box;
Ypp::PkgList m_pool;
GList *m_entries;

public:
	GtkWidget *getWidget()
	{ return m_handle_box; }

	ChangesPane()
	: m_entries (NULL)
	{
		GtkWidget *heading = gtk_label_new (_("To perform now:"));
		YGUtils::setWidgetFont (heading, PANGO_STYLE_NORMAL, PANGO_WEIGHT_BOLD, PANGO_SCALE_MEDIUM);
		gtk_misc_set_alignment (GTK_MISC (heading), 0, 0.5);
		m_entries_box = gtk_vbox_new (FALSE, 2);

		m_empty_label = gtk_label_new (_("Package changes will\nbe listed here."));
		YGUtils::setWidgetFont (m_empty_label, PANGO_STYLE_ITALIC, PANGO_WEIGHT_NORMAL, PANGO_SCALE_MEDIUM);
		gtk_label_set_justify (GTK_LABEL (m_empty_label), GTK_JUSTIFY_CENTER);
		gtk_misc_set_alignment (GTK_MISC (m_empty_label), 0.5, 0.5);

		GtkWidget *vbox = gtk_vbox_new (FALSE, 6);
		gtk_container_set_border_width (GTK_CONTAINER (vbox), 4);  // FIXME: are these inner borders?
		gtk_box_pack_start (GTK_BOX (vbox), heading, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), m_empty_label, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), m_entries_box, TRUE, TRUE, 0);

		g_signal_connect (G_OBJECT (vbox), "expose-event",
			              G_CALLBACK (paint_white_expose_cb), NULL);

		GtkWidget *scroll = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
		                                GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
		gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scroll), vbox);
		GtkWidget *port = gtk_bin_get_child (GTK_BIN (scroll));
		gtk_viewport_set_shadow_type (GTK_VIEWPORT (port), GTK_SHADOW_NONE);

/*
		ygtk_wizard_set_information_expose_hook (vbox, &vbox->allocation);
		ygtk_wizard_set_information_expose_hook (m_entries_box, &m_entries_box->allocation);
*/
		int width = YGUtils::getCharsWidth (vbox, 32);
		gtk_widget_set_size_request (vbox, width, -1);
		gtk_widget_show_all (vbox);

		GtkWidget *frame = gtk_frame_new (NULL);
		gtk_container_add (GTK_CONTAINER (frame), scroll);

		m_handle_box = gtk_handle_box_new();
		gtk_container_add (GTK_CONTAINER (m_handle_box), frame);
		gtk_handle_box_set_handle_position (GTK_HANDLE_BOX (m_handle_box), GTK_POS_TOP);
		gtk_handle_box_set_snap_edge (GTK_HANDLE_BOX (m_handle_box), GTK_POS_RIGHT);

		Ypp::QueryBase *query = new Ypp::QueryProperty ("to-modify", true);
/*		if (pkg_selector->onlineUpdateMode())
			query->addType (Ypp::Package::PATCH_TYPE);*/
		m_pool = Ypp::PkgQuery (Ypp::Package::PACKAGE_TYPE, query);
		// initialize list -- there could already be packages modified
		for (int i = 0; i < m_pool.size(); i++)
			ChangesPane::entryInserted (m_pool, i, m_pool.get (i));
		m_pool.addListener (this);

		if (undo_log_all || undo_log_changed) {
			GtkWidget *log_box = gtk_vbox_new (FALSE, 2);
			GtkWidget *heading = gtk_label_new (_("Past changes:"));
			YGUtils::setWidgetFont (heading, PANGO_STYLE_NORMAL, PANGO_WEIGHT_BOLD, PANGO_SCALE_MEDIUM);
			gtk_misc_set_alignment (GTK_MISC (heading), 0, 0.5);
			gtk_box_pack_start (GTK_BOX (vbox), heading, FALSE, TRUE, 0);
			gtk_box_pack_start (GTK_BOX (vbox), log_box, TRUE, TRUE, 0);

			for (int i = 0; Ypp::get()->getLog()->getEntry (i); i++) {
				Ypp::Log::Entry *entry = Ypp::get()->getLog()->getEntry (i);
				if (entry->package)
					if (undo_log_all || LogEntry::canUndo (entry->package, entry->action)) {
						// FIXME: free these stuff
						LogEntry *log_entry = new LogEntry (entry->package, entry->action);
						gtk_box_pack_start (GTK_BOX (log_box), log_entry->getWidget(), FALSE, TRUE, 0);
					}
			}
		}
	}

	~ChangesPane()
	{
		for (GList *i = m_entries; i; i = i->next)
			delete (Entry *) i->data;
		g_list_free (m_entries);
	}

	void startHack()  // call after init, after you did a show_all in the dialog
	{
		UpdateVisible();

		// ugly: signal modified for all entries to allow them to hide undo buttons
		GList *i;
		int index;
		for (index = 0, i = m_entries; index < m_pool.size() && i;
		     index++, i = i->next)
			((Entry *) i->data)->modified (m_pool.get (index));
	}

	void UpdateVisible()
	{
		if (details_start_hide && !(undo_log_all || undo_log_changed))
			m_entries ? gtk_widget_show (m_handle_box) : gtk_widget_hide (m_handle_box);
		if (m_entries)
			gtk_widget_hide (m_empty_label);
	}

	virtual void entryInserted (const Ypp::PkgList list, int index, Ypp::Package *package)
	{
		Entry *entry = new Entry (package);
		gtk_box_pack_start (GTK_BOX (m_entries_box), entry->getWidget(), FALSE, TRUE, 0);
		m_entries = g_list_insert (m_entries, entry, index);
		UpdateVisible();
	}

	virtual void entryDeleted  (const Ypp::PkgList list, int index, Ypp::Package *package)
	{
		GList *i = g_list_nth (m_entries, index);
		Entry *entry = (Entry *) i->data;
		gtk_container_remove (GTK_CONTAINER (m_entries_box), entry->getWidget());
		delete entry;
		m_entries = g_list_delete_link (m_entries, i);
		UpdateVisible();
	}

	virtual void entryChanged  (const Ypp::PkgList list, int index, Ypp::Package *package)
	{
		Entry *entry = (Entry *) g_list_nth_data (m_entries, index);
		entry->modified (package);
	}
};

#if 0
struct QueryListener {
	virtual void queryNotify() = 0;
	virtual void queryNotifyDelay() = 0;
};

class QueryWidget
{
protected:
	QueryListener *m_listener;
	void notify() { m_listener->queryNotify(); }
	void notifyDelay() { m_listener->queryNotifyDelay(); }

public:
	void setListener (QueryListener *listener) { m_listener = listener; }
	virtual GtkWidget *getWidget() = 0;
	virtual void writeQuery (Ypp::PkgQuery::Query *query) = 0;

	virtual bool availablePackagesOnly() { return false; }
	virtual bool installedPackagesOnly() { return false; }
	virtual ~QueryWidget() {}
};

class StoreView : public QueryWidget
{
protected:
	GtkWidget *m_view, *m_scroll, *m_box;
	enum Column { TEXT_COL, ICON_COL, ENABLED_COL, PTR_COL, TOOLTIP_COL, TOTAL_COLS };

	virtual void doBuild (GtkTreeStore *store) = 0;
	virtual void writeQuerySel (Ypp::PkgQuery::Query *query,
	                           const std::list <gpointer> &ptr) = 0;

	StoreView()
	: QueryWidget()
	{
		m_scroll = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (m_scroll),
				                             GTK_SHADOW_IN);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (m_scroll),
				                        GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
		m_view = NULL;
		m_box = gtk_vbox_new (FALSE, 2);
		gtk_box_pack_start (GTK_BOX (m_box), m_scroll, TRUE, TRUE, 0);

		// parent constructor should call build()
	}

public:
	virtual GtkWidget *getWidget() { return m_box; }

	virtual void writeQuery (Ypp::PkgQuery::Query *query)
	{
		GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (m_view));
		GtkTreeModel *model;
		GList *selected = gtk_tree_selection_get_selected_rows (selection, &model);
		std::list <gpointer> ptrs;
		for (GList *i = selected; i; i = i->next) {
			GtkTreePath *path = (GtkTreePath *) i->data;
			GtkTreeIter iter;
			gtk_tree_model_get_iter (model, &iter, path);
			gpointer ptr;
			gtk_tree_model_get (model, &iter, PTR_COL, &ptr, -1);
			if (ptr)
				ptrs.push_back (ptr);
			gtk_tree_path_free (path);
		}
		g_list_free (selected);
		writeQuerySel (query, ptrs);
	}

protected:
	void build (bool tree_mode, bool with_icons, bool multi_selection,
	            bool do_tooltip)
	{
		if (m_view)
			gtk_container_remove (GTK_CONTAINER (m_scroll), m_view);

		m_view = ygtk_tree_view_new();
		GtkTreeView *view = GTK_TREE_VIEW (m_view);
		gtk_tree_view_set_headers_visible (view, FALSE);
		gtk_tree_view_set_search_column (view, TEXT_COL);
		if (do_tooltip)
			gtk_tree_view_set_tooltip_column (view, TEXT_COL);
		gtk_tree_view_set_show_expanders (view, tree_mode);
		gtk_tree_view_set_tooltip_column (view, TOOLTIP_COL);

		GtkTreeViewColumn *column;
		GtkCellRenderer *renderer;
		if (with_icons) {
			renderer = gtk_cell_renderer_pixbuf_new();
			column = gtk_tree_view_column_new_with_attributes ("",
				renderer, "icon-name", ICON_COL, "sensitive", ENABLED_COL, NULL);
			gtk_tree_view_append_column (view, column);
		}
		renderer = gtk_cell_renderer_text_new();
		g_object_set (G_OBJECT (renderer), "ellipsize", PANGO_ELLIPSIZE_MIDDLE, NULL);
		column = gtk_tree_view_column_new_with_attributes ("",
			renderer, "markup", TEXT_COL, "sensitive", ENABLED_COL, NULL);
		gtk_tree_view_append_column (view, column);

		GtkTreeStore *store = gtk_tree_store_new (TOTAL_COLS,
			G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_POINTER, G_TYPE_STRING);
		GtkTreeModel *model = GTK_TREE_MODEL (store);
		gtk_tree_view_set_model (view, model);
		g_object_unref (G_OBJECT (model));

		GtkTreeSelection *selection = gtk_tree_view_get_selection (view);
		gtk_tree_selection_set_mode (selection,
			multi_selection ? GTK_SELECTION_MULTIPLE : GTK_SELECTION_BROWSE);
		g_signal_connect (G_OBJECT (selection), "changed",
				          G_CALLBACK (selection_cb), this);
		gtk_tree_selection_set_select_function (selection, can_select_cb, this, NULL);

		block();
		GtkTreeIter iter;
		gtk_tree_store_append (store, &iter, NULL);
		gtk_tree_store_set (store, &iter, TEXT_COL, _("All"), ENABLED_COL, TRUE,
		                    TOOLTIP_COL, _("No filter"), -1);
		doBuild (store);

		selectFirstItem();
		unblock();

		gtk_container_add (GTK_CONTAINER (m_scroll), m_view);
		gtk_widget_show (m_view);
	}

private:
	void block()
	{
		GtkTreeView *view = GTK_TREE_VIEW (m_view);
		GtkTreeSelection *selection = gtk_tree_view_get_selection (view);
		g_signal_handlers_block_by_func (selection, (gpointer) selection_cb, this);
	}
	void unblock()
	{
		GtkTreeView *view = GTK_TREE_VIEW (m_view);
		GtkTreeSelection *selection = gtk_tree_view_get_selection (view);
		g_signal_handlers_unblock_by_func (selection, (gpointer) selection_cb, this);
	}

	void selectFirstItem()
	{
		/* we use gtk_tree_view_set_cursor(), rather than gtk_tree_selection_select_iter()
		   because that one is buggy in that when the user first interacts with the
		   treeview, a change signal is sent, even if he was just expanding one node... */
		block();
		GtkTreePath *path = gtk_tree_path_new_first();
		gtk_tree_view_set_cursor (GTK_TREE_VIEW (m_view), path, NULL, FALSE);
		gtk_tree_path_free (path);
		unblock();
	}

	static void selection_cb (GtkTreeSelection *selection, StoreView *pThis)
	{
		pThis->notify();
		// if item unselected, make sure "All" is
		if (gtk_tree_selection_count_selected_rows (selection) == 0)
			pThis->selectFirstItem();
	}

	static gboolean can_select_cb (GtkTreeSelection *selection, GtkTreeModel *model,
	                               GtkTreePath *path, gboolean currently_selected,
	                               gpointer pData)
	{
		gboolean ret;
		GtkTreeIter iter;
		gtk_tree_model_get_iter (model, &iter, path);
		gtk_tree_model_get (model, &iter, ENABLED_COL, &ret, -1);
		return ret;
	}
};

struct Categories : public StoreView
{
	bool m_rpmGroups, m_onlineUpdate;

public:
	Categories (bool onlineUpdate)
	: StoreView(), m_rpmGroups (false), m_onlineUpdate (onlineUpdate)
	{
		if (!onlineUpdate) {
			GtkWidget *check = gtk_check_button_new_with_label (_("Detailed"));
			YGUtils::setWidgetFont (GTK_BIN (check)->child,
				PANGO_STYLE_NORMAL, PANGO_WEIGHT_NORMAL, PANGO_SCALE_SMALL);
			gtk_widget_set_tooltip_text (check,
				_("Group by the PackageKit-based filter or straight from the actual "
				"RPM information."));
			g_signal_connect (G_OBJECT (check), "toggled",
				              G_CALLBACK (rpm_groups_toggled_cb), this);
			gtk_box_pack_start (GTK_BOX (m_box), check, FALSE, TRUE, 0);
		}
		build (m_rpmGroups, !m_rpmGroups, false, false);
	}

protected:
	virtual void doBuild (GtkTreeStore *store)
	{
		struct inner {
			static void populate (GtkTreeStore *store, GtkTreeIter *parent,
						          Ypp::Node *category, Categories *pThis)
			{
				if (!category)
					return;
				GtkTreeIter iter;
				gtk_tree_store_append (store, &iter, parent);
				gtk_tree_store_set (store, &iter, TEXT_COL, category->name.c_str(),
					ICON_COL, category->icon,  PTR_COL, category, ENABLED_COL, TRUE, -1);
				populate (store, &iter, category->child(), pThis);
				populate (store, parent, category->next(), pThis);
			}
		};

		Ypp::Node *first_category;
		Ypp::Package::Type type = m_onlineUpdate ?
			Ypp::Package::PATCH_TYPE : Ypp::Package::PACKAGE_TYPE;
		if (!m_rpmGroups && !m_onlineUpdate)
			first_category = Ypp::get()->getFirstCategory2 (type);
		else
			first_category = Ypp::get()->getFirstCategory (type);
		inner::populate (store, NULL, first_category, this);
		if (!m_rpmGroups && !m_onlineUpdate) {
			GtkTreeView *view = GTK_TREE_VIEW (m_view);
			GtkTreeIter iter;
			gtk_tree_store_append (store, &iter, NULL);
			gtk_tree_view_set_row_separator_func (view,
				is_tree_model_iter_separator_cb, NULL, NULL);

			gtk_tree_store_append (store, &iter, NULL);
			gtk_tree_store_set (store, &iter, TEXT_COL, _("Recommended"),
				ICON_COL, GTK_STOCK_ABOUT,  PTR_COL, GINT_TO_POINTER (1),
				ENABLED_COL, TRUE, TOOLTIP_COL, _("Recommended by an installed package"), -1);
			gtk_tree_store_append (store, &iter, NULL);
			gtk_tree_store_set (store, &iter, TEXT_COL, _("Suggested"),
				ICON_COL, GTK_STOCK_ABOUT,  PTR_COL, GINT_TO_POINTER (2),
				ENABLED_COL, TRUE, TOOLTIP_COL, _("Suggested by an installed package"), -1);
			gtk_tree_store_append (store, &iter, NULL);
			gtk_tree_store_set (store, &iter, TEXT_COL, _("Fresh"),
				ICON_COL, GTK_STOCK_NEW,  PTR_COL, GINT_TO_POINTER (3),
				ENABLED_COL, TRUE, TOOLTIP_COL, _("Uploaded in the last week"), -1);
		}
	}

	virtual void writeQuerySel (Ypp::PkgQuery::Query *query, const std::list <gpointer> &ptrs)
	{
		gpointer ptr = ptrs.empty() ? NULL : ptrs.front();
		int nptr = GPOINTER_TO_INT (ptr);
		if (nptr == 1)
			query->setIsRecommended (true);
		else if (nptr == 2)
			query->setIsSuggested (true);
		else if (nptr == 3)
			query->setBuildAge (7);
		else if (ptr) {
			Ypp::Node *node = (Ypp::Node *) ptr;
			if (m_rpmGroups || m_onlineUpdate)
				query->addCategory (node);
			else
				query->addCategory2 (node);
		}
	}

	static void rpm_groups_toggled_cb (GtkToggleButton *button, Categories *pThis)
	{
		pThis->m_rpmGroups = gtk_toggle_button_get_active (button);
		pThis->build (pThis->m_rpmGroups, !pThis->m_rpmGroups, false, false);
		pThis->notify();
	}

	static gboolean is_tree_model_iter_separator_cb (
		GtkTreeModel *model, GtkTreeIter *iter, gpointer data)
	{
		gint col = data ? GPOINTER_TO_INT (data) : 0;
		gpointer ptr;
		gtk_tree_model_get (model, iter, col, &ptr, -1);
		return ptr == NULL;
	}
};

struct Repositories : public StoreView
{
public:
	Repositories (bool repoMgrEnabled)
	: StoreView()
	{
		if (repoMgrEnabled) {
			GtkWidget *align, *button, *box, *image, *label;
			box = gtk_hbox_new (FALSE, 6);
			GtkSettings *settings = gtk_settings_get_default();
			gboolean button_images;
			g_object_get (settings, "gtk-button-images", &button_images, NULL);
			if (button_images) {
				image = gtk_image_new_from_stock (GTK_STOCK_EDIT, GTK_ICON_SIZE_MENU);
				gtk_box_pack_start (GTK_BOX (box), image, FALSE, TRUE, 0);
			}
			label = gtk_label_new (_("Edit..."));
			YGUtils::setWidgetFont (label, PANGO_STYLE_NORMAL, PANGO_WEIGHT_NORMAL, PANGO_SCALE_SMALL);
			gtk_box_pack_start (GTK_BOX (box), label, TRUE, TRUE, 0);
			button = gtk_button_new();
			gtk_container_add (GTK_CONTAINER (button), box);
			gtk_widget_set_tooltip_text (button, _("Access the repositories manager tool."));
			align = gtk_alignment_new (0, 0, 0, 1);
			gtk_container_add (GTK_CONTAINER (align), button);
			g_signal_connect (G_OBJECT (button), "clicked",
				              G_CALLBACK (setup_button_clicked_cb), this);
			gtk_box_pack_start (GTK_BOX (m_box), align, FALSE, TRUE, 0);
		}
		build (false, true, true, true);
	}

	virtual ~Repositories()
	{ Ypp::get()->setFavoriteRepository (NULL); }

protected:
	virtual void doBuild (GtkTreeStore *store)
	{
		GtkTreeIter iter;
		for (int i = 0; Ypp::get()->getRepository (i); i++) {
			const Ypp::Repository *repo = Ypp::get()->getRepository (i);
			gtk_tree_store_append (store, &iter, NULL);
			std::string text = repo->name, url (repo->url);
			url = YGUtils::escapeMarkup (url);
			text += "\n<small>" + url + "</small>";
			const gchar *icon;
			if (repo->url.empty())
				icon = GTK_STOCK_MISSING_IMAGE;
			else if (repo->url.compare (0, 2, "cd", 2) == 0 ||
			         repo->url.compare (0, 3, "dvd", 3) == 0)
				icon = GTK_STOCK_CDROM;
			else if (repo->url.compare (0, 3, "iso", 3) == 0)
				icon = GTK_STOCK_FILE;
			else
				icon = GTK_STOCK_NETWORK;
			gtk_tree_store_set (store, &iter, TEXT_COL, text.c_str(),
				ICON_COL, icon, ENABLED_COL, repo->enabled, PTR_COL, repo,
				TOOLTIP_COL, text.c_str(), -1);
		}
	}

	virtual void writeQuerySel (Ypp::PkgQuery::Query *query, const std::list <gpointer> &ptrs)
	{
		for (std::list <gpointer>::const_iterator it = ptrs.begin();
		     it != ptrs.end(); it++) {
			Ypp::Repository *repo = (Ypp::Repository *) *it;
			query->addRepository (repo);
		}
		gpointer ptr = ptrs.size() == 1 ? ptrs.front() : NULL;
		Ypp::get()->setFavoriteRepository ((Ypp::Repository *) ptr);
	}

	static void setup_button_clicked_cb (GtkButton *button, Repositories *pThis)
	{ YGUI::ui()->sendEvent (new YMenuEvent ("repo_mgr")); }
};

struct Collection : public QueryWidget, YGtkPackageView::Listener
{
private:
	YGtkPackageView *m_view;
	GtkWidget *m_buttons_box, *m_box;

public:
	virtual GtkWidget *getWidget() { return m_box; }

	Collection (Ypp::Package::Type type)
	: QueryWidget()
	{
		m_view = ygtk_package_view_new (TRUE);
		m_view->appendIconColumn (NULL, ICON_PROP);
		m_view->appendTextColumn (NULL, NAME_SUMMARY_PROP);
		if (type == Ypp::Package::LANGUAGE_TYPE)
			m_view->setList (Ypp::PkgQuery (type, NULL), NULL);
		else
			populateView (m_view, type);
		m_view->setListener (this);
		gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (m_view), GTK_SHADOW_IN);

		// control buttons
		m_buttons_box = gtk_alignment_new (0, 0, 0, 0);
		GtkWidget *image, *button;
		button = gtk_button_new_with_label (_("Install All"));
		image = gtk_image_new_from_stock (GTK_STOCK_SAVE, GTK_ICON_SIZE_BUTTON);
		gtk_button_set_image (GTK_BUTTON (button), image);
		g_signal_connect (G_OBJECT (button), "clicked",
		                  G_CALLBACK (install_cb), this);
		gtk_container_add (GTK_CONTAINER (m_buttons_box), button);
		gtk_widget_set_sensitive (m_buttons_box, FALSE);

		// layout
		m_box = gtk_vbox_new (FALSE, 6);
		gtk_box_pack_start (GTK_BOX (m_box), GTK_WIDGET (m_view), TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (m_box), m_buttons_box, FALSE, TRUE, 0);
	}

	static void populateView (YGtkPackageView *view, Ypp::Package::Type type)
	{  // list 1D categories intertwined with its constituent packages
		Ypp::Node *category = Ypp::get()->getFirstCategory (type);
		for (; category; category = category->next()) {
			Ypp::PkgQuery::Query *query = new Ypp::PkgQuery::Query();
			query->addCategory (category);
			view->appendList (category->name.c_str(), Ypp::PkgQuery (type, query), NULL);
		}
	}

	virtual void packagesSelected (Ypp::PkgList selection)
	{
		gtk_widget_set_sensitive (m_buttons_box, selection.notInstalled());
		notify();
	}

	virtual void writeQuery (Ypp::PkgQuery::Query *query)
	{
		Ypp::PkgList selected = m_view->getSelected();
		for (int i = 0; selected.get (i); i++)
			query->addCollection (selected.get (i));
		if (selected.size() == 0)
			query->setClear();
	}

	void doAll (bool install /*or remove*/)
	{  // we just need to mark the collections themselves
		Ypp::PkgList selected = m_view->getSelected();
		install ? selected.install() : selected.remove();
	}

	static void install_cb (GtkButton *button, Collection *pThis)
	{ pThis->doAll (true); }
	static void remove_cb (GtkButton *button, Collection *pThis)
	{ pThis->doAll (false); }
};

static const char *find_entry_tooltip =
	_("<b>Package search:</b> Use spaces to separate your keywords. They "
	"will be matched against RPM <i>name</i> and <i>summary</i> attributes. "
	"Other criteria attributes are available by pressing the search icon.\n"
	"(usage example: \"yast dhcp\" will return yast's dhcpd tool)");
static const char *find_entry_novelty_tooltip =
	_("Number of days since the package was built by the repository.");
static const char *find_entry_file_tooltip =
	_("Note: Only applicable to installed packages.");
static std::string findPatternTooltip (const std::string &name)
{
	Ypp::PkgQuery::Query *query = new Ypp::PkgQuery::Query();
	query->addNames (name, ' ', true, false, false, false, false, true);
	query->setIsInstalled (false);
	Ypp::PkgQuery pool (Ypp::Package::PATTERN_TYPE, query);
	if (pool.size() > 0) {
		std::string _name = pool.get (0)->name();
		std::string text = _("Patterns are available that can "
			"assist you in the installment of");
		text += " <i>" + _name + "</i>.";
		return text;
	}
	return "";
}

class FindPane : public QueryWidget
{
GtkWidget *m_box, *m_name, *m_radio[5], *m_info_box, *m_info_label;
bool m_onlineUpdate;

public:
	virtual GtkWidget *getWidget() { return m_box; }

	FindPane (bool onlineUpdate)
	: m_onlineUpdate (onlineUpdate)
	{
		GtkWidget *name_box = gtk_hbox_new (FALSE, 6), *button;
		m_name = ygtk_find_entry_new();
		gtk_widget_set_tooltip_markup (m_name, find_entry_tooltip);
		g_signal_connect (G_OBJECT (m_name), "changed",
		                  G_CALLBACK (name_changed_cb), this);
		button = gtk_button_new_with_label ("");
		GtkWidget *image = gtk_image_new_from_stock (GTK_STOCK_OK, GTK_ICON_SIZE_MENU);
		gtk_button_set_image (GTK_BUTTON (button), image);
		gtk_widget_set_tooltip_text (button, _("Search!"));
		gtk_box_pack_start (GTK_BOX (name_box), m_name, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (name_box), button, FALSE, TRUE, 0);

		for (int i = 0; i < 5; i++) m_radio[i] = 0;
		GtkWidget *radio_box = gtk_vbox_new (FALSE, 0);
		m_radio[0] = gtk_radio_button_new_with_label_from_widget (NULL, _("Name & summary"));
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (m_radio[0]), TRUE);
		GtkRadioButton *radiob = GTK_RADIO_BUTTON (m_radio[0]);
		m_radio[1] = gtk_radio_button_new_with_label_from_widget (radiob, _("Description"));
		if (!onlineUpdate) {
			m_radio[2] = gtk_radio_button_new_with_label_from_widget (radiob, _("File name"));
			gtk_widget_set_tooltip_text (m_radio[2], find_entry_file_tooltip);
			m_radio[3] = gtk_radio_button_new_with_label_from_widget (radiob, _("Author"));
			if (show_novelty_filter) {
				m_radio[4] = gtk_radio_button_new_with_label_from_widget (radiob, _("Novelty (in days)"));
				g_signal_connect (G_OBJECT (m_radio[4]), "toggled", G_CALLBACK (novelty_toggled_cb), this);
				gtk_widget_set_tooltip_markup (m_radio[4], find_entry_novelty_tooltip);
			}
		}
		for (int i = 0; i < 5; i++)
			if (m_radio [i])
				gtk_box_pack_start (GTK_BOX (radio_box), m_radio[i], FALSE, TRUE, 0);

		GtkWidget *radio_frame = gtk_frame_new (_("Search in:"));
		YGUtils::setWidgetFont (gtk_frame_get_label_widget (GTK_FRAME (radio_frame)),
			PANGO_STYLE_NORMAL, PANGO_WEIGHT_BOLD, PANGO_SCALE_MEDIUM);
		gtk_frame_set_shadow_type (GTK_FRAME (radio_frame), GTK_SHADOW_NONE);
		GtkWidget *align = gtk_alignment_new (0, 0, 1, 1);
		gtk_alignment_set_padding (GTK_ALIGNMENT (align), 0, 0, 15, 0);
		gtk_container_add (GTK_CONTAINER (align), radio_box);
		gtk_container_add (GTK_CONTAINER (radio_frame), align);

		m_info_box = gtk_hbox_new (FALSE, 6);
		m_info_label = gtk_label_new ("");
		gtk_label_set_line_wrap (GTK_LABEL (m_info_label), TRUE);
		gtk_box_pack_start (GTK_BOX (m_info_box),
			gtk_image_new_from_stock (GTK_STOCK_INFO, GTK_ICON_SIZE_MENU), FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (m_info_box), m_info_label, TRUE, TRUE, 0);

		GtkWidget *separator1 = gtk_event_box_new();
		gtk_widget_set_size_request (separator1, -1, 12);
		GtkWidget *separator2 = gtk_event_box_new();
		gtk_widget_set_size_request (separator2, -1, 15);
		GtkWidget *separator3 = gtk_event_box_new();
		gtk_widget_set_size_request (separator3, -1, 50);

		GtkWidget *box = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (box), separator1, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (box), name_box, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (box), separator2, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (box), radio_frame, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (box), separator3, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (box), m_info_box, FALSE, TRUE, 0);
		m_box = box;

		g_signal_connect (G_OBJECT (m_name), "activate", G_CALLBACK (button_clicked_cb), this);
		g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (button_clicked_cb), this);
	}

	virtual void writeQuery (Ypp::PkgQuery::Query *query)
	{
		gtk_widget_hide (m_info_box);
		const char *name = gtk_entry_get_text (GTK_ENTRY (m_name));
		if (*name) {
			int item;
			for (item = 0; item < 5; item++)
				if (m_radio[item])
				    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (m_radio[item])))
						break;
			if (item >= 5) ;
			else if (item == 4) {  // novelty
				int days = atoi (name);
				query->setBuildAge (days);
			}
			else {
				bool use_name, use_summary, use_description, use_filelist, use_authors;
				use_name = use_summary = use_description = use_filelist = use_authors = false;
				switch (item) {
					case 0:  // name & summary
					default:
						use_name = use_summary = true;
						break;
					case 1:  // description
						use_name = use_summary = use_description = true;
						break;
					case 2:  // file
						use_filelist = true;
						break;
					case 3:  // author
						use_authors = true;
						break;
				}
				query->addNames (name, ' ', use_name, use_summary, use_description,
					             use_filelist, use_authors);
			}
			if (item == 0 && !m_onlineUpdate) {  // tip: user may be unaware of the patterns
				std::string text = findPatternTooltip (name);
				if (!text.empty()) {
					gtk_label_set_markup (GTK_LABEL (m_info_label), text.c_str());
					// wrapping must be performed manually in GTK
					int width;
					gtk_icon_size_lookup (GTK_ICON_SIZE_MENU, &width, NULL);
					width = getWidget()->allocation.width - width - 6;
					gtk_widget_set_size_request (m_info_label, width, -1);
					gtk_widget_show_all (m_info_box);
				}
			}
		}
	}

	virtual bool installedPackagesOnly()
	{ return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (m_radio[2])); }

	const char *searchName()
	{
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (m_radio[0]))) {
			const char *name = gtk_entry_get_text (GTK_ENTRY (m_name));
			return *name ? name : NULL;
		}
		return NULL;
	}

private:
	static void novelty_toggled_cb (GtkToggleButton *button, FindPane *pThis)
	{
		// novelty is weird; so show usage case
		const gchar *text = "";
		if (gtk_toggle_button_get_active (button))
			text = "7";
		g_signal_handlers_block_by_func (pThis->m_name, (gpointer) name_changed_cb, pThis);
		gtk_entry_set_text (GTK_ENTRY (pThis->m_name), text);
		g_signal_handlers_unblock_by_func (pThis->m_name, (gpointer) name_changed_cb, pThis);
		gtk_editable_set_position (GTK_EDITABLE (pThis->m_name), -1);
		ygtk_find_entry_set_state (YGTK_FIND_ENTRY (pThis->m_name), TRUE);
	}

	static void name_changed_cb (YGtkFindEntry *entry, FindPane *pThis)
	{
		const gchar *text = gtk_entry_get_text (GTK_ENTRY (entry));
		// novelty allows only numbers
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (pThis->m_radio[4]))) {
			gboolean correct = TRUE;
			for (const gchar *i = text; *i; i++)
				if (!g_ascii_isdigit (*i)) {
					correct = FALSE;
					break;
				}
			ygtk_find_entry_set_state (entry, correct);
		}
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (pThis->m_radio[0])))
			pThis->notifyDelay();
		else if (*text == '\0')
			pThis->notify();
	}

	static void button_clicked_cb (GtkWidget *widget, FindPane *pThis)
	{ pThis->notify(); }
};

class FindEntry : public QueryWidget
{
GtkWidget *m_name;
bool m_onlineUpdate;
GtkWidget *m_combo;

public:
	GtkWidget *getWidget() { return m_name; }

	FindEntry (bool onlineUpdate, GtkWidget *combo) : QueryWidget()
	, m_onlineUpdate (onlineUpdate), m_combo (combo)
	{
		m_name = ygtk_find_entry_new();
		gtk_widget_set_size_request (m_name, 140, -1);
		gtk_widget_set_tooltip_markup (m_name, find_entry_tooltip);
		ygtk_find_entry_insert_item (YGTK_FIND_ENTRY (m_name),
			_("Filter by name & summary"), GTK_STOCK_FIND, NULL);
		ygtk_find_entry_insert_item (YGTK_FIND_ENTRY (m_name),
			_("Filter by description"), GTK_STOCK_EDIT, NULL);
		if (!onlineUpdate) {
			ygtk_find_entry_insert_item (YGTK_FIND_ENTRY (m_name),
				_("Filter by file"), GTK_STOCK_OPEN, find_entry_file_tooltip);
			ygtk_find_entry_insert_item (YGTK_FIND_ENTRY (m_name),
				_("Filter by author"), GTK_STOCK_ABOUT, NULL);
			if (show_novelty_filter)
				ygtk_find_entry_insert_item (YGTK_FIND_ENTRY (m_name),
					_("Filter by novelty (in days)"), GTK_STOCK_NEW, find_entry_novelty_tooltip);
		}
		g_signal_connect (G_OBJECT (m_name), "changed",
		                  G_CALLBACK (name_changed_cb), this);
		g_signal_connect (G_OBJECT (m_name), "menu-item-selected",
		                  G_CALLBACK (name_item_changed_cb), this);
		g_signal_connect (G_OBJECT (m_name), "realize",
		                  G_CALLBACK (realize_cb), this);
		gtk_widget_show (m_name);
	}

	void clear()
	{
		g_signal_handlers_block_by_func (m_name, (gpointer) name_changed_cb, this);
		g_signal_handlers_block_by_func (m_name, (gpointer) name_item_changed_cb, this);
		gtk_entry_set_text (GTK_ENTRY (m_name), "");
		ygtk_find_entry_select_item (YGTK_FIND_ENTRY (m_name), 0);
		g_signal_handlers_unblock_by_func (m_name, (gpointer) name_changed_cb, this);
		g_signal_handlers_unblock_by_func (m_name, (gpointer) name_item_changed_cb, this);
	}

	const char *searchName()
	{
		YGtkFindEntry *entry = YGTK_FIND_ENTRY (m_name);
		if (ygtk_find_entry_get_selected_item (entry) == 0) {
			const char *name = gtk_entry_get_text (GTK_ENTRY (m_name));
			return *name ? name : NULL;
		}
		return NULL;
	}

	virtual void writeQuery (Ypp::PkgQuery::Query *query)
	{
		const char *name = gtk_entry_get_text (GTK_ENTRY (m_name));
		if (*name) {
			int item = ygtk_find_entry_get_selected_item (YGTK_FIND_ENTRY (m_name));
			if (item >= 5) ;
			else if (item == 4) {  // novelty
				int days = atoi (name);
				query->setBuildAge (days);
			}
			else {
				bool use_name, use_summary, use_description, use_filelist, use_authors;
				use_name = use_summary = use_description = use_filelist = use_authors = false;
				switch (item) {
					case 0:  // name & summary
					default:
						use_name = use_summary = true;
						break;
					case 1:  // description
						use_name = use_summary = use_description = true;
						break;
					case 2:  // file
						use_filelist = true;
						break;
					case 3:  // author
						use_authors = true;
						break;
				}
				query->addNames (name, ' ', use_name, use_summary, use_description,
					             use_filelist, use_authors);
			}

			if (item == 0 && !m_onlineUpdate) {  // tip: user may be unaware of the patterns
				std::string text = findPatternTooltip (name);
				if (!text.empty())
					ygtk_tooltip_show_at_widget (m_combo, YGTK_POINTER_UP_LEFT,
						text.c_str(), GTK_STOCK_DIALOG_INFO);
			}
		}
	}

	virtual bool installedPackagesOnly()
	{ return ygtk_find_entry_get_selected_item (YGTK_FIND_ENTRY (m_name)) == 2; }

private:
	static void name_changed_cb (YGtkFindEntry *entry, FindEntry *pThis)
	{
		gint nb = ygtk_find_entry_get_selected_item (entry);
		if (nb == 4) {  // novelty only allows numbers
			const gchar *text = gtk_entry_get_text (GTK_ENTRY (entry));
			gboolean correct = TRUE;
			for (const gchar *i = text; *i; i++)
				if (!g_ascii_isdigit (*i)) {
					correct = FALSE;
					break;
				}
			ygtk_find_entry_set_state (entry, correct);
		}
		pThis->notifyDelay();
	}

	static void name_item_changed_cb (YGtkFindEntry *entry, gint nb, FindEntry *pThis)
	{
		const gchar *text = "";
		if (nb == 4) text = "7";  // novelty is weird; show usage case
		g_signal_handlers_block_by_func (entry, (gpointer) name_changed_cb, pThis);
		gtk_entry_set_text (GTK_ENTRY (entry), text);
		g_signal_handlers_unblock_by_func (entry, (gpointer) name_changed_cb, pThis);
		gtk_editable_set_position (GTK_EDITABLE (entry), -1);
		ygtk_find_entry_set_state (entry, TRUE);
		pThis->notify();
	}

	static void realize_cb (GtkWidget *widget, FindEntry *pThis)
	{ gtk_widget_grab_focus (widget); }
};

class FilterCombo : public QueryWidget
{
GtkWidget *m_box, *m_combo, *m_bin;
bool m_onlineUpdate, m_repoMgrEnabled;
QueryWidget *m_queryWidget;

public:
	virtual GtkWidget *getWidget() { return m_box; }
	GtkWidget *getComboBox() { return m_combo; }

	FilterCombo (bool onlineUpdate, bool repoMgrEnabled)
	: QueryWidget(), m_onlineUpdate (onlineUpdate), m_repoMgrEnabled (repoMgrEnabled),
	  m_queryWidget (NULL)
	{
		m_combo = gtk_combo_box_new_text();
		if (onlineUpdate)
			gtk_combo_box_append_text (GTK_COMBO_BOX (m_combo), _("Severity"));
		else {
			if (show_find_pane)
				gtk_combo_box_append_text (GTK_COMBO_BOX (m_combo), _("Search"));
			gtk_combo_box_append_text (GTK_COMBO_BOX (m_combo), _("Groups"));
			gtk_combo_box_append_text (GTK_COMBO_BOX (m_combo), _("Patterns"));
			gtk_combo_box_append_text (GTK_COMBO_BOX (m_combo), _("Languages"));
			gtk_widget_set_tooltip_markup (m_combo,
				_("Packages can be organized in:\n"
/*
				"<b>Search:</b> find a given package by name, summary or some "
				"other attribute.\n"
*/
				"<b>Groups:</b> simple categorization of packages by purpose.\n"
				"<b>Patterns:</b> assists in installing all packages necessary "
				"for several working environments.\n"
				"<b>Languages:</b> adds another language to the system.\n"
				"<b>Repositories:</b> catalogues what the several configured "
				"repositories have available."));
		}
		gtk_combo_box_append_text (GTK_COMBO_BOX (m_combo), _("Repositories"));
		gtk_combo_box_set_active (GTK_COMBO_BOX (m_combo), 0);
		g_signal_connect (G_OBJECT (m_combo), "changed",
		                  G_CALLBACK (combo_changed_cb), this);

		m_bin = gtk_event_box_new();
		m_box = gtk_vbox_new (FALSE, 6);
		gtk_box_pack_start (GTK_BOX (m_box), m_combo, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (m_box), m_bin, TRUE, TRUE, 0);
		select (0);
	};

	~FilterCombo()
	{ delete m_queryWidget; }

	virtual void writeQuery (Ypp::PkgQuery::Query *query)
	{ m_queryWidget->writeQuery (query); }

	virtual bool availablePackagesOnly()
	{ return m_queryWidget->availablePackagesOnly(); }
	virtual bool installedPackagesOnly()
	{ return m_queryWidget->installedPackagesOnly(); }

	const char *searchName()
	{
		if (gtk_combo_box_get_active (GTK_COMBO_BOX (m_combo)) == 0)
			return ((FindPane *) m_queryWidget)->searchName();
		return NULL;
	}

	void select (int nb)
	{
		delete m_queryWidget;
		if (m_onlineUpdate) {
			switch (nb) {
				case 0: m_queryWidget = new Categories (m_onlineUpdate); break;
				case 1: m_queryWidget = new Repositories (m_repoMgrEnabled); break;
				default: break;
			}
		}
		else {
			if (!show_find_pane) nb++;
			switch (nb) {
				case 0: m_queryWidget = new FindPane (m_onlineUpdate); break;
				case 1: m_queryWidget = new Categories (m_onlineUpdate); break;
				case 2: m_queryWidget = new Collection (Ypp::Package::PATTERN_TYPE); break;
				case 3: m_queryWidget = new Collection (Ypp::Package::LANGUAGE_TYPE); break;
				case 4: m_queryWidget = new Repositories (m_repoMgrEnabled); break;
				default: break;
			}
		}
		m_queryWidget->setListener (m_listener);
		setChild (m_queryWidget->getWidget());
	}

private:
	void setChild (GtkWidget *widget)
	{
		if (GTK_BIN (m_bin)->child)
			gtk_container_remove (GTK_CONTAINER (m_bin), GTK_BIN (m_bin)->child);
		gtk_container_add (GTK_CONTAINER (m_bin), widget);
		gtk_widget_show_all (m_bin);
	}

	static void combo_changed_cb (GtkComboBox *combo, FilterCombo *pThis)
	{
		pThis->select (gtk_combo_box_get_active (combo));
		pThis->notify();
	}
};

#include "ygtknotebook.h"

class QueryNotebook : public QueryListener, YGtkPackageView::Listener
{
GtkWidget *m_widget, *m_notebook;
bool m_onlineUpdate;
FilterCombo *m_combo;
YGtkDetailView *m_details;
GtkWidget *m_oldPage;
guint m_timeout_id;
bool m_disabledTab, m_highlightTab;
UndoView *m_undoView;
Ypp::PkgList m_packages, m_pool;
FindEntry *m_find;

public:
	GtkWidget *getWidget() { return m_widget; }

	QueryNotebook (bool onlineUpdate, bool repoMgrEnabled)
	: m_onlineUpdate (onlineUpdate), m_timeout_id (0), m_disabledTab (true), m_highlightTab (false),
	  m_undoView (NULL)
	{
		m_notebook = ygtk_notebook_new();
		gtk_widget_show (m_notebook);
		appendPage (0, _("_Install"), GTK_STOCK_ADD);
		if (!onlineUpdate)
			appendPage (1, _("_Upgrade"), GTK_STOCK_GO_UP);
		if (!onlineUpdate)
			appendPage (2, _("_Remove"), GTK_STOCK_REMOVE);
		else
			appendPage (2, _("Installed"), GTK_STOCK_HARDDISK);
		if (!onlineUpdate)
			appendPage (3, _("_Undo"), GTK_STOCK_UNDO);

		m_details = YGTK_DETAIL_VIEW (ygtk_detail_view_new (onlineUpdate));
		GtkWidget *pkg_details_pane = gtk_vpaned_new();
		gtk_paned_pack1 (GTK_PANED (pkg_details_pane), m_notebook, TRUE, FALSE);
		gtk_paned_pack2 (GTK_PANED (pkg_details_pane), GTK_WIDGET (m_details), FALSE, TRUE);
		gtk_paned_set_position (GTK_PANED (pkg_details_pane), 600);

		m_combo = new FilterCombo (onlineUpdate, repoMgrEnabled);
		m_combo->setListener (this);
		m_combo->select (0);
		GtkWidget *hpane = gtk_hpaned_new();
		gtk_paned_pack1 (GTK_PANED (hpane), m_combo->getWidget(), FALSE, TRUE);
		gtk_paned_pack2 (GTK_PANED (hpane), pkg_details_pane, TRUE, FALSE);
		gtk_paned_set_position (GTK_PANED (hpane), 170);

		m_widget = hpane;
		gtk_widget_show_all (m_widget);
		gtk_widget_hide (GTK_WIDGET (m_details));

		m_find = NULL;
		if (!show_find_pane) {
			m_find = new FindEntry (onlineUpdate, m_combo->getComboBox());
			ygtk_notebook_set_corner_widget (YGTK_NOTEBOOK (m_notebook), m_find->getWidget());
			m_find->setListener (this);
		}

		Ypp::Package::Type type = Ypp::Package::PACKAGE_TYPE;
		if (m_onlineUpdate)
			type = Ypp::Package::PATCH_TYPE;
		m_packages = Ypp::get()->getPackages (type);
		queryNotify();
	}

	~QueryNotebook()
	{
		if (m_timeout_id)
			g_source_remove (m_timeout_id);
		delete m_undoView;
		delete m_combo;
		delete m_find;
	}

	bool confirmChanges()
	{
		if (m_onlineUpdate) return true;
		if (gtk_notebook_get_current_page (GTK_NOTEBOOK (m_notebook)) == 3) return true;

		GtkWidget *dialog = gtk_message_dialog_new (YGDialog::currentWindow(),
			GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE,
			"%s", _("Apply changes?"));
		gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
			_("Please review the changes to perfom."));
		gtk_dialog_add_buttons (GTK_DIALOG (dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_NO,
			GTK_STOCK_APPLY, GTK_RESPONSE_YES, NULL);
		gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_YES);
		gtk_window_set_default_size (GTK_WINDOW (dialog), 550, 500);
		gtk_window_set_resizable (GTK_WINDOW (dialog), TRUE);

		GtkWidget *view = m_undoView->createView (NULL);
		gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), view);
		gtk_widget_show_all (dialog);

		gint response = gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);
		return response == GTK_RESPONSE_YES;
	}

private:
	const char *getTooltip (int page)
	{
		if (m_onlineUpdate && page > 0)
			page++;
		switch (page) {
			case 0: return _("Available for install");
			case 1: return _("Upgrades");
			case 2: return m_onlineUpdate ? _("Installed patches") : _("Installed packages");
			case 3: return _("Undo history");
			default: break;
		}
		return NULL;
	}

	// callbacks
	virtual void queryNotify()
	{
		if (m_disabledTab) {  // limit users to the tabs whose query is applicable
			m_disabledTab = false;
			for (int i = 0; i < 4; i++)
				enablePage (i, true);
		}
		if (availablePackagesOnly()) {
			enablePage (m_onlineUpdate ? 1 : 2, false, 0);
			m_disabledTab = true;
		}
		else if (installedPackagesOnly()) {
			int new_page = m_onlineUpdate ? 1 : 2;
			enablePage (0, false, new_page);
			m_disabledTab = true;
		}

		GtkNotebook *notebook = GTK_NOTEBOOK (m_notebook);
		for (int i = 0; i < 3; i++) {
			GtkWidget *page = gtk_notebook_get_nth_page (notebook, i);
			if (page) YGTK_PACKAGE_VIEW (page)->clear();
		}

		Ypp::PkgQuery::Query *query = new Ypp::PkgQuery::Query();
		if (!show_find_pane)
			m_find->writeQuery (query);
		m_combo->writeQuery (query);
		m_pool = Ypp::PkgQuery (m_packages, query);

		for (int i = 0; i < 3; i++) {
			GtkWidget *page = gtk_notebook_get_nth_page (notebook, i);
			if (page) {
				YGtkPackageView *view = YGTK_PACKAGE_VIEW (page);
				Ypp::PkgQuery::Query *query = new Ypp::PkgQuery::Query();
				int n = i;
				if (m_onlineUpdate && n > 0) n++;
				switch (n) {
					case 0:  // available
						if (m_onlineUpdate)
							// special pane for patches upgrades makes little sense, so
							// we instead list them together with availables
							query->setHasUpgrade (true);
						query->setIsInstalled (false);
						break;
					case 1:  // upgrades
						query->setHasUpgrade (true);
						break;
					case 2:  // installed
						query->setIsInstalled (true);
						break;
					default: break;
				}
				Ypp::PkgQuery list (m_pool, query);
				if (!m_onlineUpdate) {
					const char *applyAll = NULL;
					if (!m_onlineUpdate && n == 1)
						applyAll = _("Upgrade All");
					view->setList (list, applyAll);
				}
				else {
					for (int i = 0; i < 6; i++) {
						Ypp::PkgQuery::Query *query = new Ypp::PkgQuery::Query();
						query->setSeverity (i);
						Ypp::PkgQuery severity_list (list, query);
						std::string str (Ypp::Package::severityStr (i));
						view->appendList (str.c_str(), severity_list, NULL);
					}
				}
			}
		}

		if (!m_onlineUpdate) {
			// set tab label bold if there's a package there with the name
			if (m_highlightTab) {
				for (int i = 0; i < 3; i++)
					highlightPage (i, false);
				m_highlightTab = false;
			}
			const char *name;
			if (show_find_pane)
				name = m_combo->searchName();
			else
				name = m_find->searchName();
			if (name) {
				Ypp::PkgQuery::Query *query = new Ypp::PkgQuery::Query();
				query->addNames (name, 0, true, false, false, false, false, true, true);
				Ypp::PkgQuery list (Ypp::Package::PACKAGE_TYPE, query);
				if (list.size()) {
					m_highlightTab = true;
					Ypp::Package *pkg = list.get (0);
					if (!pkg->isInstalled())
						highlightPage (0, true);
					else {
						highlightPage (2, true);
						if (pkg->hasUpgrade())
							highlightPage (1, true);
					}
				}
			}
		}
	}

	virtual void queryNotifyDelay()
	{
		struct inner {
			static gboolean timeout_cb (gpointer data)
			{
				QueryNotebook *pThis = (QueryNotebook *) data;
				pThis->m_timeout_id = 0;
				pThis->queryNotify();
				return FALSE;
			}
		};
		if (m_timeout_id) g_source_remove (m_timeout_id);
		m_timeout_id = g_timeout_add (500, inner::timeout_cb, this);
	}

	virtual void packagesSelected (Ypp::PkgList packages)
	{
		m_details->setPackages (packages);
		if (packages.size() > 0)
			gtk_widget_show (GTK_WIDGET (m_details));
	}

	inline bool availablePackagesOnly()
	{ return m_combo->availablePackagesOnly(); }

	inline bool installedPackagesOnly()
	{
		if (m_find && m_find->installedPackagesOnly())
			return true;
		return m_combo->installedPackagesOnly();
	}

	// utilities
	void appendPage (int nb, const char *text, const char *stock)
	{
		GtkWidget *hbox = gtk_hbox_new (FALSE, 6), *label, *icon;
		if (stock) {
			icon = gtk_image_new_from_stock (stock, GTK_ICON_SIZE_MENU);
			gtk_box_pack_start (GTK_BOX (hbox), icon, FALSE, TRUE, 0);
		}
		label = gtk_label_new (text);
		gtk_label_set_use_underline (GTK_LABEL (label), TRUE);
		gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);
		g_object_set_data (G_OBJECT (hbox), "label", label);
		gtk_widget_show_all (hbox);

		GtkWidget *page;
		if (nb < 3) {
			int col;
			switch (nb) {
				default:
				case 0: col = TO_INSTALL_PROP; break;
				case 1: col = TO_UPGRADE_PROP; break;
				case 2: col = TO_REMOVE_PROP; break;
			}
			YGtkPackageView *view = ygtk_package_view_new (FALSE);
			if (use_buttons) {
				//view->appendIconColumn (NULL, ICON_PROP);
				view->appendTextColumn (NULL, NAME_SUMMARY_PROP, 350);
				view->appendButtonColumn (NULL, col);
				view->setRulesHint (true);
			}
			else {
				if (m_onlineUpdate && nb > 0)
					view->appendEmptyColumn (25);
				else
					view->appendCheckColumn (col);
				view->appendTextColumn (_("Name"), NAME_PROP, 160);
				if (col == TO_UPGRADE_PROP) {
					view->appendTextColumn (_("Installed"), INSTALLED_VERSION_PROP, -1);
					view->appendTextColumn (_("Available"), AVAILABLE_VERSION_PROP, -1);
				}
				else
					view->appendTextColumn (_("Summary"), SUMMARY_PROP);
			}
			view->setListener (this);
			page = GTK_WIDGET (view);
		}
		else {
			m_undoView = new UndoView (this);
			page = m_undoView->getWidget();
		}
		gtk_notebook_append_page (GTK_NOTEBOOK (m_notebook), page, hbox);
	}

	void enablePage (int page_nb, bool enabled, int new_page_nb = -1)
	{
		GtkNotebook *notebook = GTK_NOTEBOOK (m_notebook);
		GtkWidget *page = gtk_notebook_get_nth_page (notebook, page_nb);
		if (page) {
			GtkWidget *label = gtk_notebook_get_tab_label (notebook, page);
			gtk_widget_set_sensitive (label, enabled);
			const char *tooltip = getTooltip (page_nb);
			if (!enabled)
				tooltip = _("Query only applicable to available packages.");
			gtk_widget_set_tooltip_text (label, tooltip);
			if (!enabled && new_page_nb >= 0) {
				GtkWidget *selected = gtk_notebook_get_nth_page (notebook,
					gtk_notebook_get_current_page (notebook));
				if (selected == page)
					gtk_notebook_set_current_page (notebook, new_page_nb);
			}
		}
	}

	void highlightPage (int page_nb, bool highlight)
	{
		GtkNotebook *notebook = GTK_NOTEBOOK (m_notebook);
		GtkWidget *label = gtk_notebook_get_tab_label (notebook,
			gtk_notebook_get_nth_page (notebook, page_nb));
		label = (GtkWidget *) g_object_get_data (G_OBJECT (label), "label");
		YGUtils::setWidgetFont (label, PANGO_STYLE_NORMAL,
			highlight ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL, PANGO_SCALE_MEDIUM);
	}
};
#endif

static void errorMsg (const std::string &header, const std::string &message)
{
	GtkWidget *dialog = gtk_message_dialog_new (YGDialog::currentWindow(),
		GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
		"%s", header.c_str());
	gtk_message_dialog_format_secondary_markup (GTK_MESSAGE_DIALOG (dialog),
		"%s", message.c_str());
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

static void import_file_cb (GtkMenuItem *item)
{
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new (_("Import Package List"),
		YGDialog::currentWindow(), GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

	int ret = gtk_dialog_run (GTK_DIALOG (dialog));
	if (ret == GTK_RESPONSE_ACCEPT) {
		char *filename;
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		if (!Ypp::get()->importList (filename)) {
			std::string error = _("Couldn't load package list from: ");
			error += filename;
			errorMsg (_("Import Failed"), error);
		}
		g_free (filename);
	}
	gtk_widget_destroy (dialog);
}

static void export_file_cb (GtkMenuItem *item)
{
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new (_("Export Package List"),
		YGDialog::currentWindow(), GTK_FILE_CHOOSER_ACTION_SAVE,
		GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
		GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);

	int ret = gtk_dialog_run (GTK_DIALOG (dialog));
	if (ret == GTK_RESPONSE_ACCEPT) {
		char *filename;
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		if (!Ypp::get()->exportList (filename)) {
			std::string error = _("Couldn't save package list to: ");
			error += filename;
			errorMsg (_("Export Failed"), error);
		}
		g_free (filename);
	}
	gtk_widget_destroy (dialog);
}

static void create_solver_testcase_cb (GtkMenuItem *item)
{
	const char *dirname = "/var/log/YaST2/solverTestcase";
	std::string msg = _("Use this to generate extensive logs to help tracking down "
	                  "bugs in the dependency resolver.\nThe logs will be stored in "
	                  "directory: ");
	msg += dirname;

	GtkWidget *dialog = gtk_message_dialog_new (YGDialog::currentWindow(),
		GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK_CANCEL,
		"%s", _("Create Dependency Resolver Test Case"));
	gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s", msg.c_str());
	int ret = gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);

	if (ret == GTK_RESPONSE_OK) {
	    if (Ypp::get()->createSolverTestcase (dirname)) {
			GtkWidget *dialog = gtk_message_dialog_new (YGDialog::currentWindow(),
				GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION,
				GTK_BUTTONS_YES_NO, "%s", _("Success"));
			msg = _("Dependency resolver test case written to");
			msg += " <tt>";
			msg += dirname;
			msg += "</tt>\n";
			msg += _("Prepare <tt>y2logs.tgz tar</tt> archive to attach to Bugzilla?");
			gtk_message_dialog_format_secondary_markup (GTK_MESSAGE_DIALOG (dialog),
                                                                                "%s", msg.c_str());
			ret = gtk_dialog_run (GTK_DIALOG (dialog));
			gtk_widget_destroy (dialog);
			if (ret == GTK_RESPONSE_YES)
				YGUI::ui()->askSaveLogs();
	    }
	    else {
	    	msg = _("Failed to create dependency resolver test case.\n"
				"Please check disk space and permissions for");
			msg += " <tt>";
			msg += dirname;
			msg += "</tt>";
			errorMsg ("Error", msg.c_str());
	    }
	}
}

class ToolsBox
{
GtkWidget *m_box;

public:
	GtkWidget *getWidget() { return m_box; }

	ToolsBox()
	{
		GtkWidget *button, *popup, *item;
		button = ygtk_menu_button_new_with_label (_("Tools"));
		popup = gtk_menu_new();

		item = gtk_menu_item_new_with_label (_("Import List..."));
		gtk_menu_shell_append (GTK_MENU_SHELL (popup), item);
		g_signal_connect (G_OBJECT (item), "activate",
		                  G_CALLBACK (import_file_cb), this);
		item = gtk_menu_item_new_with_label (_("Export List..."));
		gtk_menu_shell_append (GTK_MENU_SHELL (popup), item);
		g_signal_connect (G_OBJECT (item), "activate",
		                  G_CALLBACK (export_file_cb), this);
		gtk_menu_shell_append (GTK_MENU_SHELL (popup), gtk_separator_menu_item_new());
		item = gtk_menu_item_new_with_label (_("Generate Dependency Testcase..."));
		gtk_menu_shell_append (GTK_MENU_SHELL (popup), item);
		g_signal_connect (G_OBJECT (item), "activate",
		                  G_CALLBACK (create_solver_testcase_cb), this);

		ygtk_menu_button_set_popup (YGTK_MENU_BUTTON (button), popup);
		gtk_widget_show_all (popup);

		m_box = gtk_hbox_new (FALSE, 6);
		gtk_box_pack_start (GTK_BOX (m_box), button, FALSE, TRUE, 0);
		gtk_widget_show_all (m_box);
	}
};

//** Dialogs

static bool confirmCancel()
{
	if (!Ypp::get()->isModified())
		return true;

    GtkWidget *dialog;
	dialog = gtk_message_dialog_new (YGDialog::currentWindow(),
		 GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_NONE,
		 "%s", _("Changes not saved!"));
	gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s",
		_("Quit anyway?"));
	gtk_dialog_add_buttons (GTK_DIALOG (dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_NO,
	                        GTK_STOCK_QUIT, GTK_RESPONSE_YES, NULL);
    gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_YES);

    bool ok = gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_YES;
	gtk_widget_destroy (dialog);
    return ok;
}

bool confirmApply()
{
	if (!Ypp::get()->isModified())
		return true;

	GtkWidget *dialog = gtk_message_dialog_new (YGDialog::currentWindow(),
		GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE,
		"%s", _("Apply changes?"));
	gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
		_("Please review the changes to perfom."));
	gtk_dialog_add_buttons (GTK_DIALOG (dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_NO,
		GTK_STOCK_APPLY, GTK_RESPONSE_YES, NULL);
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_YES);
	gtk_window_set_default_size (GTK_WINDOW (dialog), 550, 500);
	gtk_window_set_resizable (GTK_WINDOW (dialog), TRUE);

#if 0
	GtkWidget *view = m_undoView->createView (NULL);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), view);
#endif
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox),
		gtk_label_new ("TO DO\n\nTo download: xxx\nHard disk needed: xxx"));
	gtk_widget_show_all (dialog);

	gint response = gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
	return response == GTK_RESPONSE_YES;
}

static bool confirmPkgs (const char *title, const char *message,
	GtkMessageType icon, const Ypp::PkgList list,
	const std::string &extraProp)
{
	GtkWidget *dialog = gtk_message_dialog_new (YGDialog::currentWindow(),
		GtkDialogFlags (0), icon, GTK_BUTTONS_NONE, "%s", title);
	gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s", message);
	gtk_dialog_add_buttons (GTK_DIALOG (dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_NO,
		GTK_STOCK_APPLY, GTK_RESPONSE_YES, NULL);
    gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_YES);
	gtk_window_set_resizable (GTK_WINDOW (dialog), TRUE);
	gtk_window_set_default_size (GTK_WINDOW (dialog), -1, 500);
	gtk_widget_show_all (dialog);

	YGtkPackageView *view = ygtk_package_view_new (TRUE);
	view->setVisible (extraProp, true);
	view->setList (list, NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (view), GTK_SHADOW_IN);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), GTK_WIDGET (view));

	bool confirm = (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_YES);
	gtk_widget_destroy (dialog);
	return confirm;
}

static bool askConfirmUnsupported()
{
	Ypp::QueryAnd *query = new Ypp::QueryAnd();
	query->add (new Ypp::QueryProperty ("is-installed", false));
	query->add (new Ypp::QueryProperty ("to-modify", true));
	query->add (new Ypp::QueryProperty ("is-supported", false));

	Ypp::PkgQuery list (Ypp::Package::PACKAGE_TYPE, query);
	if (list.size() > 0)
		return confirmPkgs (_("Unsupported Packages"),	_("Please realize that the following "
			"software is either unsupported or requires an additional customer contract "
			"for support."), GTK_MESSAGE_WARNING, list, "support");
	return true;
}

static bool acceptText (Ypp::Package *package, const std::string &title,
	const std::string &open, const std::string &text, bool question)
{
	GtkWidget *dialog = gtk_message_dialog_new (YGDialog::currentWindow(),
		(GtkDialogFlags) 0, question ? GTK_MESSAGE_QUESTION : GTK_MESSAGE_INFO,
		question ? GTK_BUTTONS_YES_NO : GTK_BUTTONS_OK,
		"%s %s", package->name().c_str(), title.c_str());
	if (!open.empty())
		gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
			"%s", open.c_str());
    gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_YES);

	GtkWidget *view = ygtk_html_wrap_new(), *scroll;
	ygtk_html_wrap_set_text (view, text.c_str(), FALSE);

	scroll = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
		                            GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type
		(GTK_SCROLLED_WINDOW (scroll), GTK_SHADOW_IN);
	gtk_container_add (GTK_CONTAINER (scroll), view);

	GtkBox *vbox = GTK_BOX (GTK_DIALOG(dialog)->vbox);
	gtk_box_pack_start (vbox, scroll, TRUE, TRUE, 6);

	gtk_window_set_resizable (GTK_WINDOW (dialog), TRUE);
	gtk_window_set_default_size (GTK_WINDOW (dialog), 550, 500);
	gtk_widget_show_all (dialog);

	gint ret = gtk_dialog_run (GTK_DIALOG (dialog));
	bool confirmed = (ret == GTK_RESPONSE_YES);
	gtk_widget_destroy (dialog);
	return confirmed;
}

static bool resolveProblems (const std::list <Ypp::Problem *> &problems)
{
	// we can't use ordinary radio buttons, as gtk+ enforces that in a group
	// one must be selected...
	#define DETAILS_PAD  25
	enum ColumnAlias {
		SHOW_TOGGLE_COL, ACTIVE_TOGGLE_COL, TEXT_COL, WEIGHT_TEXT_COL,
		TEXT_PAD_COL, APPLY_PTR_COL, TOTAL_COLS
	};

	struct inner {
		static void solution_toggled (GtkTreeModel *model, GtkTreePath *path)
		{
			GtkTreeStore *store = GTK_TREE_STORE (model);
			GtkTreeIter iter, parent;

			gboolean enabled;
			bool *apply;
			gtk_tree_model_get_iter (model, &iter, path);
			gtk_tree_model_get (model, &iter, ACTIVE_TOGGLE_COL, &enabled,
			                    APPLY_PTR_COL, &apply, -1);
			if (!apply)
				return;

			// disable all the other radios on the group, setting current
			gtk_tree_model_get_iter (model, &iter, path);
			if (gtk_tree_model_iter_parent (model, &parent, &iter)) {
				gtk_tree_model_iter_children (model, &iter, &parent);
				do {
					gtk_tree_store_set (store, &iter, ACTIVE_TOGGLE_COL, FALSE, -1);
					bool *apply;
					gtk_tree_model_get (model, &iter, APPLY_PTR_COL, &apply, -1);
					if (apply) *apply = false;
				} while (gtk_tree_model_iter_next (model, &iter));
			}

			enabled = !enabled;
			*apply = enabled;
			gtk_tree_model_get_iter (model, &iter, path);
			gtk_tree_store_set (store, &iter, ACTIVE_TOGGLE_COL, enabled, -1);
		}
		static void cursor_changed_cb (GtkTreeView *view, GtkTreeModel *model)
		{
			GtkTreePath *path;
			gtk_tree_view_get_cursor (view, &path, NULL);
			solution_toggled (model, path);
			gtk_tree_path_free (path);
		}
	};

	// model
	GtkTreeStore *store = gtk_tree_store_new (TOTAL_COLS,
		G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_INT,
		G_TYPE_INT, G_TYPE_POINTER);
	for (std::list <Ypp::Problem *>::const_iterator it = problems.begin();
	     it != problems.end(); it++) {
		GtkTreeIter problem_iter;
		gtk_tree_store_append (store, &problem_iter, NULL);
		gtk_tree_store_set (store, &problem_iter, SHOW_TOGGLE_COL, FALSE,
			TEXT_COL, (*it)->description.c_str(), WEIGHT_TEXT_COL, PANGO_WEIGHT_BOLD, -1);
		if (!(*it)->details.empty()) {
			GtkTreeIter details_iter;
			gtk_tree_store_append (store, &details_iter, &problem_iter);
			gtk_tree_store_set (store, &details_iter, SHOW_TOGGLE_COL, FALSE,
				TEXT_COL, (*it)->details.c_str(), TEXT_PAD_COL, DETAILS_PAD, -1);
		}

		for (int i = 0; (*it)->getSolution (i); i++) {
			Ypp::Problem::Solution *solution = (*it)->getSolution (i);
			GtkTreeIter solution_iter;
			gtk_tree_store_append (store, &solution_iter, &problem_iter);
			gtk_tree_store_set (store, &solution_iter, SHOW_TOGGLE_COL, TRUE,
				WEIGHT_TEXT_COL, PANGO_WEIGHT_NORMAL,
				ACTIVE_TOGGLE_COL, FALSE, TEXT_COL, solution->description.c_str(),
				APPLY_PTR_COL, &solution->apply, -1);
			if (!solution->details.empty()) {
				gtk_tree_store_append (store, &solution_iter, &problem_iter);
				gtk_tree_store_set (store, &solution_iter, SHOW_TOGGLE_COL, FALSE,
					WEIGHT_TEXT_COL, PANGO_WEIGHT_NORMAL,
					TEXT_COL, solution->details.c_str(), TEXT_PAD_COL, DETAILS_PAD, -1);
			}
		}
	}

	// interface
	GtkWidget *dialog = gtk_message_dialog_new (YGDialog::currentWindow(),
		GtkDialogFlags (0), GTK_MESSAGE_WARNING, GTK_BUTTONS_NONE, "%s",
		_("There are some conflicts on the transaction that must be solved manually."));
	gtk_dialog_add_buttons (GTK_DIALOG (dialog),
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_APPLY, GTK_RESPONSE_APPLY, NULL);
    gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_APPLY);

	GtkWidget *view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
	g_object_unref (G_OBJECT (store));
	gtk_tree_selection_set_mode (gtk_tree_view_get_selection (
		GTK_TREE_VIEW (view)), GTK_SELECTION_NONE);
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (view), FALSE);
	gtk_tree_view_set_search_column (GTK_TREE_VIEW (view), TEXT_COL);
	GtkTreeViewColumn *column;
	GtkCellRenderer *renderer;
	renderer = gtk_cell_renderer_toggle_new();
	gtk_cell_renderer_toggle_set_radio (
		GTK_CELL_RENDERER_TOGGLE (renderer), TRUE);
	// we should not connect the actual toggle button, as we toggle on row press
	g_signal_connect (G_OBJECT (view), "cursor-changed",
		G_CALLBACK (inner::cursor_changed_cb), store);
	column = gtk_tree_view_column_new_with_attributes ("", renderer,
		"visible", SHOW_TOGGLE_COL, "active", ACTIVE_TOGGLE_COL, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (view), column);
	renderer = gtk_cell_renderer_text_new();
	g_object_set (G_OBJECT (renderer), "wrap-width", 400, NULL);
	column = gtk_tree_view_column_new_with_attributes ("", renderer,
		"text", TEXT_COL, "weight", WEIGHT_TEXT_COL, "xpad", TEXT_PAD_COL, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (view), column);
	gtk_tree_view_expand_all (GTK_TREE_VIEW (view));
	gtk_widget_set_has_tooltip (view, TRUE);

	GtkWidget *scroll = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
		GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroll),
		GTK_SHADOW_IN);
	gtk_container_add (GTK_CONTAINER (scroll), view);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), scroll);

	gtk_window_set_resizable (GTK_WINDOW (dialog), TRUE);
	gtk_window_set_default_size (GTK_WINDOW (dialog), -1, 500);
	gtk_widget_show_all (dialog);

	bool apply = (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_APPLY);
	gtk_widget_destroy (dialog);
	return apply;
}

struct _QueryListener
{
	virtual void refresh() = 0;
};

struct _QueryWidget
{
	virtual GtkWidget *getWidget() = 0;

	virtual void updateType (Ypp::Package::Type type) = 0;
	virtual void updateList (Ypp::PkgList list) = 0;
	virtual bool begsUpdate() = 0;

	virtual bool writeQuery (Ypp::QueryAnd *query) = 0;

	void setListener (_QueryListener *listener)
	{ m_listener = listener; }

//protected:
	_QueryWidget() : m_listener (NULL) {}
	virtual ~_QueryWidget() {}

	void notify() { if (m_listener) m_listener->refresh(); }

	void notifyDelay()
	{
		static guint timeout_id = 0;
		struct inner {
			static gboolean timeout_cb (gpointer data)
			{
				_QueryWidget *pThis = (_QueryWidget *) data;
				timeout_id = 0;
				pThis->notify();
				return FALSE;
			}
		};
		if (timeout_id) g_source_remove (timeout_id);
		timeout_id = g_timeout_add (500, inner::timeout_cb, this);
	}

	bool modified;  // flag for internal use

private:
	_QueryListener *m_listener;
};

class SearchEntry : public _QueryWidget
{
GtkWidget *m_widget, *m_entry, *m_combo;
bool m_clearIcon;

public:
	SearchEntry (bool combo_props) : _QueryWidget()
	{
		m_entry = gtk_entry_new();
		g_signal_connect (G_OBJECT (m_entry), "realize",
		                  G_CALLBACK (grab_focus_cb), NULL);
		gtk_widget_set_size_request (m_entry, 50, -1);
		GtkWidget *entry_hbox = gtk_hbox_new (FALSE, 2), *find_label;
		find_label = gtk_label_new_with_mnemonic (_("_Find:"));
		gtk_box_pack_start (GTK_BOX (entry_hbox), find_label, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (entry_hbox), m_entry, TRUE, TRUE, 0);
		gtk_label_set_mnemonic_widget (GTK_LABEL (find_label), m_entry);

		m_clearIcon = false;
		g_signal_connect (G_OBJECT (m_entry), "changed",
		                  G_CALLBACK (entry_changed_cb), this);
		g_signal_connect (G_OBJECT (m_entry), "icon-press",
		                  G_CALLBACK (icon_press_cb), this);

		m_combo = 0;
		if (combo_props) {
			m_combo = gtk_combo_box_new_text();
			gtk_combo_box_append_text (GTK_COMBO_BOX (m_combo), _("Name & Summary"));
			gtk_combo_box_append_text (GTK_COMBO_BOX (m_combo), _("Description"));
			gtk_combo_box_append_text (GTK_COMBO_BOX (m_combo), _("File List"));
			gtk_combo_box_append_text (GTK_COMBO_BOX (m_combo), _("Author"));
			gtk_combo_box_set_active (GTK_COMBO_BOX (m_combo), 0);
			g_signal_connect (G_OBJECT (m_combo), "changed",
				              G_CALLBACK (combo_changed_cb), this);

			GtkWidget *opt_hbox = gtk_hbox_new (FALSE, 2), *empty = gtk_event_box_new();
			gtk_box_pack_start (GTK_BOX (opt_hbox), empty, FALSE, TRUE, 0);
			gtk_box_pack_start (GTK_BOX (opt_hbox), gtk_label_new (_("by")), FALSE, TRUE, 0);
			gtk_box_pack_start (GTK_BOX (opt_hbox), m_combo, TRUE, TRUE, 0);

			GtkSizeGroup *group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);
			gtk_size_group_add_widget (group, find_label);
			gtk_size_group_add_widget (group, empty);
			g_object_unref (G_OBJECT (group));

			m_widget = gtk_vbox_new (FALSE, 4);
			gtk_box_pack_start (GTK_BOX (m_widget), entry_hbox, FALSE, TRUE, 0);
			gtk_box_pack_start (GTK_BOX (m_widget), opt_hbox, FALSE, TRUE, 0);
		}
		else
			m_widget = entry_hbox;
		gtk_widget_show_all (m_widget);
	}

	virtual GtkWidget *getWidget() { return m_widget; }

	virtual void updateType (Ypp::Package::Type type) {}
	virtual void updateList (Ypp::PkgList list) {}
	virtual bool begsUpdate() { return false; }

	static void writeQueryStr (Ypp::QueryOr *query, const std::string &prop, const gchar *value, bool case_sensitive, bool whole_word)
	{
		Ypp::QueryAnd *subquery = new Ypp::QueryAnd();
		const gchar delimiter[2] = { ' ', '\0' };
		gchar **names = g_strsplit (value, delimiter, -1);
		for (gchar **i = names; *i; i++)
			subquery->add (new Ypp::QueryProperty (prop, *i, case_sensitive, whole_word));
		g_strfreev (names);
		query->add (subquery);
	}

	virtual bool writeQuery (Ypp::QueryAnd *query)
	{
		const gchar *name = gtk_entry_get_text (GTK_ENTRY (m_entry));
		if (*name) {
			int opt = 0;
			if (m_combo)
				opt = gtk_combo_box_get_active (GTK_COMBO_BOX (m_combo));

			Ypp::QueryOr *subquery = new Ypp::QueryOr();
			switch (opt) {
				case 0:
					writeQueryStr (subquery, "name", name, false, false);
					writeQueryStr (subquery, "summary", name, false, false);
					break;
				case 1:
					writeQueryStr (subquery, "description", name, false, false);
					break;
				case 2:
					writeQueryStr (subquery, "filelist", name, true, false);
					writeQueryStr (subquery, "provides", name, true, false);
					break;
				case 3:
					subquery->add (new Ypp::QueryProperty ("authors", name, false, true));
					break;
				default: break;
			}
			query->add (subquery);
			return true;
		}
		return false;
	}

	static void entry_changed_cb (GtkEditable *editable, SearchEntry *pThis)
	{
		pThis->notifyDelay();

		const gchar *name = gtk_entry_get_text (GTK_ENTRY (editable));
		bool icon = *name;
		if (pThis->m_clearIcon != icon) {
			gtk_entry_set_icon_from_stock (GTK_ENTRY (editable),
				GTK_ENTRY_ICON_SECONDARY, icon ? GTK_STOCK_CLEAR : NULL);
			gtk_entry_set_icon_activatable (GTK_ENTRY (editable),
				GTK_ENTRY_ICON_SECONDARY, icon);
			if (icon)
				gtk_entry_set_icon_tooltip_text (GTK_ENTRY (editable),
					GTK_ENTRY_ICON_SECONDARY, _("Clear"));
			pThis->m_clearIcon = icon;
		}
	}

	static void combo_changed_cb (GtkComboBox *combo, SearchEntry *pThis)
	{
		const gchar *name = gtk_entry_get_text (GTK_ENTRY (pThis->m_entry));
		if (*name)  // if entry is not empty, no need to refresh query
			pThis->notify();
		gtk_widget_grab_focus (pThis->m_entry);
	}

	static void icon_press_cb (GtkEntry *entry, GtkEntryIconPosition pos,
	                            GdkEvent *event, SearchEntry *pThis)
	{
		gtk_entry_set_text (entry, "");
		gtk_widget_grab_focus (GTK_WIDGET (entry));
	}

	static void grab_focus_cb (GtkWidget *widget)
	{ gtk_widget_grab_focus (widget); }
};

class PropertyModel
{
protected:
	GtkListStore *m_store;
	GtkTreeModel *m_filter;

public:
	enum Column { TEXT_COLUMN, VISIBLE_COLUMN, DATA_COLUMN, ICON_COLUMN, TOTAL_COLUMNS };

	PropertyModel()
	{
		m_store = gtk_list_store_new (TOTAL_COLUMNS, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_POINTER, G_TYPE_STRING);
		if (dynamic_sidebar) {
			m_filter = gtk_tree_model_filter_new (GTK_TREE_MODEL (m_store), NULL);
			gtk_tree_model_filter_set_visible_column (GTK_TREE_MODEL_FILTER (m_filter), 1);
			g_object_unref (G_OBJECT (m_store));
		}
	}

	virtual ~PropertyModel()
	{ g_object_unref (G_OBJECT (getModel())); }

	GtkTreeModel *getModel()
	{ return dynamic_sidebar ? m_filter : GTK_TREE_MODEL (m_store); }

	virtual const char *getLabel() = 0;
	virtual void setType (Ypp::Package::Type type) = 0;
	virtual void updateList (Ypp::PkgList list) = 0;
	virtual bool writeQuery (Ypp::QueryAnd *query, GtkTreeIter *iter) = 0;

	static void list_store_set_text_count (GtkListStore *store, GtkTreeIter *iter,
		const char *text, int count)
	{
		gchar *_text = g_strdup_printf ("%s <small>(%d)</small>", text, count);
		gtk_list_store_set (store, iter, 0, _text, 1, count > 0, -1);
		g_free (_text);
	}
};

class StatusModel : public PropertyModel
{
public:
	StatusModel() : PropertyModel()
	{
		GtkTreeIter iter;
		gtk_list_store_append (m_store, &iter);
		if (!dynamic_sidebar)
			gtk_list_store_set (m_store, &iter, 0, _("All packages"), -1);
		gtk_list_store_set (m_store, &iter, 1, TRUE, 2, GINT_TO_POINTER (0), 3, NULL, -1);
		gtk_list_store_append (m_store, &iter);
		if (!dynamic_sidebar)
			gtk_list_store_set (m_store, &iter, 0, _("Available"), -1);
		gtk_list_store_set (m_store, &iter, 1, TRUE, 2, GINT_TO_POINTER (1), 3, GTK_STOCK_NETWORK, -1);
		gtk_list_store_append (m_store, &iter);
		if (!dynamic_sidebar)
			gtk_list_store_set (m_store, &iter, 0, _("Installed"), -1);
		gtk_list_store_set (m_store, &iter, 1, TRUE, 2, GINT_TO_POINTER (2), 3, GTK_STOCK_HARDDISK, -1);
		gtk_list_store_append (m_store, &iter);
		if (!dynamic_sidebar)
			gtk_list_store_set (m_store, &iter, 0, _("Upgrades"), -1);
		gtk_list_store_set (m_store, &iter, 1, TRUE, 2, GINT_TO_POINTER (3), 3, GTK_STOCK_GO_UP, -1);
		gtk_list_store_append (m_store, &iter);
		if (!dynamic_sidebar)
			gtk_list_store_set (m_store, &iter, 0, _("Summary"), -1);
		gtk_list_store_set (m_store, &iter, 1, TRUE, 2, GINT_TO_POINTER (4), 3, NULL, -1);
	}

	virtual const char *getLabel()
	{ return _("_Status:"); }

	virtual void setType (Ypp::Package::Type type) {}

	virtual void updateList (Ypp::PkgList list)
	{
		if (!dynamic_sidebar) return;
		int installedNb = 0, upgradableNb = 0, notInstalledNb = 0, modifiedNb = 0;
		for (int i = 0; i < list.size(); i++) {
			Ypp::Package *pkg = list.get (i);
			if (pkg->isInstalled()) {
				installedNb++;
				if (pkg->hasUpgrade())
					upgradableNb++;
			}
			else
				notInstalledNb++;
			if (pkg->toModify())
				modifiedNb++;
		}

		GtkListStore *store = m_store;
		GtkTreeModel *model = GTK_TREE_MODEL (m_store);
		GtkTreeIter iter;
		gtk_tree_model_iter_nth_child (model, &iter, NULL, 0);
		list_store_set_text_count (store, &iter, _("Any Status"), list.size());
		gtk_tree_model_iter_nth_child (model, &iter, NULL, 1);
		list_store_set_text_count (store, &iter, _("Not Installed"), notInstalledNb);
		gtk_tree_model_iter_nth_child (model, &iter, NULL, 2);
		list_store_set_text_count (store, &iter, _("Installed"), installedNb);
		gtk_tree_model_iter_nth_child (model, &iter, NULL, 3);
		list_store_set_text_count (store, &iter, _("Upgradable"), upgradableNb);
		gtk_tree_model_iter_nth_child (model, &iter, NULL, 4);
		list_store_set_text_count (store, &iter, _("Modified"), modifiedNb);
	}

	virtual bool writeQuery (Ypp::QueryAnd *query, GtkTreeIter *iter)
	{
		gpointer _status;
		gtk_tree_model_get (getModel(), iter, 2, &_status, -1);
		int status = GPOINTER_TO_INT (_status);
		switch (status) {
			case 0: default: break;
			case 1: query->add (new Ypp::QueryProperty ("is-installed", false)); break;
			case 2: query->add (new Ypp::QueryProperty ("is-installed", true)); break;
			case 3: query->add (new Ypp::QueryProperty ("has-upgrade", true)); break;
			case 4: query->add (new Ypp::QueryProperty ("to-modify", true)); break;
		}
		if (status > 0 && status <= 4)
			return true;
		return false;
	}
};

class CategoryModel : public PropertyModel
{
bool m_type2;

public:
	CategoryModel() : PropertyModel(), m_type2 (false)
	{}

	virtual const char *getLabel()
	{ return _("_Category:"); }

	virtual void setType (Ypp::Package::Type type)
	{
		m_type2 = (type == Ypp::Package::PACKAGE_TYPE);

		gtk_list_store_clear (m_store);
		GtkTreeIter iter;
		gtk_list_store_append (m_store, &iter);
		if (!dynamic_sidebar)
			gtk_list_store_set (m_store, &iter, 0, _("All packages"), -1);
		gtk_list_store_set (m_store, &iter, 1, TRUE, 2, NULL, ICON_COLUMN, NULL, -1);
		Ypp::Node *category;
		if (m_type2)
			category = Ypp::get()->getFirstCategory2 (type);
		else
			category = Ypp::get()->getFirstCategory (type);
		for (; category; category = category->next()) {
			gtk_list_store_append (m_store, &iter);
			if (!dynamic_sidebar)
				gtk_list_store_set (m_store, &iter, 0, category->name.c_str(), -1);
			gtk_list_store_set (m_store, &iter, 1, TRUE, 2, category, ICON_COLUMN, category->icon, -1);
		}
	}

	virtual void updateList (Ypp::PkgList list)
	{
		if (!dynamic_sidebar) return;
		GtkTreeModel *model = GTK_TREE_MODEL (m_store);
		GtkTreeIter iter;
		if (gtk_tree_model_get_iter_first (model, &iter))
			do {
				Ypp::Node *category;
				gtk_tree_model_get (model, &iter, 2, &category, -1);

				if (category) {
					int categoriesNb = 0;
					for (int i = 0; i < list.size(); i++) {
						Ypp::Package *pkg = list.get (i);
						Ypp::Node *pkg_category = m_type2 ? pkg->category2() : pkg->category();
						if (pkg_category == category)
							categoriesNb++;
					}
					list_store_set_text_count (m_store, &iter, category->name.c_str(), categoriesNb);
				}
				else
					list_store_set_text_count (m_store, &iter, _("All Categories"), list.size());
			} while (gtk_tree_model_iter_next (model, &iter));
	}

	virtual bool writeQuery (Ypp::QueryAnd *query, GtkTreeIter *iter)
	{
		Ypp::Node *category;
		gtk_tree_model_get (getModel(), iter, 2, &category, -1);
		if (category) {
			if (m_type2)
				query->add (new Ypp::QueryCategory (category, true));
			else
				query->add (new Ypp::QueryCategory (category, false));
			return true;
		}
		return false;
	}
};

class RepositoryModel : public PropertyModel
{
public:
	RepositoryModel() : PropertyModel()
	{
		GtkTreeIter iter;
		gtk_list_store_append (m_store, &iter);
		if (!dynamic_sidebar)
			gtk_list_store_set (m_store, &iter, 0, _("All packages"), -1);
		gtk_list_store_set (m_store, &iter, 1, TRUE, 2, NULL, ICON_COLUMN, NULL, -1);
		for (int i = 0; Ypp::get()->getRepository (i); i++) {
			const Ypp::Repository *repo = Ypp::get()->getRepository (i);
			gtk_list_store_append (m_store, &iter);
			if (!dynamic_sidebar) {
				if (big_icons_sidebar)
					gtk_list_store_set (m_store, &iter, 0, (repo->name + '\n' + repo->url).c_str(), -1);
				else
					gtk_list_store_set (m_store, &iter, 0, repo->name.c_str(), -1);
			}
			gtk_list_store_set (m_store, &iter, 1, TRUE, 2, repo, -1);

			const gchar *icon;
			if (repo->url.empty())
				icon = GTK_STOCK_MISSING_IMAGE;
			else if (repo->url.compare (0, 2, "cd", 2) == 0 ||
			         repo->url.compare (0, 3, "dvd", 3) == 0)
				icon = GTK_STOCK_CDROM;
			else if (repo->url.compare (0, 3, "iso", 3) == 0)
				icon = GTK_STOCK_FILE;
			else
				icon = GTK_STOCK_NETWORK;
			gtk_list_store_set (m_store, &iter, ICON_COLUMN, icon, -1);
		}
	}

	virtual const char *getLabel()
	{ return _("_Repository:"); }

	virtual void setType (Ypp::Package::Type type) {}

	virtual void updateList (Ypp::PkgList list)
	{
		if (!dynamic_sidebar) return;
		GtkTreeModel *model = GTK_TREE_MODEL (m_store);
		GtkTreeIter iter;
		if (gtk_tree_model_get_iter_first (model, &iter))
			do {
				Ypp::Repository *repo;
				gtk_tree_model_get (model, &iter, 2, &repo, -1);
				if (repo) {
					int sum = 0;
					for (int i = 0; i < list.size(); i++) {
						Ypp::Package *pkg = list.get (i);
						for (int v = 0; pkg->getAvailableVersion (v); v++) {
							const Ypp::Package::Version *version = pkg->getAvailableVersion (v);
							if (version->repo == repo) {
								sum++;
								break;
							}
						}
					}
					if (big_icons_sidebar)
						list_store_set_text_count (m_store, &iter, (repo->name + '\n' + repo->url).c_str(), sum);
					else
						list_store_set_text_count (m_store, &iter, repo->name.c_str(), sum);
				}
				else
					list_store_set_text_count (m_store, &iter, _("All Repositories"), list.size());
			} while (gtk_tree_model_iter_next (model, &iter));
	}

	virtual bool writeQuery (Ypp::QueryAnd *query, GtkTreeIter *iter)
	{
		Ypp::Repository *repo;
		gtk_tree_model_get (getModel(), iter, 2, &repo, -1);
		if (repo) {
			query->add (new Ypp::QueryRepository (repo));
			return true;
		}
		return false;
	}
};

class PropertyView : public _QueryWidget
{
GtkWidget *m_widget, *m_view;
PropertyModel *m_model;

public:
	PropertyView (PropertyModel *model) : _QueryWidget()
	{
		m_model = model;
		m_view = gtk_tree_view_new_with_model (m_model->getModel());
		GtkTreeView *tview = GTK_TREE_VIEW (m_view);
		gtk_tree_view_set_headers_visible (tview, FALSE);
		gtk_tree_view_set_search_column (tview, 0);
		gtk_tree_view_set_tooltip_column (tview, 0);
		GtkTreeSelection *selection = gtk_tree_view_get_selection (tview);
		gtk_tree_selection_set_mode (selection, GTK_SELECTION_BROWSE);
		GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
		g_object_set (G_OBJECT (renderer), "ellipsize",
			dynamic_sidebar ? PANGO_ELLIPSIZE_MIDDLE : PANGO_ELLIPSIZE_END, NULL);
		GtkTreeViewColumn *column;
		if (icons_sidebar || big_icons_sidebar) {
			GtkCellRenderer *renderer = gtk_cell_renderer_pixbuf_new();
			column = gtk_tree_view_column_new_with_attributes (NULL,
				renderer, "icon-name", PropertyModel::ICON_COLUMN, NULL);
			g_object_set (G_OBJECT (renderer), "stock-size", big_icons_sidebar ? GTK_ICON_SIZE_LARGE_TOOLBAR : GTK_ICON_SIZE_MENU, NULL);
			gtk_tree_view_append_column (tview, column);
		}
		column = gtk_tree_view_column_new_with_attributes (
			NULL, renderer, "markup", PropertyModel::TEXT_COLUMN, NULL);
		gtk_tree_view_append_column (tview, column);

		GtkWidget *scroll = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scroll),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scroll), GTK_SHADOW_IN);
		gtk_container_add (GTK_CONTAINER (scroll), m_view);

		if (layered_sidebar)
			m_widget = scroll;
		else if (expander_sidebar) {
			m_widget = gtk_expander_new_with_mnemonic (m_model->getLabel());
			gtk_container_add (GTK_CONTAINER (m_widget), scroll);
		}
		else {
			GtkWidget *label = gtk_label_new_with_mnemonic (m_model->getLabel());
			gtk_misc_set_alignment (GTK_MISC (label), 0, .5);
			gtk_label_set_mnemonic_widget (GTK_LABEL (label), m_view);

			m_widget = gtk_vbox_new (FALSE, 4);
			gtk_box_pack_start (GTK_BOX (m_widget), label, FALSE, TRUE, 0);
			gtk_box_pack_start (GTK_BOX (m_widget), scroll, TRUE, TRUE, 0);
		}

		selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (m_view));
		g_signal_connect (G_OBJECT (selection), "changed",
		                  G_CALLBACK (selection_changed_cb), this);
	}

	~PropertyView()
	{ delete m_model; }

	virtual GtkWidget *getWidget()
	{ return m_widget; }

	virtual void updateType (Ypp::Package::Type type)
	{
		m_model->setType (type);

		GtkTreeSelection *selection;
		GtkTreeIter iter;
		selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (m_view));
		g_signal_handlers_block_by_func (selection, (gpointer) selection_changed_cb, this);
		gtk_tree_model_get_iter_first (m_model->getModel(), &iter);
		gtk_tree_selection_select_iter (selection, &iter);
		g_signal_handlers_unblock_by_func (selection, (gpointer) selection_changed_cb, this);
	}

	virtual void updateList (Ypp::PkgList list)
	{ m_model->updateList (list); }

	virtual bool begsUpdate() { return dynamic_sidebar; }

	virtual bool writeQuery (Ypp::QueryAnd *query)
	{
		GtkTreeIter iter;
		GtkTreeSelection *selection;
		selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (m_view));
		if (gtk_tree_selection_get_selected (selection, NULL, &iter))
			return m_model->writeQuery (query, &iter);
		return false;
	}

	static void selection_changed_cb (GtkTreeSelection *selection, _QueryWidget *pThis)
	{
		if (gtk_tree_selection_get_selected (selection, NULL, NULL))
			pThis->notify();
	}
};

class PropertyCombo : public _QueryWidget
{
GtkWidget *m_widget, *m_combo;
PropertyModel *m_model;

public:
	PropertyCombo (PropertyModel *model) : _QueryWidget()
	{
		m_model = model;
		m_combo = gtk_combo_box_new_with_model (m_model->getModel());

		GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
		gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (m_combo), renderer, TRUE);
		gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (m_combo), renderer,
			"markup", 0, NULL);

		GtkWidget *label = gtk_label_new_with_mnemonic (m_model->getLabel());
		gtk_misc_set_alignment (GTK_MISC (label), 0, .5);
		gtk_label_set_mnemonic_widget (GTK_LABEL (label), m_combo);

		m_widget = gtk_hbox_new (FALSE, 2);
		gtk_box_pack_start (GTK_BOX (m_widget), label, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (m_widget), m_combo, TRUE, TRUE, 0);
		g_signal_connect (G_OBJECT (m_combo), "changed",
		                  G_CALLBACK (changed_cb), this);
	}

	~PropertyCombo()
	{ delete m_model; }

	virtual GtkWidget *getWidget()
	{ return m_widget; }

	virtual void updateType (Ypp::Package::Type type)
	{
		m_model->setType (type);

		g_signal_handlers_block_by_func (m_combo, (gpointer) changed_cb, this);
		gtk_combo_box_set_active (GTK_COMBO_BOX (m_combo), 0);
		g_signal_handlers_unblock_by_func (m_combo, (gpointer) changed_cb, this);
	}

	virtual void updateList (Ypp::PkgList list)
	{ m_model->updateList (list); }

	virtual bool begsUpdate() { return dynamic_sidebar; }

	virtual bool writeQuery (Ypp::QueryAnd *query)
	{
		GtkTreeIter iter;
		if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX (m_combo), &iter))
			return m_model->writeQuery (query, &iter);
		return false;
	}

	static void changed_cb (GtkComboBox *combo, PropertyCombo *pThis)
	{ pThis->notify(); }
};

#define BROWSER_PATH "/usr/bin/firefox"

class DetailBox
{
GtkWidget *m_widget, *m_text, *m_pkg_view, *m_link_popup;
std::string m_link_str;

public:
	GtkWidget *getWidget() { return m_widget; }

	DetailBox()
	: m_link_popup (NULL)
	{
		m_text = ygtk_rich_text_new();
/*
		ygtk_rich_text_set_text (YGTK_RICH_TEXT (m_text),
			_("<i>Write a little introductory text here</i>"));
*/
		YGtkPackageView *view = ygtk_package_view_new (TRUE);
		m_pkg_view = GTK_WIDGET (view);
		//gtk_widget_set_size_request (m_pkg_view, -1, 200);

		GtkWidget *vbox = gtk_vbox_new (FALSE, 18);
		gtk_box_pack_start (GTK_BOX (vbox), m_text, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), m_pkg_view, TRUE, TRUE, 0);

		m_widget = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (m_widget),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (m_widget), vbox);

		g_signal_connect (G_OBJECT (m_text), "link-clicked",
		                  G_CALLBACK (link_clicked_cb), this);
		g_signal_connect (G_OBJECT (vbox), "expose-event",
		                  G_CALLBACK (paint_white_expose_cb), NULL);
	}

	~DetailBox()
	{
		if (m_link_popup)
			gtk_widget_destroy (m_link_popup);
	}

	void setPackage (Ypp::Package *package)
	{
		if (package) {
			std::string text = package->description (HTML_MARKUP);
			ygtk_rich_text_set_text (YGTK_RICH_TEXT (m_text), text.c_str());
			gtk_widget_show (m_widget);
		}
		else
			ygtk_rich_text_set_plain_text (YGTK_RICH_TEXT (m_text), "");

		if (package && package->type() != Ypp::Package::PACKAGE_TYPE) {
			Ypp::QueryBase *query = new Ypp::QueryCollection (package);
			Ypp::PkgQuery list (Ypp::Package::PACKAGE_TYPE, query);
			((YGtkPackageView *) m_pkg_view)->setList (list, NULL);
			gtk_widget_show (m_pkg_view);
		}
		else
			gtk_widget_hide (m_pkg_view);
	}

	static void link_clicked_cb (YGtkRichText *text, const gchar *link, DetailBox *pThis)
	{
		if (!pThis->m_link_popup) {
			GtkWidget *menu = pThis->m_link_popup = gtk_menu_new();
			gtk_menu_attach_to_widget (GTK_MENU (menu), GTK_WIDGET (text), NULL);

			GtkWidget *item;
			if (g_file_test (BROWSER_PATH, G_FILE_TEST_IS_EXECUTABLE)) {
				std::string label;
				if (getuid() == 0) {
					const char *username = getenv ("USERNAME");
					if (!username || !(*username))
						username = "root";
					label = _("_Open (as ");
					label += username;
					label += ")";
				}
				else
					label = _("_Open");
				item = gtk_image_menu_item_new_with_mnemonic (label.c_str());
				gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (item),
					gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_MENU));
				gtk_widget_show (item);
				g_signal_connect (G_OBJECT (item), "activate",
					              G_CALLBACK (open_link_cb), pThis);
				gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
			}
			item = gtk_image_menu_item_new_from_stock (GTK_STOCK_COPY, NULL);
			gtk_widget_show (item);
			g_signal_connect (G_OBJECT (item), "activate",
				              G_CALLBACK (copy_link_cb), pThis);
			gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
		}
		gtk_menu_popup (GTK_MENU (pThis->m_link_popup), NULL, NULL, NULL, NULL, 
		                0, gtk_get_current_event_time());
		pThis->m_link_str = link;
	}

	static void open_link_cb (GtkMenuItem *item, DetailBox *pThis)
	{
		const std::string &link = pThis->m_link_str;
		std::string command;
		command.reserve (256);

		const char *username = 0;
		if (getuid() == 0) {
			username = getenv ("USERNAME");
			if (username && !(*username))
				username = 0;
		}

		if (username) {
			command += "gnomesu -u ";
			command += username;
			command += " -c \"" BROWSER_PATH " --new-window ";
			command += link;
			command += "\"";
		}
		else {
			command += BROWSER_PATH " --new-window ";
			command += link;
		}
		command += " &";
		system (command.c_str());
	}

	static void copy_link_cb (GtkMenuItem *item, DetailBox *pThis)
	{
		const std::string &link = pThis->m_link_str;
		GtkClipboard *clipboard =
			gtk_widget_get_clipboard (pThis->m_text, GDK_SELECTION_CLIPBOARD);
		gtk_clipboard_set_text (clipboard, link.c_str(), -1);
	}
};

class Toolbar
{
GtkWidget *m_toolbar;
GtkWidget *m_install_button, *m_upgrade_button, *m_remove_button;
Ypp::PkgList m_packages;

public:
	GtkWidget *getWidget() { return m_toolbar; }

	Toolbar (bool yast_style)
	{
		if (yast_style) {
			m_toolbar = gtk_hbox_new (FALSE, 6);
			m_install_button = gtk_button_new_with_label (_("Install"));
			gtk_button_set_image (GTK_BUTTON (m_install_button),
				gtk_image_new_from_stock (GTK_STOCK_ADD, GTK_ICON_SIZE_BUTTON));
			gtk_box_pack_start (GTK_BOX (m_toolbar), m_install_button, FALSE, TRUE, 0);
			m_upgrade_button = gtk_button_new_with_label (_("Upgrade"));
			gtk_button_set_image (GTK_BUTTON (m_upgrade_button),
				gtk_image_new_from_stock (GTK_STOCK_GO_UP, GTK_ICON_SIZE_BUTTON));
			gtk_box_pack_start (GTK_BOX (m_toolbar), m_upgrade_button, FALSE, TRUE, 0);
			m_remove_button = gtk_button_new_with_label (_("Remove"));
			gtk_button_set_image (GTK_BUTTON (m_remove_button),
				gtk_image_new_from_stock (GTK_STOCK_REMOVE, GTK_ICON_SIZE_BUTTON));
			gtk_box_pack_start (GTK_BOX (m_toolbar), m_remove_button, FALSE, TRUE, 0);
		}
		else {
			m_toolbar = gtk_toolbar_new();
			m_install_button = GTK_WIDGET (gtk_tool_button_new (
				gtk_image_new_from_stock (GTK_STOCK_ADD, GTK_ICON_SIZE_LARGE_TOOLBAR), _("Install")));
			gtk_tool_item_set_is_important (GTK_TOOL_ITEM (m_install_button), TRUE);
			gtk_toolbar_insert (GTK_TOOLBAR (m_toolbar), GTK_TOOL_ITEM (m_install_button), -1);
			m_upgrade_button = GTK_WIDGET (gtk_tool_button_new (
				gtk_image_new_from_stock (GTK_STOCK_GO_UP, GTK_ICON_SIZE_LARGE_TOOLBAR), _("Upgrade")));
			gtk_tool_item_set_is_important (GTK_TOOL_ITEM (m_upgrade_button), TRUE);
			gtk_toolbar_insert (GTK_TOOLBAR (m_toolbar), GTK_TOOL_ITEM (m_upgrade_button), -1);
			m_remove_button = GTK_WIDGET (gtk_tool_button_new (
				gtk_image_new_from_stock (GTK_STOCK_REMOVE, GTK_ICON_SIZE_LARGE_TOOLBAR), _("Remove")));
			gtk_tool_item_set_is_important (GTK_TOOL_ITEM (m_remove_button), TRUE);
			gtk_toolbar_insert (GTK_TOOLBAR (m_toolbar), GTK_TOOL_ITEM (m_remove_button), -1);
		}

		g_signal_connect (G_OBJECT (m_install_button), "clicked",
			              G_CALLBACK (install_clicked_cb), this);
		g_signal_connect (G_OBJECT (m_upgrade_button), "clicked",
			              G_CALLBACK (upgrade_clicked_cb), this);
		g_signal_connect (G_OBJECT (m_remove_button), "clicked",
			              G_CALLBACK (remove_clicked_cb), this);

		setPackages (Ypp::PkgList());
	}

	void setPackages (Ypp::PkgList list)
	{
		m_packages = list;
		if (list.size() == 0) {
			gtk_widget_set_sensitive (m_install_button, FALSE);
			gtk_widget_set_sensitive (m_upgrade_button, FALSE);
			gtk_widget_set_sensitive (m_remove_button, FALSE);
		}
		else {
			bool installed = list.installed();
			gtk_widget_set_sensitive (m_install_button, !installed);
			gtk_widget_set_sensitive (m_remove_button, installed);
			gtk_widget_set_sensitive (m_upgrade_button, list.upgradable());
		}
	}

	static void install_clicked_cb (GtkWidget *button, Toolbar *pThis)
	{
		pThis->m_packages.install();
	}

	static void upgrade_clicked_cb (GtkWidget *button, Toolbar *pThis)
	{
		pThis->m_packages.install();
	}

	static void remove_clicked_cb (GtkWidget *button, Toolbar *pThis)
	{
		pThis->m_packages.remove();
	}
};

static GtkWidget *gtk_vpaned_append (GtkWidget *vpaned, GtkWidget *child)
{
	if (!vpaned)
		return child;
	if (!GTK_IS_VPANED (vpaned) || gtk_paned_get_child2 (GTK_PANED (vpaned))) {
		GtkWidget *ret = gtk_vpaned_new();
		gtk_paned_pack1 (GTK_PANED (ret), vpaned, TRUE, FALSE);
		gtk_paned_pack2 (GTK_PANED (ret), child, TRUE, FALSE);
		return ret;
	}
	gtk_paned_pack2 (GTK_PANED (vpaned), child, TRUE, FALSE);
	return vpaned;
}

// expander box

static void ygtk_expander_box_size_allocate (GtkWidget *vbox, GtkAllocation *alloc)
{
	GtkContainer *container = GTK_CONTAINER (vbox);
	GList *children = gtk_container_get_children (container);
	int expandable_height = alloc->height, expand_nb = 0;
	for (GList *i = children; i; i  = i->next) {
		GtkWidget *child = (GtkWidget *) i->data;
		if (!gtk_expander_get_expanded (GTK_EXPANDER (child))) {
			GtkRequisition req;
			gtk_widget_get_child_requisition (child, &req);
			expandable_height -= req.height;
		}
		else
			expand_nb++;
	}
	int each_expandable_height = 0;
	if (expand_nb)
		each_expandable_height = expandable_height / expand_nb;
	int y = 0;
	for (GList *i = children; i; i  = i->next) {
		GtkWidget *child = (GtkWidget *) i->data;
		if (gtk_expander_get_expanded (GTK_EXPANDER (child))) {
			GtkAllocation child_alloc = { alloc->x, alloc->y + y, alloc->width, each_expandable_height };
			gtk_widget_size_allocate (child, &child_alloc);
			y += child_alloc.height;
		}
		else {
			GtkRequisition req;
			gtk_widget_get_child_requisition (child, &req);

			GtkAllocation child_alloc = { alloc->x, alloc->y + y, alloc->width, req.height };
			gtk_widget_size_allocate (child, &child_alloc);
			y += child_alloc.height;
		}
	}
	g_list_free (children);
}

typedef struct YGtkExpanderBox
{ GtkVBox parent; } YGtkExpanderBox;

typedef struct YGtkExpanderBoxClass
{ GtkVBoxClass parent_class; } YGtkExpanderBoxClass;

G_DEFINE_TYPE (YGtkExpanderBox, ygtk_expander_box, GTK_TYPE_VBOX)

static void ygtk_expander_box_init (YGtkExpanderBox *box)
{
}

static void ygtk_expander_box_class_init (YGtkExpanderBoxClass *klass)
{
	ygtk_expander_box_parent_class = g_type_class_peek_parent (klass);

	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
	widget_class->size_allocate = ygtk_expander_box_size_allocate;
}

static GtkWidget *gtk_expander_vbox_new (int spacing)
{
	GtkWidget *widget = (GtkWidget *) g_object_new (ygtk_expander_box_get_type(), NULL);
	GTK_BOX (widget)->spacing = spacing;
	return widget;
}

// tooltip box

typedef struct YGtkTooltipBox
{ GtkEventBox parent; } YGtkTooltipBox;

typedef struct YGtkTooltipBoxClass
{ GtkEventBoxClass parent_class; } YGtkTooltipBoxClass;

G_DEFINE_TYPE (YGtkTooltipBox, ygtk_tooltip_box, GTK_TYPE_EVENT_BOX)

static void ygtk_tooltip_box_init (YGtkTooltipBox *tooltip)
{
	GdkColor yellow = { 0, 246 << 8, 246 << 8, 187 << 8 };
	gtk_widget_modify_bg (GTK_WIDGET (tooltip), GTK_STATE_NORMAL, &yellow);
}

static gboolean ygtk_tooltip_box_expose_event (GtkWidget *tooltip, GdkEventExpose *event)
{
	gtk_paint_flat_box (tooltip->style, tooltip->window,
		GTK_STATE_NORMAL, GTK_SHADOW_OUT, NULL, tooltip,
		"tooltip", 0, 0, tooltip->allocation.width,
		tooltip->allocation.height);
	GtkWidget *child = GTK_BIN (tooltip)->child;
	if (child)
		gtk_container_propagate_expose (GTK_CONTAINER (tooltip), child, event);
	return TRUE;
}

static void ygtk_tooltip_box_class_init (YGtkTooltipBoxClass *klass)
{
	ygtk_tooltip_box_parent_class = g_type_class_peek_parent (klass);

	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
	widget_class->expose_event = ygtk_tooltip_box_expose_event;
}

static GtkWidget *ygtk_tooltip_box_new (void)
{ return (GtkWidget *) g_object_new (ygtk_tooltip_box_get_type(), NULL); }

// stack (or deck) widget

class StackWidget
{
GtkWidget *m_combo, *m_placeholder, *m_widget;
std::vector <GtkWidget *> m_children;

public:
	GtkWidget *getWidget()
	{ return m_widget; }

	StackWidget()
	{
		if (layered_tabs_sidebar) {
			m_widget = gtk_notebook_new();
			gtk_notebook_set_tab_pos (GTK_NOTEBOOK (m_widget), GTK_POS_LEFT);
		}
		else {
			m_combo = gtk_combo_box_new_text();
			m_placeholder = gtk_event_box_new();
			m_widget = gtk_vbox_new (FALSE, 0);
			gtk_box_pack_start (GTK_BOX (m_widget), m_combo, FALSE, TRUE, 0);
			gtk_box_pack_start (GTK_BOX (m_widget), m_placeholder, TRUE, TRUE, 0);
			g_signal_connect (G_OBJECT (m_combo), "changed",
				              G_CALLBACK (combo_changed_cb), this);
		}
	}

	~StackWidget()
	{
		for (unsigned int i = 0; i < m_children.size(); i++)
			g_object_unref (G_OBJECT (m_children[i]));
	}

	void append (const char *title, GtkWidget *widget)
	{
		m_children.push_back (widget);
		g_object_ref_sink (G_OBJECT (widget));
		gtk_widget_show_all (widget);
		if (layered_tabs_sidebar) {
			GtkWidget *label = gtk_label_new (title);
			gtk_label_set_angle (GTK_LABEL (label), 90);
			gtk_notebook_append_page (GTK_NOTEBOOK (m_widget), widget, label);
		}
		else {
			gtk_combo_box_append_text (GTK_COMBO_BOX (m_combo), title);
			if (gtk_combo_box_get_active (GTK_COMBO_BOX (m_combo)) == -1)
				gtk_combo_box_set_active (GTK_COMBO_BOX (m_combo), 0);
		}
	}

	static void combo_changed_cb (GtkComboBox *combo, StackWidget *pThis)
	{
		int i = gtk_combo_box_get_active (combo);
		GtkWidget *child = pThis->m_children[i];
		GtkWidget *old_child = GTK_BIN (pThis->m_placeholder)->child;
		if (old_child)
			gtk_container_remove (GTK_CONTAINER (pThis->m_placeholder), old_child);
		gtk_container_add (GTK_CONTAINER (pThis->m_placeholder), child);
	}
};

#define COLS_NB 5

class StartupMenu : public _QueryWidget
{
GtkWidget *m_widget, *m_list_widget, *m_parent;
Ypp::Node *m_category;
int m_status;

public:
	StartupMenu (GtkWidget *list_widget) : _QueryWidget(), m_category (NULL), m_status (-1)
	{
		m_parent = 0;
		m_list_widget = list_widget;

		m_widget = gtk_vbox_new (FALSE, 12);
		gtk_container_set_border_width (GTK_CONTAINER (m_widget), 12);  // FIXME: are these the inner borders?
		g_signal_connect (G_OBJECT (m_widget), "expose-event",
			              G_CALLBACK (paint_white_expose_cb), NULL);

		GtkWidget *header, *table;
		header = header_new (_("Categories"));
		int categories_nb = 0;
		for (Ypp::Node *i = Ypp::get()->getFirstCategory2 (Ypp::Package::PACKAGE_TYPE);
		     i; i = i->next())
			categories_nb++;
		table = table_new (categories_nb+1);
		GtkWidget *button = button_new (NULL, _("All Packages"));
		table_pack (table, button);
		g_signal_connect (G_OBJECT (button), "clicked",
		                  G_CALLBACK (category_clicked_cb), this);
		for (Ypp::Node *i = Ypp::get()->getFirstCategory2 (Ypp::Package::PACKAGE_TYPE);
		     i; i = i->next()) {
			GtkWidget *button = button_new (i->icon, i->name.c_str());
			table_pack (table, button);
			g_object_set_data (G_OBJECT (button), "category", i);
			g_signal_connect (G_OBJECT (button), "clicked",
			                  G_CALLBACK (category_clicked_cb), this);
		}
		gtk_box_pack_start (GTK_BOX (m_widget), header, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (m_widget), table, FALSE, TRUE, 0);

		header = header_new (_("Status"));
		table = table_new (4);
		button = button_new (GTK_STOCK_NETWORK, _("Available"));
		g_object_set_data (G_OBJECT (button), "status", GINT_TO_POINTER (0));
		g_signal_connect (G_OBJECT (button), "clicked",
		                  G_CALLBACK (status_clicked_cb), this);
		table_pack (table, button);
		button = button_new (GTK_STOCK_HARDDISK, _("Installed"));
		g_object_set_data (G_OBJECT (button), "status", GINT_TO_POINTER (1));
		g_signal_connect (G_OBJECT (button), "clicked",
		                  G_CALLBACK (status_clicked_cb), this);
		table_pack (table, button);
		button = button_new (GTK_STOCK_GO_UP, _("Upgrades"));
		g_object_set_data (G_OBJECT (button), "status", GINT_TO_POINTER (2));
		g_signal_connect (G_OBJECT (button), "clicked",
		                  G_CALLBACK (status_clicked_cb), this);
		table_pack (table, button);
		button = button_new (GTK_STOCK_UNDO, _("Modified"));
		g_object_set_data (G_OBJECT (button), "status", GINT_TO_POINTER (3));
		g_signal_connect (G_OBJECT (button), "clicked",
		                  G_CALLBACK (status_clicked_cb), this);
		table_pack (table, button);
		gtk_box_pack_start (GTK_BOX (m_widget), header, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (m_widget), table, FALSE, TRUE, 0);

		g_object_ref_sink (m_list_widget);
		g_object_ref_sink (m_widget);
		gtk_widget_show_all (m_widget);
	}

	~StartupMenu()
	{
		g_object_unref (G_OBJECT (m_widget));
		g_object_unref (G_OBJECT (m_list_widget));
	}

	static GtkWidget *header_new (const char *title)
	{
		GtkWidget *label = gtk_label_new (title);
		YGUtils::setWidgetFont (label, PANGO_STYLE_NORMAL, PANGO_WEIGHT_BOLD, PANGO_SCALE_LARGE);
		GdkColor gray = { 0, 200 << 8, 200 << 8, 200 << 8 };
		gtk_widget_modify_fg (label, GTK_STATE_NORMAL, &gray);
		GtkWidget *header = gtk_hbox_new (FALSE, 2);
		gtk_box_pack_start (GTK_BOX (header), label, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (header), gtk_hseparator_new(), TRUE, TRUE, 0);
		return header;
	}

	static GtkWidget *table_new (int nb)
	{
		int rows = nb / COLS_NB, cols = COLS_NB;
		if (nb % COLS_NB != 0)
			rows++;
		GtkWidget *table = gtk_table_new (rows, cols, TRUE);
		gtk_table_set_row_spacings (GTK_TABLE (table), 6);
		gtk_table_set_col_spacings (GTK_TABLE (table), 6);
		return table;
	}

	static void table_pack (GtkWidget *table, GtkWidget *child)
	{  // unlike gtk_table_attach(), no need to pass the exact positioning
	  // and no, gtk_container_add() doesn't work
		GList *children = gtk_container_get_children (GTK_CONTAINER (table));
		guint pos = g_list_length (children);
		g_list_free (children);
		guint cols_nb;
		g_object_get (G_OBJECT (table), "n-columns", &cols_nb, NULL);
		guint col = pos % cols_nb, row = pos / cols_nb;
		gtk_table_attach (GTK_TABLE (table), child, col, col+1, row, row+1,
			GTK_FILL, GTK_FILL, 0, 0);
	}

	static GtkWidget *button_new (const char *icon, const char *title)
	{
		GtkWidget *button = gtk_button_new_with_label (title);
		if (icon) {
			GtkWidget *image = gtk_image_new_from_icon_name (icon, GTK_ICON_SIZE_BUTTON);
			gtk_button_set_image (GTK_BUTTON (button), image);
			gtk_widget_show (image);
		}
		return button;
	}

	virtual GtkWidget *getWidget() { return m_widget; }

	virtual void updateType (Ypp::Package::Type type) {}
	virtual void updateList (Ypp::PkgList list) {}
	virtual bool begsUpdate() { return false; }

	virtual bool writeQuery (Ypp::QueryAnd *query)
	{
		if (m_category) {
			query->add (new Ypp::QueryCategory (m_category, true));
			return true;
		}
		switch (m_status) {
			case 0:
				query->add (new Ypp::QueryProperty ("is-installed", false));
				return true;
			case 1:
				query->add (new Ypp::QueryProperty ("is-installed", true));
				return true;
			case 2:
				query->add (new Ypp::QueryProperty ("has-upgrade", true));
				return true;
			case 3:
				query->add (new Ypp::QueryProperty ("to-modify", true));
				return true;
			case -1: default: break;
		}
		return false;
	}

	void applySelection()
	{
		if (!m_parent)
			m_parent = gtk_widget_get_parent (m_widget);
		gtk_container_remove (GTK_CONTAINER (m_parent), m_widget);
		gtk_container_add (GTK_CONTAINER (m_parent), m_list_widget);
		notify();
	}

	void restore()
	{
		gtk_container_remove (GTK_CONTAINER (m_parent), m_list_widget);
		gtk_container_add (GTK_CONTAINER (m_parent), m_widget);
	}

	static void category_clicked_cb (GtkButton *button, StartupMenu *pThis)
	{
		pThis->m_category = (Ypp::Node *) g_object_get_data (G_OBJECT (button), "category");
		pThis->applySelection();
	}

	static void status_clicked_cb (GtkButton *button, StartupMenu *pThis)
	{
		pThis->m_status = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (button), "status"));
		pThis->applySelection();
	}
};

class UndoBox : public Ypp::PkgList::Listener
{
GtkWidget *m_undo_box, *m_undo_package, *m_undo_details;
Ypp::PkgList m_pool;
Ypp::Package *m_package;

public:
	GtkWidget *getWidget()
	{ return m_undo_box; }

	UndoBox()
	{
		Ypp::QueryBase *query = new Ypp::QueryProperty ("to-modify", true);
//		if (pkg_selector->onlineUpdateMode())
		m_pool = Ypp::PkgQuery (Ypp::Package::PACKAGE_TYPE, query);
		// TODO: there could already be packages modified
		m_pool.addListener (this);

		m_undo_box = ygtk_tooltip_box_new();
		GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
		gtk_container_set_border_width (GTK_CONTAINER (vbox), 6);
		gtk_container_add (GTK_CONTAINER (m_undo_box), vbox);
		GtkWidget *title = gtk_label_new (_("Software changes"));
		gtk_misc_set_alignment (GTK_MISC (title), 0, .5);
		YGUtils::setWidgetFont (title, PANGO_STYLE_NORMAL, PANGO_WEIGHT_BOLD, PANGO_SCALE_MEDIUM);
		gtk_box_pack_start (GTK_BOX (vbox), title, FALSE, TRUE, 0);
		GtkWidget *package_box = gtk_hbox_new (FALSE, 4);
		m_undo_package = gtk_label_new ("");
		gtk_misc_set_alignment (GTK_MISC (m_undo_package), 0, .5);
		GtkWidget *undo_button = gtk_button_new_from_stock (GTK_STOCK_UNDO);
		g_signal_connect (G_OBJECT (undo_button), "clicked",
		                  G_CALLBACK (undo_clicked_cb), this);
		gtk_box_pack_start (GTK_BOX (package_box), m_undo_package, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (package_box), undo_button, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), package_box, FALSE, TRUE, 6);
		m_undo_details = gtk_label_new ("");
		gtk_misc_set_alignment (GTK_MISC (m_undo_details), 0, .5);
		gtk_box_pack_start (GTK_BOX (vbox), m_undo_details, FALSE, TRUE, 0);
		GtkWidget *details_button = gtk_link_button_new (_("View details..."));
		GtkWidget *details_align = gtk_alignment_new (1, .5, 0, 0);
		gtk_container_add (GTK_CONTAINER (details_align), details_button);
		gtk_box_pack_start (GTK_BOX (vbox), details_align, FALSE, TRUE, 0);
	}

	~UndoBox()
	{
		m_pool.removeListener (this);
	}

	void startHack()  // call after init, after you did a show_all in the dialog
	{ gtk_widget_hide (m_undo_box); }

	void setPackage (Ypp::Package *package)
	{
		m_package = package;

		std::string action;
		if (package->toInstall()) {
			if (package->isInstalled())
				action = _("upgrade");
			else
				action = _("add");
		}
		else //if (package->toRemove())
			action = _("remove");

		gchar *package_str = g_strdup_printf ("%s %s", action.c_str(), package->name().c_str());
		gtk_label_set_text (GTK_LABEL (m_undo_package), package_str);
		g_free (package_str);

		const Ypp::PkgList packages (Ypp::get()->getPackages (Ypp::Package::PACKAGE_TYPE));
		int added = 0, removed = 0;
		for (int i = 0; i < packages.size(); i++) {
			if (packages.get (i)->toInstall())
				added++;
			if (packages.get (i)->toRemove())
				removed++;
		}

		std::string delta_str;
		Ypp::Disk *disk = Ypp::get()->getDisk();
		for (int i = 0; disk->getPartition (i); i++) {
			const Ypp::Disk::Partition *part = disk->getPartition (i);
			if (part->path == "/usr" || part->path == "/usr/" || part->path == "/")
				delta_str = part->delta_str;
		}

		gchar *added_str = 0, *removed_str = 0;
		if (added)
			added_str = g_strdup_printf (_("Added %d packages\n"), added);
		if (removed)
			removed_str = g_strdup_printf (_("Removed %d packages\n"), removed);

		gchar *details = g_strdup_printf (_("%s%sTotalling: %s"), added_str ? added_str : "", removed_str ? removed_str : "", delta_str.c_str());
		gtk_label_set_text (GTK_LABEL (m_undo_details), details);
		g_free (details);
		if (added_str)
			g_free (added_str);
		if (removed_str)
			g_free (removed_str);

		gtk_widget_show (m_undo_box);
	}

	virtual void entryInserted (const Ypp::PkgList list, int index, Ypp::Package *package)
	{ setPackage (package); }

	virtual void entryDeleted  (const Ypp::PkgList list, int index, Ypp::Package *package)
	{ gtk_widget_hide (m_undo_box); }

	virtual void entryChanged  (const Ypp::PkgList list, int index, Ypp::Package *package) {}

	static void undo_clicked_cb (GtkButton *button, UndoBox *pThis)
	{ pThis->m_package->undo(); }
};

class UI : public YGtkPackageView::Listener, _QueryListener, Ypp::Disk::Listener
{
GtkWidget *m_widget, *m_disk_label, *m_arrange_combo;
YGtkPackageView *m_all_view, *m_installed_view, *m_available_view, *m_upgrades_view;
std::list <_QueryWidget *> m_query;
DetailBox *m_details;
DiskView *m_disk;
StartupMenu *m_startup_menu;
Ypp::Package::Type m_type;
Toolbar *m_toolbar;
UndoBox *m_undo_box;

public:
	GtkWidget *getWidget() { return m_widget; }

	UI (YPackageSelector *sel)
	{
		m_toolbar = 0;
		m_arrange_combo = 0;

		if (sel->onlineUpdateMode())
			m_type = Ypp::Package::PATCH_TYPE;
		else if (!sel->searchMode())
			m_type = Ypp::Package::PATTERN_TYPE;
		else
			m_type = Ypp::Package::PACKAGE_TYPE;

		m_all_view = m_installed_view = m_available_view = m_upgrades_view = NULL;

		_QueryWidget *search_entry = 0;
		if (search_entry_top) {
			search_entry = new SearchEntry (false);
			m_query.push_back (search_entry);
			gtk_widget_set_size_request (search_entry->getWidget(), 160, -1);
		}

		UndoView *undo_view = 0;
		if (undo_side || undo_tab)
			undo_view = new UndoView();

		GtkWidget *packages_view;
		if (status_tabs) {
			m_installed_view = ygtk_package_view_new (TRUE);
			m_installed_view->setListener (this);
			m_available_view = ygtk_package_view_new (TRUE);
			m_available_view->setListener (this);
			m_upgrades_view = ygtk_package_view_new (TRUE);
			m_upgrades_view->setListener (this);

			packages_view = ygtk_notebook_new();
			const char **labels;
			if (status_tabs_as_actions) {
				const char *t[] = { _("_Install"), _("_Upgrade"), _("_Remove"), _("Undo") };
				labels = t;
			}
			else {
				const char *t[] = { _("_Available"), _("_Upgrades"), _("_Installed"), _("Summary") };
				labels = t;
			}
			gtk_notebook_append_page (GTK_NOTEBOOK (packages_view),
				GTK_WIDGET (m_available_view), gtk_label_new_with_mnemonic (labels[0]));
			gtk_notebook_append_page (GTK_NOTEBOOK (packages_view),
				GTK_WIDGET (m_upgrades_view), gtk_label_new_with_mnemonic (labels[1]));
			gtk_notebook_append_page (GTK_NOTEBOOK (packages_view),
				GTK_WIDGET (m_installed_view), gtk_label_new_with_mnemonic (labels[2]));

			if (undo_tab) {
				GtkWidget *box = gtk_event_box_new();
				gtk_container_add (GTK_CONTAINER (box), undo_view->createView (this, true));
				gtk_container_set_border_width (GTK_CONTAINER (box), 6);
				gtk_notebook_append_page (GTK_NOTEBOOK (packages_view),
					box, gtk_label_new_with_mnemonic (labels[3]));
			}

			if (search_entry) {
				// FIXME: only the entry itself is shown, without the "Find:" label
				ygtk_notebook_set_corner_widget (
					YGTK_NOTEBOOK (packages_view), search_entry->getWidget());
			}
		}
		else {
			m_all_view = ygtk_package_view_new (TRUE);
			m_all_view->setListener (this);
			gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (m_all_view), GTK_SHADOW_IN);
			packages_view = GTK_WIDGET (m_all_view);
		}

		GtkWidget *packages_box = gtk_vbox_new (FALSE, 4);
		if (arrange_by) {
			GtkWidget *arrange_box = gtk_hbox_new (FALSE, 4);
			gtk_box_pack_start (GTK_BOX (arrange_box), gtk_label_new (_("Software arranged by")), FALSE, TRUE, 0);
			m_arrange_combo = gtk_combo_box_new_text();
			gtk_combo_box_append_text (GTK_COMBO_BOX (m_arrange_combo), _("Groups"));
			gtk_combo_box_append_text (GTK_COMBO_BOX (m_arrange_combo), _("Repositories"));
			gtk_combo_box_append_text (GTK_COMBO_BOX (m_arrange_combo), _("Status"));
			gtk_combo_box_set_active (GTK_COMBO_BOX (m_arrange_combo), 0);
			gtk_box_pack_start (GTK_BOX (arrange_box), m_arrange_combo, FALSE, TRUE, 0);
			g_signal_connect (G_OBJECT (m_arrange_combo), "changed",
			                  G_CALLBACK (arrange_combo_changed_cb), this);

			gtk_box_pack_start (GTK_BOX (packages_box), arrange_box, FALSE, TRUE, 0);
		}

		{
			GtkWidget *header_box = gtk_hbox_new (FALSE, 6);
			GtkWidget *header = gtk_label_new_with_mnemonic (_("_Listing:"));
			gtk_misc_set_alignment (GTK_MISC (header), 0, .5);
			gtk_label_set_mnemonic_widget (GTK_LABEL (header), GTK_WIDGET (m_all_view));
			gtk_box_pack_start (GTK_BOX (header_box), header, TRUE, TRUE, 0);

			if (status_top) {
				PropertyCombo *combo = new PropertyCombo (new StatusModel());
				m_query.push_back (combo);
				gtk_box_pack_start (GTK_BOX (header_box), combo->getWidget(), FALSE, TRUE, 0);
			}
			if (categories_top) {
				PropertyCombo *combo = new PropertyCombo (new CategoryModel());
				m_query.push_back (combo);
				gtk_box_pack_start (GTK_BOX (header_box), combo->getWidget(), FALSE, TRUE, 0);
			}
			if (repositories_top) {
				PropertyCombo *combo = new PropertyCombo (new RepositoryModel());
				m_query.push_back (combo);
				gtk_box_pack_start (GTK_BOX (header_box), combo->getWidget(), FALSE, TRUE, 0);
			}
			if (search_entry && !status_tabs)
				gtk_box_pack_start (GTK_BOX (header_box), search_entry->getWidget(), FALSE, TRUE, 0);

			if (startup_menu) {
				GtkWidget *button = gtk_button_new_from_stock (GTK_STOCK_GO_BACK);
				g_signal_connect (G_OBJECT (button), "clicked",
				                  G_CALLBACK (back_clicked_cb), this);
				gtk_box_pack_start (GTK_BOX (header_box), button, FALSE, TRUE, 0);
			}

			gtk_box_pack_start (GTK_BOX (packages_box), header_box, FALSE, TRUE, 0);
		}

		gtk_box_pack_start (GTK_BOX (packages_box), packages_view, TRUE, TRUE, 0);

		if (!toolbar_top && toolbar_yast)
			gtk_box_pack_start (GTK_BOX (packages_box), (m_toolbar = new Toolbar (true))->getWidget(), FALSE, TRUE, 0);

		GtkWidget *packages_pane = gtk_hpaned_new();
		gtk_paned_pack1 (GTK_PANED (packages_pane), packages_box, TRUE, FALSE);
		ChangesPane *changes_pane = 0;
		GtkWidget *undo_widget;
		if (undo_side) {
			if (undo_old_style) {
				changes_pane = new ChangesPane();
				undo_widget = changes_pane->getWidget();
			}
			else
				undo_widget = undo_view->createView (this, false);
			gtk_paned_pack2 (GTK_PANED (packages_pane), undo_widget, FALSE, TRUE);
			gtk_paned_set_position (GTK_PANED (packages_pane), 350);
		}

		GtkWidget *view_pane = gtk_vpaned_new();
		gtk_paned_pack1 (GTK_PANED (view_pane), packages_pane, TRUE, FALSE);
		m_details = new DetailBox();
		gtk_paned_pack2 (GTK_PANED (view_pane), m_details->getWidget(), FALSE, TRUE);
		gtk_paned_set_position (GTK_PANED (view_pane), 500);

#if 0
		GtkWidget *combo = gtk_combo_box_new_text();
		if (sel->onlineUpdateMode()) {
			gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Patch"));
			gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);
		}
		else {
			gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Package"));
			gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Pattern"));
			gtk_combo_box_append_text (GTK_COMBO_BOX (combo), _("Language"));
			int i = sel->searchMode() ? 0 : 1;
			gtk_combo_box_set_active (GTK_COMBO_BOX (combo), i);
		}
		g_signal_connect (G_OBJECT (combo), "changed",
		                  G_CALLBACK (type_changed_cb), this);

		m_disk = new DiskView();
		m_disk_label = gtk_label_new ("");

		GtkWidget *type_hbox = gtk_hbox_new (FALSE, 6);
		gtk_box_pack_start (GTK_BOX (type_hbox), m_disk_label, FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (type_hbox), m_disk->getWidget(), FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (type_hbox), gtk_event_box_new(), TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (type_hbox), gtk_label_new (_("Type:")), FALSE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (type_hbox), combo, FALSE, TRUE, 0);

		GtkWidget *view_vbox = gtk_vbox_new (FALSE, 6);
		gtk_box_pack_start (GTK_BOX (view_vbox), view_pane, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX (view_vbox), type_hbox, FALSE, TRUE, 0);
#endif
		GtkWidget *side_vbox = gtk_vbox_new (FALSE, 12);
		if (search_entry_side) {
			_QueryWidget *search_entry = new SearchEntry (true);
			m_query.push_back (search_entry);
			gtk_box_pack_start (GTK_BOX (side_vbox), search_entry->getWidget(), FALSE, TRUE, 0);
		}

		_QueryWidget *category_view;
		GtkWidget *vpaned = 0, *vbox = 0;
		StackWidget *stack1 = 0, *stack2 = 0;
		if (expander_sidebar)
			vbox = gtk_expander_vbox_new (2);
		if (layered_sidebar) {
			stack1 = new StackWidget();
			stack2 = new StackWidget();
		}

		if (categories_side) {
			_QueryWidget *view = new PropertyView (new CategoryModel());
			category_view = view;
			m_query.push_back (view);
			if (expander_sidebar) {
				gtk_expander_set_expanded (GTK_EXPANDER (view->getWidget()), TRUE);
				gtk_box_pack_start (GTK_BOX (vbox), view->getWidget(), FALSE, TRUE, 0);
			}
			else if (grid_sidebar)
				;
			else if (stack1)
				stack1->append (_("Categories"), view->getWidget());
			else
				vpaned = gtk_vpaned_append (vpaned, view->getWidget());
		}
		if (repositories_side) {
			_QueryWidget *view = new PropertyView (new RepositoryModel());
			m_query.push_back (view);
			if (expander_sidebar)
				gtk_box_pack_start (GTK_BOX (vbox), view->getWidget(), FALSE, TRUE, 0);
			else if (stack1)
				stack1->append (_("Repositories"), view->getWidget());
			else
				vpaned = gtk_vpaned_append (vpaned, view->getWidget());
		}
		if (status_side) {
			_QueryWidget *view = new PropertyView (new StatusModel());
			m_query.push_back (view);
			if (expander_sidebar) {
				gtk_expander_set_expanded (GTK_EXPANDER (view->getWidget()), TRUE);
				gtk_box_pack_start (GTK_BOX (vbox), view->getWidget(), FALSE, TRUE, 0);
			}
			else if (stack2)
				stack2->append (_("Status"), view->getWidget());
			else
				vpaned = gtk_vpaned_append (vpaned, view->getWidget());
		}
		if (grid_sidebar && category_view && vpaned) {
			GtkWidget *hpaned = gtk_hpaned_new();
			gtk_paned_pack1 (GTK_PANED (hpaned), category_view->getWidget(), TRUE, FALSE);
			gtk_paned_pack2 (GTK_PANED (hpaned), vpaned, TRUE, FALSE);
			vpaned = hpaned;
		}

		if (vpaned)
			gtk_box_pack_start (GTK_BOX (side_vbox), vpaned, TRUE, TRUE, 0);
		if (vbox)
			gtk_box_pack_start (GTK_BOX (side_vbox), vbox, TRUE, TRUE, 0);
		if (layered_sidebar) {
			GtkWidget *vpaned = gtk_vpaned_new();
			gtk_paned_pack1 (GTK_PANED (vpaned), stack1->getWidget(), TRUE, FALSE);
			gtk_paned_pack2 (GTK_PANED (vpaned), stack2->getWidget(), TRUE, FALSE);
			gtk_box_pack_start (GTK_BOX (side_vbox), vpaned, TRUE, TRUE, 0);
		}

		m_undo_box = 0;
		if (undo_box) {
			m_undo_box = new UndoBox();
			gtk_box_pack_start (GTK_BOX (side_vbox), m_undo_box->getWidget(), FALSE, TRUE, 0);
		}

		GtkWidget *side_pane = gtk_hpaned_new();
		gtk_paned_pack1 (GTK_PANED (side_pane), side_vbox, FALSE, TRUE);
		gtk_paned_pack2 (GTK_PANED (side_pane), view_pane, TRUE, FALSE);
		gtk_paned_set_position (GTK_PANED (side_pane), grid_sidebar ? 250 : 150);

		vbox = gtk_vbox_new (FALSE, 6);
		if (toolbar_top) {
			m_toolbar = new Toolbar (false);
			gtk_box_pack_start (GTK_BOX (vbox), m_toolbar->getWidget(), FALSE, TRUE, 0);
		}
		gtk_box_pack_start (GTK_BOX (vbox), side_pane, TRUE, TRUE, 0);
		m_widget = vbox;
		gtk_widget_show_all (m_widget);

		if (details_start_hide)
			gtk_widget_hide (m_details->getWidget());
		else
			m_details->setPackage (NULL);
		if (!categories_side && !search_entry_side && !status_side)
			gtk_widget_hide (side_vbox);
		if (changes_pane)
			changes_pane->startHack();
		if (m_undo_box)
			m_undo_box->startHack();

		if (startup_menu) {
			m_startup_menu = new StartupMenu (m_widget);
			m_query.push_back (m_startup_menu);
			m_widget = m_startup_menu->getWidget();
		}

		for (std::list <_QueryWidget *>::const_iterator it = m_query.begin();
		     it != m_query.end(); it++) {
			(*it)->updateType (m_type);
			(*it)->setListener (this);
		}
		refresh();
		updateDisk();
		Ypp::get()->getDisk()->addListener (this);
	}

	~UI()
	{
		for (std::list <_QueryWidget *>::const_iterator it = m_query.begin();
		     it != m_query.end(); it++)
			delete *it;
		delete m_disk;
		delete m_startup_menu;
		delete m_undo_box;
	}

private:
	void updateWidget (_QueryWidget *widget)
	{
		Ypp::QueryAnd *query = new Ypp::QueryAnd();
		for (std::list <_QueryWidget *>::const_iterator it = m_query.begin();
		     it != m_query.end(); it++) {
			if (*it == widget)
				continue;
			(*it)->writeQuery (query);
		}

		Ypp::PkgQuery list (m_type, query);
		widget->updateList (list);
	}

	virtual void refresh()
	{
		if (m_all_view)
			m_all_view->clear();
		if (m_installed_view)
			m_installed_view->clear();
		if (m_available_view)
			m_available_view->clear();
		if (m_upgrades_view)
			m_upgrades_view->clear();

		// for arrange_by
		const char *header = 0;
		Ypp::Node *category;

		for (int cycle = 0; true; cycle++) {  // for arrange_by
			Ypp::QueryAnd *query = new Ypp::QueryAnd();
			for (std::list <_QueryWidget *>::const_iterator it = m_query.begin();
				 it != m_query.end(); it++)
				(*it)->modified = (*it)->writeQuery (query);

			if (arrange_by) {
				int arrange = gtk_combo_box_get_active (GTK_COMBO_BOX (m_arrange_combo));
				if (arrange == 0) {  // groups
					if (cycle == 0)
						category = Ypp::get()->getFirstCategory2 (Ypp::Package::PACKAGE_TYPE);
					else
						category = category->next();
					if (!category)
						break;
					header = category->name.c_str();
					query->add (new Ypp::QueryCategory (category, true));
				}
				else if (arrange == 1) {  // repositories
					const Ypp::Repository *repo = Ypp::get()->getRepository (cycle);
					if (repo) {
						header = repo->name.c_str();
						query->add (new Ypp::QueryRepository (repo));
					}
					else
						break;
				}
				else if (arrange == 2) {  // statuses
					if (cycle == 0) {
						header = _("Available");
						query->add (new Ypp::QueryProperty ("is-installed", false));
					}
					else if (cycle == 1) {
						header = _("Upgrades");
						query->add (new Ypp::QueryProperty ("has-upgrade", true));
					}
					else if (cycle == 2) {
						header = _("Installed");
						query->add (new Ypp::QueryProperty ("is-installed", true));
					}
					else if (cycle == 3) {
						header = _("Modified");
						query->add (new Ypp::QueryProperty ("to-modify", true));
					}
					else
						break;
				}
			}

			Ypp::PkgQuery list (m_type, query);
			if (m_all_view)
//				m_all_view->setList (list, NULL);
				m_all_view->packList (header, list, NULL);

			if (m_installed_view) {
				Ypp::QueryAnd *query = new Ypp::QueryAnd();
				query->add (new Ypp::QueryProperty ("is-installed", true));
				Ypp::PkgQuery _list (list, query);
				m_installed_view->packList (header, _list, NULL);
			}
			if (m_available_view) {
				Ypp::QueryAnd *query = new Ypp::QueryAnd();
				query->add (new Ypp::QueryProperty ("is-installed", false));
				Ypp::PkgQuery _list (list, query);
				m_available_view->packList (header, _list, NULL);
			}
			if (m_upgrades_view) {
				Ypp::QueryAnd *query = new Ypp::QueryAnd();
				query->add (new Ypp::QueryProperty ("has-upgrade", true));
				Ypp::PkgQuery _list (list, query);
				m_upgrades_view->packList (header, _list, _("Upgrade all"));
			}

			for (std::list <_QueryWidget *>::const_iterator it = m_query.begin();
				 it != m_query.end(); it++)
				if ((*it)->begsUpdate()) {
					if ((*it)->modified)
						updateWidget (*it);
					else
						(*it)->updateList (list);
				}

			if (!arrange_by)
				break;
		}
	}

	virtual void packagesSelected (Ypp::PkgList packages)
	{
		m_details->setPackage (packages.size() ? packages.get (0) : NULL);
		if (m_toolbar)
			m_toolbar->setPackages (packages);
	}

	virtual void updateDisk()
	{
#if 0
		Ypp::Disk *disk = Ypp::get()->getDisk();
		const Ypp::Disk::Partition *part = 0;
		for (int i = 0; disk->getPartition (i); i++) {
			const Ypp::Disk::Partition *p = disk->getPartition (i);
			if (p->path == "/usr" || p->path == "/usr/") {
				part = p;
				break;
			}
			if (p->path == "/")
				part = p;
		}
		if (part) {
			gchar *text;
			if (part->delta)
				text = g_strdup_printf ("Free disk space: %s (%s%s)", part->free_str.c_str(),
					part->delta > 0 ? "+" : "", part->delta_str.c_str());
			else
				text = g_strdup_printf ("Free disk space: %s", part->free_str.c_str());
			gtk_label_set_text (GTK_LABEL (m_disk_label), text);
			g_free (text);
		}
		else
			gtk_label_set_text (GTK_LABEL (m_disk_label), "");
#endif
	}

	static void type_changed_cb (GtkComboBox *combo, UI *pThis)
	{
		pThis->m_type = (Ypp::Package::Type) gtk_combo_box_get_active (combo);
		for (std::list <_QueryWidget *>::const_iterator it = pThis->m_query.begin();
		     it != pThis->m_query.end(); it++)
			(*it)->updateType (pThis->m_type);
		pThis->refresh();
	}

	static void arrange_combo_changed_cb (GtkComboBox *combo, UI *pThis)
	{
		pThis->refresh();
	}

	static void back_clicked_cb (GtkButton *button, UI *pThis)
	{
		pThis->m_startup_menu->restore();
	}
};

#include "pkg-selector-help.h"

class YGPackageSelector : public YPackageSelector, public YGWidget, public Ypp::Interface
{
ToolsBox *m_tools;
GtkWidget *m_progressbar;
guint m_button_timeout_id;

public:
	YGPackageSelector (YWidget *parent, long mode)
	: YPackageSelector (NULL, mode)
	, YGWidget (this, parent, YGTK_TYPE_WIZARD, NULL)
	{
		setBorder (0);
		bool onlineUpdate = onlineUpdateMode();
		YGDialog *dialog = YGDialog::currentDialog();
		dialog->setCloseCallback (confirm_cb, this);
		int width, height;
		YGUI::ui()->pkgSelectorSize (&width, &height);
		dialog->setMinSize (width, height);

		YGtkWizard *wizard = YGTK_WIZARD (getWidget());
		ygtk_wizard_set_header_icon (wizard,
			THEMEDIR "/icons/22x22/apps/yast-software.png");
		const char *title = onlineUpdate ? _("Online Update") : _("Software Manager");
		ygtk_wizard_set_header_text (wizard, title);
		ygtk_wizard_set_help_text (wizard, _("Please wait..."));

		ygtk_wizard_set_button_label (wizard,  wizard->abort_button,
		                              _("_Cancel"), GTK_STOCK_CANCEL);
		ygtk_wizard_set_button_str_id (wizard, wizard->abort_button, "cancel");
		ygtk_wizard_set_button_label (wizard,  wizard->back_button, NULL, NULL);
		ygtk_wizard_set_button_label (wizard,  wizard->next_button,
		                              _("A_pply"), GTK_STOCK_APPLY);
		ygtk_wizard_set_button_str_id (wizard, wizard->next_button, "accept");
		g_signal_connect (G_OBJECT (wizard), "action-triggered",
		                  G_CALLBACK (wizard_action_cb), this);
		ygtk_wizard_enable_button (wizard, wizard->next_button, FALSE);

#if 0
		m_tools = new ToolsBox();
		ygtk_wizard_set_extra_button (wizard, m_tools->getWidget());
#endif

#if 0
		GtkWidget *menu_bar = gtk_menu_bar_new(), *item, *submenu, *subitem;
		item = gtk_menu_item_new_with_label (_("File"));
			submenu = gtk_menu_new();
			gtk_menu_item_set_submenu (GTK_MENU_ITEM (item), submenu);
			subitem = gtk_menu_item_new_with_label (_("Import..."));
			gtk_menu_shell_append (GTK_MENU_SHELL (submenu), subitem);
			g_signal_connect (G_OBJECT (subitem), "activate",
				              G_CALLBACK (import_file_cb), this);
			subitem = gtk_menu_item_new_with_label (_("Export..."));
			gtk_menu_shell_append (GTK_MENU_SHELL (submenu), subitem);
			g_signal_connect (G_OBJECT (subitem), "activate",
				              G_CALLBACK (export_file_cb), this);
			subitem = gtk_separator_menu_item_new();
			gtk_menu_shell_append (GTK_MENU_SHELL (submenu), subitem);
			subitem = gtk_image_menu_item_new_from_stock (GTK_STOCK_SAVE, NULL);
			gtk_menu_shell_append (GTK_MENU_SHELL (submenu), subitem);
			subitem = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT, NULL);
			gtk_menu_shell_append (GTK_MENU_SHELL (submenu), subitem);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu_bar), item);
		item = gtk_menu_item_new_with_label (_("View"));
		gtk_menu_shell_append (GTK_MENU_SHELL (menu_bar), item);
		item = gtk_menu_item_new_with_label (_("Tools"));
			submenu = gtk_menu_new();
			gtk_menu_item_set_submenu (GTK_MENU_ITEM (item), submenu);
			subitem = gtk_menu_item_new_with_label (_("Generate Dependency Testcase..."));
			gtk_menu_shell_append (GTK_MENU_SHELL (submenu), subitem);
			g_signal_connect (G_OBJECT (subitem), "activate",
				              G_CALLBACK (create_solver_testcase_cb), this);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu_bar), item);
		gtk_widget_show_all (menu_bar);
		ygtk_wizard_set_custom_menu (wizard, menu_bar);
#endif

		m_progressbar = gtk_progress_bar_new();
		GtkWidget *empty = gtk_event_box_new();
		gtk_widget_set_size_request (empty, -1, 36);
		gtk_widget_show (empty);

		GtkWidget *vbox = gtk_vbox_new (FALSE, 6);
		gtk_box_pack_end (GTK_BOX (vbox), empty, FALSE, TRUE, 0);
		gtk_box_pack_end (GTK_BOX (vbox), m_progressbar, FALSE, TRUE, 0);
		gtk_widget_show (vbox);
		ygtk_wizard_set_child (YGTK_WIZARD (wizard), vbox);
		Ypp::get()->setInterface (this);

/*		m_notebook = new QueryNotebook (onlineUpdate, repoMgrEnabled());
		gtk_box_pack_start (GTK_BOX (vbox), m_notebook->getWidget(), TRUE, TRUE, 0);
*/
		UI *ui = new UI (this);
		gtk_box_pack_start (GTK_BOX (vbox), ui->getWidget(), TRUE, TRUE, 0);
		gtk_widget_hide (empty);

		const char **help = onlineUpdate ? patch_help : pkg_help;
		std::string str;
		str.reserve (6144);
		for (int i = 0; help[i]; i++)
			str.append (help [i]);
		ygtk_wizard_set_help_text (wizard, str.c_str());
		dialog->setTitle (title);
		m_button_timeout_id = g_timeout_add (7500, wizard_enable_button_cb, this);
	}

	virtual ~YGPackageSelector()
	{
		if (m_button_timeout_id)
			g_source_remove (m_button_timeout_id);
//		delete m_notebook;
#if 0
		delete m_tools;
#endif
		Ypp::finish();
	}

	// (Y)Gtk callbacks
	static gboolean wizard_enable_button_cb (gpointer data)
	{
		YGPackageSelector *pThis = (YGPackageSelector *) data;
		pThis->m_button_timeout_id = 0;
		YGtkWizard *wizard = YGTK_WIZARD (pThis->getWidget());
		ygtk_wizard_enable_button (wizard, wizard->next_button, TRUE);
		return FALSE;
	}

    static bool confirm_cb (void *pThis)
    { return confirmCancel(); }

	static void wizard_action_cb (YGtkWizard *wizard, gpointer id,
	                              gint id_type, YGPackageSelector *pThis)
	{
		const gchar *action = (gchar *) id;
		yuiMilestone() << "Closing PackageSelector with '" << action << "'\n";
		if (!strcmp (action, "accept")) {
			if (!pThis->onlineUpdateMode()) {
				if (pThis->confirmUnsupported())
					if (!askConfirmUnsupported())
						return;
			}
			if (confirmApply())
				YGUI::ui()->sendEvent (new YMenuEvent ("accept"));
		}
		else if (!strcmp (action, "cancel"))
			if (confirmCancel())
				YGUI::ui()->sendEvent (new YCancelEvent());
	}

	// Ypp callbacks
	virtual bool acceptLicense (Ypp::Package *package, const std::string &license)
	{
		return acceptText (package, _("License Agreement"),
			_("Do you accept the terms of this license?"), license, true);
	}

	virtual void notifyMessage (Ypp::Package *package, const std::string &msg)
	{ acceptText (package, _("Notification"), "", msg, false); }

	virtual bool resolveProblems (const std::list <Ypp::Problem *> &problems)
	{ return ::resolveProblems (problems); }

	virtual bool allowRestrictedRepo (const Ypp::PkgList list)
	{
		return confirmPkgs (_("Dependencies from Filtered Repositories"),
			_("The following packages have necessary dependencies that are not provided "
			  "by the filtered repository. Install them?"), GTK_MESSAGE_WARNING, list,
			  "repository");
	}

	virtual void loading (float progress)
	{
		if (progress == 1) {
			if (GTK_WIDGET_VISIBLE (m_progressbar)) {
				gtk_widget_hide (m_progressbar);
				while (g_main_context_iteration (NULL, FALSE)) ;
				//gtk_widget_set_sensitive (getWidget(), TRUE);
			}
			YGUI::ui()->normalCursor();
		}
		else {
			if (progress == 0)
				YGUI::ui()->busyCursor();
			else {  // progress=0 may be to trigger cursor only
				//gtk_widget_set_sensitive (getWidget(), FALSE);
				gtk_widget_show (m_progressbar);
				gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (m_progressbar), progress);
			}
			while (g_main_context_iteration (NULL, FALSE)) ;
		}
	}

	YGWIDGET_IMPL_COMMON (YPackageSelector)
};

#include "pkg/YGPackageSelectorPluginImpl.h"

YPackageSelector *
YGPackageSelectorPluginImpl::createPackageSelector (YWidget *parent, long modeFlags)
{
	modeFlags |= YPkg_SearchMode;
	return new YGPackageSelector (parent, modeFlags);
}

YWidget *
YGPackageSelectorPluginImpl::createPatternSelector (YWidget *parent, long modeFlags)
{
	modeFlags &= YPkg_SearchMode;
	return new YGPackageSelector (parent, modeFlags);
}

YWidget *
YGPackageSelectorPluginImpl::createSimplePatchSelector (YWidget *parent, long modeFlags)
{
	modeFlags |= YPkg_OnlineUpdateMode;
	return new YGPackageSelector (parent, modeFlags);
}

