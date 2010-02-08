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
		FbMasterData()
			: m_id(0), m_type(FT_NOTHING) {};
		FbMasterData(const int id, const FbFolderType type)
			: m_id(id), m_type(type) {};
		FbMasterData(const FbMasterData & data)
			: m_id(data.m_id), m_type(data.m_type) {};
		bool operator ==(const FbMasterData & data)
			{ return m_id == data.m_id && m_type == data.m_type; };
		FbMasterData & operator =(const FbMasterData & data)
			{ m_id = data.m_id; m_type = data.m_type; return *this; };
		const int GetId() const { return m_id; };
		const FbFolderType GetType() const { return m_type; };
		void Show(wxEvtHandler * frame) const {};
	private:
		int m_id;
		FbFolderType m_type;
};

#endif // __FBMASTERDATA_H__
