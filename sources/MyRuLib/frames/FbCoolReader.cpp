#include "FbCoolReader.h"

#ifdef FB_INCLUDE_READER

#include <wx/dir.h>
#include <wx/power.h>
#include <wx/mstream.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <crengine.h>
#include "FbConst.h"
#include "FbParams.h"
#include "MyRuLibApp.h"

#define RENDER_TIMER_ID  123
#define CURSOR_TIMER_ID  125

//-----------------------------------------------------------------------------
//  FbContentModel
//-----------------------------------------------------------------------------

class FbContentData;

class FbContentModel: public FbTreeModel
{
	public: 
		FbContentModel( LVDocView * docView );
	private: 
		int m_page;
		friend class FbContentData;
};

class FbContentData: public FbParentData
{
	public:
		FbContentData(FbModel & model, FbParentData * parent, LVTocItem * item);
		LVTocItem * Get() const { return m_item; }
	public:
		virtual wxString GetValue(FbModel & model, size_t col) const;
		virtual bool IsBold(FbModel & model) const { return Count(model); }
	protected:
        LVTocItem * m_item;
	private:
		DECLARE_CLASS(FbContentData);
};

FbContentModel::FbContentModel( LVDocView * docView )
	: m_page(docView->getCurPage())
{
	m_root = new FbContentData(*this, NULL, docView->getToc());
	if (!m_position && m_root->Count(*this)) m_position = 1;
}

IMPLEMENT_CLASS(FbContentData, FbTreeModel)

FbContentData::FbContentData(FbModel & model, FbParentData * parent, LVTocItem * item)
	: FbParentData(model, parent)
	, m_item(item)
{
	FbContentModel & owner = (FbContentModel&) model;
    int page = item->getPage();
    if ( parent && page <= owner.m_page ) {
		owner.m_position++;
	}

	int count = item->getChildCount();
    for ( int i = 0; i < count; i++ ) {
		LVTocItem * child = item->getChild(i);
		if (child->getName().empty()) continue;
		new FbContentData(model, this, child);
    }
}

wxString FbContentData::GetValue(FbModel & model, size_t col) const
{
	switch (col) {
		case  0: return m_item->getName().c_str();
		case  1: return Format(m_item->getPage());
		default: return wxEmptyString;
	}
}

//-----------------------------------------------------------------------------
//  FbCoolReader::ContentDlg
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE( FbCoolReader::ContentDlg, wxDialog )
	EVT_TREE_ITEM_ACTIVATED(wxID_ANY, FbCoolReader::ContentDlg::OnActivated)
END_EVENT_TABLE()

FbCoolReader::ContentDlg::ContentDlg( wxWindow* parent, const wxString& title )
	: wxDialog( parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE  | wxRESIZE_BORDER )
{
	SetMinSize(wxSize(400, 300));
	wxBoxSizer * sizer = new wxBoxSizer( wxVERTICAL );
	
	m_treeview.Create( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|fbTR_VRULES);
	m_treeview.AddColumn(0, _("Title"), 300, wxALIGN_LEFT);
	m_treeview.AddColumn(1, _("Page"), 50, wxALIGN_RIGHT);
	sizer->Add( &m_treeview, 1, wxALL|wxEXPAND, 5 );
	
	wxStdDialogButtonSizer * sdbSizerBtn = CreateStdDialogButtonSizer( wxOK | wxCANCEL );
	sizer->Add( sdbSizerBtn, 0, wxEXPAND | wxALL, 5 );

	this->SetSizer( sizer );
	this->Layout();
	sizer->Fit( this );

	m_treeview.SetFocus();

	this->Centre( wxBOTH );
}

void FbCoolReader::ContentDlg::Assign( LVDocView * docView )
{
	SetTitle(docView->getTitle().c_str());
	FbContentModel * model = new FbContentModel(docView);
	m_treeview.AssignModel(model);
	int pos = model->GetPosition() > 2 ? model->GetPosition() - 2 : 0;
	m_treeview.SetScrollPos(pos);
}

LVTocItem * FbCoolReader::ContentDlg::GetSelection()
{
	FbModelItem current = m_treeview.GetCurrent();
	FbContentData * data = wxDynamicCast(&current, FbContentData);
	return data ? data->Get() : NULL;
}

void FbCoolReader::ContentDlg::OnActivated( wxTreeEvent & event )
{
	EndModal(wxID_OK);
}

//-----------------------------------------------------------------------------
//  FbCoolReader
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(FbCoolReader, wxWindow)

