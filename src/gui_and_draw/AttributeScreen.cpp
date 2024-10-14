//
// This file is released under the terms of the NASA Open Source Agreement (NOSA)
// version 1.3 as detailed in the LICENSE file which accompanies this software.
//

// AttributeScreen.cpp: GUI Stuff for working with attributes
// Tim Cuatt
//
//////////////////////////////////////////////////////////////////////

#include "AttributeScreen.h"
#include "AttributeEditor.h"
#include "AttributeManager.h"
#include "LinkMgr.h"
#include "ParmMgr.h"
#include "VehicleMgr.h"
#include "ManageGeomScreen.h"
#include "AdvLinkScreen.h"
#include "ManageMeasureScreen.h"
#include "ParmLinkScreen.h"
#include "ParmScreen.h"
#include "ScreenBase.h"

//===============================================================================//
//===============================================================================//
//===============================================================================//

AttributeExplorer::AttributeExplorer( ScreenMgr* mgr ) : BasicScreen( mgr, 800, 680, "Attribute Explorer" )
{
    m_ScreenMgr = mgr;

    int editor_ht = 520;

    int tree_w = 450;

    m_AttrDetailLayout.SetGroupAndScreen( m_FLTK_Window, this );
    m_FLTK_Window->resizable( m_AttrDetailLayout.GetGroup() );
    m_FLTK_Window->size_range( m_FLTK_Window->w()/2, m_FLTK_Window->h()/2 );

    m_AttrDetailLayout.AddX( 5 );
    m_AttrDetailLayout.AddSubGroupLayout( m_TreeGroupLayout, tree_w, m_AttrDetailLayout.GetRemainY() );

    m_AttrDetailLayout.AddX( tree_w + 5 );
    m_AttrDetailLayout.AddSubGroupLayout( m_CommonEntryLayout, m_AttrDetailLayout.GetW() - tree_w - 15, m_AttrDetailLayout.GetRemainY() );

    m_AttrDetailLayout.SetX( m_AttrDetailLayout.GetStartX() );
    m_AttrDetailLayout.SetY( editor_ht );
    m_AttrDetailLayout.AddSubGroupLayout( m_ResizableLayout, m_AttrDetailLayout.GetW(), m_AttrDetailLayout.GetRemainY());
    m_AttrDetailLayout.GetGroup()->resizable( m_ResizableLayout.GetGroup() );

    m_TreeGroupLayout.ForceNewLine();
    m_TreeGroupLayout.AddYGap();

    m_CommonEntryLayout.ForceNewLine();

    // insert AttrTree GUI
    m_TreeGroupLayout.AddDividerBox( "Attribute Search" );

    bool capitalize = true;
    int w_search_btn = m_TreeGroupLayout.GetW() / 3;

    m_TreeGroupLayout.SetChoiceButtonWidth( 0 );
    m_AttrTypeSearchChoice.AddItem( "All Types" , vsp::INVALID_TYPE );
    m_AttrTypeSearchChoice.AddItem( NameValData::GetTypeName( vsp::BOOL_DATA , capitalize ) , vsp::BOOL_DATA );
    m_AttrTypeSearchChoice.AddItem( NameValData::GetTypeName( vsp::INT_DATA, capitalize ), vsp::INT_DATA );
    m_AttrTypeSearchChoice.AddItem( NameValData::GetTypeName( vsp::DOUBLE_DATA, capitalize ), vsp::DOUBLE_DATA );
    m_AttrTypeSearchChoice.AddItem( NameValData::GetTypeName( vsp::STRING_DATA, capitalize ), vsp::STRING_DATA );
    m_AttrTypeSearchChoice.AddItem( NameValData::GetTypeName( vsp::VEC3D_DATA, capitalize ), vsp::VEC3D_DATA );
    m_AttrTypeSearchChoice.AddItem( NameValData::GetTypeName( vsp::INT_MATRIX_DATA, capitalize ), vsp::INT_MATRIX_DATA );
    m_AttrTypeSearchChoice.AddItem( NameValData::GetTypeName( vsp::DOUBLE_MATRIX_DATA, capitalize ), vsp::DOUBLE_MATRIX_DATA );
    m_AttrTypeSearchChoice.AddItem( NameValData::GetTypeName( vsp::ATTR_COLLECTION_DATA, capitalize ), vsp::ATTR_COLLECTION_DATA );

    m_ObjTypeSearchChoice.AddItem( "All Objects", vsp::ATTROBJ_FREE );
    m_ObjTypeSearchChoice.AddItem( "Parm", vsp::ATTROBJ_PARM );
    m_ObjTypeSearchChoice.AddItem( "Geom", vsp::ATTROBJ_GEOM );
    m_ObjTypeSearchChoice.AddItem( "Vehicle", vsp::ATTROBJ_VEH );
    m_ObjTypeSearchChoice.AddItem( "Subsurf", vsp::ATTROBJ_SUBSURF );
    m_ObjTypeSearchChoice.AddItem( "Measure", vsp::ATTROBJ_MEASURE );
    m_ObjTypeSearchChoice.AddItem( "Link", vsp::ATTROBJ_LINK );
    m_ObjTypeSearchChoice.AddItem( "AdvLink", vsp::ATTROBJ_ADVLINK );
    m_ObjTypeSearchChoice.AddItem( "Attribute Groups", vsp::ATTROBJ_ATTR );

    m_TreeGroupLayout.AddInput( m_AttrSearchIn, "Search");

    m_TreeGroupLayout.SetSameLineFlag( true );
    m_TreeGroupLayout.AddButton( m_CaseSensitiveButton, "Case Sensitive");
    m_TreeGroupLayout.AddChoice( m_ObjTypeSearchChoice, "Sort Type" );
    m_TreeGroupLayout.AddChoice( m_AttrTypeSearchChoice, "Sort Object" );

    m_CaseSensitiveButton.SetWidth( w_search_btn );
    m_AttrTypeSearchChoice.SetWidth( w_search_btn );
    m_ObjTypeSearchChoice.SetWidth( w_search_btn );

    m_AttrTypeSearchChoice.SetX( m_TreeGroupLayout.GetStartX() + w_search_btn );
    m_ObjTypeSearchChoice.SetX( m_TreeGroupLayout.GetStartX() + 2 * w_search_btn );

    m_TreeGroupLayout.ForceNewLine();
    m_TreeGroupLayout.SetSameLineFlag( false );

    m_TreeGroupLayout.AddYGap();
    m_TreeGroupLayout.AddDividerBox( "Attribute Tree" );

    int w_node_btn = m_TreeGroupLayout.GetW() / 2;
    m_TreeGroupLayout.SetSameLineFlag( true );

    m_TreeGroupLayout.AddButton(m_CloseTrigger, "[-] Close Inactive Nodes");
    m_TreeGroupLayout.AddButton(m_OpenTrigger, "[+] Open All Nodes");

    m_CloseTrigger.SetWidth( w_node_btn );
    m_OpenTrigger.SetWidth( w_node_btn );

    m_OpenTrigger.SetX( m_TreeGroupLayout.GetStartX() + w_node_btn );

    m_TreeGroupLayout.ForceNewLine();

    m_AttrTreeWidget.Init( mgr, &m_TreeGroupLayout, m_FLTK_Window, this, staticScreenCB, true, m_TreeGroupLayout.GetY(), editor_ht );
    m_TreeGroupLayout.GetGroup()->resizable( m_AttrTreeWidget.GetTreeWidget() );

    m_TreeGroupLayout.SetY( m_TreeGroupLayout.GetY() + editor_ht );

    int w_btn_tree = m_TreeGroupLayout.GetW() / 3;
    m_TreeGroupLayout.AddButton( m_CopyButton, "Copy" );
    m_TreeGroupLayout.AddButton( m_PasteButton, "Paste" );
    m_TreeGroupLayout.AddButton( m_CutButton, "Cut" );
    m_CopyButton.SetWidth( w_btn_tree );
    m_CutButton.SetWidth( w_btn_tree );
    m_PasteButton.SetWidth( w_btn_tree );

    m_CutButton.SetX( m_TreeGroupLayout.GetStartX() + w_btn_tree );
    m_PasteButton.SetX( m_TreeGroupLayout.GetStartX() + 2 * w_btn_tree );

    m_CopyButton.GetFlButton()->shortcut( FL_CTRL + 'c' );
    m_PasteButton.GetFlButton()->shortcut( FL_CTRL + 'v' );
    m_CutButton.GetFlButton()->shortcut( FL_CTRL + 'x' );

    // name data entry box
    m_CommonEntryLayout.AddYGap();

    m_CommonEntryLayout.AddDividerBox( "Add/Delete Attributes" );

    m_CommonEntryLayout.SetSameLineFlag( true );

    m_CommonEntryLayout.SetChoiceButtonWidth( 0 );
    int w_btn = m_CommonEntryLayout.GetW() / 3;
    m_AttrTypeChoice.AddItem( NameValData::GetTypeName( vsp::BOOL_DATA , capitalize ) , vsp::BOOL_DATA );
    m_AttrTypeChoice.AddItem( NameValData::GetTypeName( vsp::INT_DATA, capitalize ), vsp::INT_DATA );
    m_AttrTypeChoice.AddItem( NameValData::GetTypeName( vsp::DOUBLE_DATA, capitalize ), vsp::DOUBLE_DATA );
    m_AttrTypeChoice.AddItem( NameValData::GetTypeName( vsp::STRING_DATA, capitalize ), vsp::STRING_DATA );
    m_AttrTypeChoice.AddItem( NameValData::GetTypeName( vsp::VEC3D_DATA, capitalize ), vsp::VEC3D_DATA );
    m_AttrTypeChoice.AddItem( NameValData::GetTypeName( vsp::INT_MATRIX_DATA, capitalize ), vsp::INT_MATRIX_DATA );
    m_AttrTypeChoice.AddItem( NameValData::GetTypeName( vsp::DOUBLE_MATRIX_DATA, capitalize ), vsp::DOUBLE_MATRIX_DATA );
    m_AttrTypeChoice.AddItem( NameValData::GetTypeName( vsp::ATTR_COLLECTION_DATA, capitalize ), vsp::ATTR_COLLECTION_DATA );

    m_CommonEntryLayout.AddChoice( m_AttrTypeChoice, "Type" );
    m_CommonEntryLayout.AddButton( m_AttrAddTrigger, "Add" );
    m_CommonEntryLayout.AddButton( m_AttrDelTrigger, "Delete" );

    m_AttrTypeChoice.SetWidth( w_btn );
    m_AttrAddTrigger.SetWidth( w_btn );
    m_AttrDelTrigger.SetWidth( w_btn );

    m_AttrAddTrigger.SetX( m_CommonEntryLayout.GetStartX() + w_btn );
    m_AttrDelTrigger.SetX( m_CommonEntryLayout.GetStartX() + 2 * w_btn );

    m_CommonEntryLayout.ForceNewLine();
    m_CommonEntryLayout.ForceNewLine();

    m_CommonEntryLayout.SetSameLineFlag( false );

    m_CommonEntryLayout.AddYGap();
    m_DataLabel = m_CommonEntryLayout.AddDividerBox( "Attribute Data Entry" );

    m_CommonEntryLayout.AddInput( m_AttrNameIn, "Name" );
    m_CommonEntryLayout.AddInput( m_AttrDescIn, "Desc" );

    // create subgroup with no data entry gui; used only for the Attribute Group
    m_CommonEntryLayout.AddSubGroupLayout( m_EmptyEntryLayout, m_CommonEntryLayout.GetW(), m_CommonEntryLayout.GetRemainY() );

    // create subgroup for toggling which label is shown in this screen (and to provide for future use of cell-data GUI for matrices/vectors)
    m_CommonEntryLayout.AddSubGroupLayout( m_ToggleEntryLayout, m_CommonEntryLayout.GetW(), m_CommonEntryLayout.GetRemainY() );
    m_ToggleEntryLayout.AddButton( m_AttrDataToggleIn, "Bool Value" );
    m_ToggleEntryLayout.AddResizeBox(); //sacrificial resizable component, prevents the rest of the layout from squishing

    m_CommonEntryLayout.AddSubGroupLayout( m_InlineEntryLayout, m_CommonEntryLayout.GetW(), m_CommonEntryLayout.GetRemainY() );
    m_InlineEntryLayout.AddInput( m_InlineDataIn, "Data" );
    m_InlineEntryLayout.AddResizeBox(); //sacrificial resizable component, prevents the rest of the layout from squishing

    m_CommonEntryLayout.AddSubGroupLayout( m_StringEntryLayout, m_CommonEntryLayout.GetW(), m_CommonEntryLayout.GetRemainY() );
    m_CommonEntryLayout.GetGroup()->resizable( m_StringEntryLayout.GetGroup() );

    // create text editor, and tie its widget callbacks to the staticScreenCB for updating the attribute when called
    bool resizable = true;
    m_DataText = m_StringEntryLayout.AddVspTextEditor( editor_ht, resizable );
    m_DataBuffer = new Fl_Text_Buffer;
    m_DataText->callback( staticScreenCB, this );
    m_DataText->buffer( m_DataBuffer );
    m_DataText->textfont( FL_COURIER );

    m_DataText->remove_key_binding( FL_Enter, FL_TEXT_EDITOR_ANY_STATE );
    m_DataText->remove_key_binding( FL_KP_Enter, FL_TEXT_EDITOR_ANY_STATE );
    m_DataText->add_key_binding( FL_Enter, FL_TEXT_EDITOR_ANY_STATE , VspTextEditor::kf_accept );
    m_DataText->add_key_binding( FL_Enter, FL_SHIFT , Fl_Text_Editor::kf_enter );
    m_DataText->add_key_binding( FL_Enter, FL_CTRL , Fl_Text_Editor::kf_enter );
    m_DataText->add_key_binding( FL_KP_Enter, FL_TEXT_EDITOR_ANY_STATE , VspTextEditor::kf_accept );
    m_DataText->add_key_binding( FL_KP_Enter, FL_SHIFT , Fl_Text_Editor::kf_enter );
    m_DataText->add_key_binding( FL_KP_Enter, FL_CTRL , Fl_Text_Editor::kf_enter );
    m_DataBuffer->text( "" );


    // add vec3d layout
    m_CommonEntryLayout.AddSubGroupLayout( m_Vec3dEntryLayout, m_CommonEntryLayout.GetW(), m_CommonEntryLayout.GetRemainY() );
    m_Vec3dSpreadSingle = m_Vec3dEntryLayout.AddSpreadSheet < vec3d >( editor_ht - 2*m_Vec3dEntryLayout.GetStdHeight()  );
    m_Vec3dEntryLayout.GetGroup()->resizable( m_Vec3dSpreadSingle );
    m_Vec3dSpreadSingle->set_HeaderOffset( 'X' - 'A' );
    m_Vec3dSpreadSingle->set_ChangeCallback( staticScreenCB, this );

    int w_vec3d_btn = m_Vec3dEntryLayout.GetW() / 4;

    m_Vec3dEntryLayout.AddButton( m_AttrVec3dRowAdd , "Add Row" );
    m_Vec3dEntryLayout.AddButton( m_AttrVec3dRowDel , "Del Row" );
    m_AttrVec3dRowAdd.SetWidth( w_vec3d_btn );
    m_AttrVec3dRowDel.SetWidth( w_vec3d_btn );

    m_Vec3dEntryLayout.ForceNewLine();

    // add int & double matrix layouts
    m_CommonEntryLayout.AddSubGroupLayout( m_IntMatEntryLayout, m_CommonEntryLayout.GetW(), m_CommonEntryLayout.GetRemainY() );
    m_IntMatrixSpreadSheet = m_IntMatEntryLayout.AddSpreadSheet < vector < int > >( editor_ht - 2*m_IntMatEntryLayout.GetStdHeight()  );
    m_IntMatEntryLayout.GetGroup()->resizable( m_IntMatrixSpreadSheet );
    m_IntMatrixSpreadSheet->set_ChangeCallback( staticScreenCB, this );

    int w_imat_btn = m_IntMatEntryLayout.GetW() / 4;

    m_IntMatEntryLayout.SetSameLineFlag( true );
    m_IntMatEntryLayout.AddButton( m_AttrImatRowAdd , "Add Row" );
    m_IntMatEntryLayout.AddButton( m_AttrImatColAdd , "Add Col" );
    m_IntMatEntryLayout.ForceNewLine();
    m_IntMatEntryLayout.AddButton( m_AttrImatRowDel , "Del Row" );
    m_IntMatEntryLayout.AddButton( m_AttrImatColDel , "Del Col" );

    m_AttrImatRowAdd.SetWidth( w_imat_btn );
    m_AttrImatRowDel.SetWidth( w_imat_btn );
    m_AttrImatColAdd.SetWidth( w_imat_btn );
    m_AttrImatColDel.SetWidth( w_imat_btn );
    m_AttrImatColAdd.SetX( m_CommonEntryLayout.GetStartX() + w_imat_btn );
    m_AttrImatColDel.SetX( m_CommonEntryLayout.GetStartX() + w_imat_btn );

    m_IntMatEntryLayout.ForceNewLine();

    m_CommonEntryLayout.AddSubGroupLayout( m_DblMatEntryLayout, m_CommonEntryLayout.GetW(), m_CommonEntryLayout.GetRemainY() );
    m_DoubleMatrixSpreadSheet = m_DblMatEntryLayout.AddSpreadSheet < vector < double > >( editor_ht - 2*m_DblMatEntryLayout.GetStdHeight() );
    m_DblMatEntryLayout.GetGroup()->resizable( m_DoubleMatrixSpreadSheet );
    m_DoubleMatrixSpreadSheet->set_ChangeCallback( staticScreenCB, this );

    int w_dmat_btn = m_DblMatEntryLayout.GetW() / 4;

    m_DblMatEntryLayout.SetSameLineFlag( true );
    m_DblMatEntryLayout.AddButton( m_AttrDmatRowAdd , "Add Row" );
    m_DblMatEntryLayout.AddButton( m_AttrDmatColAdd , "Add Col" );
    m_DblMatEntryLayout.ForceNewLine();
    m_DblMatEntryLayout.AddButton( m_AttrDmatRowDel , "Del Row" );
    m_DblMatEntryLayout.AddButton( m_AttrDmatColDel , "Del Col" );

    m_AttrDmatRowAdd.SetWidth( w_dmat_btn );
    m_AttrDmatRowDel.SetWidth( w_dmat_btn );
    m_AttrDmatColAdd.SetWidth( w_dmat_btn );
    m_AttrDmatColDel.SetWidth( w_dmat_btn );
    m_AttrDmatColAdd.SetX( m_CommonEntryLayout.GetStartX() + w_dmat_btn );
    m_AttrDmatColDel.SetX( m_CommonEntryLayout.GetStartX() + w_dmat_btn );

    m_DblMatEntryLayout.ForceNewLine();

    //initialize pointers etc.
    m_CurAttrGroup = &m_StringEntryLayout;
    m_AttrID.clear();
    m_CollID.clear();
    m_GroupID.clear();

    Vehicle* veh = VehicleMgr.GetVehicle();
    m_AttrBoolParmPtr = &veh->m_AttrBoolButtonParm;
    m_CaseSensParmPtr = &veh->m_AttrCaseSensitivity;

    m_AttrTypeChoice.SetVal( vsp::STRING_DATA );
    m_AttrTypeSearchChoice.SetVal( vsp::INVALID_TYPE );
    m_ObjTypeSearchChoice.SetVal( vsp::ATTROBJ_FREE );
    AttrTypeDispGroup( vsp::STRING_DATA, &m_StringEntryLayout );

}

