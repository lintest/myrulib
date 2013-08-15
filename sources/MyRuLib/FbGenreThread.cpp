#include "FbGenreThread.h"
#include "FbDatabase.h"
#include "FbBookEvent.h"
#include "FbConst.h"

void * FbGenreThread::Entry()
{
	FbCommonDatabase database;
	database.JoinThread(this);
	FbSQLite3Transaction trans(&database, WXSQLITE_TRANSACTION_EXCLUSIVE);

	wxString msg = _("Rebuild the list of genres");

	database.ExecuteUpdate(wxT("DROP TABLE genres"));
	database.ExecuteUpdate(wxT("CREATE TABLE genres(id_book integer, id_genre CHAR(2));"));
	database.ExecuteUpdate(wxT("CREATE INDEX genres_book ON genres(id_book);"));
	database.ExecuteUpdate(wxT("CREATE INDEX genres_genre ON genres(id_genre);"));

	int count = database.ExecuteScalar(wxT("SELECT count(DISTINCT id) FROM books"));
	if (count == 0) return NULL;

	int pos = 0;
	wxString sql = wxT("SELECT id, genres FROM books");
	FbSQLite3ResultSet res = database.ExecuteQuery(sql);
	while (res.NextRow()) {
		int id = res.GetInt(0);
		wxString genres = res.GetString(1);
		int len = genres.Len();
		for (int i = 0; i < len; i+=2) {
			wxString sql = wxT("INSERT INTO genres(id_book, id_genre) VALUES(?,?)");
			FbSQLite3Statement stmt = database.PrepareStatement(sql);
			stmt.Bind(1, id);
			stmt.Bind(2, genres.Mid(i, 2));
			FbSQLite3ResultSet result = stmt.ExecuteQuery();
		}
		FbProgressEvent(ID_PROGRESS_UPDATE, msg, ++pos * 1000 / count).Post();
	}

	trans.Commit();
	FbProgressEvent(ID_PROGRESS_UPDATE).Post();

	return NULL;
}