BEGIN_EVENT_TABLE( FbCoolReader, wxWindow )
	EVT_ERASE_BACKGROUND(FbCoolReader::OnEraseBackground)
	EVT_PAINT( FbCoolReader::OnPaint )
	EVT_SIZE( FbCoolReader::OnSize )
	EVT_KEY_DOWN( FbCoolReader::OnKeyDown )
	EVT_MOUSEWHEEL( FbCoolReader::OnMouseWheel )
	EVT_LEFT_DOWN( FbCoolReader::OnMouseLDown )
	EVT_RIGHT_DOWN( FbCoolReader::OnMouseRDown )
	EVT_MOTION( FbCoolReader::OnMouseMotion )
	EVT_TIMER(RENDER_TIMER_ID, FbCoolReader::OnTimer)
	EVT_TIMER(CURSOR_TIMER_ID, FbCoolReader::OnTimer)
	EVT_MENU( Menu_File_About, FbCoolReader::OnAbout )
	EVT_MENU( wxID_OPEN, FbCoolReader::OnFileOpen )
	EVT_MENU( wxID_SAVE, FbCoolReader::OnFileSave )
	EVT_MENU( ID_READER_CONTENT, FbCoolReader::OnShowContent )
	EVT_MENU( ID_READER_ZOOM_IN, FbCoolReader::OnCommand )
	EVT_MENU( ID_READER_ZOOM_OUT, FbCoolReader::OnCommand )
	EVT_MENU( Menu_View_Rotate, FbCoolReader::OnRotate )
	EVT_INIT_DIALOG( FbCoolReader::OnInitDialog )
	EVT_SCROLLWIN( FbCoolReader::OnScroll )
END_EVENT_TABLE()

#ifdef __WXMSW__

#include <wx/msw/private.h>

void LoadStylesheet(lString8 &css)
{
	HRSRC hResource = ::FindResource(wxGetInstance(), wxT("fb2_css"), wxT("RC_DATA"));
	if ( !hResource ) return;

	HGLOBAL hData = ::LoadResource(wxGetInstance(), hResource);
	if ( !hData ) return;

	void * data = ::LockResource(hData);
	if ( !data ) return;

	unsigned long size = ::SizeofResource(wxGetInstance(), hResource);

	css = lString8( (lString8::value_type*) data, size );
}

#else

void LoadStylesheet(lString8 &css)
{
	#include "fb2_css.inc"
	css = lString8( (lString8::value_type*) locale_binary_file, sizeof(locale_binary_file) );
}

#endif

#if USE_FREETYPE

class FbFontRegistrator: public wxDirTraverser
{
public:
	virtual wxDirTraverseResult OnFile(const wxString& filename) {
		wxString ext = filename.AfterLast(wxT('.')).Lower();
		if (ext == wxT("ttf") || ext == wxT("otf") || ext == wxT("pfa") || ext == wxT("pfb")) {
			lString8 fn = UnicodeToLocal(filename.c_str());
			CRLog::trace("loading font: %s", fn.c_str());
			if ( !fontMan->RegisterFont(fn) ) {
				CRLog::trace("    failed\n");
			}
		}
		return wxDIR_CONTINUE;
	}
	virtual wxDirTraverseResult OnDir(const wxString& WXUNUSED(dirname)) {
		return wxDIR_CONTINUE;
	}
};

bool FbCoolReader::InitCREngine()
{
	if ( fontMan ) return fontMan->GetFontCount();
	CRLog::setStdoutLogger();
	CRLog::trace("InitCREngine");
	HyphMan::initDictionaries( lString16() );
	InitFontManager( lString8() );
	
	FbFontRegistrator registrator;
#ifdef __WXMSW__
	wxDir(wxGetOSDirectory() + wxT("\\Fonts\\")).Traverse(registrator);
#else // __WXMSW__
	wxDir(wxT("/usr/local/share/fonts/")).Traverse(registrator);
	wxDir(wxT("/usr/share/fonts/")).Traverse(registrator);
	wxDir(wxT("~/.fonts")).Traverse(registrator);
	wxDir(wxT("~/fonts")).Traverse(registrator);
#endif // __WXMSW__

	int count = fontMan->GetFontCount();
	if (count) {
		printf("%d fonts loaded.\n", count);
	} else {
		printf("Fatal Error: Cannot open font file(s) .ttf \nCannot work without font\n" );
	}

	return count;
}

#else // USE_FREETYPE

bool FbCoolReader::InitCREngine()
{
	if ( fontMan ) return fontMan->GetFontCount();
	CRLog::setStdoutLogger();
	CRLog::trace("InitCREngine");
	HyphMan::initDictionaries( lString16() );
	return InitFontManager( lString8() );
}

#endif // USE_FREETYPE

