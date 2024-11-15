#if !defined(TREECOL_WIDGET_INCLUDED_)
#define TREECOL_WIDGET_INCLUDED_

#include <stdio.h>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Tree.H>

//==== Tree With Columns ====//
const char * const tree_open_xpm[] = {
  "11 11 3 1",
  ".    c #fefefe",
  "#    c #444444",
  "@    c #000000",
  "###########",
  "#.........#",
  "#.........#",
  "#....@....#",
  "#....@....#",
  "#..@@@@@..#",
  "#....@....#",
  "#....@....#",
  "#.........#",
  "#.........#",
  "###########"
};

const char * const tree_close_xpm[] = {
  "11 11 3 1",
  ".    c #fefefe",
  "#    c #444444",
  "@    c #000000",
  "###########",
  "#.........#",
  "#.........#",
  "#.........#",
  "#.........#",
  "#..@@@@@..#",
  "#.........#",
  "#.........#",
  "#.........#",
  "#.........#",
  "###########"
};

// DERIVE CUSTOM CLASS FROM Fl_Tree_Item TO SHOW DATA IN COLUMNS
class TreeRowItem : public Fl_Tree_Item
{
public:
  TreeRowItem(Fl_Tree *tree, const char *text);
  int draw_item_content(int render);
  void SetLeftMargin( const int & left_margin )
  {
    m_LeftMargin = left_margin;
  }
  int GetLeftMargin()
  {
    return m_LeftMargin;
  }
protected:
  int m_LeftMargin;
};

// Small convenience class to handle adding columns.
//    TreeRowItem does most of the work.
//
class TreeWithColumns : public Fl_Tree
{
  protected:
    int column_near_mouse();
    void change_cursor(Fl_Cursor newcursor);

  public:
    TreeWithColumns(int X,int Y,int W,int H,const char *L=0);

    bool colseps_flag;     // enable/disable column separator lines
    bool resizing_flag;    // enable/disable interactive resizing
    char col_char;         // column delimiter character
    int *col_widths;       // array of column widths (supplied by application)
    int first_col_minw;    // minimum width of first column
    int drag_col;          // internal: column being FL_DRAG'ed
    Fl_Cursor last_cursor; // internal: last mouse cursor value

    int  first_column_minw()          { return first_col_minw; }
    void first_column_minw(int val)   { first_col_minw = val; }
    // Enable/disable the vertical column lines
    void column_separators(bool val)  { colseps_flag = val; }
    bool column_separators() const    { return colseps_flag; }
    // Enable/disable the vertical column lines
    void resizing(bool val)           { resizing_flag = val; }
    bool resizing() const             { return resizing_flag; }
    // Change the column delimiter character
    void column_char(char val)        { this->col_char = val; }
    char column_char() const          { return col_char; }
    // Set the column array.
    //     Make sure the last entry is zero.
    //     User allocated array must remain allocated for lifetime of class instance.
    //     Must be large enough for all columns in data!
    //
    void column_widths(int *val)      { this->col_widths = val; }
    int *column_widths() const        { return col_widths; }
    TreeRowItem *AddRow(const char *s, TreeRowItem *parent_item = nullptr );

    // Manage column resizing
    int handle( int e );

    // Hide these base class methods from the API; we don't want app using them,
    // as we expect all items in the tree to be TreeRowItems, not Fl_Tree_Items.
  private:
    using Fl_Tree::add;
};

// Handle custom drawing of the item
//
//    All we're responsible for is drawing the 'label' area of the item
//    and it's background. Fl_Tree gives us a hint as to what the
//    foreground and background colors should be via the fg/bg parameters,
//    and whether we're supposed to render anything or not.
//
//    The only other thing we must do is return the maximum X position
//    of scrollable content, i.e. the right most X position of content
//    that we want the user to be able to use the horizontal scrollbar
//    to reach.
//


#endif // TREECOL_WIDGET_INCLUDED_