AttributeExplorer::~AttributeExplorer()
{
    m_DataText->buffer( NULL );
    delete m_DataBuffer;
}

bool AttributeExplorer::Update()
{
    // populate Tree with empty branch IDs
    GetEmptyColls();

    // Assign current Attribute AttachID, AttachType, AttributeName, and AttributeDataPtr
    // Update the Attribute GUI's pointer to the appropriate AttributeData object
    m_CaseSensitiveButton.Update( m_CaseSensParmPtr->GetID() );
    m_AttrDataToggleIn.Update( m_AttrBoolParmPtr->GetID() );
    m_AttrTreeWidget.SetSearchTerms( m_AttrTypeSearchChoice.GetVal(), m_ObjTypeSearchChoice.GetVal(), m_AttrSearchIn.GetString(), m_CaseSensParmPtr->Get() );
    m_AttrTreeWidget.Update();
    SetAttrData();

    GroupLayout* group_choice  = nullptr;

    NameValData* attr_ptr = AttributeMgr.GetAttributePtr( m_AttrID );

    if ( attr_ptr )
    {
        switch ( attr_ptr->GetType() )
        {
        case vsp::BOOL_DATA:
            group_choice = &m_ToggleEntryLayout;
            break;
        case vsp::STRING_DATA:
            group_choice = &m_StringEntryLayout;
            break;
        case vsp::INT_DATA:
            group_choice = &m_InlineEntryLayout;
            break;
        case vsp::DOUBLE_DATA:
            group_choice = &m_InlineEntryLayout;
            break;
        case vsp::ATTR_COLLECTION_DATA:
            group_choice = &m_EmptyEntryLayout;
            break;
        case vsp::VEC3D_DATA:
            group_choice = &m_Vec3dEntryLayout;
            break;
        case vsp::INT_MATRIX_DATA:
            group_choice = &m_IntMatEntryLayout;
            break;
        case vsp::DOUBLE_MATRIX_DATA:
            group_choice = &m_DblMatEntryLayout;
            break;
        }
    }
    UpdateAttrFields( group_choice );
    return true;
};