void FbCoolReader::GetFonts(wxArrayString & fonts)
{
	if (!InitCREngine()) return;
	lString16Collection list;
	fontMan->getFaceList(list);

	fonts.Empty();
	size_t count = list.length();
	for (size_t i = 0; i < count; i++) {
		fonts.Add(list[i].c_str());
	}
}

FbCoolReader * FbCoolReader::Open(wxAuiNotebook * parent, const wxString &filename, bool select)
{
	if (!InitCREngine()) {
		return NULL;
	} else {
		FbCoolReader * reader = new FbCoolReader(parent);
		bool ok = reader->LoadDocument(filename);
		if (ok) {
			reader->_wm.activateWindow( reader->_docwin );
			wxString title = reader->GetDocView()->getTitle().c_str();
			parent->AddPage(reader, TrimTitle(title), select );
		} else {
			wxDELETE(reader);
		}
		return reader;
	}
}

FbCoolReader::FbCoolReader(wxAuiNotebook * parent)
	: wxWindow(parent, ID_FRAME_READ, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxFULL_REPAINT_ON_RESIZE | wxTAB_TRAVERSAL | wxWANTS_CHARS)
	, _renderTimer( this, RENDER_TIMER_ID )
	, _cursorTimer( this, CURSOR_TIMER_ID )
	, _normalCursor(wxCURSOR_ARROW)
	, _linkCursor(wxCURSOR_HAND)
	, _firstRender(false)
	, _allowRender(true)
	, _screen(300,400)
	, _wm(&_screen)
	, _docwin(new CRDocViewWindow(&_wm))
{
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
	SetScrollbar(wxVERTICAL, 0, 1, 100, false);
	Setup(false);
}

FbCoolReader::~FbCoolReader()
{
}

void FbCoolReader::Setup(bool refresh)
{
	GetDocView()->setPageMargins( lvRect(12, 12, 12, 6) );

	lString8 css;
	LoadStylesheet(css);
	GetDocView()->setStyleSheet( css );

	static int fontSizes[] = {14, 16, 18, 20, 24, 28, 32, 36};
	LVArray<int> sizes( fontSizes, sizeof(fontSizes)/sizeof(int) );
	GetDocView()->setFontSizes( sizes, false );
	GetDocView()->SetRotateAngle( CR_ROTATE_ANGLE_0 );

	int flags = PGHDR_AUTHOR | PGHDR_TITLE | PGHDR_PAGE_NUMBER | PGHDR_PAGE_COUNT | PGHDR_CHAPTER_MARKS;
	GetDocView()->setPageHeaderInfo( flags );

	fontMan->SetAntialiasMode( 2 );

	GetDocView()->setDefaultFontFace( UnicodeToUtf8(FbParams(FB_READER_FONT_NAME).Str().c_str()) );
	GetDocView()->setStatusFontFace ( UnicodeToUtf8(FbParams(FB_HEADER_FONT_NAME).Str().c_str()) );
	GetDocView()->setFontSize       ( FbParams(FB_READER_FONT_SIZE) );
	GetDocView()->setStatusFontSize ( FbParams(FB_HEADER_FONT_SIZE) );
	GetDocView()->setTextColor      ( (int)FbParams(FB_READER_FONT_COLOUR) );
	GetDocView()->setBackgroundColor( (int)FbParams(FB_READER_BACK_COLOUR) );
	GetDocView()->setStatusColor    ( (int)FbParams(FB_HEADER_FONT_COLOUR) );
	GetDocView()->setDefaultInterlineSpace(FbParams(FB_READER_INTERLINE));
	GetDocView()->setViewMode(DVM_PAGES);
	SetBackgroundColour(getBackgroundColour());
  
	if (refresh) Refresh();
}

wxColour FbCoolReader::getBackgroundColour()
{
#if (COLOR_BACKBUFFER==1)
	lUInt32 cl = GetDocView()->getBackgroundColor();
#else
	lUInt32 cl = 0xFFFFFF;
#endif
	wxColour wxcl( (cl>>16)&255, (cl>>8)&255, (cl>>0)&255 );
	return wxcl;
}

void FbCoolReader::OnTimer(wxTimerEvent& event)
{
	//printf("FbCoolReader::OnTimer() \n");
	if ( event.GetId() == RENDER_TIMER_ID ) {
		int dx;
		int dy;
		GetClientSize( &dx, &dy );
		//if ( _docview->IsRendered() && dx == _docview->GetWidth()
		//        && dy == _docview->GetHeight() )
		//    return; // no resize
		if (dx<50 || dy<50 || dx>3000 || dy>3000)
		{
			return;
		}

		if ( _firstRender ) {
			GetDocView()->restorePosition();
			_firstRender = false;
			_allowRender = true;
		}

		_wm.reconfigure( dx, dy, CR_ROTATE_ANGLE_0 );

		Paint();
		UpdateScrollBar();
	} else if ( event.GetId() == CURSOR_TIMER_ID ) {
		SetCursor( wxCursor( wxCURSOR_BLANK ) );
	}
}

