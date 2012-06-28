#include "FbMainMenu.h"
#include "FbConst.h"

FbMenuBar::FbMenuBar()
{
	Append(new MenuFile   , _("&File")    );
	Append(new MenuEdit   , _("&Edit")    );
	Append(new MenuLib    , _("&Library") );
	Append(new MenuFrame  , _("&Catalog") );
	Append(new MenuTree   , _("&Books")   );
	Append(new MenuWindow , _("&Window")  );
	Append(new MenuHelp   , _("&?")       );
}

FbMenuBar::MenuFile::MenuFile()
{
	AppendImg(wxID_NEW,  _("Add file")   + (wxString)wxT("\tCtrl+N"), wxART_NEW         );
	AppendImg(wxID_OPEN, _("Add folder") + (wxString)wxT("\tCtrl+O"), wxART_FOLDER_OPEN );
	AppendSeparator();
	AppendImg(wxID_SAVE, _("Export")     + (wxString)wxT("\tCtrl+S"), wxART_FILE_SAVE   );
	AppendImg(wxID_EXIT, _("Exit")       + (wxString)wxT("\tAlt-F4"), wxART_QUIT        );
}

FbMenuBar::MenuEdit::MenuEdit()
{
	AppendImg(wxID_CUT       , _("Cut")   + (wxString)wxT("\tCtrl+X"), wxART_CUT   );
	AppendImg(wxID_COPY      , _("Copy")  + (wxString)wxT("\tCtrl+C"), wxART_COPY  );
	AppendImg(wxID_PASTE     , _("Paste") + (wxString)wxT("\tCtrl+V"), wxART_PASTE );
	AppendSeparator();
	Append(ID_MASTER_APPEND  , _("Append")  );
	Append(ID_MASTER_MODIFY  , _("Modify")  );
	Append(ID_MASTER_REPLACE , _("Replace") );
	AppendImg(wxID_DELETE    , _("Delete"), wxART_DELETE);
	Append(ID_DELETE_LINK    , _("Delete link"));
	AppendSeparator();
	Append(wxID_SELECTALL    , _("Select all") + (wxString)wxT("\tCtrl+A"));
	Append(ID_UNSELECTALL    , _("Undo selection"));
	AppendSeparator();
	#ifdef FB_INCLUDE_READER
	Append(ID_READER_OPTIONS , _("Cool Reader options"));
	#endif // FB_INCLUDE_READER
	Append(wxID_PREFERENCES  , _("Settings"));
}

FbMenuBar::MenuFrame::MenuFrame()
{
	wxMenuItem * submenu = FbMenuRefs::Create(this);

	AppendImg(ID_MENU_SEARCH, _("Search"), wxART_FIND);
	if (submenu) Append(submenu);
	AppendSeparator();
	Append(ID_FRAME_AUTH, _("Authors")    );
	Append(ID_FRAME_GENR, _("Genres")     );
	Append(ID_FRAME_SEQN, _("Series")     );
	Append(ID_FRAME_DATE, _("Calendar")   );
	Append(ID_FRAME_ARCH, _("Archive")    )->Enable(false);
	AppendSeparator();
	Append(ID_FRAME_FLDR, _("My folders") );
	Append(ID_FRAME_DOWN, _("Downloads")  );
}

FbMenuBar::MenuLib::MenuLib()
{
	Append(ID_MENU_DB_OPEN   , _("Open (or create) collection"));
	Append(wxID_FILE         , _("Open recent"), new MenuRecent);
	AppendSeparator();
	Append(ID_MENU_DB_INFO   , _("Information about collection"));
	Append(ID_MENU_GENRES    , _("Rebuild the list of genres"));
	Append(ID_MENU_OPTIMIZE  , _("Clear empty variables"));
	Append(ID_MENU_VACUUM    , _("Optimize DB"));
	Append(ID_MENU_UPDATE    , _("Online update database"));
	AppendSeparator();
	Append(ID_MENU_CONFIG    , _("Options"));
}

FbMenuBar::MenuHelp::MenuHelp()
{
	Append(ID_OPEN_WEB, _("Website"));
	AppendImg(wxID_ABOUT, _("About"), wxART_HELP_PAGE);
}

FbMenuBar::MenuTree::MenuTree()
{
	Append(ID_SHOW_COLUMNS , _("Table columns") );
	Append(wxID_ANY        , _("List of books") , new MenuListMode);
	Append(ID_ORDER_MENU   , _("Sort by")       , new FbMenuSort );
	Append(wxID_ANY        , _("Preview area")  , new MenuPreview);
	AppendSeparator();
	Append(ID_FILTER_SET   , _("Filter setup...")  );
	AppendCheckItem(ID_FILTER_USE, _("Use filter"));
	AppendCheckItem(ID_FILTER_DEL, _("Show deleted"));
	AppendSeparator();
	Append(ID_EDIT_COMMENTS , _("Add comments"));
}

#ifdef FB_INCLUDE_READER

FbMenuBar::MenuRead::MenuRead()
{
	Append( ID_READER_CONTENT, _("Table of Contents") );
	Append( wxID_FIND, _("Find text...") );
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

#endif // FB_INCLUDE_READER

FbMenuBar::MenuRecent::MenuRecent()
{
}

FbMenuBar::MenuListMode::MenuListMode()
{
	AppendRadioItem(ID_MODE_TREE, _("Hierarchical"));
	AppendRadioItem(ID_MODE_LIST, _("Simple list"));
}

FbMenuBar::MenuTabArt::MenuTabArt()
{
	AppendRadioItem(ID_ART_DEFAULT,  _("Standart"));
	AppendRadioItem(ID_ART_COMPACT,  _("Compact"));
	AppendRadioItem(ID_ART_STANDART, _("Convex"));
	AppendRadioItem(ID_ART_BUTTONS,  _("Buttons"));
	#ifdef __WXMSW__
	AppendRadioItem(ID_ART_TOOLBAR,  _("Light"));
	#endif
	AppendRadioItem(ID_ART_VSTUDIO,  _("Studio"));
	AppendRadioItem(ID_ART_MOZILLA,  _("Mozilla"));
}

FbMenuBar::MenuPreview::MenuPreview()
{
	AppendRadioItem(ID_SPLIT_VERTICAL,   _("Right"));
	AppendRadioItem(ID_SPLIT_HORIZONTAL, _("Bottom"));
	AppendRadioItem(ID_SPLIT_NOTHING,    _("Not specified"));
}

FbMenuBar::MenuWindow::MenuWindow()
{
	Append(wxID_CLOSE,     _("Close window") + (wxString)wxT("\tCtrl+W"));
	Append(wxID_CLOSE_ALL, _("Close all"));
	AppendSeparator();
	Append(ID_WINDOW_NEXT, _("Next") + (wxString)wxT("\tCtrl+Tab"));
	Append(ID_WINDOW_PREV, _("Previous") + (wxString)wxT("\tCtrl+Shift+Tab"));
	AppendSeparator();
	Append(wxID_ANY, _("Tabs"), new MenuTabArt());
	AppendCheckItem(ID_FULLSCREEN,   _("Fullscreen mode") + (wxString)wxT("\tF11"));
	AppendCheckItem(ID_TEXTLOG_SHOW, _("Show log window") + (wxString)wxT("\tF12"));
	AppendCheckItem(ID_STATUS_SHOW,  _("Show status bar"));
}
