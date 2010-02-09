#ifndef __FBMASTERDATA_H__
#define __FBMASTERDATA_H__

#include <wx/wx.h>
#include <wx/treebase.h>

enum FbFolderType {
	FT_FOLDER = 1,
	FT_RATING,
	FT_COMMENT,
	FT_DOWNLOAD,
	FT_AUTHOR,
	FT_GENRE,
	FT_SEQNAME,
	FT_NOTHING,
};

class FbMasterData: public wxTreeItemData
{
	public:
		virtual bool operator ==(const FbMasterData & data) const = 0;
		virtual const int GetId() const = 0;
		virtual const FbFolderType GetType() const = 0;
		virtual void Show(wxEvtHandler * frame) const = 0;
};

class FbMasterAuthor: public FbMasterData
{
	public:
		FbMasterAuthor(const int id = 0): m_id(id) {};
		FbMasterAuthor(const FbMasterAuthor & data): m_id(data.m_id) {};
		virtual bool operator ==(const FbMasterData & data) const
			{ return GetType() == data.GetType() && operator==((FbMasterAuthor&)data); };
		bool operator ==(const FbMasterAuthor & data) const
			{ return m_id == data.m_id; };
	public:
		virtual const int GetId() const { return m_id; };
		virtual const FbFolderType GetType() const { return FT_AUTHOR; };
		virtual void Show(wxEvtHandler * frame) const {};
	private:
		int m_id;
};

class FbMasterSeqname: public FbMasterData
{
	public:
		FbMasterSeqname(const int id = 0)
			: m_id(id) {};
		FbMasterSeqname(const FbMasterSeqname & data)
			: m_id(data.m_id) {};
		virtual bool operator ==(const FbMasterData & data) const
			{ return GetType() == data.GetType() && operator==((FbMasterSeqname&)data); };
		bool operator ==(const FbMasterSeqname & data) const
			{ return m_id == data.m_id; };
	public:
		virtual const int GetId() const { return m_id; };
		virtual const FbFolderType GetType() const { return FT_SEQNAME; };
		virtual void Show(wxEvtHandler * frame) const {};
	private:
		int m_id;
};

class FbMasterFolder: public FbMasterData
{
	public:
		FbMasterFolder(const int id, const FbFolderType type)
			: m_id(id), m_type(type) {};
		FbMasterFolder(const FbMasterFolder & data)
			: m_id(data.m_id), m_type(data.m_type) {};
		virtual bool operator ==(const FbMasterData & data) const
			{ return GetType() == data.GetType() && m_id == ((FbMasterFolder&)data).m_id; };
	public:
		virtual const int GetId() const { return m_id; };
		virtual const FbFolderType GetType() const { return m_type; };
		virtual void Show(wxEvtHandler * frame) const {};
	private:
		int m_id;
		FbFolderType m_type;
};

#endif // __FBMASTERDATA_H__