void FbCoolReader::Paint()
{
	UpdateScrollBar();
	Refresh( FALSE );
}

void FbCoolReader::UpdateScrollBar()
{
	if ( !GetDocView()->IsRendered() ) return;
	const LVScrollInfo * lvsi = GetDocView()->getScrollInfo();

	int pos = GetScrollPos(wxVERTICAL);
	int size = GetScrollThumb(wxVERTICAL);
	int range = GetScrollRange(wxVERTICAL);

	if (
		(pos != lvsi->pos) ||
		(size != lvsi->pagesize) ||
		(range != lvsi->maxpos + lvsi->pagesize)
		) {
		SetScrollbar(
			wxVERTICAL,
			lvsi->pos,      //int position,
			lvsi->pagesize, //int thumbSize,
			lvsi->maxpos + lvsi->pagesize,   //int range,
			true//const bool refresh = true
		);
	}
/*
	wxStatusBar * sb = ((wxControl*)GetParent())->GetStatusBar();
	if ( sb )
		sb->SetStatusText( wxString( lvsi->posText.c_str() ), 1 );
*/
}

void FbCoolReader::OnMouseMotion(wxMouseEvent& event)
{
	int x = event.GetX();
	int y = event.GetY();
	ldomXPointer ptr = GetDocView()->getNodeByPoint( lvPoint( x, y ) );
	if ( ptr.isNull() ) {
		return;
	}
	lString16 href = ptr.getHRef();
	if ( href.empty() ) {
		SetCursor(_normalCursor);
	} else {
		SetCursor(_linkCursor);
	}

	SetCursor( wxNullCursor );
}

void FbCoolReader::OnMouseLDown( wxMouseEvent & event )
{
	int x = event.GetX();
	int y = event.GetY();
	//lString16 txt = _docview->getPageText( true );
	//CRLog::debug( "getPageText : %s", UnicodeToUtf8(txt).c_str() );
	ldomXPointer ptr = GetDocView()->getNodeByPoint( lvPoint( x, y ) );
	if ( ptr.isNull() ) {
		CRLog::debug( "FbCoolReader::OnMouseLDown() : node not found!\n");
		return;
	}
	lString16 href = ptr.getHRef();
	if ( ptr.getNode()->isText() ) {
		lString8 s = UnicodeToUtf8( ptr.toString() );
		CRLog::debug("Text node clicked (%d, %d): %s", x, y, s.c_str() );
		ldomXRange * wordRange = new ldomXRange();
		if ( ldomXRange::getWordRange( *wordRange, ptr ) ) {
			wordRange->setFlags( 0x10000 );
			GetDocView()->getDocument()->getSelections().clear();
			GetDocView()->getDocument()->getSelections().add( wordRange );
			GetDocView()->updateSelections();
		} else {
			delete wordRange;
		}
		if ( !href.empty() ) {
			GetDocView()->goLink( href );
		}
		Paint();
		printf("text : %s     \t", s.c_str() );
	} else {
		printf("element : %s  \t", UnicodeToUtf8( ptr.toString() ).c_str() );
	}
	lvPoint pt2 = ptr.toPoint();
	printf("  (%d, %d)  ->  (%d, %d)\n", x, y+GetDocView()->GetPos(), pt2.x, pt2.y);
}

FbCoolReader::MenuBook::MenuBook()
{ 
	Append( wxID_OPEN, wxT( "&Open...\tCtrl+O" ) );
	Append( Menu_View_History, wxT( "Recent books list\tF4" ) );
	Append( wxID_SAVE, wxT( "&Save...\tCtrl+S" ) );
	AppendSeparator();
	Append( ID_READER_CONTENT, (wxString)_("Table of Contents") + wxT("\tF5" ) );
	Append( ID_READER_OPTIONS, (wxString)_("Cool Reader options") + wxT( "\tF9" ) );
	AppendSeparator();
	Append( ID_READER_ZOOM_IN, wxT( "Zoom In" ) );
	Append( ID_READER_ZOOM_OUT, wxT( "Zoom Out" ) );
	AppendSeparator();
	Append( Menu_View_ToggleFullScreen, wxT( "Toggle Fullscreen\tAlt+Enter" ) );
	Append( Menu_View_TogglePages, wxT( "Toggle Pages/Scroll\tCtrl+P" ) );
	Append( Menu_View_TogglePageHeader, wxT( "Toggle page heading\tCtrl+H" ) );
};

