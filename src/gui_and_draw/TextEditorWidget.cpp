#include "TextEditorWidget.h"
#include <iostream>

VspTextEditor::VspTextEditor(int X, int Y, int W, int H,  const char* l) : Fl_Text_Editor( X, Y, W, H, l)
{

}

static void kill_selection(Fl_Text_Editor* e) {
  if (e->buffer()->selected()) {
    e->insert_position(e->buffer()->primary_selection()->start());
    e->buffer()->remove_selection();
  }
}

int VspTextEditor::kf_accept(int, Fl_Text_Editor* e)
{
    kill_selection(e);
    e->show_insert_position();
    e->set_changed();
    e->do_callback(FL_REASON_CHANGED);
    return 1;
}