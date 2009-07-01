#include "BookInfo.h"
#include "FbParams.h"
#include "FbGenres.h"
#include "MyRuLibApp.h"
#include "db/Sequences.h"
#include "ParseCtx.h"

WX_DEFINE_OBJARRAY(SeqItemArray);

extern wxString strAlphabet;
extern wxString strRusJO;
extern wxString strRusJE;

WX_DECLARE_OBJARRAY(AuthorItem, AuthorArray);

WX_DEFINE_OBJARRAY(AuthorArray);

wxString AuthorItem::GetFullName()
{
    wxString result = last;
    if (!first.IsEmpty()) result += (wxT(" ") + first);
    if (!middle.IsEmpty()) result += (wxT(" ") + middle);
    return result.Trim(false).Trim(true);
}

class SequenceItem
{
public:
    SequenceItem(): number(0) {};
public:
    wxString seqname;
    long number;
};

WX_DECLARE_OBJARRAY(SequenceItem, SequenceArray);

WX_DEFINE_OBJARRAY(SequenceArray);

class TitleParsingContext: public ParsingContext
{
public:
    wxString title;
    AuthorArray authors;
    SequenceArray sequences;
    wxString genres;
    AuthorItem * author;
    wxString text;
};

extern "C" {
static void StartElementHnd(void *userData, const XML_Char *name, const XML_Char **atts)
{
    TitleParsingContext *ctx = (TitleParsingContext*)userData;
    wxString node_name = ctx->CharToLower(name);
    wxString path = ctx->Path();

	if (path == wxT("fictionbook/description/title-info/")) {
	    if (node_name == wxT("author")) {
            ctx->author = new AuthorItem;
	    } else if (node_name == wxT("sequence")) {
	        SequenceItem * seqitem = new SequenceItem;
            const XML_Char **a = atts;
            while (*a) {
                wxString attr = ctx->CharToLower(a[0]).Trim(false).Trim(true);
                wxString text = ctx->CharToString(a[1]).Trim(false).Trim(true);
                if (attr == wxT("name")) {
                    seqitem->seqname = text;
                } else if (attr == wxT("number")) {
                    text.ToLong(&seqitem->number);
                }
                a += 2;
            }
            ctx->sequences.Add(seqitem);
	    }
	}
	ctx->AppendTag(node_name);
    ctx->text.Empty();
}
}

extern "C" {
static void EndElementHnd(void *userData, const XML_Char* name)
{
    TitleParsingContext *ctx = (TitleParsingContext*)userData;
    wxString node_name = ctx->CharToLower(name);
	ctx->RemoveTag(node_name);
    wxString path = ctx->Path();

	if (path == wxT("fictionbook/description/title-info/")) {
        ctx->text.Trim(false).Trim(true);
	    if (node_name == wxT("author")) {
	        ctx->authors.Add(ctx->author);
	    } else if (node_name == wxT("book-title")) {
	        ctx->title = ctx->text;
	    } else if (node_name == wxT("genre")) {
            ctx->genres += FbGenres::Char(ctx->text);
	    }
	} else if (path == wxT("fictionbook/description/title-info/author/")) {
        ctx->text.Trim(false).Trim(true);
        if (node_name == wxT("first-name"))
            ctx->author->first = ctx->text;
        if (node_name == wxT("middle-name"))
            ctx->author->middle = ctx->text;
        if (node_name == wxT("last-name"))
            ctx->author->last = ctx->text;
	} else if (path == wxT("fictionbook/description/")) {
	    if (node_name == wxT("title-info")) {
	        ctx->Stop();
	    }
	}
}
}

extern "C" {
static void TextHnd(void *userData, const XML_Char *s, int len)
{
    TitleParsingContext *ctx = (TitleParsingContext*)userData;

    wxString str = ctx->CharToString(s, len);

    if (!ctx->IsWhiteOnly(str)) ctx->text += str;
}
}

bool BookInfo::Load(wxInputStream& stream)
{
    const size_t BUFSIZE = 1024;
    char buf[BUFSIZE];
    bool done;

    TitleParsingContext ctx;

    XML_SetUserData(ctx.GetParser(), (void*)&ctx);
    XML_SetElementHandler(ctx.GetParser(), StartElementHnd, EndElementHnd);
    XML_SetCharacterDataHandler(ctx.GetParser(), TextHnd);

    bool ok = true;
    do {
        size_t len = stream.Read(buf, BUFSIZE).LastRead();
        done = (len < BUFSIZE);

        if ( !XML_Parse(ctx.GetParser(), buf, len, done) ) {
			XML_Error error_code = XML_GetErrorCode(ctx.GetParser());
			if ( error_code == XML_ERROR_ABORTED ) {
				done = true;
			} else {
				wxString error(XML_ErrorString(error_code), *wxConvCurrent);
				wxLogError(_("XML parsing error: '%s' at line %d"), error.c_str(), XML_GetCurrentLineNumber(ctx.GetParser()));
				ok = false;
	            break;
			}
        }
    } while (!done);

    title = ctx.title;

	genres = ctx.genres;

    for (size_t i=0; i<ctx.authors.Count(); i++)
        authors.Add( FindAuthor(ctx.authors[i]) );
	if (authors.Count() == 0) authors.Add(0);

    for (size_t i=0; i<ctx.sequences.Count(); i++) {
        int seq = FindSequence(ctx.sequences[i].seqname);
        sequences.Add(SeqItem(seq, ctx.sequences[i].number));
    }

    return ok;
}

BookInfo::BookInfo(wxInputStream& stream, int flags)
    :m_ok(false)
{
    m_ok = Load(stream);
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

int BookInfo::FindAuthor(AuthorItem &author)
{
	wxString search_name = author.GetFullName();
	if (search_name.IsEmpty()) return 0;
	MakeLower(search_name);
	search_name.Replace(strRusJO, strRusJE);

    wxCriticalSectionLocker enter(wxGetApp().m_DbSection);

	wxString sql = wxT("SELECT id FROM authors WHERE first_name=? AND middle_name=? AND last_name=? ORDER BY search_name");
	PreparedStatement* pStatement = wxGetApp().GetDatabase()->PrepareStatement(sql);
	pStatement->SetParamString(1, author.first);
	pStatement->SetParamString(2, author.middle);
	pStatement->SetParamString(3, author.last);
	DatabaseResultSet* result = pStatement->ExecuteQuery();
	if (result->Next()) return result->GetResultInt(1);

    wxString letter = search_name.Left(1);
    MakeUpper(letter);
    if (strAlphabet.Find(letter) == wxNOT_FOUND) letter = wxT("#");

	Authors authors(wxGetApp().GetDatabase());
    AuthorsRow * row = authors.New();
    row->id = - NewId(DB_NEW_AUTHOR);
    row->letter = letter;
    row->search_name = search_name;
    row->full_name = author.GetFullName();
    row->first_name = author.first;
    row->middle_name = author.middle;
    row->last_name = author.last;
    row->Save();
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

