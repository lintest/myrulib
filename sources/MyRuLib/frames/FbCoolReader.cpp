#include "FbCoolReader.h"

#ifdef FB_INCLUDE_READER

#include <wx/dir.h>
#include <wx/power.h>
#include <wx/mstream.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/dcbuffer.h>
#include <crengine.h>
#include "FbConst.h"
#include "FbParams.h"
#include "MyRuLibApp.h"

//-----------------------------------------------------------------------------
//  FbContentModel
//-----------------------------------------------------------------------------

class FbContentData;

class FbContentModel: public FbTreeModel
{
	public:
		FbContentModel( LVTocItem * toc, int page );
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

FbContentModel::FbContentModel( LVTocItem * toc, int page )
	: m_page(page)
{
	m_root = new FbContentData(*this, NULL, toc);
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

bool FbCoolReader::ContentDlg::Execute( LVDocView * view )
{
    LVTocItem * toc = view->getToc();
    if ( !toc || !toc->getChildCount() ) return false;

	static ContentDlg * dlg = new ContentDlg( wxGetApp().GetTopWindow() );
    if ( dlg->Assign(view)->ShowModal() == wxID_OK ) {
		if ( LVTocItem * sel = dlg->GetSelection() ) {
			view->goToBookmark( sel->getXPointer() );
			return true;
		}
    }
	return false;
}

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

FbCoolReader::ContentDlg * FbCoolReader::ContentDlg::Assign( LVDocView * view )
{
	SetTitle(view->getTitle().c_str());
	FbContentModel * model = new FbContentModel(view->getToc(), view->getCurPage());
	m_treeview.AssignModel(model);
	int pos = model->GetPosition() > 2 ? model->GetPosition() - 2 : 0;
	m_treeview.SetScrollPos(pos);
	return this;
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
	EVT_MENU( Menu_File_About, FbCoolReader::OnAbout )
	EVT_MENU( wxID_OPEN, FbCoolReader::OnFileOpen )
	EVT_MENU( wxID_SAVE, FbCoolReader::OnFileSave )
	EVT_MENU( ID_READER_CONTENT, FbCoolReader::OnShowContent )
	EVT_MENU( ID_READER_ZOOM_IN, FbCoolReader::OnCommand )
	EVT_MENU( ID_READER_ZOOM_OUT, FbCoolReader::OnCommand )
	EVT_MENU( ID_READER_HEADER, FbCoolReader::OnShowHeader )
	EVT_UPDATE_UI(ID_READER_HEADER, FbCoolReader::OnShowHeaderUI )
	EVT_MENU(ID_MODE_TREE, FbCoolReader::OnShowContent)
	EVT_MENU(ID_MODE_LIST, FbCoolReader::OnShowContent)
	EVT_MENU( Menu_View_Rotate, FbCoolReader::OnRotate )
	EVT_INIT_DIALOG( FbCoolReader::OnInitDialog )
	EVT_SCROLLWIN( FbCoolReader::OnScroll )
	EVT_IDLE( FbCoolReader::OnIdle )
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

#else // __WXMSW__

void LoadStylesheet(lString8 &css)
{
	#include "fb2_css.inc"
	css = lString8( (lString8::value_type*) locale_binary_file, sizeof(locale_binary_file) );
}

#endif // __WXMSW__

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
		FbCoolReader * reader = new FbCoolReader();
		bool ok = reader->LoadDocument(filename);
		if (ok) {
			reader->Create(parent);
		} else {
			wxDELETE(reader);
		}
		return reader;
	}
}

FbCoolReader::FbCoolReader()
	: m_dirty(true)
	, _firstRender(false)
	, _allowRender(true)
	, _screen(300,400)
	, _wm(&_screen)
	, _docwin(new CRDocViewWindow(&_wm))
{
	Setup(false);
}

FbCoolReader::~FbCoolReader()
{
}

bool FbCoolReader::Create(wxAuiNotebook * parent)
{
	bool ok = wxWindow::Create(parent, ID_FRAME_READ, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxFULL_REPAINT_ON_RESIZE | wxTAB_TRAVERSAL | wxWANTS_CHARS);
	if (ok) {
		SetBackgroundColour(getBackgroundColour());
		SetBackgroundStyle(wxBG_STYLE_CUSTOM);
		SetScrollbar(wxVERTICAL, 0, 1, 100, false);
		_wm.activateWindow( _docwin );

		wxString title = GetDocView()->getTitle().c_str();
		parent->AddPage(this, TrimTitle(title), select );
		Repaint();
	}
	return ok;
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

	fontMan->SetAntialiasMode( 2 );

	SetupPageHeader();
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

void FbCoolReader::SetupPageHeader()
{
	int flags = FbParams(FB_READER_SHOW_HEADER) 
		? PGHDR_AUTHOR | PGHDR_TITLE | PGHDR_PAGE_NUMBER | PGHDR_PAGE_COUNT | PGHDR_CHAPTER_MARKS
		: 0;
	GetDocView()->setPageHeaderInfo( flags );
}

void FbCoolReader::OnShowHeader( wxCommandEvent& event )
{
	int flags = GetDocView()->getPageHeaderInfo();
	FbParams(FB_READER_SHOW_HEADER) = !flags;
	SetupPageHeader();
	Repaint();
}

void FbCoolReader::OnShowHeaderUI( wxUpdateUIEvent& event )
{
	event.Check(GetDocView()->getPageHeaderInfo());
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

void FbCoolReader::OnIdle (wxIdleEvent &WXUNUSED(event))
{
	if (!m_dirty) return;
	m_dirty = false;

	int dx, dy;
	GetClientSize( &dx, &dy );
	if (dx<50 || dy<50 || dx>3000 || dy>3000) return;

	_wm.reconfigure( dx, dy, CR_ROTATE_ANGLE_0 );

	UpdateScrollBar();
	Refresh();
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
		Repaint();
		printf("text : %s     \t", s.c_str() );
	} else {
		printf("element : %s  \t", UnicodeToUtf8( ptr.toString() ).c_str() );
	}
	lvPoint pt2 = ptr.toPoint();
	printf("  (%d, %d)  ->  (%d, %d)\n", x, y+GetDocView()->GetPos(), pt2.x, pt2.y);
}

FbCoolReader::MenuBook::MenuBook()
{
	Append( ID_READER_CONTENT, _("Table of Contents") );
	AppendSeparator();
	Append( ID_READER_ZOOM_IN, _( "Zoom In" ) );
	Append( ID_READER_ZOOM_OUT, _( "Zoom Out" ) );
	AppendCheckItem( ID_READER_HEADER, _("Show page header") );
	AppendSeparator();
	Append( wxID_OPEN, wxT( "&Open...\tCtrl+O" ) );
	Append( wxID_SAVE, wxT( "&Save...\tCtrl+S" ) );
	AppendSeparator();
	Append( ID_READER_OPTIONS, (wxString)_("Cool Reader options") + wxT( "\tF9" ) );
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
	Repaint();
}

void FbCoolReader::OnCommand(wxCommandEvent& event)
{
	switch ( event.GetId() ) {
		case ID_READER_ZOOM_IN:  doCommand( DCMD_ZOOM_IN            , 0 );  break;
		case ID_READER_ZOOM_OUT: doCommand( DCMD_ZOOM_OUT           , 0 ); break;
		case Menu_Link_Forward:  doCommand( DCMD_LINK_FORWARD       , 1 ); break;
		case Menu_Link_Back:     doCommand( DCMD_LINK_BACK          , 1 ); ; break;
		case Menu_Link_Next:     doCommand( DCMD_LINK_NEXT          , 1 ); break;
		case Menu_Link_Prev:     doCommand( DCMD_LINK_PREV          , 1 ); break;
		case Menu_Link_Go:       doCommand( DCMD_LINK_GO            , 1 ); break;
		case Menu_View_NextLine: doCommand( DCMD_LINEDOWN           , 1 ); break;
		case Menu_View_PrevLine: doCommand( DCMD_LINEUP             , 1 ); break;
		case Menu_View_Begin:    doCommand( DCMD_BEGIN              , 0 ); break;
		case Menu_View_End:	     doCommand( DCMD_END                , 0 ); break;
		case Menu_View_Text:     doCommand( DCMD_TOGGLE_TEXT_FORMAT , 0 ); break;
		case Menu_View_Scroll:   ToggleViewMode(); break;
		case Menu_View_NextPage:
			doCommand( DCMD_PAGEDOWN, 1 );
			GetDocView()->cachePageImage( 0 );
			GetDocView()->cachePageImage( 1 );
			break;
		case Menu_View_PrevPage:
			doCommand( DCMD_PAGEUP, 1 );
			GetDocView()->cachePageImage( 0 );
			GetDocView()->cachePageImage( -1 );
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
	GetDocView()->savePosition();
	GetDocView()->Clear();

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
	Repaint();
	//_docview->restorePosition();
	//_docview->Render();
	//UpdateScrollBar();
	//Paint();
	//===========================================
	return res;
}

void FbCoolReader::goToBookmark(ldomXPointer bm)
{
	GetDocView()->goToBookmark(bm);
	UpdateScrollBar();
	Repaint();
}

void FbCoolReader::SetRotate( cr_rotate_angle_t angle )
{
	GetDocView()->SetRotateAngle( angle );
	UpdateScrollBar();
	Repaint();
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
	Repaint();
}

void FbCoolReader::OnPaint(wxPaintEvent& event)
{
	wxAutoBufferedPaintDC dc (this);
	wxBrush brush(getBackgroundColour(), wxSOLID);
	dc.SetBackground(brush);
	dc.Clear();

	if (m_dirty) return;

	int dx, dy;
	GetClientSize( &dx, &dy );
	if ( !GetDocView()->IsRendered() && (GetDocView()->GetWidth() != dx || GetDocView()->GetHeight() != dy) ) {
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
	Repaint();
}

// LVDocViewCallback override
void FbCoolReader::OnExternalLink( lString16 url, ldomNode * node )
{
	::wxLaunchDefaultBrowser( wxString(url.c_str()) );
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

void FbCoolReader::OnInitDialog(wxInitDialogEvent& event)
{
	wxAcceleratorEntry entries[40];
	int a=0;
	entries[a++].Set(wxACCEL_CTRL,  (int) 'O',     wxID_OPEN);
	entries[a++].Set(wxACCEL_CTRL,  (int) 'S',     wxID_SAVE);
	entries[a++].Set(wxACCEL_CTRL,  (int) 'P',     Menu_View_Scroll);
	entries[a++].Set(wxACCEL_CTRL,  (int) 'H',     Menu_View_Header);
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
	entries[a++].Set(wxACCEL_CTRL,    (int) 'T',     Menu_View_Text);
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
		GetDocView()->savePosition();
		LoadDocument( dlg.GetPath() );
		GetDocView()->restorePosition();
		UpdateScrollBar();
		//Invalidate();
		Refresh();
		Update();
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
	if ( ContentDlg::Execute(GetDocView()) ) Refresh();
}

#endif // FB_INCLUDE_READER