void FbCoolReader::OnMouseRDown( wxMouseEvent & event )
{
	MenuBook menu;
	PopupMenu(&menu);
}

void FbCoolReader::ToggleViewMode()
{
	LVDocViewMode new_mode = GetDocView()->getViewMode() == DVM_PAGES ? DVM_SCROLL : DVM_PAGES;
	GetDocView()->setViewMode( new_mode );
	UpdateScrollBar();
	Paint();
}

void FbCoolReader::OnCommand(wxCommandEvent& event)
{
	switch ( event.GetId() ) {
	case ID_READER_ZOOM_IN:
		{
			wxCursor hg( wxCURSOR_WAIT );
			this->SetCursor( hg );
			wxSetCursor( hg );
			//===========================================
			doCommand( DCMD_ZOOM_IN, 0 );
			//===========================================
			this->SetCursor( wxNullCursor );
			wxSetCursor( wxNullCursor );
		}
		break;
	case ID_READER_ZOOM_OUT:
		{
			wxCursor hg( wxCURSOR_WAIT );
			this->SetCursor( hg );
			wxSetCursor( hg );
			//===========================================
			doCommand( DCMD_ZOOM_OUT, 0 );
			//===========================================
			this->SetCursor( wxNullCursor );
			wxSetCursor( wxNullCursor );
		}
		break;
	case Menu_View_NextPage:
		doCommand( DCMD_PAGEDOWN, 1 );
		GetDocView()->cachePageImage( 0 );
		GetDocView()->cachePageImage( 1 );
		break;
	case Menu_Link_Forward:
		doCommand( DCMD_LINK_FORWARD, 1 );
		break;
	case Menu_Link_Back:
		doCommand( DCMD_LINK_BACK, 1 );
		break;
	case Menu_Link_Next:
		doCommand( DCMD_LINK_NEXT, 1 );
		break;
	case Menu_Link_Prev:
		doCommand( DCMD_LINK_PREV, 1 );
		break;
	case Menu_Link_Go:
		doCommand( DCMD_LINK_GO, 1 );
		break;
	case Menu_View_PrevPage:
		doCommand( DCMD_PAGEUP, 1 );
		GetDocView()->cachePageImage( 0 );
		GetDocView()->cachePageImage( -1 );
		break;
	case Menu_View_NextLine:
		doCommand( DCMD_LINEDOWN, 1 );
		break;
	case Menu_View_PrevLine:
		doCommand( DCMD_LINEUP, 1 );
		break;
	case Menu_View_Begin:
		doCommand( DCMD_BEGIN, 0 );
		break;
	case Menu_View_End:
		doCommand( DCMD_END, 0 );
		break;
	case Menu_View_TogglePages:
		ToggleViewMode();
		break;
	case Menu_View_Text_Format:
		doCommand( DCMD_TOGGLE_TEXT_FORMAT, 0 );
		break;
	}
}

void FbCoolReader::OnScroll(wxScrollWinEvent& event)
{
	wxEventType type = event.GetEventType();
	if (type == wxEVT_SCROLLWIN_TOP      ) doCommand( DCMD_BEGIN,    0 ); else
	if (type == wxEVT_SCROLLWIN_BOTTOM   ) doCommand( DCMD_END,      0 ); else
	if (type == wxEVT_SCROLLWIN_LINEUP   ) doCommand( DCMD_LINEUP,   0 ); else
	if (type == wxEVT_SCROLLWIN_LINEDOWN ) doCommand( DCMD_LINEDOWN, 0 ); else
	if (type == wxEVT_SCROLLWIN_PAGEUP   ) doCommand( DCMD_PAGEUP,   0 ); else
	if (type == wxEVT_SCROLLWIN_PAGEDOWN ) doCommand( DCMD_PAGEDOWN, 0 ); else

	if ((type == wxEVT_SCROLLWIN_THUMBTRACK)||(type == wxEVT_SCROLLWIN_THUMBRELEASE)) {
		doCommand( DCMD_GO_POS, GetDocView()->scrollPosToDocPos(event.GetPosition()) );
	}
}

void FbCoolReader::OnMouseWheel(wxMouseEvent& event)
{
	int rotation = event.GetWheelRotation();
	if ( rotation > 0 ) doCommand( DCMD_LINEUP, 3 ); else
	if ( rotation < 0 ) doCommand( DCMD_LINEDOWN, 3 );
}

