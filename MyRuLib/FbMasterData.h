#ifndef __FBMASTERDATA_H__
#define __FBMASTERDATA_H__

#include <wx/wx.h>
#include <wx/treebase.h>
#include "FbBookList.h"
#include "FbDatabase.h"
#include "FbThread.h"

class FbFrameBase;

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
		virtual FbMasterData * Clone() const = 0;
		virtual const int GetId() const = 0;
		virtual const FbFolderType GetType() const = 0;
		virtual void Show(FbFrameBase * frame) const = 0;
	protected:
		class AggregateFunction : public wxSQLite3AggregateFunction
		{
			public:
				virtual void Aggregate(wxSQLite3FunctionContext& ctx);
				virtual void Finalize(wxSQLite3FunctionContext& ctx);
		};
		class BaseThread: public FbThread
		{
			public:
				BaseThread(FbFrameBase * frame, FbMasterData const * data);
			protected:
				virtual wxString GetSQL(const wxString & condition);
				virtual wxString GetOrder();
				virtual void CreateList(wxSQLite3ResultSet &result);
				virtual void CreateTree(wxSQLite3ResultSet &result);
				virtual void InitDatabase(FbCommonDatabase &database);
				void EmptyBooks();
				void FillBooks(wxSQLite3ResultSet &result);
			protected:
				static wxCriticalSection sm_queue;
				AggregateFunction m_aggregate;
				FbGenreFunction m_genre;
				FbFrameBase * m_frame;
				FbListMode m_mode;
				wxString m_filter;
				wxString m_order;
		};
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
		virtual FbMasterData * Clone() const
			{ return new FbMasterAuthor(*this); };
	public:
		virtual const int GetId() const { return m_id; };
		virtual const FbFolderType GetType() const { return FT_AUTHOR; };
		virtual void Show(FbFrameBase * frame) const;
	private:
		int m_id;
	protected:
		class AuthorThread: public BaseThread
		{
			public:
				AuthorThread(FbFrameBase * frame, FbMasterAuthor const * data)
					: BaseThread(frame, data), m_author(data->m_id), m_number(sm_skiper.NewNumber()) {};
				virtual void *Entry();
			protected:
				virtual void CreateTree(wxSQLite3ResultSet &result);
				virtual wxString GetSQL(const wxString & condition);
			private:
				static FbThreadSkiper sm_skiper;
				int m_author;
				int m_number;
		};
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
		virtual FbMasterData * Clone() const
			{ return new FbMasterSeqname(*this); };
	public:
		virtual const int GetId() const { return m_id; };
		virtual const FbFolderType GetType() const { return FT_SEQNAME; };
		virtual void Show(FbFrameBase * frame) const;
	private:
		int m_id;
	protected:
		class SequenThread: public BaseThread
		{
			public:
				SequenThread(FbFrameBase * frame, FbMasterSeqname const * data)
					: BaseThread(frame, data), m_master(data->m_id), m_number(sm_skiper.NewNumber()) {};
				virtual void *Entry();
			protected:
				virtual void CreateTree(wxSQLite3ResultSet &result);
				virtual wxString GetOrder();
			private:
				static FbThreadSkiper sm_skiper;
				int m_master;
				int m_number;
		};

};

class FbMasterGenre: public FbMasterData
{
	public:
		FbMasterGenre(const int id = 0)
			: m_id(id) {};
		FbMasterGenre(const FbMasterGenre & data)
			: m_id(data.m_id) {};
		virtual bool operator ==(const FbMasterData & data) const
			{ return GetType() == data.GetType() && operator==((FbMasterGenre&)data); };
		bool operator ==(const FbMasterGenre & data) const
			{ return m_id == data.m_id; };
		virtual FbMasterData * Clone() const
			{ return new FbMasterGenre(*this); };
	public:
		virtual const int GetId() const { return m_id; };
		virtual const FbFolderType GetType() const { return FT_GENRE; };
		virtual void Show(FbFrameBase * frame) const;
	private:
		int m_id;
	protected:
		class SubgenreFunction : public wxSQLite3ScalarFunction
		{
			public:
				SubgenreFunction(int code);
			protected:
				virtual void Execute(wxSQLite3FunctionContext& ctx);
			private:
				wxString m_code;
		};
		class GenresThread: public BaseThread
		{
			public:
				GenresThread(FbFrameBase * frame, FbMasterGenre const * data)
					: BaseThread(frame, data), m_subgenre(data->m_id), m_number(sm_skiper.NewNumber()) {};
				virtual void *Entry();
			private:
				static FbThreadSkiper sm_skiper;
				SubgenreFunction m_subgenre;
				int m_number;
		};
};

