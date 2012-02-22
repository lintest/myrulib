#include "FbFileReader.h"
#include "FbParams.h"
#include "FbConst.h"
#include "FbExtractInfo.h"
#include "FbDatabase.h"
#include "MyRuLibApp.h"
#include "FbDataPath.h"
#include "FbDownloader.h"
#include "FbCollection.h"

#include <wx/mimetype.h>
#include <wx/stdpaths.h>
#include <wx/wxsqlite3.h>

bool FbTempEraser::sm_erase = true;

//-----------------------------------------------------------------------------
//  FbTempEraser
//-----------------------------------------------------------------------------

FbTempEraser::~FbTempEraser()
{
	if (!sm_erase) return;

	for (size_t i = 0; i < m_filelist.GetCount(); i++)
		wxRemoveFile(m_filelist[i]);
}

void FbTempEraser::Add(const wxString &filename)
{
	static FbTempEraser eraser;
	eraser.m_filelist.Add(filename);
}

//-----------------------------------------------------------------------------
//  FbZipInputStream
//-----------------------------------------------------------------------------

static wxCSConv & GetConv866()
{
	static wxCSConv conv(wxT("cp866"));
	return conv;
}

FbZipInputStream::FbZipInputStream(const wxString & archname, const wxString & filename)
	: wxZipInputStream(new wxFFileInputStream(archname), GetConv866()), m_ok(true)
{
	while (m_entry = GetNextEntry()) {
		bool ok = (m_entry->GetInternalName() == filename);
		if (ok) { m_ok = OpenEntry(*m_entry); break; }
	}
}

FbZipInputStream::FbZipInputStream(const wxString & archname, bool info)
	: wxZipInputStream(new wxFFileInputStream(archname), GetConv866()), m_ok(true)
{
	while (m_entry = GetNextEntry()) {
		bool ok = (m_entry->GetInternalName().Right(4).Lower() == wxT(".fbd")) == info;
		if (ok) { m_ok = OpenEntry(*m_entry); break; }
	}
}

FbZipInputStream::FbZipInputStream(wxInputStream * stream, bool info)
	: wxZipInputStream(stream, GetConv866()), m_ok(true)
{
	while (m_entry = GetNextEntry()) {
		wxString name = m_entry->GetInternalName();
		if (name == wxT("mimetype")) { m_ok = false; return; } // Check EPUB
		bool found = (name.Right(4).Lower() == wxT(".fbd")) == info;
		if (found) { m_ok = OpenEntry(*m_entry); break; }
	}
}

wxFileOffset FbZipInputStream::SeekI(wxFileOffset pos, wxSeekMode mode)
{
	if (m_entry && pos == 0 && mode == wxFromStart) {
		return OpenEntry(*m_entry) ? 0 : wxInvalidOffset;
	} else {
		return wxZipInputStream::SeekI(pos, mode);
	}
}

//-----------------------------------------------------------------------------
//  FbFileReader
//-----------------------------------------------------------------------------

static wxString FildFile(const wxString & name, const wxString & path, const wxString & root)
{
	if (name.IsEmpty()) return wxEmptyString;

	wxFileName filename = name;

	filename.Normalize(wxPATH_NORM_ALL, root);
	if (filename.FileExists()) return filename.GetFullPath();

	if (!path.IsEmpty() && wxFileName::FileExists(path)) return path;

	return wxEmptyString;
}