void FbCoolReader::OnKeyDown(wxKeyEvent& event)
{
	switch (event.GetKeyCode()) {
		case WXK_NUMPAD_ADD:      doCommand( DCMD_ZOOM_IN,  0 ); break;
		case WXK_NUMPAD_SUBTRACT: doCommand( DCMD_ZOOM_OUT, 0 ); break;
		case WXK_UP:              doCommand( DCMD_LINEUP,   1 ); break;
		case WXK_DOWN:            doCommand( DCMD_LINEDOWN, 1 ); break;
		case WXK_BACK:
		case WXK_PAGEUP:          doCommand( DCMD_PAGEUP,   1 ); break;
		case WXK_SPACE:
		case WXK_PAGEDOWN:        doCommand( DCMD_PAGEDOWN, 1 ); break;
		case WXK_HOME:            doCommand( DCMD_BEGIN,    0 ); break;
		case WXK_END:             doCommand( DCMD_END,      0 ); break;
		default: event.Skip();
	}
}

bool FbCoolReader::LoadDocument( const wxString & fname )
{
	_renderTimer.Stop();

	GetDocView()->savePosition();
	GetDocView()->Clear();

	wxCursor hg( wxCURSOR_WAIT );
	this->SetCursor( hg );
	wxSetCursor( hg );
	//===========================================
	//printf("   loading...  ");
	bool res = GetDocView()->LoadDocument( fname.c_str() );
	//printf("   done. \n");
	//DEBUG
	//_docview->exportWolFile( "test.wol", true );
	//_docview->SetPos(0);
	if ( !res )
		GetDocView()->createDefaultDocument(lString16(L"File open error"), lString16(L"Cannot open file ") + fname.c_str() );
	lString16 title = GetDocView()->getAuthors();
	if ( !title.empty() && !GetDocView()->getTitle().empty() )
		title << L". ";
	title << GetDocView()->getTitle();

	//UpdateScrollBar();
	_firstRender = true;
	_allowRender = false;
	ScheduleRender();
	//_docview->restorePosition();
	//_docview->Render();
	//UpdateScrollBar();
	//Paint();
	//===========================================
	wxSetCursor( wxNullCursor );
	this->SetCursor( wxNullCursor );
	return res;
}

void FbCoolReader::goToBookmark(ldomXPointer bm)
{
	GetDocView()->goToBookmark(bm);
	UpdateScrollBar();
	Paint();
}

void FbCoolReader::SetRotate( cr_rotate_angle_t angle )
{
	GetDocView()->SetRotateAngle( angle );
	UpdateScrollBar();
	Paint();
}

void FbCoolReader::Rotate( bool ccw )
{
	int angle = (GetDocView()->GetRotateAngle() + 4 + (ccw?-1:1)) & 3;
	SetRotate( (cr_rotate_angle_t) angle );
}

void FbCoolReader::doCommand( LVDocCmd cmd, int param )
{
	_docwin->onCommand( cmd, param );
	UpdateScrollBar();
	Paint();
}

void FbCoolReader::Resize(int dx, int dy)
{
	//printf("   Resize(%d,%d) \n", dx, dy );
	if ( dx==0 && dy==0 ) {
		GetClientSize( &dx, &dy );
	}
	if ( GetDocView()->IsRendered() && GetDocView()->GetWidth() == dx && GetDocView()->GetHeight() == dy )
		return; // no resize
	if (dx<5 || dy<5 || dx>3000 || dy>3000)
	{
		return;
	}

	_renderTimer.Stop();
	_renderTimer.Start( 100, wxTIMER_ONE_SHOT );

	SetCursor( wxNullCursor );
}

void FbCoolReader::OnPaint(wxPaintEvent& event)
{
	//printf("   OnPaint()  \n" );
	wxPaintDC dc(this);
	if ( !_allowRender ) {
		dc.Clear();
		return;
	}

	int dx, dy;
	GetClientSize( &dx, &dy );
	if ( !GetDocView()->IsRendered() && (GetDocView()->GetWidth() != dx || GetDocView()->GetHeight() != dy) ) {
		if ( _firstRender ) {
			GetDocView()->restorePosition();
			_firstRender = false;
		}

		GetDocView()->Resize( dx, dy );
		return;
	}

	// draw
	_wm.update( true );
	wxBitmap bmp = _screen.getWxBitmap();
	dc.DrawBitmap( bmp, 0, 0, false );
}

void FbCoolReader::OnSize(wxSizeEvent& event)
{
	wxSize size = GetClientSize();
	Resize( size.x, size.y );
}

// LVDocViewCallback override
void FbCoolReader::OnExternalLink( lString16 url, ldomNode * node )
{
	::wxLaunchDefaultBrowser( wxString(url.c_str()) );
}