void AttributeExplorer::SetAttrData()
{
    m_AttrID = m_AttrTreeWidget.GetTreeAttrID();
    m_CollID = m_AttrTreeWidget.GetTreeCollID();
    m_GroupID.clear();
    if ( AttributeMgr.GetAttributeType( m_AttrID ) == vsp::ATTR_COLLECTION_DATA )
    {
        m_GroupID = AttributeMgr.GetChildCollection( m_AttrID );
    }
};

void AttributeExplorer::AddEmptyCollID( vector < string > coll_ids )
{
    m_AttrTreeWidget.AddEmptyCollID( coll_ids );
};

void AttributeExplorer::GetEmptyColls()
{
    vector < VspScreen* > screen_vec = m_ScreenMgr->GetAllScreens();

    vector < VspScreen* > geom_screen_vec;

    vector < string > coll_id_vec;

    for ( int i = 0; i != screen_vec.size(); ++i )
    {
        if ( screen_vec[i]->IsShown() )
        {
            if ( screen_vec[i]->GetScreenType() == vsp::VSP_ADV_LINK_SCREEN )
            {
                static_cast< AdvLinkScreen* >( screen_vec[i] )->GetCollIDs( coll_id_vec );
            }
            else if ( screen_vec[i]->GetScreenType() == vsp::VSP_MEASURE_SCREEN )
            {
                static_cast< ManageMeasureScreen* >( screen_vec[i] )->GetCollIDs( coll_id_vec );
            }
            else if ( screen_vec[i]->GetScreenType() == vsp::VSP_PARM_LINK_SCREEN )
            {
                static_cast< ParmLinkScreen* >( screen_vec[i] )->GetCollIDs( coll_id_vec );
            }
            else if ( screen_vec[i]->GetScreenType() == vsp::VSP_PARM_SCREEN )
            {
                static_cast< ParmScreen* >( screen_vec[i] )->GetCollIDs( coll_id_vec );
            }
            else if ( screen_vec[i]->GetScreenType() == vsp::VSP_MANAGE_GEOM_SCREEN )
            {
                geom_screen_vec = static_cast< ManageGeomScreen* >(screen_vec[i])->GetGeomScreenVec();
                for ( int j = 0; j != geom_screen_vec.size(); ++j )
                {
                    if ( geom_screen_vec[j]->IsShown() )
                    {
                        if ( geom_screen_vec[j]->GetScreenType() == vsp::VEH_GEOM_SCREEN )
                        {
                            static_cast< VehScreen* >( geom_screen_vec[j] )->GetCollIDs( coll_id_vec );
                        }
                        else
                        {
                            static_cast< GeomScreen* >( geom_screen_vec[j] )->GetCollIDs( coll_id_vec );
                        }
                    }
                }
            }
        }
    }

    AddEmptyCollID( coll_id_vec );
}