class FbMasterDownld: public FbMasterData
{
	public:
		FbMasterDownld(const int id = 0)
			: m_id(id) {};
		FbMasterDownld(const FbMasterDownld & data)
			: m_id(data.m_id) {};
		virtual bool operator ==(const FbMasterData & data) const
			{ return GetType() == data.GetType() && operator==((FbMasterDownld&)data); };
		bool operator ==(const FbMasterDownld & data) const
			{ return m_id == data.m_id; };
		virtual FbMasterData * Clone() const
			{ return new FbMasterDownld(*this); };
	public:
		virtual const int GetId() const { return m_id; };
		virtual const FbFolderType GetType() const { return FT_DOWNLOAD; };
		virtual void Show(FbFrameBase * frame) const;
	private:
		int m_id;
	protected:
		class DownldThread: public BaseThread
		{
			public:
				DownldThread(FbFrameBase * frame, FbMasterDownld const * data)
					: BaseThread(frame, data), m_folder(data->m_id), m_number(sm_skiper.NewNumber()) {};
				virtual void *Entry();
			protected:
				virtual wxString GetOrder();
			private:
				static FbThreadSkiper sm_skiper;
				int m_folder;
				int m_number;
		};
};

class FbMasterSearch: public FbMasterData
{
	public:
		FbMasterSearch(const wxString &title, const wxString &author = wxEmptyString)
			: m_title(title), m_author(author) {};
		FbMasterSearch(const FbMasterSearch & data)
			: m_title(data.m_title), m_author(data.m_author) {};
		virtual bool operator ==(const FbMasterData & data) const
			{ return GetType() == data.GetType() && operator==((FbMasterSearch&)data); };
		bool operator ==(const FbMasterSearch & data) const
			{ return m_title == data.m_title && m_author == data.m_author; };
		virtual FbMasterData * Clone() const
			{ return new FbMasterSearch(*this); };
	public:
		virtual const int GetId() const { return 0; };
		virtual const FbFolderType GetType() const { return FT_DOWNLOAD; };
		virtual void Show(FbFrameBase * frame) const;
	private:
		wxString m_title;
		wxString m_author;
	protected:
		class SearchThread: public BaseThread
		{
			public:
				SearchThread(FbFrameBase * frame, FbMasterSearch const * data)
					: BaseThread(frame, data), m_title(data->m_title), m_author(data->m_author) {};
				virtual void *Entry();
			private:
				wxString m_title;
				wxString m_author;
		};
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
		virtual FbMasterData * Clone() const
			{ return new FbMasterFolder(*this); };
	public:
		virtual const int GetId() const { return m_id; };
		virtual const FbFolderType GetType() const { return m_type; };
		virtual void Show(FbFrameBase * frame) const;
	private:
		int m_id;
		FbFolderType m_type;
	protected:
		class FolderThread: public BaseThread
		{
			public:
				FolderThread(FbFrameBase * frame, FbMasterFolder const * data)
					: BaseThread(frame, data), m_folder(data->m_id), m_number(sm_skiper.NewNumber()), m_type(data->m_type) {};
				virtual void *Entry();
			private:
				static FbThreadSkiper sm_skiper;
				int m_folder;
				int m_number;
				FbFolderType m_type;
		};
};

#endif // __FBMASTERDATA_H__
