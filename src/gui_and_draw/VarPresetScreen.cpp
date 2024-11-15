//
// This file is released under the terms of the NASA Open Source Agreement (NOSA)
// version 1.3 as detailed in the LICENSE file which accompanies this software.
//

// ParmDebugScreen.cpp: implementation of the ParmDebugScreen class.
//
//////////////////////////////////////////////////////////////////////

#include "VarPresetScreen.h"
#include "ParmMgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VarPresetScreen::VarPresetScreen( ScreenMgr* mgr ) : TabScreen( mgr, 300, 600, "Variable Presets", "", (-6 * 20) - 25) // 6 rows * 20 StdHeight of UI objects, 25 Height of Tabs
{
    //==== Variables ====//
    m_NVarLast = 0;

    // Hidden Var Tree Tab until complete
    //Fl_Group* tree_tab = AddTab( "Var Tree" );
    //( (Vsp_Group*) tree_tab )->SetAllowDrop( true );
    //tree_tab->callback( staticScreenCB, this );

    Fl_Group* pick_tab = AddTab( "Create" );
    ( (Vsp_Group*) pick_tab )->SetAllowDrop( true );
    pick_tab->callback( staticScreenCB, this );

    Fl_Group* adj_tab = AddTab( "Adjust" );
    Fl_Group* pick_group = AddSubGroup( pick_tab, 5 );
    m_AdjustGroup = AddSubScroll( adj_tab, 5 );
    m_AdjustGroup->type( Fl_Scroll::VERTICAL_ALWAYS );

    // Set up change state persistent area at bottom/top? of GUI.
    m_ChangeStateLayout.SetGroupAndScreen( m_FLTK_Window, this );

    m_ChangeStateLayout.AddY(2 * m_ChangeStateLayout.GetGapHeight()
        + m_ChangeStateLayout.GetDividerHeight() );

    m_ChangeStateLayout.SetChoiceButtonWidth( m_ChangeStateLayout.GetButtonWidth() );

    m_ChangeStateLayout.AddDividerBox( "Select Preset" );
    m_ChangeStateLayout.AddChoice( m_GroupChoice, "Group" );
    m_ChangeStateLayout.AddChoice( m_SettingChoice, "Setting");
    m_ChangeStateLayout.AddButton( m_ApplyButton, "Apply" );
    m_ChangeStateLayout.AddYGap();

    m_ChangeStateLayout.AddDividerBox( "Edit" );

    m_ChangeStateLayout.SetButtonWidth( ( m_ChangeStateLayout.GetRemainX() ) / 2 );
    m_ChangeStateLayout.SetSameLineFlag( true );
    m_ChangeStateLayout.SetFitWidthFlag( false );
    m_ChangeStateLayout.AddButton( m_SaveButton , "Save Changes" );
    m_SaveButton.Deactivate();
    m_ChangeStateLayout.AddButton( m_DeleteButton, "Delete Current" );

    // Everything Relevant to "Pick" Tab
    m_PickLayout.SetGroupAndScreen( pick_group, this );

    m_PickLayout.AddDividerBox( "Create Preset" );
    m_PickLayout.SetSameLineFlag( true );
    m_PickLayout.AddInput( m_GroupInput, "Group", m_PickLayout.GetButtonWidth() );
    m_PickLayout.SetFitWidthFlag( false );
    m_PickLayout.AddButton( m_AddGroupButton, "Add" );
    m_PickLayout.ForceNewLine();

    m_PickLayout.SetFitWidthFlag( true );
    m_PickLayout.AddInput( m_SettingInput, "Setting", m_PickLayout.GetButtonWidth() );
    m_PickLayout.SetFitWidthFlag( false );
    m_PickLayout.AddButton( m_AddSettingButton, "Add" );
    m_PickLayout.ForceNewLine();
    m_PickLayout.SetSameLineFlag( false );
    m_PickLayout.SetFitWidthFlag( true );
    m_PickLayout.AddYGap();

    m_PickLayout.AddDividerBox( "Variable" );

    m_PickLayout.SetChoiceButtonWidth( m_PickLayout.GetButtonWidth() );

    m_PickLayout.AddParmPicker( m_ParmPicker );

    m_PickLayout.SetFitWidthFlag( false );
    m_PickLayout.SetSameLineFlag( true );

    m_PickLayout.AddYGap();

    m_PickLayout.SetButtonWidth( ( m_PickLayout.GetRemainX() ) / 2 );
    m_PickLayout.AddButton( m_AddVarButton, "Add Variable" );
    m_PickLayout.AddButton( m_DelVarButton, "Delete Variable" );

    m_PickLayout.ForceNewLine();

    m_PickLayout.SetFitWidthFlag( true );
    m_PickLayout.SetSameLineFlag( false );

    m_PickLayout.AddYGap();

    m_PickLayout.AddDividerBox( "Variable List" );

    // Pointer for the widths of each column in the browser to support resizing
    // Last column width must be 0
    static int col_widths[] = { 100, 100, 90, 0 }; // widths for each column

    int browser_h = 210;
    m_VarBrowser = m_PickLayout.AddColResizeBrowser( col_widths, 3, browser_h );
    m_VarBrowser->callback( staticScreenCB, this );

    pick_tab->show();

    // Everything Relevant to "Adjust" Tab
    m_AdjustLayout.SetGroupAndScreen( m_AdjustGroup, this );
}

