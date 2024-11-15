//
// This file is released under the terms of the NASA Open Source Agreement (NOSA)
// version 1.3 as detailed in the LICENSE file which accompanies this software.
//

// AttributeEditor.cpp: Extensive attribute editor screen.
// Tim Cuatt
//
//////////////////////////////////////////////////////////////////////

#include "AttributeEditor.h"
#include "AttributeScreen.h"
#include "ParmMgr.h"
#include "ScreenMgr.h"

//==== Attribute Tree GUI ====//
AttributeTree::AttributeTree()
{
    m_Screen = NULL;
    m_AttrRootCollID.clear();
    m_AttrID.clear();
    m_CollID.clear();
    m_TreeItem = nullptr;
    m_AutoSelectID = "";
    m_FilterAttrType = vsp::INVALID_TYPE;
    m_FilterObjType = vsp::ATTROBJ_FREE;
    m_FilterStr = "";
}

AttributeTree::~AttributeTree()
{
    m_TreeItem = nullptr;
    m_MapTreeAttributes.clear();
    m_MapTreeCollections.clear();
    m_MapTreeFullIDs.clear();

    m_SelectID.clear();
}

void AttributeTree::Init( ScreenMgr* mgr, GroupLayout * layout, Fl_Group* group, VspScreen *screen, Fl_Callback *cb, bool mod_start, int start_y, int browser_h )
{

    m_AttrTree = layout->AddTreeWithColumns( browser_h );
    m_AttrTree->showroot( true );
    m_AttrTree->callback( StaticDeviceCB, this );
    m_AttrTree->selectmode( FL_TREE_SELECT_SINGLE );
    m_AttrTree->item_reselect_mode( FL_TREE_SELECTABLE_ALWAYS );
    m_AttrTree->sortorder( FL_TREE_SORT_ASCENDING );

    m_ColWidths[0] = 120;
    m_ColWidths[1] = 40;
    m_ColWidths[2] = 80;
    m_ColWidths[3] = 0;

    m_AttrTree->column_widths( m_ColWidths );            // set column widths array
    m_AttrTree->resizing(true);                       // enable interactive resizing
    m_AttrTree->column_char('\t');                    // use tab char as column delimiter
    m_AttrTree->first_column_minw(100);               // minimum width of first column

    int fontsize = 12;
    m_AttrTree->item_labelsize( fontsize );

    m_Screen = screen;
    m_ScreenMgr = mgr;
}

void AttributeTree::Activate()
{
    m_AttrTree->activate();
}

void AttributeTree::Deactivate()
{
    m_AttrTree->deactivate();
}

void AttributeTree::Update()
{
    m_OpenBranchVec.clear();
    for ( int i = 0; i != m_OpenBranchUpdate.size(); ++i )
    {
        m_OpenBranchVec.push_back( m_OpenBranchUpdate.at(i) );
    }
    m_OpenBranchUpdate.clear();

    TrimCloseVec();

    UpdateTree();
}