void testHyphen( const char * str )
{
	lString16 s16 = Utf8ToUnicode( lString8(str) );
	lUInt16 widths[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
	lUInt8 flags[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	HyphMan::hyphenate( s16.c_str(), s16.length(), widths, flags, 1, 15 );
	lString8 buf( str );
	buf << " = ";
	for ( unsigned i=0; i<s16.length(); i++ ) {
		buf << str[i];
		if ( flags[i] & LCHAR_ALLOW_HYPH_WRAP_AFTER )
			buf << '-';
	}
	printf("%s\n", buf.c_str());
}

/*
void FbCoolReader::OnHistItemActivated( wxListEvent& event )
{
	long index = event.GetIndex();
	if ( index == 0 && _view->GetDocView()->isDocumentOpened()) {
		SetActiveMode( am_book );
		return;
	}
	if ( index>=0 && index<_view->GetDocView()->getHistory()->getRecords().length() ) {
		lString16 pathname = _view->GetDocView()->getHistory()->getRecords()[index]->getFilePath() +
			_view->GetDocView()->getHistory()->getRecords()[index]->getFileName();
		if ( !pathname.empty() ) {
			Update();
			SetActiveMode( am_book );
			_view->LoadDocument( wxString( pathname.c_str()) );
			UpdateToolbar();
		}
	}
}
*/

lString8 readFileToString( const char * fname )
{
	lString8 buf;
	LVStreamRef stream = LVOpenFileStream(fname, LVOM_READ);
	if (!stream)
		return buf;
	int sz = stream->GetSize();
	if (sz>0)
	{
		buf.insert( 0, sz, ' ' );
		stream->Read( buf.modify(), sz, NULL );
	}
	return buf;
}

void FbCoolReader::OnInitDialog(wxInitDialogEvent& event)
{
	// stylesheet can be placed to file fb2.css
	// if not found, default stylesheet will be used
	//char cssfn[1024];
	//sprintf( cssfn, "fb2.css"); //, exedir
	//lString8 css = readFileToString( (UnicodeToLocal(_appDir) + cssfn).c_str() );
	lString8 css;
	LVLoadStylesheetFile( _appDir + L"fb2.css", css );
#ifdef _LINUX
	if ( css.empty() )
		LVLoadStylesheetFile( L"/usr/share/cr3/fb2.css", css );
		//css = readFileToString( "/usr/share/crengine/fb2.css" );
	if ( css.empty() )
		LVLoadStylesheetFile( L"/usr/local/share/cr3/fb2.css", css );
		//css = readFileToString( "/usr/local/share/crengine/fb2.css" );
#endif
	if (css.length() > 0)
	{
		printf("Style sheet file found.\n");
		GetDocView()->setStyleSheet( css );
	}

	wxAcceleratorEntry entries[40];
	int a=0;
	entries[a++].Set(wxACCEL_CTRL,  (int) 'O',     wxID_OPEN);
	entries[a++].Set(wxACCEL_CTRL,  (int) 'S',     wxID_SAVE);
	entries[a++].Set(wxACCEL_CTRL,  (int) 'P',     Menu_View_TogglePages);
	entries[a++].Set(wxACCEL_CTRL,  (int) 'H',     Menu_View_TogglePageHeader);
	entries[a++].Set(wxACCEL_CTRL,  (int) 'R',     Menu_View_Rotate);
	entries[a++].Set(wxACCEL_NORMAL,  WXK_F3,      wxID_OPEN);
	entries[a++].Set(wxACCEL_NORMAL,  WXK_F2,      wxID_SAVE);
	entries[a++].Set(wxACCEL_NORMAL,  WXK_UP,      Menu_View_PrevLine);
	entries[a++].Set(wxACCEL_NORMAL,  WXK_DOWN,    Menu_View_NextLine);
	entries[a++].Set(wxACCEL_NORMAL,  WXK_SPACE,    Menu_View_NextLine);
	entries[a++].Set(wxACCEL_NORMAL,  WXK_NUMPAD_ADD,      Menu_View_ZoomIn);
	entries[a++].Set(wxACCEL_NORMAL,  WXK_NUMPAD_SUBTRACT, Menu_View_ZoomOut);
	entries[a++].Set(wxACCEL_NORMAL,  WXK_ADD,      Menu_View_ZoomIn);
	entries[a++].Set(wxACCEL_NORMAL,  WXK_SUBTRACT, Menu_View_ZoomOut);
	entries[a++].Set(wxACCEL_CTRL,    WXK_NUMPAD_ADD,      Menu_View_ZoomIn);
	entries[a++].Set(wxACCEL_CTRL,    WXK_NUMPAD_SUBTRACT, Menu_View_ZoomOut);
	entries[a++].Set(wxACCEL_NORMAL,  (int) '+',     Menu_View_ZoomIn);
	entries[a++].Set(wxACCEL_NORMAL,  (int) '-',     Menu_View_ZoomOut);
	entries[a++].Set(wxACCEL_NORMAL,  (int) '=',     Menu_View_ZoomIn);
	entries[a++].Set(wxACCEL_NORMAL,  WXK_PAGEUP,    Menu_View_PrevPage);
	entries[a++].Set(wxACCEL_NORMAL,  WXK_PAGEDOWN,  Menu_View_NextPage);
	entries[a++].Set(wxACCEL_NORMAL,  WXK_HOME,      Menu_View_Begin);
	entries[a++].Set(wxACCEL_NORMAL,  WXK_END,       Menu_View_End);
	entries[a++].Set(wxACCEL_CTRL,    (int) 'T',     Menu_View_Text_Format);
	entries[a++].Set(wxACCEL_ALT,     WXK_RETURN,     Menu_View_ToggleFullScreen);
	entries[a++].Set(wxACCEL_NORMAL,  WXK_F5,      Menu_View_TOC);
	entries[a++].Set(wxACCEL_NORMAL,  WXK_F4,      Menu_View_History);

	entries[a++].Set(wxACCEL_NORMAL,  WXK_F9,      Menu_File_Options);
	entries[a++].Set(wxACCEL_NORMAL,  WXK_F12,      Menu_File_Quit);
	entries[a++].Set(wxACCEL_NORMAL,  WXK_BACK,      Menu_Link_Back);
	entries[a++].Set(wxACCEL_SHIFT,   WXK_BACK,      Menu_Link_Forward);
	entries[a++].Set(wxACCEL_NORMAL,  WXK_TAB,      Menu_Link_Next);
	entries[a++].Set(wxACCEL_NORMAL,  WXK_RETURN,      Menu_Link_Go);
	entries[a++].Set(wxACCEL_SHIFT,   WXK_TAB,      Menu_Link_Prev);

//    wxAcceleratorTable accel(a, entries);
//    SetAcceleratorTable(accel);
}

void FbCoolReader::OnRotate( wxCommandEvent& event )
{
	Rotate();
}

void FbCoolReader::OnFileOpen( wxCommandEvent& WXUNUSED( event ) )
{
	wxFileDialog dlg( this, wxT( "Choose a file to open" ),
		wxT( "" ),
		wxT( "" ),//const wxString& defaultFile = "",
		wxT("All supported files|*.fb2;*.fbz;*.txt;*.zip;*.rtf;*.epub;*.tcr;*.html;*.htm;*.shtml;*.xhtml|FictionBook files (*.fb2)|*.fb2;*.fbz|RTF files (*.rtf)|*.rtf|Text files (*.txt, *.tcr)|*.txt;*.tcr|HTML files|*.html;*.htm;*.shtml;*.xhtml|EPUB files (*.epub)|*.epub|ZIP archieves (*.zip)|*.zip"), //const wxString& wildcard = "*.*",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST //long style = wxFD_DEFAULT_STYLE,
		//const wxPoint& pos = wxDefaultPosition,
		//const wxSize& sz = wxDefaultSize,
		//const wxString& name = "filedlg"
	);

	if ( dlg.ShowModal() == wxID_OK ) {
		//
		Update();
		wxCursor hg( wxCURSOR_WAIT );
		this->SetCursor( hg );
		wxSetCursor( hg );
		GetDocView()->savePosition();
		LoadDocument( dlg.GetPath() );
		GetDocView()->restorePosition();
		UpdateScrollBar();
		//Invalidate();
		Refresh();
		Update();
		wxSetCursor( wxNullCursor );
		this->SetCursor( wxNullCursor );
	}

}

void FbCoolReader::OnFileSave( wxCommandEvent& WXUNUSED( event ) )
{
}

void FbCoolReader::OnAbout( wxCommandEvent& WXUNUSED( event ) )
{
}

void FbCoolReader::OnShowHistory( wxCommandEvent& event )
{
}

void FbCoolReader::OnShowContent( wxCommandEvent& event )
{
    LVTocItem * toc = GetDocView()->getToc();
    if ( !toc || !toc->getChildCount() ) return;

	static ContentDlg * dlg = new ContentDlg( wxGetApp().GetTopWindow() );
	dlg->Assign(GetDocView());
    if ( dlg->ShowModal() == wxID_OK ) {
        LVTocItem * sel = dlg->GetSelection();
        if ( sel ) {
            ldomXPointer ptr = sel->getXPointer();
            GetDocView()->goToBookmark( ptr );
            Refresh();
        }
    }
}

#endif // FB_INCLUDE_READER
