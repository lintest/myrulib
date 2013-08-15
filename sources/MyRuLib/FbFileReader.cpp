#include "FbFileReader.h"
#include "FbParams.h"
#include "FbConst.h"
#include "FbExtractInfo.h"
#include "FbDatabase.h"
#include "MyRuLibApp.h"
#include "FbDataPath.h"
#include "FbDownloader.h"
#include "FbCollection.h"
#include "FbParsingCtx.h"

#include <wx/mimetype.h>
#include <wx/stdpaths.h>
#include "wx/FbSQLite3.h"

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

FbZipInputStream::FbZipInputStream(const wxString & archname, const wxString & filename, bool info)
	: wxZipInputStream(new wxFFileInputStream(archname), GetConv866()), m_ok(true)
{
	FbSmartPtr<wxZipEntry> fileEntry = 0;
	FbSmartPtr<wxZipEntry> infoEntry = 0;
	wxString infoname = filename.BeforeLast(wxT('.')) + wxT(".fbd");
	while (FbSmartPtr<wxZipEntry> entry = GetNextEntry()) {
		if (entry->GetInternalName() == filename) { fileEntry = entry.Reset(); }
		else if (entry->GetInternalName() == infoname) { infoEntry = entry.Reset(); }
	}
	m_entry = (info && infoEntry) ? infoEntry.Reset() : fileEntry.Reset();
	m_ok = m_entry && OpenEntry(*m_entry);
}

FbZipInputStream::FbZipInputStream(const wxString & archname, const wxString & filename)
	: wxZipInputStream(new wxFFileInputStream(archname), GetConv866()), m_ok(true)
{
	while (m_entry = GetNextEntry()) {
		bool ok = (m_entry->GetInternalName() == filename);
		if (ok) { m_ok = OpenEntry(*m_entry); return; }
	}
	m_ok = false;
}

FbZipInputStream::FbZipInputStream(const wxString & archname, bool info)
	: wxZipInputStream(new wxFFileInputStream(archname), GetConv866()), m_ok(true)
{
	while (m_entry = GetNextEntry()) {
		bool ok = (m_entry->GetInternalName().Right(4).Lower() == wxT(".fbd")) == info;
		if (ok) { m_ok = OpenEntry(*m_entry); return; }
	}
	m_ok = false;
}