void AttributeTree::UpdateTree()
{
    bool use_auto_select = m_AutoSelectID.size();

    TreeRowItem* root;
    TreeRowItem* focus_item;
    const TreeRowItem* child_item;

    AttributeCollection* ac;
    NameValData* nvd;

    string root_id = "";

    m_TreeItem = nullptr;
    m_MapTreeAttributes.clear();
    m_MapTreeCollections.clear();
    m_MapTreeFullIDs.clear();

    if ( m_AttrTree )
    {
        m_AttrTree->clear();   //GUI element
    }

    string header_row = "AttrTree\tType\tData";
    root = m_AttrTree->AddRow( header_row.c_str() );

    root->labelbgcolor( FL_WHITE );
    root->labelcolor( FL_BLACK );
    root->labelfont( FL_HELVETICA_BOLD );

    if ( !m_AttrRootCollID.empty() )
    {
        root_id = AttributeMgr.GetObjectParent( m_AttrRootCollID );
    }

    vector < vector < vector < string > > > tree_id_coll_vecs = AttributeMgr.GetAttrTreeVec( root_id , m_OpenBranchVec, true, m_FilterAttrType, m_FilterObjType, m_FilterStr, m_FilterCase ); //

    vector < vector < string > > tree_id_vecs = tree_id_coll_vecs[0];
    vector < vector < string > > tree_coll_vecs = tree_id_coll_vecs[1];

    vector < vector < int > > tree_id_count_vecs;
    vector < string > id_vec;

    int counter;
    string last_id;
    string id;
    string map_id;
    // iterate through each vector of ids
    vector < string > unique_names = {
        "Geoms",
        "Measures",
        "Links",
    };

    for ( int i = 0; i != tree_id_vecs.size(); ++i )
    {
        counter = 0;
        last_id = "";
        id = "";
        bool unique_name_flag;

        vector < int > count_vec;
        // iterate through each id or name
        for ( int j = 0; j != tree_id_vecs[ i ].size(); ++j )
        {
            id = tree_id_vecs[ i ][ j ];

            unique_name_flag = false;
            for ( int k = 0; k != unique_names.size(); ++k )
            {
                if ( unique_names[ k ].compare( id ) == 0 )
                {
                    unique_name_flag = true;
                }
            }
            if ( AttributeMgr.AllUpper( id ) || unique_name_flag )
            {
                counter = 0;
            }
            else
            {
                counter ++;
            }

            count_vec.push_back( counter );
        }
        tree_id_count_vecs.push_back( count_vec );
    }

    if ( m_AttrTree )
    {
        for ( int i = 0; i != tree_id_vecs.size(); ++i )
        {
            focus_item = root;

            id_vec.clear();

            for ( int j = 0; j != tree_id_vecs[i].size(); ++j )
            {
                string id = tree_id_vecs[i][j];
                string coll_id = tree_coll_vecs[i][j];

                string row_str = AttributeMgr.GetName( id );

                id_vec.push_back( id );

                child_item = nullptr;

                if ( focus_item )
                {
                    map < TreeRowItem*, vector < string > > :: iterator id_iter;

                    bool exists = false;
                    for ( id_iter = m_MapTreeFullIDs.begin(); id_iter != m_MapTreeFullIDs.end(); id_iter ++ )
                    {
                        if ( !exists && !CheckVecMatch( id_vec, id_iter->second ) )
                        {
                            child_item = id_iter->first;
                            exists = true;
                        }
                    }
                }

                if ( !child_item )
                {
                    ac = AttributeMgr.GetCollectionPtr( coll_id );
                    nvd = AttributeMgr.GetAttributePtr( id );

                    if ( nvd )
                    {
                        row_str += '\t' + nvd->GetTypeName( nvd->GetType(), true , true ) + '\t' + nvd->GetAsString( true );
                    }
                    else
                    {
                        row_str += "\t-\t-";
                    }

                    child_item = m_AttrTree->AddRow( row_str.c_str(), focus_item );

                    // creating a child item inserts its id vec and tree item into the map of ID vectors, used to disambiguate same-named containers
                    m_MapTreeFullIDs.insert( { const_cast< TreeRowItem* >( child_item ), id_vec } );

                    if ( nvd )
                    {
                        m_MapTreeAttributes.insert( { const_cast< TreeRowItem* >( child_item ), id } );
                        if ( ( !use_auto_select && !CheckVecMatch( m_SelectID, id_vec ) ) || id.compare( m_AutoSelectID ) == 0 )
                        {
                            m_AttrTree->select_only( const_cast< TreeRowItem* >( child_item ), 0 );
                            m_TreeItem = const_cast< TreeRowItem* >( child_item );
                        }

                        if ( nvd->GetType() == vsp::ATTR_COLLECTION_DATA )
                        {
                            const_cast< TreeRowItem* >( child_item )->labelcolor( FL_DARK_MAGENTA );
                        }
                    }
                    else
                    {
                        const_cast< TreeRowItem* >( child_item )->labelfont( FL_HELVETICA_BOLD );
                        const_cast< TreeRowItem* >( child_item )->labelcolor( FL_DARK_MAGENTA );
                    }
                    if ( ac )
                    {
                        m_MapTreeCollections.insert( { const_cast< TreeRowItem* >( child_item ), coll_id } );
                        if ( ( !use_auto_select && !CheckVecMatch( m_SelectID, id_vec ) ) || id.compare( m_AutoSelectID ) == 0 )
                        {
                            m_AttrTree->select_only( const_cast< TreeRowItem* >( child_item ), 0 );
                            m_TreeItem = const_cast< TreeRowItem* >( child_item );
                        }
                    }

                    for ( int i = 0; i != m_CloseVec.size(); ++i )
                    {
                        if ( !CheckVecMatch( m_CloseVec.at( i ), id_vec ) )
                        {
                            m_AttrTree->close( const_cast< TreeRowItem* >( child_item ), false );
                        }
                    }
                }
                focus_item = const_cast< TreeRowItem* >( child_item );
            }
        }
    }
    m_AutoSelectID.clear();
    m_AttrTree->redraw();
    SetTreeAttrID();
}