void AttributeExplorer::UpdateAttrFields( GroupLayout* group )
{
    bool attrDataBool = false;
    int attrDataInt;
    double attrDataDbl;
    string attrDataStr = "";
    string attrDescStr = "";

    string attrBufferText = "";
    string attrInlineText = "";

    string header_str = "Attribute Explorer";
    string name_str = "";

    //default to string data
    int attr_type = -2;

    NameValData* attr_ptr = AttributeMgr.GetAttributePtr( m_AttrID );
    AttributeCollection* ac_ptr = AttributeMgr.GetCollectionPtr( m_CollID );
    AttributeCollection* ac_group_ptr = AttributeMgr.GetCollectionPtr( m_GroupID );

    if ( attr_ptr )
    {
        name_str = attr_ptr->GetName();
        attr_type = attr_ptr->GetType();

        attrDataBool = attr_ptr->GetBool( 0 );
        attrDataInt = attr_ptr->GetInt( 0 );
        attrDataDbl = attr_ptr->GetDouble( 0 );
        attrDataStr = attr_ptr->GetString( 0 );

        attrDescStr = attr_ptr->GetDoc();

        if ( attr_type == vsp::VEC3D_DATA
          || attr_type == vsp::INT_MATRIX_DATA
          || attr_type == vsp::DOUBLE_MATRIX_DATA )
        {
            NameValData* col_attr = ac_ptr->FindPtr( name_str+"_col" );
            NameValData* row_attr = ac_ptr->FindPtr( name_str+"_row" );

            if ( col_attr && col_attr->GetType() == vsp::STRING_DATA )
            {
                m_Vec3dSpreadSingle->set_col_header_txt( col_attr->GetString( 0 ) );
                m_IntMatrixSpreadSheet->set_col_header_txt( col_attr->GetString( 0 ) );
                m_DoubleMatrixSpreadSheet->set_col_header_txt( col_attr->GetString( 0 ) );
            }
            else
            {
                m_Vec3dSpreadSingle->set_col_user_header_flag( false );
                m_IntMatrixSpreadSheet->set_col_user_header_flag( false );
                m_DoubleMatrixSpreadSheet->set_col_user_header_flag( false );
            }

            if ( row_attr && row_attr->GetType() == vsp::STRING_DATA )
            {
                m_Vec3dSpreadSingle->set_row_header_txt( row_attr->GetString( 0 ) );
                m_IntMatrixSpreadSheet->set_row_header_txt( row_attr->GetString( 0 ) );
                m_DoubleMatrixSpreadSheet->set_row_header_txt( row_attr->GetString( 0 ) );
            }
            else
            {
                m_Vec3dSpreadSingle->set_row_user_header_flag( false );
                m_IntMatrixSpreadSheet->set_row_user_header_flag( false );
                m_DoubleMatrixSpreadSheet->set_row_user_header_flag( false );
            }
        }

        switch ( attr_type )
        {
        case vsp::BOOL_DATA:
            m_AttrBoolParmPtr->Set( attrDataBool );
            break;
        case vsp::STRING_DATA:
            attrBufferText = attrDataStr;
            break;
        case vsp::INT_DATA:
            attrInlineText = to_string( attrDataInt );
            break;
        case vsp::DOUBLE_DATA:
            attrInlineText = to_string( attrDataDbl );
            break;
        case vsp::ATTR_COLLECTION_DATA:
            break;
        case vsp::VEC3D_DATA:
            m_Vec3dSpreadSingle->set_data( &( attr_ptr->GetVec3dData() ) );
            break;
        case vsp::INT_MATRIX_DATA:
            m_IntMatrixSpreadSheet->set_data( &( attr_ptr->GetIntMatData() ) );
            break;
        case vsp::DOUBLE_MATRIX_DATA:
            m_DoubleMatrixSpreadSheet->set_data( &( attr_ptr->GetDoubleMatData() ) );
            break;
        }

        m_CutButton.Activate();
        m_CopyButton.Activate();

    }
    else
    {
        m_CutButton.Deactivate();
        m_CopyButton.Deactivate();
    }

    if ( ac_group_ptr )
    {
        m_PasteButton.Activate();
        header_str += " : ";
        header_str += AttributeMgr.GetName( ac_group_ptr->GetAttachID() );
    }
    else if ( ac_ptr )
    {
        m_PasteButton.Activate();
        header_str += " : ";
        header_str += AttributeMgr.GetName( ac_ptr->GetAttachID() );
    }
    else
    {
        m_PasteButton.Deactivate();
    }

    m_AttrNameIn.Update( name_str );
    m_DataBuffer->text( attrBufferText.c_str() );
    m_InlineDataIn.Update( attrInlineText.c_str() );

    m_AttrDescIn.Update( attrDescStr.c_str() );
    SetTitle(header_str);

    AttrTypeDispGroup( attr_type , group );

    if ( !group || ( attr_ptr && attr_ptr->IsProtected() ) )
    {
        m_CutButton.Deactivate();
        m_CopyButton.Deactivate();

        m_AttrNameIn.Deactivate();
        m_AttrDescIn.Deactivate();
        m_AttrDelTrigger.Deactivate();
        m_AttrVec3dRowAdd.Deactivate();
        m_AttrVec3dRowDel.Deactivate();
        m_AttrDmatRowAdd.Deactivate();
        m_AttrDmatRowDel.Deactivate();
        m_AttrDmatColAdd.Deactivate();
        m_AttrDmatColDel.Deactivate();
        m_AttrImatRowAdd.Deactivate();
        m_AttrImatRowDel.Deactivate();
        m_AttrImatColAdd.Deactivate();
        m_AttrImatColDel.Deactivate();
    }
    else
    {
        m_AttrNameIn.Activate();
        m_AttrDescIn.Activate();
        m_AttrDelTrigger.Activate();
        m_AttrVec3dRowAdd.Activate();
        m_AttrVec3dRowDel.Activate();
        m_AttrDmatRowAdd.Activate();
        m_AttrDmatRowDel.Activate();
        m_AttrDmatColAdd.Activate();
        m_AttrDmatColDel.Activate();
        m_AttrImatRowAdd.Activate();
        m_AttrImatRowDel.Activate();
        m_AttrImatColAdd.Activate();
        m_AttrImatColDel.Activate();
    }
}