FbFileReader::FbFileReader(int id, bool info)
	: m_id(id), m_stream(NULL)
{
	FbCommonDatabase database;
	bool show_error = !info;
	{
		wxString sql = wxT("SELECT md5sum, file_name, file_type FROM books WHERE id="); sql << id;
		wxSQLite3ResultSet result = database.ExecuteQuery(sql);
		if (!result.NextRow()) return;
		m_md5sum = result.GetString(0);
		m_message = result.GetString(1);
		m_filetype = result.GetString(2).Lower();
		if (m_message.IsEmpty()) {
			m_message << id << wxT('.') << m_filetype;
		}
		if  (m_filetype == wxT("fb2")) info = false;
	}

	{
		wxString filename = FbDownloader::GetFilename(m_md5sum);
		if (wxFileName::FileExists(filename)) {
			bool zipped = false;
			m_stream = new wxFFileInputStream(filename);
			if (m_stream->IsOk()) {
				if ( m_filetype != wxT("zip") ) {
					unsigned char buf[2] = {0, 0};
					m_stream->Read(buf, 2);
					m_stream->SeekI(0);
					if (memcmp(buf, "PK", 2) == 0) {
						m_stream = new FbZipInputStream(m_stream, info);
						if (m_stream->IsOk()) return;
						wxDELETE(m_stream);
						m_stream = new wxFFileInputStream(filename);
					}
				}
				m_filename = filename;
				return;
			}
			wxDELETE(m_stream);
		}
		filename << wxT(".zip");
		if (wxFileName::FileExists(filename)) {
			m_stream = new FbZipInputStream(filename, info);
			if (m_stream->IsOk()) return; else wxDELETE(m_stream);
		}
	}

	wxString sql;
	wxString error;
	wxString root = wxGetApp().GetLibPath();
	if (id < 0) sql = wxT("SELECT DISTINCT id_archive,file_name,file_path,1,id_archive*id_archive FROM books WHERE id=?1 UNION ");
	sql << wxT("SELECT DISTINCT id_archive,file_name,file_path,2,id_archive*id_archive FROM files WHERE id_book=?1 ORDER BY 1,4,5 desc");
	wxSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, id);
	wxSQLite3ResultSet result = stmt.ExecuteQuery();
	while ( result.NextRow() ) {
		bool primamy = result.GetInt(4) == 1;
		wxString file_name = result.GetString(1);
		wxString file_path = result.GetString(0);
		if (show_error && primamy) error = GetError(file_name);
		if (int arch = result.GetInt(0)) {
			wxString sql = wxT("SELECT id, file_name, file_path FROM archives WHERE id="); sql << arch;
			wxSQLite3ResultSet result = database.ExecuteQuery(sql);
			if (result.NextRow()) {
				wxString arch_name = result.GetString(1);
				wxString arch_path = result.GetString(2);
				if (show_error && primamy) error = GetError(arch_name, file_name);
				arch_name = FildFile(arch_name, arch_path, root);
				if (arch_name.IsEmpty()) continue;
				m_stream = new FbZipInputStream(arch_name, file_name);
				if (m_stream->IsOk()) return; 
				wxDELETE(m_stream);
			}
		} else {
			file_name = FildFile(file_name, file_path, root);
			if (file_name.IsEmpty()) continue;
			m_stream = new wxFFileInputStream(file_name);
			if (m_stream->IsOk()) { m_filename = file_name; return; }
			wxDELETE(m_stream);
		}
	}
	if (show_error) wxLogError(_("Book not found %s"), error.c_str());
}

FbFileReader::~FbFileReader()
{
	wxDELETE(m_stream);
}

wxString FbFileReader::GetError(const wxString &name, const wxString &path)
{
	if (name.IsEmpty()) {
		wxString path = FbParams(FB_CONFIG_TYPE).Str();
		wxString file; file << m_id << wxT('.') << m_filetype;
		return wxString::Format(wxT("$(%s)/%s"), path.c_str(), file.c_str());
	} else {
		if (path.IsEmpty()) return name;
		return path + wxT('/') + name;
	}
}

static void SaveFile(wxInputStream & in, const wxString &filepath)
{
	FbTempEraser::Add(filepath);
	wxFileOutputStream out(filepath);
	out.Write(in);
	out.Close();
}