void AttributeTree::SetTreeAttrID()
{
    // if selected_id_vec is size 1, assign attribute pointer
    // if all selected IDs share a collection, assign collection pointer
    m_SelectID.clear();
    m_AttrID.clear();
    m_CollID.clear();

    if ( m_MapTreeAttributes.count( m_TreeItem ) )
    {
        m_AttrID = m_MapTreeAttributes.at( m_TreeItem );
    }
    if ( m_MapTreeCollections.count( m_TreeItem ) )
    {
        m_CollID = m_MapTreeCollections.at( m_TreeItem );
    }
    if ( m_MapTreeFullIDs.count( m_TreeItem ) )
    {
        m_SelectID = m_MapTreeFullIDs.at( m_TreeItem );
    }
}

void AttributeTree::AddEmptyCollID( const string & coll_id )
{
    bool already_contains = false;
    for (int i = 0; i != m_OpenBranchUpdate.size(); ++i )
    {
        if ( m_OpenBranchUpdate[i].compare( coll_id ) == 0 )
        {
            already_contains = true;
        }
    }
    if ( !already_contains )
    {
        m_OpenBranchUpdate.push_back( coll_id );
    }
}

void AttributeTree::AddEmptyCollID( const vector < string > & coll_ids )
{
    bool already_contains = false;
    for (int i = 0; i != coll_ids.size(); ++i )
    {
        AddEmptyCollID( coll_ids[i] );
    }
}

void AttributeTree::SetEmptyCollID( const vector < string > & coll_ids )
{
    m_OpenBranchUpdate = coll_ids;
}

void AttributeTree::SetSearchTerms( int attr_type, int attach_type, const string & attr_str, bool case_sens )
    {
        m_FilterAttrType = attr_type;
        m_FilterObjType = attach_type;
        m_FilterStr = attr_str;
        m_FilterCase = case_sens;

        if ( m_FilterAttrType != vsp::INVALID_TYPE || m_FilterObjType != vsp::ATTROBJ_FREE || !m_FilterStr.empty() )
        {
            SetAllNodes( true );
        }
    }

bool AttributeTree::ItemState( const vector < string > & id_vec )
{
    for ( int i = 0; i != m_CloseVec.size(); ++i )
    {
        if ( !CheckVecMatch( m_CloseVec.at( i ), id_vec ) )
        {
            return false;
        }
    }
    return true;
}

bool AttributeTree::ItemState( TreeRowItem* tree_item )
{
    vector < string > id_vec;
    bool item_state;

    if ( m_MapTreeFullIDs.count( tree_item ) )
    {
        id_vec = m_MapTreeFullIDs.at( tree_item );
    }
    item_state = ItemState( id_vec );
    return item_state;
}

void AttributeTree::SetState( const vector < string > & id_vec, bool open_state )
{
    if ( ItemState( id_vec ) != open_state )
    {
        ToggleState( id_vec );
    }
}

void AttributeTree::ToggleState( const vector < string > & id_vec )
{
    vector < vector < string > > :: iterator iter;
    if ( ItemState( id_vec ) )
    {
        m_CloseVec.push_back( id_vec );
    }
    else
    {
        iter = find( m_CloseVec.begin(), m_CloseVec.end(), id_vec );
        m_CloseVec.erase( iter );
    }
}