void AttributeExplorer::SetTreeAutoSelectID( const string & id )
{
    m_AttrTreeWidget.SetAutoSelectID( id );
}

void AttributeExplorer::AttributeAdd()
{
    AttributeCollection* ac_ptr = AttributeMgr.GetCollectionPtr( m_CollID );
    AttributeCollection* ac_group_ptr = AttributeMgr.GetCollectionPtr( m_GroupID );

    string new_id;
    if ( ac_group_ptr )
    {
        AttributeMgr.GuiAddAttribute( ac_group_ptr, m_AttrTypeChoice.GetVal() );
        new_id = ac_group_ptr->GetLastAddedID();
    }
    else if ( ac_ptr )
    {
        AttributeMgr.GuiAddAttribute( ac_ptr, m_AttrTypeChoice.GetVal() );
        new_id = ac_ptr->GetLastAddedID();
    }
    m_AttrTreeWidget.SetAutoSelectID( new_id );
}

void AttributeExplorer::AttributeModify( GuiDevice* gui_device, Fl_Widget *w )
{
    NameValData* attr_ptr = AttributeMgr.GetAttributePtr( m_AttrID );

    //AttributeModify will detect if the attribute has changed to a new & valid attribute, and create that attribute in the place of the old one
    if ( attr_ptr )
    {
        int attrType = attr_ptr->GetType();

        //rename
        if ( gui_device == &m_AttrNameIn )
        {
            AttributeMgr.SetAttributeName( m_AttrID, m_AttrNameIn.GetString() );
        }

        //change description
        else if ( gui_device == &m_AttrDescIn )
        {
            AttributeMgr.SetAttributeDoc( m_AttrID, m_AttrDescIn.GetString() );
        }

        //modify bool data
        else if ( gui_device == &m_AttrDataToggleIn && attrType == vsp::BOOL_DATA )
        {
            AttributeMgr.SetAttributeBool( m_AttrID, m_AttrBoolParmPtr->Get() );
        }

        //modify int data
        else if ( gui_device == &m_InlineDataIn && attrType == vsp::INT_DATA )
        {
            if ( AttributeEditor::canMakeInt( m_InlineDataIn.GetString() ) )
            {
                AttributeMgr.SetAttributeInt( m_AttrID, stoi( m_InlineDataIn.GetString() ) );
            }
        }

        //modify double data
        else if ( gui_device == &m_InlineDataIn && attrType == vsp::DOUBLE_DATA )
        {
            if ( AttributeEditor::canMakeDbl( m_InlineDataIn.GetString() ) )
            {
                AttributeMgr.SetAttributeDouble( m_AttrID, stod( m_InlineDataIn.GetString() ) );
            }
        }

        //modify string data
        else if ( static_cast<Fl_Text_Editor * >( w ) == m_DataText && attrType == vsp::STRING_DATA )
        {
            AttributeMgr.SetAttributeString( m_AttrID, m_DataBuffer->text() );
        }

        //vec3d & matrix data modification already done via the spreadsheet widgets

        //vec3d resizing
        else if ( gui_device == &m_AttrVec3dRowAdd && attrType == vsp::VEC3D_DATA )
        {
            vector < vec3d > * attrVec3dPtr = &attr_ptr->GetVec3dData();
            ResizeVector( attrVec3dPtr, 1, vec3d(0.,0.,0.) );
        }

        else if ( gui_device == &m_AttrVec3dRowDel && attrType == vsp::VEC3D_DATA )
        {
            vector < vec3d > * attrVec3dPtr = &attr_ptr->GetVec3dData();
            ResizeVector( attrVec3dPtr, -1, vec3d(0.,0.,0.) );
        }

        //int matrix resizing
        else if ( gui_device == &m_AttrImatRowAdd && attrType == vsp::INT_MATRIX_DATA )
        {
            vector < vector < int > > * attrImatPtr = &attr_ptr->GetIntMatData();
            ResizeMat( attrImatPtr, {1, 0}, 0 );
        }

        else if ( gui_device == &m_AttrImatRowDel && attrType == vsp::INT_MATRIX_DATA )
        {
            vector < vector < int > > * attrImatPtr = &attr_ptr->GetIntMatData();
            ResizeMat( attrImatPtr, {-1, 0}, 0 );
        }

        else if ( gui_device == &m_AttrImatColAdd && attrType == vsp::INT_MATRIX_DATA )
        {
            vector < vector < int > > * attrImatPtr = &attr_ptr->GetIntMatData();
            ResizeMat( attrImatPtr, {0, 1}, 0 );
        }

        else if ( gui_device == &m_AttrImatColDel && attrType == vsp::INT_MATRIX_DATA )
        {
            vector < vector < int > > * attrImatPtr = &attr_ptr->GetIntMatData();
            ResizeMat( attrImatPtr, {0, -1}, 0 );
        }

        //double matrix resizing
        else if ( gui_device == &m_AttrDmatRowAdd && attrType == vsp::DOUBLE_MATRIX_DATA )
        {
            vector < vector < double > > * attrDmatPtr = &attr_ptr->GetDoubleMatData();
            ResizeMat( attrDmatPtr, {1, 0}, 0. );
        }

        else if ( gui_device == &m_AttrDmatRowDel && attrType == vsp::DOUBLE_MATRIX_DATA )
        {
            vector < vector < double > > * attrDmatPtr = &attr_ptr->GetDoubleMatData();
            ResizeMat( attrDmatPtr, {-1, 0}, 0. );
        }

        else if ( gui_device == &m_AttrDmatColAdd && attrType == vsp::DOUBLE_MATRIX_DATA )
        {
            vector < vector < double > > * attrDmatPtr = &attr_ptr->GetDoubleMatData();
            ResizeMat( attrDmatPtr, {0, 1}, 0. );
        }

        else if ( gui_device == &m_AttrDmatColDel && attrType == vsp::DOUBLE_MATRIX_DATA )
        {
            vector < vector < double > > * attrDmatPtr = &attr_ptr->GetDoubleMatData();
            ResizeMat( attrDmatPtr, {0, -1}, 0. );
        }

        AttributeMgr.SetAttrDirtyFlag( attr_ptr->GetID() );
    }
}

