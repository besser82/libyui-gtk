/********************************************************************
 *           YaST2-GTK - http://en.opensuse.org/YaST2-GTK           *
 ********************************************************************/

#include <config.h>
#include <YGUI.h>
#include "YGUtils.h"
#include "YGWidget.h"

#include "YLabel.h"

class YGLabel : public YLabel, public YGWidget
{
public:
	YGLabel (YWidget *parent, const string &text, bool heading, bool outputField)
	: YLabel (NULL, text, heading, outputField),
	  YGWidget (this, parent, true, GTK_TYPE_LABEL, NULL)
	{
		IMPL
		gtk_misc_set_alignment (GTK_MISC (getWidget()), 0.0, 0.5);
		if (outputField) {
			gtk_label_set_selectable (GTK_LABEL (getWidget()), TRUE);
			gtk_label_set_single_line_mode (GTK_LABEL (getWidget()), TRUE);
			YGUtils::setWidgetFont (getWidget(), PANGO_STYLE_ITALIC, PANGO_WEIGHT_NORMAL,
			                        PANGO_SCALE_MEDIUM);
		}
		if (heading)
			YGUtils::setWidgetFont (getWidget(), PANGO_STYLE_NORMAL, PANGO_WEIGHT_BOLD,
			                        PANGO_SCALE_LARGE);
		setLabel (text);
	}

	virtual void setText (const string &label)
	{
		YLabel::setText (label);
		gtk_label_set_label (GTK_LABEL (getWidget()), label.c_str());
		std::string::size_type i = label.find ('\n', 0);
		if (isOutputField()) {  // must not have a breakline
			if (i != std::string::npos) {
				std::string l (label, 0, i);
				gtk_label_set_label (GTK_LABEL (getWidget()), l.c_str());
			}
		}
		else
			gtk_label_set_selectable (GTK_LABEL (getWidget()), i != std::string::npos);
	}

	YGWIDGET_IMPL_COMMON
	YGWIDGET_IMPL_USE_BOLD (YLabel)
};

YLabel *YGWidgetFactory::createLabel (YWidget *parent, const string &text, bool heading,
                                      bool outputField)
{
	return new YGLabel (parent, text, heading, outputField);
}

