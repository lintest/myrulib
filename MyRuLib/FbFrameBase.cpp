#include "FbFrameBase.h"
#include <wx/artprov.h>
#include "FbConst.h"

FbFrameBase::FbFrameBase()
{
}

FbFrameBase::FbFrameBase(wxAuiMDIParentFrame * parent, wxWindowID id, const wxString & title)
{
	Create(parent, id, title);
}

bool FbFrameBase::Create(wxAuiMDIParentFrame * parent, wxWindowID id, const wxString & title)
{
	bool res = wxAuiMDIChildFrame::Create(parent, id, title);
	if (res) CreateControls();
	return res;
}

wxMenuBar * FbFrameBase::CreateMenuBar()
{
	wxMenuBar * menuBar = new wxMenuBar;
	wxMenuItem * tempItem;
	wxMenu * menu;

	menu = new wxMenu;
	(tempItem = menu->Append(wxID_ANY, wxT("X")))->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW));
	menu->Append(wxID_NEW, _("Добавить файл"))->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW));
	menu->Append(wxID_OPEN, _("Добавить директорию"))->SetBitmap(wxArtProvider::GetBitmap(wxART_FOLDER_OPEN));
	menu->AppendSeparator();
	menu->Append(wxID_SAVE, _("Записать на устройство"))->SetBitmap(wxArtProvider::GetBitmap(wxART_FILE_SAVE));
	menu->AppendSeparator();
	menu->Append(wxID_EXIT, _("Выход\tAlt+F4"))->SetBitmap(wxArtProvider::GetBitmap(wxART_QUIT));
	menu->Delete(tempItem);
	menuBar->Append(menu, _("&Файл"));

	menu = new wxMenu;
	(tempItem = menu->Append(wxID_ANY, wxT("X")))->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW));
	menu->Append(wxID_SELECTALL, _("Выделить все\tCtrl+A"));
	menu->Delete(tempItem);
	menuBar->Append(menu, _("&Правка"));

	menu = new wxMenu;
	(tempItem = menu->Append(wxID_ANY, wxT("X")))->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW));
	menu->Append(wxID_ANY, _("Параметры базы данных"));
	menu->Append(wxID_ANY, _("Реструктуризация БД"));
	menu->AppendSeparator();
	menu->Append(wxID_PREFERENCES, _("Настройки"));
	menu->Delete(tempItem);
	menuBar->Append(menu, _("&Сервис"));

	menu = new wxMenu;
	(tempItem = menu->Append(wxID_ANY, wxT("X")))->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW));
	menu->AppendRadioItem(ID_SPLIT_HORIZONTAL, _("&Просмотр справа"));
	menu->AppendRadioItem(ID_SPLIT_VERTICAL, _("&Просмтр снизу"));
	menu->AppendSeparator();
	menu->Append(ID_LOG_TEXTCTRL, _("Скрыть окно сообщений\tCtrl+Z"));
	menu->Delete(tempItem);
	menuBar->Append(menu, _("&Вид"));

	menu = new wxMenu;
	(tempItem = menu->Append(wxID_ANY, wxT("X")))->SetBitmap(wxArtProvider::GetBitmap(wxART_NEW));
	menu->Append(wxID_ABOUT, _("О программе…"))->SetBitmap(wxArtProvider::GetBitmap(wxART_HELP_PAGE));
	menu->Delete(tempItem);
	menuBar->Append(menu, _("&?"));

	return menuBar;
}

void FbFrameBase::OnActivated(wxActivateEvent & event)
{
    /*
	AUIDocViewMainFrame * frame = wxDynamicCast(GetMDIParentFrame(),
		AUIDocViewMainFrame);
	if(frame)
	{
		frame->GetLOGTextCtrl()->SetValue(wxString::Format(
			_("Some help text about '%s'"),	GetTitle().GetData()));
	}
	*/
}
