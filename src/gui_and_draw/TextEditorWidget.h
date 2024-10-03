#if !defined(TEXTEDIT_WIDGET_INCLUDED_)
#define TEXTEDIT_WIDGET_INCLUDED_

#include <vector>
#include <cstdio>
#include <cstdlib>
#include <FL/Fl_Text_Editor.H>

class VspTextEditor : public Fl_Text_Editor
{
public:
    VspTextEditor( int X, int Y, int W, int H,  const char* l );

    typedef int (*Key_Func)(int key, Fl_Text_Editor* editor);

    static int kf_accept(int, Fl_Text_Editor*);
};

#endif // TEXTEDIT_WIDGET_INCLUDED_