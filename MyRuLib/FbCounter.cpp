#include "FbCounter.h"

FbCounter::FbCounter(wxSQLite3Database & database, long style)
	: m_database(database), m_style(style)
{
	if (HasFlag(fbCF_AUTH)) CreateTable(wxT('a'));
	if (HasFlag(fbCF_SEQN)) CreateTable(wxT('s'));
	if (HasFlag(fbCF_DATE)) CreateTable(wxT('d'));
}

FbCounter::~FbCounter(void)
{
}

void FbCounter::CreateTable(const wxString & name)
{
	wxString sql = wxString::Format(wxT("CREATE TEMP TABLE tmp_%s(id INTEGER PRIMARY KEY)"), name.c_str());
	m_database.ExecuteUpdate(sql);
}

void FbCounter::Add(const wxString & books)
{
	if (HasFlag(fbCF_AUTH)) {
		wxString sql = wxString::Format(wxT("INSERT INTO tmp_a SELECT DISTINCT id_author FROM books WHERE id IN (%s)"), books.c_str());
		m_database.ExecuteUpdate(sql);
	}

	if (HasFlag(fbCF_SEQN)) {
		wxString sql = wxString::Format(wxT("INSERT INTO tmp_s SELECT DISTINCT id_seq FROM bookseq WHERE id_book IN (%s)"), books.c_str());
		m_database.ExecuteUpdate(sql);
	}

	if (HasFlag(fbCF_DATE)) {
		wxString sql = wxString::Format(wxT("INSERT INTO tmp_d SELECT DISTINCT created FROM books WHERE id IN (%s)"), books.c_str());
		m_database.ExecuteUpdate(sql);
	}
}

void FbCounter::Add(int book)
{
	wxString books;  
	books << book;
	Add(books);
}

void FbCounter::Execute()
{
	if (HasFlag(fbCF_AUTH)) {
		wxString sql = wxT("UPDATE authors SET number=(SELECT COUNT(id) FROM books WHERE books.id_author=authors.id) WHERE id IN (SELECT id FROM tmp_a)");
		m_database.ExecuteUpdate(sql);
	}

	if (HasFlag(fbCF_SEQN)) {
		wxString sql = wxT("UPDATE sequences SET number=(SELECT COUNT(id_book) FROM bookseq WHERE bookseq.id_seq=sequences.id) WHERE id IN (SELECT id FROM tmp_s)");
		m_database.ExecuteUpdate(sql);
	}

	if (HasFlag(fbCF_DATE)) {
		wxString sql = wxT("INSERT OR REPLACE INTO dates \
			select \
				created, \
				case when lib_min = 999999 then 0 else lib_min end as lib_min, lib_max, lib_num, \
				usr_min, case when usr_max = -999999 then 0 else usr_max end as usr_max, usr_num \
			from (select \
				created, \
				sum(case when id>0 then 1 else 0 end) as lib_num, \
				min(case when id>0 then id else 999999 end) as lib_min, \
				max(case when id>0 then id else 0 end) as lib_max, \
				sum(case when id<0 then 1 else 0 end) as usr_num, \
				min(case when id<0 then id else 0 end) as usr_min, \
				max(case when id<0 then id else -999999 end) as usr_max \
			from ( \
				 select distinct id, case when id>0 then 1 else -1 end as sign, created \
				 from books where created in (SELECT id FROM tmp_d) \
			) group by created)");
		m_database.ExecuteUpdate(sql);
	}
}