void AttributeTree::SetAllNodes( bool open_state )
{

    m_CloseVec.clear();
    if ( !open_state )
    {
        map < TreeRowItem*, vector < string > > :: iterator id_iter;
        for ( id_iter = m_MapTreeFullIDs.begin(); id_iter != m_MapTreeFullIDs.end(); id_iter ++ )
        {
            m_CloseVec.push_back( id_iter->second );
        }
    }
}

void AttributeTree::TrimCloseVec()
{
    bool vec_found;
    vector < vector < string > > close_vec_update;
    vector < vector < string > > open_vec_update;
    vector < string > open_vec;
    vector < string > open_vec_slice;

    string collid;
    string attachid;

    map < TreeRowItem*, vector < string > > :: iterator map_iter;

    for ( int j = 0; j != m_OpenBranchVec.size(); ++j )
        {
            collid = m_OpenBranchVec[j];
            attachid = AttributeMgr.GetObjectParent( m_OpenBranchVec[j] );
            //find the map tree item with that ID, and get its vector to that point...
            for ( map_iter = m_MapTreeFullIDs.begin(); map_iter != m_MapTreeFullIDs.end(); map_iter ++ )
            {
                //if the last item of the map vec == that of the open branch vec's ID, then we have id'd the vector to root.
                if ( map_iter->second.back() == attachid )
                {
                    open_vec = map_iter->second;
                    open_vec.push_back( collid );
                    open_vec_update.push_back( open_vec );
                }
            }
        }

    for ( int i = 0; i != m_CloseVec.size(); ++i )
    {
        vec_found = false;
        for ( map_iter = m_MapTreeFullIDs.begin(); map_iter != m_MapTreeFullIDs.end(); map_iter ++ )
        {
            if ( !CheckVecMatch( map_iter->second, m_CloseVec.at(i) ) )
            {
                // also if the map_iter doesn't contain any of the IDs from the OpenBranchVec...
                // need the item that corresponds to the OpenBranchVec's IDs->Root
                vec_found = true;
            }
        }

        for ( int j = 0; j != open_vec_update.size(); ++j )
        {
            // if the keep_open_vector is at least as long as the close_vector, then compare the vector of strings, cutting the open-vec to the length of the close_vec
            if ( open_vec_update.at(j).size() >= m_CloseVec.at(i).size() )
            {
                open_vec_slice = vector < string >( open_vec_update.at(j).begin(), open_vec_update.at(j).begin() + m_CloseVec.at(i).size() );
                if ( !CheckVecMatch( open_vec_slice, m_CloseVec.at(i) ) )
                {
                    vec_found = false;
                }
            }
        }

        if ( vec_found )
        {
            close_vec_update.push_back( m_CloseVec.at(i) );
        }
    }

    m_CloseVec = close_vec_update;
}

void AttributeTree::ClearExtraIDs()
{
    m_OpenBranchUpdate.clear();
    m_OpenBranchVec.clear();
}

// return pos int for failure states, 0 for perfect match
int AttributeTree::CheckVecMatch( const vector < string > & vec1, const vector < string > & vec2 )
{
    if ( vec1.size() != vec2.size() )
    {
        // fail type 1 = unequal length of vectors
        return 1;
    }
    string str1;
    string str2;

    for ( int i = 0; i != vec1.size(); ++i )
    {
        str1 = vec1.at(i);
        str2 = vec2.at(i);
        if ( str1.compare(str2) != 0 )
        {
            // fail type 2 = one of the internal strings doesn't match
            return 2;
        }
    }
    // if no failures detected, return 0 for exact match
    return 0;
}

void AttributeTree::SetAutoSelectID( const string & id )
{
    m_AutoSelectID = id;
}

void AttributeTree::SetTreeRootID( const string & attrCollectionID )
{
    m_AttrRootCollID = attrCollectionID;
    AttributeCollection* ac = AttributeMgr.GetCollectionPtr( m_AttrRootCollID );
    if ( ac )
    {
        AddEmptyCollID( m_AttrRootCollID );
    }
}