VarPresetScreen::~VarPresetScreen()
{
}

bool VarPresetScreen::Update()
{
    TabScreen::Update();

    int i;
    char str[256];

    if ( OldVarPresetMgr.GetPresetVec().size() == 0 )
    {
        m_GroupChoice.Deactivate();
        m_SettingChoice.Deactivate();
        m_ApplyButton.Deactivate();
        m_DeleteButton.Deactivate();
        m_SettingInput.Deactivate();
        m_AddSettingButton.Deactivate();
        m_ParmPicker.Deactivate();
        m_AddVarButton.Deactivate();
        m_DelVarButton.Deactivate();
        m_VarBrowser->deactivate();
    }
    else if ( OldVarPresetMgr.GetDeleteFlag() == 1 )
    {
        m_GroupChoice.Activate();
        m_SettingChoice.Activate();
        m_ApplyButton.Activate();
        m_DeleteButton.Deactivate();
        m_SettingInput.Deactivate();
        m_AddSettingButton.Deactivate();
        m_ParmPicker.Deactivate();
        m_AddVarButton.Deactivate();
        m_DelVarButton.Deactivate();
        m_VarBrowser->deactivate();
    }
    else
    {
        if ( OldVarPresetMgr.GetNumSet() == 0 )
        {
            m_GroupChoice.Activate();
            m_DeleteButton.Activate();
            m_SettingInput.Activate();
            m_AddSettingButton.Activate();
            m_SettingChoice.Deactivate();
            m_ApplyButton.Deactivate();
            m_ParmPicker.Deactivate();
            m_AddVarButton.Deactivate();
            m_DelVarButton.Deactivate();
            m_VarBrowser->deactivate();
        }
        else
        {
            m_GroupChoice.Activate();
            m_SettingChoice.Activate();
            m_ApplyButton.Activate();
            m_DeleteButton.Activate();
            m_SettingInput.Activate();
            m_AddSettingButton.Activate();
            m_ParmPicker.Activate();
            m_AddVarButton.Activate();
            m_DelVarButton.Activate();
            m_VarBrowser->activate();
        }
    }

    // Predefined Parms
    m_GroupInput.Update( OldVarPresetMgr.GetActiveGroupText() );
    m_SettingInput.Update( OldVarPresetMgr.GetActiveSettingText() );

    // ==== Update Menus ==== //
    // This used to only update when a flag to update it was set or the
    // number of variable presets changed. This was not working so was simplified
    // to update the menus every iteration. The additional time is negligible and
    // this is consistent with updates for other menu items.
    RebuildMenus( OldVarPresetMgr.GetActiveGroupIndex() );

    if ( OldVarPresetMgr.GetPresetVec().empty() )
    {
        m_GroupChoice.ClearItems();
        m_GroupChoice.UpdateItems();
        m_SettingChoice.ClearItems();
        m_SettingChoice.UpdateItems();
    }

    // Check that all Parms exist.  Needed in case a Geom with DesVars is
    // deleted.
    OldVarPresetMgr.CheckVars();

    // Re-sort DesVars.  Needed in case a Geom's name is changed.
    if ( !OldVarPresetMgr.SortVars() )
    {
        RebuildAdjustTab();
    }

    //==== Update Parm Picker ====//
    m_ParmPicker.Update();

    //==== Update Parm Browser ====//
    int h_pos = m_VarBrowser->hposition();
    int v_pos = m_VarBrowser->vposition();
    m_VarBrowser->clear();

    m_VarBrowser->column_char( ':' );         // use : as the column character

    snprintf( str, sizeof( str ), "@b@.COMP:@b@.GROUP:@b@.PARM" );
    m_VarBrowser->add( str );

    int num_vars = OldVarPresetMgr.GetNumVars();
    for ( i = 0 ; i < num_vars ; i++ )
    {
        string parmid = OldVarPresetMgr.GetVar( i );

        string c_name, g_name, p_name;
        ParmMgr.GetNames( parmid, c_name, g_name, p_name );

        snprintf( str, sizeof( str ), "%s:%s:%s", c_name.c_str(), g_name.c_str(), p_name.c_str() );
        m_VarBrowser->add( str );
    }

    int index = OldVarPresetMgr.GetCurrVarIndex();
    if ( index >= 0 && index < num_vars )
    {
        m_VarBrowser->select( index + 2 );
    }

    m_VarBrowser->hposition( h_pos );
    m_VarBrowser->vposition( v_pos );

    // Parameter GUI got out of sync.  Probably from File->New or similar.
    if ( m_NVarLast != num_vars )
    {
        RebuildAdjustTab();
    }

    //==== Update Parm Adjust Tab ====//
    for ( i = 0 ; i < num_vars ; i++ )
    {
        m_ParmSliderVec[i].Update( OldVarPresetMgr.GetVar( i ) );
    }

    // ==== Check if Save Necessary ==== //
    CheckSaveStatus( OldVarPresetMgr.GetActiveGroupIndex(), OldVarPresetMgr.GetActiveSettingIndex() );

    m_FLTK_Window->redraw();

    return false;
}