static bool GetSystemCommand(const wxString &filename, const wxString &filetype, wxString &command)
{
	FbSmartPtr<wxFileType> ft = wxTheMimeTypesManager->GetFileTypeFromExtension(filetype);
	return ft && ft->GetOpenCommand(&command, wxFileType::MessageParameters(filename, wxEmptyString));
}

#ifdef __WXGTK__

// helper class for storing arguments as char** array suitable for passing to
// execvp(), whatever form they were passed to us
class FbArgsArray
{
public:
    FbArgsArray(const wxArrayString& args)
    {
        Init(args.size());

        for ( int i = 0; i < m_argc; i++ )
        {
            wxWX2MBbuf arg = wxSafeConvertWX2MB(args[i]);
            m_argv[i] = strdup(arg);
        }
    }

    ~FbArgsArray()
    {
        for ( int i = 0; i < m_argc; i++ )
        {
            free(m_argv[i]);
        }

        delete [] m_argv;
    }

    operator char**() const { return m_argv; }

private:
    void Init(int argc)
    {
        m_argc = argc;
        m_argv = new char *[m_argc + 1];
        m_argv[m_argc] = NULL;
    }

    int m_argc;
    char **m_argv;
};

static void FbExecute(const wxString & command, const wxString & filepath)
{
	wxFileName filename = filepath;

	wxArrayString args;
	args.Add(command);
	if (command.BeforeFirst(wxT(' ')) == wxT("wine")) {
		args.Add(wxT("wine"));
		args.Add(command.AfterFirst(wxT(' ')));
		filename.SetPath( FbParams(FB_WINE_DIR));
	}
	args.Add(filename.GetFullPath());

	FbArgsArray argv(args);
	if (fork() == 0) execvp(*argv, argv);
}

static void FbExecute(const wxString & command)
{
	wxArrayString args;
	args.Add(command);
	FbArgsArray argv(args);
	if (fork() == 0) execvp(*argv, argv);
}

#else // __WXGTK__

static void FbExecute(const wxString & command, const wxString & filename)
{
	wxExecute(command + wxT(' ') + wxT('"') + filename + wxT('"'));
}

static void FbExecute(wxString & command)
{
	wxExecute(command);
}

#endif // __WXGTK__

class FbTempFileName : public wxString
{
public:
	virtual ~FbTempFileName() {
		if (!IsEmpty()) wxRemoveFile(*this);
	}
	FbTempFileName & Init(wxInputStream & in) {
		wxString::operator=(wxFileName::CreateTempFileName(wxT("fb")));
		wxFileOutputStream out(*this);
		out.Write(in);
		return * this;
	}
};

static bool NotEqualExt(const wxString & filename, const wxString & filetype)
{
	return filename.Right(filetype.Length() + 1).Lower() != wxString(wxT('.')) + filetype;
}

#include "frames/FbCoolReader.h"

