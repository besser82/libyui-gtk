//                       YaST2-GTK                                //
// YaST webpage - http://developer.novell.com/wiki/index.php/YaST //

#include <YGUI.h>
#include <YUIComponent.h>

class YGUIComponent : public YUIComponent
{
public:

	YGUIComponent() : YUIComponent() {}

	virtual string name() const { return "gtk"; }

	virtual YUI *createUI (int argc, char **argv,
			       bool with_threads, const char * macro_file)
	{
#ifdef IMPL_DEBUG
		fprintf (stderr, "Create a gtk+ UI: %d '%s' !\n", with_threads, macro_file);
#endif
		return new YGUI (argc, argv, with_threads, macro_file);
	}
};

class Y2CCGtk : public Y2ComponentCreator
{
public:
	Y2CCGtk () : Y2ComponentCreator (Y2ComponentBroker::BUILTIN) { };

	bool isServerCreator () const { return true; };
	
	Y2Component *create (const char * name) const
	{
		if (!strcmp (name, "gtk") )
			return new YGUIComponent ();
		else
			return 0;
	}
};

// Singleton plugin registration instance.
Y2CCGtk g_y2ccgtk;