void AttributeTree::DeviceCB( Fl_Widget *w )
{
    m_TreeItem = static_cast < TreeRowItem* >( m_AttrTree->find_clicked() );

    // if item is clicked on collapse icon, toggle it and store that in the open/close vecs
    if ( m_TreeItem && m_TreeItem->event_on_collapse_icon( m_AttrTree->prefs() ) )
    {
        vector < string > id_vec;

        if ( m_MapTreeFullIDs.count( m_TreeItem ) )
        {
            id_vec = m_MapTreeFullIDs.at( m_TreeItem );
        }

        // if tree_item is in the openVec, then add it to the closed vec and remove it from openVec
        if ( id_vec.size() )
        {
            ToggleState( id_vec );
        }
    }

    SetTreeAttrID();

    if ( m_Screen )
    {
        m_Screen->CallBack( w );
    }
}

// ==== Attribute Editor ==== //
void AttributeEditor::Init( ScreenMgr* mgr, GroupLayout * layout, Fl_Group* group, VspScreen *screen, Fl_Callback *cb, bool mod_start, int start_y, int browser_h )
{
    GuiDevice::Init( screen );

    m_ScreenMgr = mgr;

    // ==== build layout ==== //
    layout->AddSubGroupLayout( m_AttrCommonGroup, layout->GetW(), layout->GetRemainY() );

    if ( mod_start )
    {
        m_AttrCommonGroup.SetY( start_y );
    }

    m_AttrCommonGroup.AddDividerBox( "Attribute List" );
    m_AttrTreeWidget.Init( mgr, layout, group, screen, cb, true, m_AttrCommonGroup.GetY(), browser_h );

    m_AttrCommonGroup.SetY( browser_h + m_AttrCommonGroup.GetY() - 15 );
    m_AttrCommonGroup.AddButton( m_AttributeExpandTrigger, "Open Attribute Explorer" );
    m_AttrCommonGroup.Show();

    // m_AttrCollID.clear();
}

void AttributeEditor::Show()
{
    Update();
}

void AttributeEditor::Update()
{
    m_AttrTreeWidget.Update();
}

void AttributeEditor::SetEditorCollID( string collID )
{
    m_AttrCollID = collID;
    m_AttrTreeWidget.SetTreeRootID( collID );
}

void AttributeEditor::DeviceCB( Fl_Widget* w )
{
    // for GUI events in the column browser:
    if ( w == m_AttrTreeWidget.GetTreeWidget() )
    {
        if ( Fl::event_clicks() != 0 )
        {
            string id = m_AttrTreeWidget.GetSelectedID();
            static_cast< AttributeExplorer* >( m_ScreenMgr->GetScreen( vsp::VSP_ATTRIBUTE_EXPLORER_SCREEN ) )->SetTreeAutoSelectID( id );
            m_ScreenMgr->ShowScreen( vsp::VSP_ATTRIBUTE_EXPLORER_SCREEN );
        }
    }
    m_ScreenMgr->SetUpdateFlag( true );
}

void AttributeEditor::GuiDeviceCallBack( GuiDevice* gui_device )
{
    if ( gui_device == &m_AttributeExpandTrigger )
    {
        string id = m_AttrTreeWidget.GetSelectedID();
        static_cast< AttributeExplorer* >( m_ScreenMgr->GetScreen( vsp::VSP_ATTRIBUTE_EXPLORER_SCREEN ) )->SetTreeAutoSelectID( id );
        m_ScreenMgr->ShowScreen( vsp::VSP_ATTRIBUTE_EXPLORER_SCREEN );
    }
    m_ScreenMgr->SetUpdateFlag( true );
}

bool AttributeEditor::canMakeInt( const::string& str )
{
    bool int_flag = false;
    try
    {
        stoi( str );
        int_flag = true;
    }
    catch ( ... ) {}
    return int_flag;
}

bool AttributeEditor::canMakeDbl( const::string& str )
{
    bool dbl_flag = false;
    try
    {
        stod( str );
        dbl_flag = true;
    }
    catch ( ... ) {}
    return dbl_flag;
}