void FbFileReader::Open() const
{
	if (!IsOk()) {
		if ( m_id > 0 ) {
			bool ok = wxMessageBox(_("Download book file?"), _("Confirmation"), wxOK | wxCANCEL) == wxOK;
			if ( ok ) DoDownload();
		} else {
			wxLogError(_("Book not found %s"), m_message.c_str());
		}
		return;
	}

	bool ok = false;
	wxString command;
	wxString sql = wxT("SELECT command FROM types WHERE file_type=?");
	if (!ok) ok = !(command = FbLocalDatabase().Str(m_filetype, sql)).IsEmpty();
	if (!ok) ok = !(command = FbCommonDatabase().Str(m_filetype, sql)).IsEmpty();
	if (command == wxT('*')) { command.Empty(); ok = false; }

	FbTempFileName tempfile;

	#ifdef FB_INCLUDE_READER
	FbSmartPtr<wxInputStream> file;
	if (!ok) {
		wxString filename = GetFileName();
		if (filename.IsEmpty()) filename = tempfile.Init(*m_stream);
		if (FbCoolReader::Open(m_id, filename)) return;
		m_stream->Reset();
	}
	#endif

	#ifdef __WXGTK__
	if (!ok) { command = wxT("xdg-open"); ok = true; }
    #endif

	wxString filename = GetFileName();
	if (filename.IsEmpty() || NotEqualExt(filename, m_filetype)) {
		wxFileName filepath = m_md5sum;
		filepath.SetPath(FbParamItem::GetPath(FB_TEMP_DIR));
		filepath.SetExt(m_filetype);
		filename = filepath.GetFullPath();
		FbTempEraser::Add(filename);
		if (!filepath.DirExists()) filepath.Mkdir(0755, wxPATH_MKDIR_FULL);
		if (tempfile.IsEmpty()) {
			SaveFile(*m_stream, filename);
		} else {
			wxCopyFile(tempfile, filename);
		}
	}

	if (ok) {
		#ifdef __WXMSW__
		filename.Prepend(wxT('"')).Append(wxT('"'));
		ShellExecute(NULL, NULL, command, filename, NULL, SW_SHOW);
		#else
		FbExecute(command, filename);
		#endif
	} else {
		if (GetSystemCommand(filename, m_filetype, command)) {
			FbExecute(command);
		} else {
			FbMessageBox(_("Associated application not found"), m_filetype);
		}
	}
}

void FbFileReader::DoDownload() const
{
	if (m_id<0) return;
	wxString md5sum = FbCommonDatabase().GetMd5(m_id);
	if (md5sum.IsEmpty()) return;

	bool ok = false;
	FbLocalDatabase database;
	int folder = - database.NewId(FB_NEW_DOWNLOAD);
	{
		wxString sql = wxT("UPDATE states SET download=? WHERE md5sum=?");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, folder);
		stmt.Bind(2, md5sum);
		ok = stmt.ExecuteUpdate();
	}
	if (!ok) {
		wxString sql = wxT("INSERT INTO states(download, md5sum) VALUES (?,?)");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, folder);
		stmt.Bind(2, md5sum);
		stmt.ExecuteUpdate();
	}

	{
		FbMasterDownInfo info = FbMasterDownInfo::DT_WAIT;
		FbMasterEvent(ID_UPDATE_MASTER, info, m_id, true).Post();
	}

	{
		FbMasterDownInfo info = FbMasterDownInfo::DT_ERROR;
		FbMasterEvent(ID_UPDATE_MASTER, info, m_id, false).Post();
	}

	wxGetApp().StartDownload();
}

//-----------------------------------------------------------------------------
//  ZipReader
//-----------------------------------------------------------------------------

ZipReader::ZipReader(int id, bool bShowError, bool bInfoOnly)
	:conv(wxT("cp866")), m_file(NULL), m_zip(NULL), m_zipOk(false), m_fileOk(false), m_id(id)
{
	FbCommonDatabase database;

	OpenDownload(database, bInfoOnly);
	if (IsOk()) return;

	FbExtractArray items(database, id);

	wxString error_name;
	wxString sLibraryDir = wxGetApp().GetLibPath();

	for (size_t i = 0; i<items.Count(); i++) {
		FbExtractItem & item = items[i];
		wxString file_name = item.FileName(bInfoOnly);
		if (i==0) error_name = item.ErrorName();
		if (item.id_archive) {
			wxFileName zip_file;
			m_zipOk = item.FindZip(sLibraryDir, zip_file);
			if (m_zipOk) OpenZip(zip_file.GetFullPath(), file_name);
		} else if (item.librusec) {
			if (FbParamItem::IsGenesis()) {
				wxFileName book_file = sLibraryDir + wxT('/') + file_name;
				m_zipOk = book_file.IsOk() && book_file.FileExists();
				if (m_zipOk) OpenFile(book_file.GetFullPath());
			}
		} else {
			wxFileName book_file;
			m_zipOk = item.FindBook(sLibraryDir, book_file);
			if (m_zipOk) {
				if (bInfoOnly && book_file.GetExt().Lower() != wxT("fb2")) {
					m_zipOk = false;
				} else {
					OpenFile(book_file.GetFullPath());
				}
			}
		}
		if (IsOk()) return;
	}
	if (bShowError) wxLogError(_("Book not found %s"), error_name.c_str());
}