void VarPresetScreen::RebuildAdjustTab()
{
    //printf( "Rebuild Adjust Tab ================================== \n" );
    m_AdjustGroup->clear();
    m_AdjustLayout.SetGroup( m_AdjustGroup );
    m_AdjustLayout.InitWidthHeightVals();

    m_ParmSliderVec.clear();

    unsigned int num_vars = OldVarPresetMgr.GetNumVars();
    m_ParmSliderVec.resize( num_vars );

    string lastContID;

    for ( int i = 0 ; i < num_vars ; i++ )
    {
        string pID = OldVarPresetMgr.GetVar( i );

        Parm* p = ParmMgr.FindParm( pID );

        string contID = p->GetContainerID();

        if ( contID.compare( lastContID ) != 0 )
        {
            lastContID = contID;
            m_AdjustLayout.AddDividerBox( ParmMgr.FindParmContainer( contID )->GetName() );
        }

        m_AdjustLayout.AddSlider( m_ParmSliderVec[i], "AUTO_UPDATE", 10, "%7.3f" );
        m_ParmSliderVec[i].Update( pID );
    }

    m_NVarLast = num_vars;

}

void VarPresetScreen::RebuildMenus( int g_index )
{
    //printf( "Rebuilding Menus ====================================== \n" );
    int s_index;
    vector < OldPreset > m_PresetVec = OldVarPresetMgr.GetPresetVec();

    m_GroupChoice.ClearItems();
    m_SettingChoice.ClearItems();

    if ( m_PresetVec.size() != 0 || g_index != -1 )
    {
        if ( g_index == -1 )
        {
            g_index = 0;
            s_index = 0;
        }
        else
        {
            s_index = OldVarPresetMgr.GetActiveSettingIndexFromGroup( g_index );
        }

        // Add Item to Apply Lists
        for ( int i = 0; i < m_PresetVec.size(); i++ )
        {
            m_GroupChoice.AddItem( m_PresetVec[i].GetGroupName() );
            if ( i ==  g_index )
            {
                for ( int j = 0; j < m_PresetVec[i].GetNumSet(); j++ )
                {
                    m_SettingChoice.AddItem( m_PresetVec[i].GetSettingName(j) );
                }
            }
        }

        m_SettingChoice.UpdateItems();
        m_SettingChoice.SetVal( s_index );

        m_GroupChoice.UpdateItems();
        m_GroupChoice.SetVal( g_index );
    }
}

void VarPresetScreen::CheckSaveStatus( int g_index, int s_index )
{
    //printf( "Checking Save Status ============================= \n" );
    // Compare Currently Saved Values with Current
    // if different notify user to save changes
    vector <OldPreset> m_PresetVec = OldVarPresetMgr.GetPresetVec();
    if ( g_index != -1 && s_index != -1 )
    {
        int num_vars = OldVarPresetMgr.GetNumVars();
        vector <double> p_val = m_PresetVec[ g_index ].GetParmVals( s_index );
        vector <string> p_IDs = m_PresetVec[ g_index ].GetParmIDs();
        for ( int i = 0; i < num_vars; i++ )
        {
            string pID = OldVarPresetMgr.GetVar( i );
            Parm* p = ParmMgr.FindParm( pID );
            for ( int j = 0; j < p_IDs.size(); j++ )
            {
                if ( pID.compare(p_IDs[j]) == 0 )
                {
                    if ( p->Get() != p_val[j] )
                    {
                        m_SaveButton.GetFlButton()->label( "*SAVE CHANGES*" );
                        m_SaveButton.Activate();
                        return;
                    }
                    else
                    {
                        m_SaveButton.Deactivate();
                        m_SaveButton.GetFlButton()->label( "Save Changes" );
                    }
                }
            }
        }
    }
}

