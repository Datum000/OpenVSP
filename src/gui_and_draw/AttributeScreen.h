//
// This file is released under the terms of the NASA Open Source Agreement (NOSA)
// version 1.3 as detailed in the LICENSE file which accompanies this software.
//

// AttributeScreen.h: GUI Stuff for working with attributes
// Tim Cuatt
//
//////////////////////////////////////////////////////////////////////

#if !defined(VSP_ATTRSCREEN_INCLUDED_)
#define VSP_ATTRSCREEN_INCLUDED_

#include "ScreenBase.h"
#include "GuiDevice.h"
#include "AttributeEditor.h"
#include "ScreenMgr.h"
#include "ResultsMgr.h"
#include "TextEditorWidget.h"

#include <FL/Fl.H>

using std::string;
using std::vector;

class AttributeExplorer: public BasicScreen
{
public:
    AttributeExplorer( ScreenMgr* mgr );
    virtual ~AttributeExplorer();

    bool Update( );

    void SetAttrData( );
    void AddEmptyCollID( vector < string > coll_ids );
    void GetEmptyColls();

    void UpdateAttrFields( GroupLayout* group );

    void SetTreeAutoSelectID( const string & id );
    void AttributeAdd();
    void AttributeModify( GuiDevice* gui_device, Fl_Widget* w );
    void ResizeColVec3d( vector < vec3d > *v3dPtr, const int row_delta = 0, const vec3d &new_vec = {0., 0., 0.} );
    void ResizeIntMat( vector < vector < int > > *matPtr, const pair < int, int > row_col_delta = { 0, 0 }, const int &new_val = 0 );
    void ResizeDoubleMat( vector < vector < double > > *matPtr, const pair < int, int > row_col_delta = { 0, 0 }, const double &new_val = 0. );

    pair < int, int > GetIntMatSize( vector < vector < int > > *matPtr );
    pair < int, int > GetDoubleMatSize( vector < vector < double > > *matPtr );

    virtual void CallBack( Fl_Widget *w );

    void GuiDeviceCallBack( GuiDevice* gui_device );
    static void staticScreenCB( Fl_Widget *w, void* data )
    {
        ( ( AttributeExplorer* )data )->CallBack( w );
    }

    void AttrTypeDispGroup( int attr_type, GroupLayout * group );

protected:

    Choice m_AttrTypeSearchChoice;
    Choice m_ObjTypeSearchChoice;
    StringInput m_AttrSearchIn;
    BoolParm* m_CaseSensParmPtr;
    ToggleButton m_CaseSensitiveButton;
    TriggerButton m_CloseTrigger;
    TriggerButton m_OpenTrigger;

    AttributeTree m_AttrTreeWidget;

    ScreenMgr* m_ScreenMgr;

    GroupLayout m_AttrDetailLayout;

    TriggerButton m_AttrAddTrigger;
    TriggerButton m_AttrDelTrigger;
    Choice m_AttrTypeChoice;

    StringInput m_AttrNameIn;
    StringInput m_AttrDescIn;

    SpreadColumnVec3d *m_Vec3dSpreadSingle;
    SpreadSheetInt *m_IntMatrixSpreadSheet;
    SpreadSheetDouble *m_DoubleMatrixSpreadSheet;

    Fl_Box* m_DataLabel;

    GroupLayout m_TreeGroupLayout;
    GroupLayout m_TreeWidgetLayout;
    GroupLayout m_CommonEntryLayout;
    GroupLayout m_ResizableLayout;

    GroupLayout m_EmptyEntryLayout;
    GroupLayout m_ToggleEntryLayout;
    GroupLayout m_InlineEntryLayout;
    GroupLayout m_StringEntryLayout;
    GroupLayout m_Vec3dEntryLayout;
    GroupLayout m_IntMatEntryLayout;
    GroupLayout m_DblMatEntryLayout;

    TriggerButton m_AttrVec3dRowAdd;
    TriggerButton m_AttrVec3dRowDel;

    TriggerButton m_AttrDmatRowAdd;
    TriggerButton m_AttrDmatRowDel;
    TriggerButton m_AttrDmatColAdd;
    TriggerButton m_AttrDmatColDel;

    TriggerButton m_AttrImatRowAdd;
    TriggerButton m_AttrImatRowDel;
    TriggerButton m_AttrImatColAdd;
    TriggerButton m_AttrImatColDel;

    BoolParm* m_AttrBoolParmPtr;
    ToggleButton m_AttrDataToggleIn;

    StringInput m_InlineDataIn;

    VspTextEditor* m_DataText;
    Fl_Text_Buffer* m_DataBuffer;

    TriggerButton m_CopyButton;
    TriggerButton m_PasteButton;
    TriggerButton m_CutButton;

    GroupLayout* m_CurAttrGroup;

    string m_AttrID;
    string m_CollID;
    string m_GroupID;

};

#endif