ZipReader::~ZipReader()
{
	wxDELETE(m_zip);
	wxDELETE(m_file);
}

void ZipReader::OpenDownload(FbDatabase &database, bool bInfoOnly)
{
	wxString filetype;
	{
		wxString sql = wxT("SELECT md5sum, file_type FROM books WHERE id=?");
		wxSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, m_id);
		wxSQLite3ResultSet result = stmt.ExecuteQuery();
		if ( result.NextRow() ) {
			m_md5sum = result.GetString(0);
			filetype = result.GetString(1).Lower();
			if (filetype == wxT("fb2")) bInfoOnly = false;
		} else return;
	}

	wxFileName zip_file = FbDownloader::GetFilename(m_md5sum, false);
	m_zipOk = zip_file.FileExists();
	if (m_zipOk && !bInfoOnly) {
		m_file = new wxFFileInputStream(zip_file.GetFullPath());
		if ( filetype != wxT("zip") ) {
			unsigned char buf[2];
			size_t count = m_file->Read(buf, 2).LastRead();
			if ( count>1 && buf[0]=='P' && buf[1]=='K') {
				OpenEntry(bInfoOnly);
				if ( IsOk() ) return;
			}
			m_file->SeekI(0);
		}
		wxDELETE(m_zip);
		m_result = m_file;
		m_fileOk = true;
		return;
	}

	zip_file.SetExt(wxT("zip"));
	m_zipOk = zip_file.FileExists();
	if (m_zipOk) {
		m_file = new wxFFileInputStream(zip_file.GetFullPath());
		m_zip = new wxZipInputStream(*m_file, conv);
		m_result = m_zip;
		OpenEntry(bInfoOnly);
	}
}

bool ZipReader::OpenEntry(bool bInfoOnly)
{
	m_result = m_zip = new wxZipInputStream(*m_file, conv);
    FbSmartPtr<wxZipEntry> entry;
	while (entry = m_zip->GetNextEntry()) {
		bool ok = (entry->GetInternalName().Right(4).Lower() == wxT(".fbd")) == bInfoOnly;
		if (ok) return m_fileOk = m_zip->OpenEntry(*entry);
	}
	return false;
}

void ZipReader::OpenZip(const wxString &zipname, const wxString &filename)
{
	m_file = new wxFFileInputStream(zipname);
	m_result = m_zip = new wxZipInputStream(*m_file, conv);

	m_zipOk  = m_file->IsOk();
	m_fileOk = m_zipOk && FindEntry(filename);

	wxString zipText = (zipname.IsNull() ? zipname : wxT(" ") + zipname );
	wxString fileText = (filename.IsNull() ? filename : wxT(" ") + filename );
	m_info = wxString::Format(_("Not found an archive (%s), containing file (%s)."), zipText.c_str(), fileText.c_str());
}

void ZipReader::OpenFile(const wxString &filename)
{
	m_file = new wxFFileInputStream(filename);
	m_zip = NULL;
	m_result = m_file;

	m_zipOk  = true;
	m_fileOk = m_file->IsOk();

	wxString zipText = wxEmptyString;
	wxString fileText = (filename.IsNull() ? filename : wxT(" ") + filename );
	m_info = wxString::Format(_("Not found file %s"), fileText.c_str());
}

bool ZipReader::FindEntry(const wxString &file_name)
{
    FbSmartPtr<wxZipEntry> entry;
	while (entry = m_zip->GetNextEntry()) {
		if (entry->GetInternalName() == file_name) return m_zip->OpenEntry(*entry);
	}
	return false;
}

void ZipReader::ShowError()
{
	wxMessageBox(GetErrorText());
}
