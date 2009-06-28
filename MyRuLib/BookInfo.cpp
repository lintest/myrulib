#include "BookInfo.h"
#include "FbParams.h"
#include "FbGenres.h"
#include "MyRuLibApp.h"
#include "Sequences.h"

WX_DEFINE_OBJARRAY(SeqItemArray);

extern wxString strAlphabet;
extern wxString strRusJO;
extern wxString strRusJE;

BookInfo::BookInfo(wxInputStream& stream, int flags)
    :m_ok(false)
{
    FbDocument xml;
	if (xml.Load(stream, wxT("UTF-8"))) m_ok = ReadXml(xml, flags);
}

bool BookInfo::ReadXml(const FbDocument &xml, int flags)
{

	FbNode * node = xml.GetRoot();
	if (!node) return false;

	node = node->Find(wxT("description"));
	if (!node) return false;

	node = node->Find(wxT("title-info"));
	if (!node) return false;

	node = node->m_child;
    while (node) {
		wxString name = node->GetName();
        if ((flags & BIF_TITLE_INFO)!=0) {
            if ( name == wxT("author") ) {
                wxString value = xml.GetAuthor(node);
                if (!value.IsEmpty()) authors.Add( FindAuthor(value) );
            } else {
                wxString value = (node->m_text);
                if ( name == wxT("genre") ) {
                    genres += FbGenres::Char(value);
                } else if ( name == wxT("book-title") ) {
                    title = value;
                } else if ( name == wxT("sequence") ) {
                    wxString name = node->Prop(wxT("name"));
                    int seq = FindSequence(name);
                    if (seq) {
                        wxString number = node->Prop(wxT("number"));
                        long num = 0;
                        number.ToLong(&num);
                        sequences.Add(SeqItem(seq, num));
                    }
                }
            }
        }
        if ((flags & BIF_ANNOTATION)!=0) {
            if ( name == wxT("annotation")) annotation = node->m_text;
        }
		node = node->m_next;
    }

	if (authors.Count() == 0) authors.Add(0);

	return true;
}

int BookInfo::FindAuthor(wxString &full_name) {

	if (full_name.IsEmpty()) return 0;

	wxString search_name = full_name;
	MakeLower(search_name);
	search_name.Replace(strRusJO, strRusJE);

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	Authors authors(wxGetApp().GetDatabase());
	AuthorsRow * row = authors.Name(search_name);

	if (!row) {
		wxString letter = search_name.Left(1);
		MakeUpper(letter);
		if (strAlphabet.Find(letter) == wxNOT_FOUND) letter = wxT("#");
		row = authors.New();
		row->id = (full_name.IsEmpty() ? 0 : - NewId(DB_NEW_AUTHOR));
		row->letter = letter;
		row->search_name = search_name;
		row->full_name = full_name;
		row->Save();
	}
	return row->id;
}

int BookInfo::FindSequence(wxString &name) {

	if (name.IsEmpty()) return 0;

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	Sequences sequences(wxGetApp().GetDatabase());
	SequencesRow * seqRow = sequences.Name(name);

	if (!seqRow) {
		seqRow = sequences.New();
		seqRow->id = - NewId(DB_NEW_SEQUENCE);
		seqRow->value = name;
		seqRow->Save();
	}

	return seqRow->id;
}

void BookInfo::MakeLower(wxString & data){
#ifdef __WIN32__
      int len = data.length() + 1;
      wxChar * buf = new wxChar[len];
      wxStrcpy(buf, data.c_str());
      CharLower(buf);
      data = buf;
      delete [] buf;
#else
      data.MakeLower();
#endif
}

void BookInfo::MakeUpper(wxString & data){
#ifdef __WIN32__
      int len = data.length() + 1;
      wxChar * buf = new wxChar[len];
      wxStrcpy(buf, data.c_str());
      CharUpper(buf);
      data = buf;
      delete [] buf;
#else
      data.MakeUpper();
#endif
}

int BookInfo::NewId(int param)
{
	Params params(wxGetApp().GetDatabase());
	ParamsRow * row = params.Id(param);
	row->value++;
	row->Save();

	return row->value;
}

