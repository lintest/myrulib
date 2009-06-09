/***************************************************************
 * Name:      FbParser.h
 * Purpose:   FictionBook XML parser
 * Author:    Kandrashin Denis (mail@kandr.ru)
 * Created:   2009-06-03
 * Copyright: Kandrashin Denis (www.lintest.ru)
 * License:
 **************************************************************/

#ifndef FBPARSER_H
#define FBPARSER_H

#define FB_DEBUG_PARSING

#include <wx/wx.h>
#include <wx/defs.h>
#include <wx/string.h>
#include <wx/object.h>
#include <wx/list.h>
#include <wx/stream.h>

enum FbNodeType {
	FB_UNKNOWN,
	FB_DOCUMENT,
	FB_DESCRIPTION,
	FB_AUTHOR,
	FB_TITLE,
	FB_GENRE,
	FB_BOOKDATE,
	FB_ANNOTATION,
};

class FbProperty;

class FbNode {
public:
	FbNode(const wxString &name = wxEmptyString, FbNodeType type = FB_UNKNOWN);
	virtual ~FbNode();
	void Append(FbNode *node);
	FbNode * Find(const wxString &name);
	wxString Prop(const wxString &name);
	wxString GetName() const {return m_name;};
	void AddProperty(const wxString& name, const wxString& value);
	void AddProperty(FbProperty *prop);
#ifdef FB_DEBUG_PARSING
	void Print(wxString &text, int level = 0);
#endif //FB_DEBUG_PARSING
public:
	wxString m_name;
	wxString m_text;
	FbNodeType m_type;
	FbNode * m_parent;
	FbNode * m_child;
	FbNode * m_next;
	FbProperty * m_properties;
private:
	FbProperty * m_last_prop;
	FbNode * m_last_child;
};

class FbDocument : public wxObject
{
public:
    FbDocument();
    FbDocument(const wxString& filename, const wxString& encoding = wxT("UTF-8"));
    FbDocument(wxInputStream& stream, const wxString& encoding = wxT("UTF-8"));
    virtual ~FbDocument() { wxDELETE(m_root); }

    // Parses .xml file and loads data. Returns TRUE on success, FALSE
    // otherwise.
    virtual bool Load(const wxString& filename, const wxString& encoding = wxT("UTF-8"));
    virtual bool Load(wxInputStream& stream, const wxString& encoding = wxT("UTF-8"));

    bool IsOk() const { return m_root != NULL; }

    // Returns root node of the document.
    FbNode *GetRoot() const { return m_root; }
    void SetRoot(FbNode *node) { wxDELETE(m_root); m_root = node; }
    void SetVersion(const wxString& version) { m_version = version; }
    void SetFileEncoding(const wxString& encoding) { m_fileEncoding = encoding; }

    // Returns version of document (may be empty).
    wxString GetVersion() const { return m_version; }
    // Returns encoding of document (may be empty).
    // Note: this is the encoding original file was saved in, *not* the
    // encoding of in-memory representation!
    wxString GetFileEncoding() const { return m_fileEncoding; }

	wxString GetAuthor(FbNode *root) const;

private:
    wxString   m_version;
    wxString   m_fileEncoding;
    FbNode *m_root;

    DECLARE_CLASS(FbDocument)
};

#endif // FBPARSER_H