FbZipInputStream::FbZipInputStream(wxInputStream * stream, bool info)
	: wxZipInputStream(stream, GetConv866()), m_ok(true)
{
	while (m_entry = GetNextEntry()) {
		wxString name = m_entry->GetInternalName();
		if (name == wxT("mimetype")) { m_ok = false; return; } // Check EPUB
		bool ok = (name.Right(4).Lower() == wxT(".fbd")) == info;
		if (ok) { m_ok = OpenEntry(*m_entry); return; }
	}
	m_ok = false;
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

static wxString FindFile(const wxString & name, const wxString & path, const wxString & root)
{
	if (root.IsEmpty()) return name;
	if (name.IsEmpty()) return wxEmptyString;
	wxFileName filename = name;
	filename.Normalize(wxPATH_NORM_ALL, root);
	if (filename.FileExists()) return filename.GetFullPath();
	if (!path.IsEmpty() && wxFileName::FileExists(path)) return path;
	return wxEmptyString;
}

wxString Ext(const wxString &filename);

FbFileReader::FbFileReader(int id, bool info)
	: m_id(id), m_stream(NULL)
{
	FbCommonDatabase database;
	{
		wxString sql = wxT("SELECT md5sum, file_type FROM books WHERE id="); sql << id;
		FbSQLite3ResultSet result = database.ExecuteQuery(sql);
		if (!result.NextRow()) return;
		m_md5sum = result.GetString(0).Lower();
		m_filetype = result.GetString(1).Lower();
		if (m_filetype == wxT("fb2")) info = false;
		if (m_filetype == wxT("epub")) info = false;
	}

	{
		wxString filename = FbDownloader::GetFilename(m_md5sum);
		if (wxFileName::FileExists(filename)) {
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

	wxString root = wxGetApp().GetLibPath();
	wxString sql = wxT("SELECT DISTINCT id_archive,file_name,file_path,1,id_archive*id_archive FROM books WHERE id=?1 UNION ");
	sql << wxT("SELECT DISTINCT id_archive,file_name,file_path,2,id_archive*id_archive FROM files WHERE id_book=?1 ORDER BY 4,5");
	FbSQLite3Statement stmt = database.PrepareStatement(sql);
	stmt.Bind(1, id);
	FbSQLite3ResultSet result = stmt.ExecuteQuery();
	while ( result.NextRow() ) {
		bool primary = result.GetInt(3) == 1;
		wxString file_name = result.GetString(1);
		if (primary) m_message = GetError(file_name);
		if (file_name.IsEmpty()) continue;
		if (int arch = result.GetInt(0)) {
			wxString sql = wxT("SELECT id,file_name,file_path FROM archives WHERE id="); sql << arch;
			FbSQLite3ResultSet result = database.ExecuteQuery(sql);
			if (result.NextRow()) {
				wxString arch_name = result.GetString(1);
				if (primary) m_message = GetError(file_name, arch_name);
				arch_name = FindFile(arch_name, result.GetString(2), root);
				if (arch_name.IsEmpty()) continue;
				m_stream = new FbZipInputStream(arch_name, file_name, info);
				if (m_stream->IsOk()) {
					if ((m_filetype == wxT("zip") || Ext(file_name) != wxT("zip"))) return;
					m_stream = new FbMemoryInputStream(m_stream, m_stream->GetLength());
					m_stream = new FbZipInputStream(m_stream, info);
					if (m_stream->IsOk()) return;
				}
				wxDELETE(m_stream);
			}
		} else {
			if (file_name.IsEmpty()) file_name << m_id / 1000 * 1000 << wxT('/') << m_md5sum;
			file_name = FindFile(file_name, result.GetString(2), root);
			if (file_name.IsEmpty()) continue;
			m_stream = new wxFFileInputStream(file_name);
			if (m_stream->IsOk()) { m_filename = file_name; return; }
			wxDELETE(m_stream);
		}
	}
}

FbFileReader::~FbFileReader()
{
	wxDELETE(m_stream);
}

wxString FbFileReader::GetFileName() const
{
	if (!m_filename.IsEmpty()) return m_filename;
	return wxString() << m_id << wxT('.') << m_filetype;
}

wxString FbFileReader::GetError(const wxString &name, const wxString &path)
{
	if (name.IsEmpty()) {
		wxString path = FbParams(FB_CONFIG_TYPE).Str();
		wxString file; file << m_id << wxT('.') << m_filetype;
		return wxString::Format(wxT("$(%s)/%s"), path.c_str(), file.c_str());
	} else {
		if (path.IsEmpty()) return name;
		return path + wxT(':') + wxT(' ') + name;
	}
}

static void SaveFile(wxInputStream & in, const wxString &filepath)
{
	FbTempEraser::Add(filepath);
	wxFileOutputStream out(filepath);
	out.Write(in);
	out.Close();
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

#else // __WXGTK__

static bool GetSystemCommand(const wxString &filename, const wxString &filetype, wxString &command)
{
	FbSmartPtr<wxFileType> ft = wxTheMimeTypesManager->GetFileTypeFromExtension(filetype);
	return ft && ft->GetOpenCommand(&command, wxFileType::MessageParameters(filename, wxEmptyString));
}

static void FbExecute(const wxString & command, const wxString & filename)
{
	wxExecute(command + wxT(' ') + wxT('"') + filename + wxT('"'));
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

bool FbFileReader::WrongExt() const
{
	return m_filename.Right(m_filetype.Length() + 1).Lower() != wxString(wxT('.')) + m_filetype;
}

#include "frames/FbCoolReader.h"

void FbFileReader::ShowError() const
{
	wxLogError(_("Book not found %s"), m_message.c_str());
}

void FbFileReader::ShellExecute(const wxString &filename)
{
	wxString filetype = Ext(filename);
#ifdef __WXMSW__
	wxString filepath = filename;
	filepath.Prepend(wxT('"')).Append(wxT('"'));
	wxChar command[MAX_PATH];
	bool ok = (int) FindExecutable(filepath, NULL, command) > 32;
	if (ok) {
		::ShellExecute(NULL, NULL, command, filepath, NULL, SW_SHOW);
	} else {
		FbMessageBox(_("Associated application not found"), filetype);
	}
#else
	#ifdef __WXGTK__
	wxString command = wxT("xdg-open");
	FbExecute(command, filename);
	#else
    wxString command;
	bool ok = GetSystemCommand(filename, filetype, command);
	if (ok) {
		wxExecute(command);
	} else {
		FbMessageBox(_("Associated application not found"), filetype);
	}
    #endif // __WXGTK__
#endif // __WXMSW__
}

void FbFileReader::Open() const
{
	if (!IsOk()) {
		if ( m_id > 0 ) {
			bool ok = wxMessageBox(_("Download book file?"), _("Confirmation"), wxOK | wxCANCEL) == wxOK;
			if ( ok ) DoDownload();
		} else {
			ShowError();
		}
		return;
	}

	bool ok = false;
	wxString command;
	wxString sql = wxT("SELECT command FROM types WHERE file_type=?");
	if (!ok) ok = !(command = FbLocalDatabase().Str(m_filetype, sql)).IsEmpty();
	if (!ok) ok = !(command = FbCommonDatabase().Str(m_filetype, sql)).IsEmpty();
	bool coolreader = !ok && FbParams(FB_USE_COOLREADER);
	if (command == wxT('*')) { coolreader = true; command.Empty(); ok = false; }

	FbTempFileName tempfile;
	#ifdef FB_INCLUDE_READER
	if (coolreader) {
		wxString filename = m_filename;
		if (filename.IsEmpty()) filename = tempfile.Init(*m_stream);
		if (FbCoolReader::Open(m_id, filename)) return;
		m_stream->Reset();
	}
	#else
	wxUnusedVar(coolreader);
	#endif

	#ifdef __WXGTK__
	if (!ok) { command = wxT("xdg-open"); ok = true; }
    #endif

	wxString filename = CreateDataFile(tempfile);

	if (ok) {
		#ifdef __WXMSW__
		filename.Prepend(wxT('"')).Append(wxT('"'));
		::ShellExecute(NULL, NULL, command, filename, NULL, SW_SHOW);
		#else
		FbExecute(command, filename);
		#endif // __WXMSW__
	} else {
		ShellExecute(filename);
	}
}

#ifdef __WXMSW__

#include "config.h"
#include <wininet.h>

static wxString CreateCacheFile(const wxString &url, const wxString &ext, wxInputStream &stream, const wxString &tempfile)
{
	HMODULE hModule = LoadLibrary(wxT("wininet.dll"));
	if (!hModule) return wxEmptyString;

	typedef BOOL (WINAPI * LPCreateUrl)(LPCWSTR lpszUrlName, DWORD dwExpectedFileSize, LPCWSTR lpszFileExtension, LPWSTR lpszFileName, DWORD dwReserved);
    LPCreateUrl pCreateProc = (LPCreateUrl) GetProcAddress( hModule, "CreateUrlCacheEntryW");

	typedef BOOL (WINAPI * LPCommitUrl) (LPCWSTR lpszUrlName, LPCWSTR lpszLocalFileName, FILETIME ExpireTime, FILETIME LastModifiedTime, DWORD CacheEntryType, LPWSTR lpszHeaderInfo, DWORD dwHeaders, LPCWSTR lpszFileExtension, LPCWSTR lpszOriginalUrl);
	LPCommitUrl pCommitProc = (LPCommitUrl) GetProcAddress( hModule, "CommitUrlCacheEntryW");

	if (!pCreateProc || !pCommitProc) return wxEmptyString;

	wxChar filename[MAX_PATH];
	bool ok = pCreateProc(url.c_str(), 0, ext.c_str(), filename, 0);

	if (tempfile.IsEmpty()) {
		SaveFile(stream, filename);
	} else {
		wxCopyFile(tempfile, filename);
	}

	FILETIME time = { 0, 0 };
	pCommitProc(url.c_str(), ext.c_str(), time, time, NORMAL_CACHE_ENTRY, NULL, 0, 0, NULL);

	return filename;
}

wxString FbFileReader::CreateDataFile(const wxString &tempfile) const
{
	if (m_filename.IsEmpty() || WrongExt()) {
		wxString url = wxT(PACKAGE_NAME);
		url << wxT(':') << m_md5sum << wxT('/') << GetFileName();
		wxString filename = CreateCacheFile(url, m_filetype, *m_stream, tempfile);
		if (filename.IsEmpty()) return CreateTempFile(tempfile);
		FbTempEraser::Add(filename);
		return filename;
	}
	return m_filename;
}

void FbFileReader::ShellExecute(const wxString &archname, const wxString &filename)
{
	if (filename.IsEmpty()) return ShellExecute(archname);

	wxFFileInputStream in(archname);
	wxZipInputStream zip(in, GetConv866());
	while (FbSmartPtr<wxZipEntry> entry = zip.GetNextEntry()) {
		if (entry->GetInternalName() != filename) continue;
		if (!zip.OpenEntry(*entry)) break;
		wxString url = wxT(PACKAGE_NAME);
		url << wxT(':') << archname << wxT('/') << filename;
		wxString tempfile = CreateCacheFile(url, Ext(filename), zip, wxEmptyString);
		FbTempEraser::Add(tempfile);
		return ShellExecute(tempfile);
	}

	ShellExecute(archname);
}

#else

wxString FbFileReader::CreateDataFile(const wxString &tempfile) const
{
	if (m_filename.IsEmpty() || WrongExt()) {
		return CreateTempFile(tempfile);
	}
	return m_filename;
}

void FbFileReader::ShellExecute(const wxString &archname, const wxString &filename)
{
	return ShellExecute(archname);
}

#endif // __WXMSW__

wxString FbFileReader::CreateTempFile(const wxString &tempfile) const
{
	wxFileName filepath = m_md5sum;
	filepath.SetPath(FbParamItem::GetPath(FB_TEMP_DIR));
	filepath.SetExt(m_filetype);
	wxString filename = filepath.GetFullPath();
	if (!filepath.DirExists()) filepath.Mkdir(0755, wxPATH_MKDIR_FULL);
	if (tempfile.IsEmpty()) {
		SaveFile(*m_stream, filename);
	} else {
		wxCopyFile(tempfile, filename);
	}
	FbTempEraser::Add(filename);
	return filename;
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
		FbSQLite3Statement stmt = database.PrepareStatement(sql);
		stmt.Bind(1, folder);
		stmt.Bind(2, md5sum);
		ok = stmt.ExecuteUpdate();
	}
	if (!ok) {
		wxString sql = wxT("INSERT INTO states(download, md5sum) VALUES (?,?)");
		FbSQLite3Statement stmt = database.PrepareStatement(sql);
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

