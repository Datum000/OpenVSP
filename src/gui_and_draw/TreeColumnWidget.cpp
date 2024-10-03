#include "TreeColumnWidget.h"

TreeRowItem::TreeRowItem(Fl_Tree *tree, const char *text) : Fl_Tree_Item(tree) {
this->label(text);
m_LeftMargin = 5;
}

int TreeRowItem::draw_item_content(int render) {
  TreeWithColumns *treewc = (TreeWithColumns*)tree();
  Fl_Color fg = drawfgcolor();
  Fl_Color bg = drawbgcolor();
  const int *col_widths = treewc->column_widths();
  //    Show the date and time as two small strings
  //    one on top of the other in a single item.
  //
  // Our item's label dimensions
  int X = label_x(), Y = label_y(),
      W = label_w(), H = label_h(),
      RX = treewc->x() - treewc->hposition() + treewc->first_column_minw(),  // start first column at a fixed position
      RY = Y+H-fl_descent();  // text draws here
  // Render background
  if ( render ) {
    if ( is_selected() ) { fl_draw_box(prefs().selectbox(),X,Y,W,H,bg); }
    else                 { fl_color(bg); fl_rectf(X,Y,W,H); }
    fl_font(labelfont(), labelsize());
  }
  // Render columns
  if ( render ) fl_push_clip(X,Y,W,H);
  {
    // Draw each column
    //   ..or if not rendering, at least calculate width of row so we can return it.
    //
    int t=0;
    const char *s = label();
    char delim_str[2] = { treewc->column_char(), 0 };  // strcspn() wants a char[]
    while ( *s ) {
      int n = strcspn(s, delim_str);         // find index to next delimiter char in 's' (or eos if none)
      if ( n>0 && render ) {                 // renderable string with at least 1 or more chars?
        int XX = ( t==0 ) ? X : RX;          // TBD: Rename XX to something more meaningful
        // Don't clip last column.
        //    See if there's more columns after this one; if so, clip the column.
        //    If not, let column run to edge of widget
        //
        int CW = col_widths[t];              // clip width based on column width
        // If first column, clip to 2nd column's left edge
        if ( t==0 ) { CW = (RX+col_widths[0])-XX; }
        // If last column, clip to right edge of widget
        if ( *(s+n) == 0 ) { CW = (x()+w()-XX); }
        // Draw the text
        //    We want first field (PID) indented, rest of fields fixed column.
        //
        fl_color(fg);
        fl_push_clip(XX, Y, CW, H);          // prevent text from running into next column
        fl_draw(s, n, XX+m_LeftMargin, RY);
        fl_pop_clip();                       // clip off
        // Draw vertical lines for all columns except first
        if ( t>0 && treewc->column_separators() ) {
          fl_color(FL_BLACK);
          fl_line(RX,Y,RX,Y+H);
        }
      }
      if ( *(s+n) == treewc->column_char() ) {
        s += n+1;               // skip field + delim
        RX += col_widths[t++];  // keep track of fixed column widths for all except right column
        continue;
      } else {
        // Last field? Return entire length of unclipped field
        RX += fl_width(s) + m_LeftMargin;
        s += n;
      }
    }
  }
  if ( render ) fl_pop_clip();
  return RX;                    // return right most edge of what we've rendered
}

TreeWithColumns::TreeWithColumns( int X, int Y, int W, int H, const char *L ) : Fl_Tree( X, Y, W, H, L )
{
    colseps_flag     = true;
    resizing_flag    = true;
    col_char         = '\t';
    col_widths       = 0;
    first_col_minw   = 80;
    drag_col         = -1;
    last_cursor      = FL_CURSOR_DEFAULT;
    // We need the default tree icons on all platforms.
    //    For some reason someone made Fl_Tree have different icons on the Mac,
    //    which doesn't look good for this application, so we force the icons
    //    to be consistent with the '+' and '-' icons and dotted connection lines.
    //
    connectorstyle(FL_TREE_CONNECTOR_DOTTED);
    openicon(new Fl_Pixmap(tree_open_xpm));
    closeicon(new Fl_Pixmap(tree_close_xpm));
}

int TreeWithColumns::column_near_mouse()
{
    // Event not inside browser area? (eg. scrollbar) Early exit
    if ( !Fl::event_inside(_tix,_tiy,_tiw,_tih) ) return(-1);
    int mousex = Fl::event_x() + hposition();
    int colx = x() + first_col_minw;
    for ( int t=0; col_widths[t]; t++ ) {
    colx += col_widths[t];
    int diff = mousex - colx;
    // Mouse near column? Return column #
    if ( diff >= -4 && diff <= 4 ) return(t);
    }
    return(-1);
    // Change the mouse cursor
    //     Does nothing if cursor already set to same value.
}

void TreeWithColumns::change_cursor( Fl_Cursor newcursor )
{
    if ( newcursor == last_cursor ) return;
    window()->cursor(newcursor);
    last_cursor = newcursor;
}

TreeRowItem* TreeWithColumns::AddRow( const char *s, TreeRowItem *parent_item )
{
    TreeRowItem *item = new TreeRowItem(this, s);  // create new item
    if ( !parent_item ) // wants root item as parent?
    {
      if ( !root() || ( strcmp( root()->label(), "ROOT" ) == 0 ) ) // default root item or no root
      {
          this->root(item);                          // make this item the new root
          // Special colors for root item -- this is the "header"
          item->labelfgcolor(0xffffff00);
          item->labelbgcolor(0x8888ff00);
          return item;
      }
      else
      {
          parent_item = (TreeRowItem*)root();        // use root as parent
      }
    }
    parent_item->add(prefs(), "", item);           // add item to hierarchy
    return item;                                   // return the new item
}

int TreeWithColumns::handle( int e )
{
    if ( !resizing_flag ) return Fl_Tree::handle(e);    // resizing off? early exit
    // Handle column resizing
    int ret = 0;
    switch ( e ) {
    case FL_ENTER:
        ret = 1;
        break;
    case FL_MOVE:
        change_cursor( (column_near_mouse() >= 0) ? FL_CURSOR_WE : FL_CURSOR_DEFAULT);
        ret = 1;
        break;
    case FL_PUSH: {
        int whichcol = column_near_mouse();
        if ( whichcol >= 0 ) {
        // Clicked on resizer? Start dragging that column
        drag_col = whichcol;
        change_cursor(FL_CURSOR_DEFAULT);
        return 1;   // eclipse event from Fl_Tree's handle()
        }
        break;
    }
    case FL_DRAG:
        if ( drag_col != -1 ) {
        // Sum up column widths to determine position
        int mousex = Fl::event_x() + hposition();
        int newwidth = mousex - (x() + first_column_minw());
        for ( int t=0; col_widths[t] && t<drag_col; t++ )
            { newwidth -= col_widths[t]; }
        // Apply new width, redraw interface
        col_widths[drag_col] = newwidth;
        if ( col_widths[drag_col] < 2 ) col_widths[drag_col] = 2;  // XXX: 2 should be a class member
        recalc_tree();
        redraw();
        return 1;        // eclipse event from Fl_Tree's handle()
        }
        break;
    case FL_LEAVE:
    case FL_RELEASE:
        change_cursor(FL_CURSOR_DEFAULT);          // ensure normal cursor
        if ( drag_col != -1 && e == FL_RELEASE ) { // release during drag mode?
        drag_col = -1;   // disable drag mode
        return 1;        // eclipse event from base class; we handled it
        }
        drag_col = -1;
        ret = 1;
        break;
    }
    return(Fl_Tree::handle(e) ? 1 : ret);
}