void AttributeExplorer::AttrTypeDispGroup( int attr_type, GroupLayout * group )
{
    // rename data entry label
    bool cap = true;
    bool short_name = false;

    string data_name = NameValData::GetTypeName( attr_type, cap, short_name );
    if ( data_name.size() )
    {
        data_name += " ";
    }
    data_name += "Data";

    NameValData* attr_ptr = AttributeMgr.GetAttributePtr( m_AttrID );

    if ( attr_ptr )
    {
        pair < int, int > row_col_size = { 0, 0 };

        if ( attr_ptr->GetType() == vsp::INT_MATRIX_DATA )
        {
            row_col_size = GetMatSize( &(attr_ptr->GetIntMatData()) );
        }
        else if ( attr_ptr->GetType() == vsp::DOUBLE_MATRIX_DATA )
        {
            row_col_size = GetMatSize( &(attr_ptr->GetDoubleMatData()) );
        }
        if ( row_col_size.first && row_col_size.second )
        {
            data_name += " : " + to_string(row_col_size.first) + " x " + to_string(row_col_size.second);
        }
    }

    m_DataLabel->copy_label( data_name.c_str() );

    // select active GUI group by datatype
    if ( m_CurAttrGroup == group && group )
    {
        return;
    }

    m_EmptyEntryLayout.Hide();
    m_ToggleEntryLayout.Hide();
    m_InlineEntryLayout.Hide();
    m_StringEntryLayout.Hide();
    m_Vec3dEntryLayout.Hide();
    m_IntMatEntryLayout.Hide();
    m_DblMatEntryLayout.Hide();

    m_CurAttrGroup = group;

    if ( group )
    {
        group->Show();
    }

}

