#ifndef __FBMASTERINFO_H__
#define __FBMASTERINFO_H__

#include <wx/object.h>
#include "FbThread.h"
#include "FbBookTypes.h"
#include "FbCollection.h"

class FbBookPanel;

class FbMasterInfo : public wxObject
{
	public:
		FbMasterInfo()
			: m_order(0), m_mode(FB2_MODE_LIST) {}
		FbMasterInfo(const FbMasterInfo & info)
			: m_order(info.m_order) {}
		virtual ~FbMasterInfo()
			{}
		FbBookFields GetOrder() const
			{ return m_order; }
		FbMasterInfo & SetOrder(FbBookFields order)
			{ m_order = order; return *this; }
		FbListMode GetMode() const
			{ return m_mode; }
		FbMasterInfo & SetMode(FbListMode mode)
			{ m_mode = mode; return *this; }
	public:
		virtual FbThread * GetThread(FbBookPanel * owner) const = 0;
		virtual FbMasterInfo * Clone() const = 0;
	private:
		FbBookFields m_order;
		FbListMode m_mode;
		DECLARE_CLASS(FbMasterInfo);
};

class FbMasterAuthorInfo: public FbMasterInfo
{
	public:
		FbMasterAuthorInfo(int id)
			: m_id(id) {}
		FbMasterAuthorInfo(const FbMasterAuthorInfo &info)
			: FbMasterInfo(info), m_id(info.m_id) {}
		virtual FbMasterInfo * Clone() const
			{ return new FbMasterAuthorInfo(*this); }
		virtual FbThread * GetThread(FbBookPanel * owner) const;
	private:
		int m_id;
		DECLARE_CLASS(FbMasterAuthorInfo);
};

#endif // __FBMASTERINFO_H__
