//
// This file is released under the terms of the NASA Open Source Agreement (NOSA)
// version 1.3 as detailed in the LICENSE file which accompanies this software.
//

// AttributeEditor.h: Extensive attribute editor screen.
// Tim Cuatt
//
//////////////////////////////////////////////////////////////////////

#if !defined(ATTR_EDITOR_)
#define ATTR_EDITOR_

#include "GroupLayout.h"
#include "AttributeManager.h"
#include "TreeColumnWidget.h"

class ScreenMgr;

using std::string;

//==== Attribute Tree ====//
class AttributeTree : public GuiDevice
{
public:
    AttributeTree();
    ~AttributeTree();
    void Init( ScreenMgr* mgr, GroupLayout * layout, Fl_Group* group, VspScreen *screen, Fl_Callback *cb, bool mod_start = false, int start_y = 0, int browser_h = 150 );
    void Activate();
    void Deactivate();
    void Update();
    void UpdateTree();
    void SetTreeRootID( const string & attrCollectionID = "" );
    void SetTreeAttrID();
    void AddEmptyCollID( const string & coll_id );
    void AddEmptyCollID( const vector < string > & coll_ids );
    void SetEmptyCollID( const vector < string > & coll_ids );

    void SetSearchTerms( int attr_type, int attach_type,const string & attr_str, bool case_sens );

    bool ItemState( const vector < string > & local_id_vec );
    bool ItemState( TreeRowItem* tree_item );

    void SetState( const vector < string > & local_id_vec, bool open_state );
    void ToggleState( const vector < string > & local_id_vec );
    void SetAllNodes( bool open_state );
    void TrimCloseVec();

    void ClearExtraIDs();

    static int CheckVecMatch( const vector < string > & vec1, const vector < string > & vec2 );

    void SetAutoSelectID( const string & id );

    string GetSelectedID()
    {
        if ( m_SelectID.size() )
        {
            return m_SelectID.back();
        }
        return string();
    }

    string GetTreeAttrID()
    {
        return m_AttrID;
    }

    string GetTreeCollID()
    {
        return m_CollID;
    }

    string GetTreeRootCollID()
    {
        return m_AttrRootCollID;
    }

    Fl_Tree* GetTreeWidget()
    {
        return m_AttrTree;
    }

    void DeviceCB( Fl_Widget *w );

protected:

    string m_AutoSelectID;

    virtual void SetValAndLimits( Parm* p )                      {} // Do Nothing


    int m_ColWidths[4];

    int m_FilterAttrType;
    int m_FilterObjType;
    string m_FilterStr;
    bool m_FilterCase;

    GroupLayout m_TreeGroup;

    vector < string > m_SelectID; //build select ID from positively-ID'd anchor point (anything with an actual ID) and vector path from that one.
    string m_AttrID;
    string m_CollID;
    string m_AttrRootCollID;

    vector < string > m_OpenBranchUpdate;
    vector < string > m_OpenBranchVec;

    vector < vector < string > > m_CloseVec;  // id vector of open/closed local node vectors

    TreeWithColumns* m_AttrTree;

    VspScreen* m_Screen;
    ScreenMgr* m_ScreenMgr;

private:

    map< TreeRowItem*, string > m_MapTreeAttributes;
    map< TreeRowItem*, string > m_MapTreeCollections;
    map< TreeRowItem*, vector < string > > m_MapTreeFullIDs;

    TreeRowItem* m_TreeItem;
};

class AttributeEditor : public GuiDevice
{
public:
    // Initialize the member GUI devices and set their callbacks
    void Init( ScreenMgr* mgr, GroupLayout * layout, Fl_Group* group, VspScreen *screen, Fl_Callback *cb, bool mod_start = false, int start_y = 0, int browser_h = 150 );
    void Show();
    void Update();
    void SetEditorCollID( string collID = "" );
    string GetAttrCollID()
    {
        return m_AttrCollID;
    }

    void DeviceCB( Fl_Widget *w );
    void GuiDeviceCallBack( GuiDevice* gui_device );
    static void staticScreenCB( Fl_Widget *w, void* data )
    {
        ( ( AttributeEditor* )data )->DeviceCB( w );
    }

    static bool canMakeInt( const string& str );
    static bool canMakeDbl( const string& str );

    virtual void SetValAndLimits(){};

protected:

    virtual void SetValAndLimits( Parm* p )                      {} // Do Nothing

    AttributeTree m_AttrTreeWidget;

    GroupLayout m_AttrCommonGroup;
    GroupLayout m_AttrStrGroup;
    GroupLayout m_AttrToggleGroup;

    VspScreen* m_Screen;
    ScreenMgr* m_ScreenMgr;

private:

    string m_AttrCollID;

};

#endif