void AttributeExplorer::CallBack( Fl_Widget *w )
{
    // if either text editor is called, run AttributeModify without changing the name
    if ( static_cast<Fl_Text_Editor * >( w ) == m_DataText )
    {
        AttributeModify( nullptr, m_DataText );
    }
    else if ( w == m_Vec3dSpreadSingle
           || w == m_IntMatrixSpreadSheet
           || w == m_DoubleMatrixSpreadSheet )
    {
        AttributeModify( nullptr, w );
    }

    m_ScreenMgr->SetUpdateFlag( true );
}

void AttributeExplorer::GuiDeviceCallBack( GuiDevice* gui_device )
{
    // if name input is called, run AttributeModify with a name-change gui device input
    if ( gui_device == &m_AttrAddTrigger )
    {
        AttributeAdd();
    }
    else if ( gui_device == &m_AttrDelTrigger )
    {
        AttributeMgr.DeleteAttribute( m_AttrID );
        m_AttrID.clear();
    }
    else if ( gui_device == &m_OpenTrigger )
    {
        m_AttrTreeWidget.SetAllNodes( true );
    }
    else if ( gui_device == &m_CloseTrigger )
    {
        m_AttrTreeWidget.SetAllNodes( false );
    }
    else if ( gui_device == &m_CutButton )
    {
        AttributeMgr.CutAttributeUtil( m_AttrID );
    }
    else if ( gui_device == &m_CopyButton )
    {
        AttributeMgr.CopyAttributeUtil( m_AttrID );
    }
    else if ( !m_CollID.empty() && ( gui_device == &m_PasteButton ) )
    {
        if ( !m_GroupID.empty() )
        {
            AttributeMgr.PasteAttributeUtil( m_GroupID );
        }
        else
        {
            AttributeMgr.PasteAttributeUtil( m_CollID );
        }

    }
    else
    {
        AttributeModify( gui_device, nullptr );
    }

    m_ScreenMgr->SetUpdateFlag( true );
}