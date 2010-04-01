// a tree item (NOTE: this class is storage only, does not generate events)
class  wxTreeListItem
{
public:
    // ctors & dtor
    wxTreeListItem() { m_data = NULL; }
    wxTreeListItem( wxTreeListMainWindow *owner,
                    wxTreeListItem *parent,
                    const wxArrayString& text,
                    int image,
                    int selImage,
                    FbTreeItemData *data );

    ~wxTreeListItem();

    // trivial accessors
    wxArrayTreeListItems& GetChildren() { return m_children; }

    const wxString GetText() const
    {
        return GetText(0);
    }
    const wxString GetText (int column) const
    {
        if(m_text.GetCount() > 0)
        {
            if( IsVirtual() )   return m_owner->GetItemText( m_data, column );
            else                return m_text[column];
        }
        return wxEmptyString;
    }

    int GetImage (wxTreeItemIcon which = wxTreeItemIcon_Normal) const
        { return m_images[which]; }
    int GetImage (int column, wxTreeItemIcon which=wxTreeItemIcon_Normal) const
    {
        if(column == m_owner->GetMainColumn()) return m_images[which];
        if(column < (int)m_col_images.GetCount()) return m_col_images[column];
        return NO_IMAGE;
    }

    FbTreeItemData *GetData() const { return m_data; }

    // returns the current image for the item (depending on its
    // selected/expanded/whatever state)
    int GetCurrentImage() const;

    void SetText (const wxString &text );
    void SetText (int column, const wxString& text)
    {
        if (column < (int)m_text.GetCount()) {
            m_text[column] = text;
        }else if (column < m_owner->GetColumnCount()) {
            int howmany = m_owner->GetColumnCount();
            for (int i = m_text.GetCount(); i < howmany; ++i) m_text.Add (wxEmptyString);
            m_text[column] = text;
        }
    }
    void SetImage (int image, wxTreeItemIcon which) { m_images[which] = image; }
    void SetImage (int column, int image, wxTreeItemIcon which)
    {
        if (column == m_owner->GetMainColumn()) {
            m_images[which] = image;
        }else if (column < (int)m_col_images.GetCount()) {
            m_col_images[column] = image;
        }else if (column < m_owner->GetColumnCount()) {
            int howmany = m_owner->GetColumnCount();
            for (int i = m_col_images.GetCount(); i < howmany; ++i) m_col_images.Add (NO_IMAGE);
            m_col_images[column] = image;
        }
    }

    void SetData(FbTreeItemData *data) { m_data = data; }

    void SetHasPlus(bool has = true) { m_hasPlus = has; }

    void SetBold(bool bold) { m_isBold = bold; }

    int GetX() const { return m_x; }
    int GetY() const { return m_y; }

    void SetX (int x) { m_x = x; }
    void SetY (int y) { m_y = y; }

    int  GetHeight() const { return m_height; }
    int  GetWidth()  const { return m_width; }

    void SetHeight (int height) { m_height = height; }
    void SetWidth (int width) { m_width = width; }

    int GetTextX() const { return m_text_x; }
    void SetTextX (int text_x) { m_text_x = text_x; }

    wxTreeListItem *GetItemParent() const { return m_parent; }

    // operations
    // deletes all children
    void DeleteChildren();

    // get count of all children (and grand children if 'recursively')
    size_t GetChildrenCount(bool recursively = true) const;

    void Insert(wxTreeListItem *child, size_t index)
    { m_children.Insert(child, index); }

    void GetSize( int &x, int &y, const wxTreeListMainWindow* );

    // return the item at given position (or NULL if no item), onButton is
    // true if the point belongs to the item's button, otherwise it lies
    // on the button's label
    wxTreeListItem *HitTest (const wxPoint& point,
                             const wxTreeListMainWindow *,
                             int &flags, int& column, int level);

    void Expand() { m_isCollapsed = false; }
    void Collapse() { m_isCollapsed = true; }

    void SetHilight( bool set = true ) { m_hasHilight = set; }

    // status inquiries
    bool HasChildren() const { return !m_children.IsEmpty(); }
    bool IsSelected()  const { return m_hasHilight != 0; }
    bool IsExpanded()  const { return !m_isCollapsed; }
    bool HasPlus()     const { return m_hasPlus || HasChildren(); }
    bool IsBold()      const { return m_isBold != 0; }
    bool IsVirtual()   const { return m_owner->IsVirtual(); }

    // attributes
    // get them - may be NULL
    wxTreeItemAttr *GetAttributes() const { return m_attr; }
    // get them ensuring that the pointer is not NULL
    wxTreeItemAttr& Attr()
    {
        if ( !m_attr )
        {
            m_attr = new wxTreeItemAttr;
            m_ownsAttr = true;
        }
        return *m_attr;
    }
    // set them
    void SetAttributes(wxTreeItemAttr *attr)
    {
        if ( m_ownsAttr ) delete m_attr;
        m_attr = attr;
        m_ownsAttr = false;
    }
    // set them and delete when done
    void AssignAttributes(wxTreeItemAttr *attr)
    {
        SetAttributes(attr);
        m_ownsAttr = true;
    }

private:
    wxTreeListMainWindow  *m_owner;        // control the item belongs to

    // since there can be very many of these, we save size by chosing
    // the smallest representation for the elements and by ordering
    // the members to avoid padding.
    wxArrayString      m_text;    // labels to be rendered for item

    FbTreeItemData     *m_data;         // user-provided data

    wxArrayTreeListItems m_children; // list of children
    wxTreeListItem  *m_parent;       // parent of this item

    wxTreeItemAttr     *m_attr;         // attributes???

    // tree ctrl images for the normal, selected, expanded and
    // expanded+selected states
    short               m_images[wxTreeItemIcon_Max];
    wxArrayShort m_col_images; // images for the various columns (!= main)

    // main column item positions
    wxCoord             m_x;            // (virtual) offset from left (vertical line)
    wxCoord             m_y;            // (virtual) offset from top
    wxCoord             m_text_x;       // item offset from left
    short               m_width;        // width of this item
    unsigned char       m_height;       // height of this item

    // use bitfields to save size
    int                 m_isCollapsed :1;
    int                 m_hasHilight  :1; // same as focused
    int                 m_hasPlus     :1; // used for item which doesn't have
                                          // children but has a [+] button
    int                 m_isBold      :1; // render the label in bold font
    int                 m_ownsAttr    :1; // delete attribute when done
};