void VarPresetScreen::Show()
{
    Update();
    TabScreen::Show();
}

void VarPresetScreen::Hide()
{
    m_FLTK_Window->hide();
}

void VarPresetScreen::CallBack( Fl_Widget* w )
{
    assert( m_ScreenMgr );

    if( Fl::event() == FL_PASTE || Fl::event() == FL_DND_RELEASE )
    {
        string ParmID( Fl::event_text() );
        if ( OldVarPresetMgr.GetDeleteFlag() )
        {
            return;
        }

        if ( OldVarPresetMgr.GetNumSet() != 0 )
        {
            if ( OldVarPresetMgr.CheckForDuplicateParm( ParmID ) )
            {
                return;
            }
            else
            {
                OldVarPresetMgr.AddVar( ParmID );
                OldVarPresetMgr.SavePreset();
                RebuildAdjustTab();
            }
        }
    }
    else if (  w == m_VarBrowser )
    {
        int sel = m_VarBrowser->value();
        OldVarPresetMgr.SetCurrVarIndex( sel - 2 );

        string parmid = OldVarPresetMgr.GetCurrVar();

        m_ParmPicker.SetParmChoice( parmid );
    }
}

void VarPresetScreen::GuiDeviceCallBack( GuiDevice* device )
{
    assert( m_ScreenMgr );

    if ( device == &m_AddVarButton )
    {
        OldVarPresetMgr.AddCurrVar( );
        OldVarPresetMgr.SavePreset();
    }
    else if ( device == &m_DelVarButton )
    {
        OldVarPresetMgr.DelCurrVar( );
        OldVarPresetMgr.SavePreset();
    }
    else if ( device == &m_ParmPicker )
    {
        OldVarPresetMgr.SetWorkingParmID( m_ParmPicker.GetParmChoice() );
        OldVarPresetMgr.SetCurrVarIndex( -1 );
    }

    //====Preset Edits Related ====//
    else if ( device == &m_GroupChoice )
    {
        OldVarPresetMgr.GroupChange( m_GroupChoice.GetVal() );
    }
    else if ( device == &m_SettingChoice )
    {
        OldVarPresetMgr.SettingChange( m_SettingChoice.GetVal() );
    }
    else if ( device == &m_AddGroupButton )
    {
        if ( !m_GroupInput.GetString().empty() )
        {
            // Get Strings from TextInput
            string groupText = m_GroupInput.GetString();
            OldVarPresetMgr.AddGroup( groupText );
        }
    }
    else if ( device == &m_AddSettingButton )
    {
        if ( !m_SettingInput.GetString().empty() )
        {
            // Get Strings from TextInput
            string settingText = m_SettingInput.GetString();
            OldVarPresetMgr.AddSetting( settingText );
        }
    }
    else if ( device == &m_DeleteButton )
    {
        if ( !OldVarPresetMgr.GetDeleteFlag() )
        {
            OldVarPresetMgr.DeletePreset( OldVarPresetMgr.GetActiveGroupIndex(), OldVarPresetMgr.GetActiveSettingIndex() );
            RebuildMenus( OldVarPresetMgr.GetActiveGroupIndex() );
        }
    }
    else if ( device == &m_ApplyButton )
    {
        if ( m_GroupChoice.GetVal() != -1 && m_SettingChoice.GetVal() != -1 )
        {
            if ( OldVarPresetMgr.GetActiveSettingIndex() == -1 && !OldVarPresetMgr.GetDeleteFlag() )
            {
                fl_alert( "Error: At least 1 setting required before switching groups." );
            }
            else
            {
                OldVarPresetMgr.GroupChange( m_GroupChoice.GetVal() );
                OldVarPresetMgr.SettingChange( m_SettingChoice.GetVal() );
                OldVarPresetMgr.ApplySetting();
            }
        }
    }
    else if ( device == &m_GroupChoice )
    {
        RebuildMenus( m_GroupChoice.GetVal() );
    }
    else if ( device == &m_SettingChoice )
    {
    }
    else if ( device == &m_SaveButton )
    {
        OldVarPresetMgr.SavePreset();
    }
    else
    {
        return;
    }

    m_ScreenMgr->SetUpdateFlag( true );
}
