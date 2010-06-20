#ifndef __FBMASTERINFO_H__
#define __FBMASTERINFO_H__

#include <wx/object.h>
#include "FbThread.h"
#include "FbThread.h"
#include "FbCollection.h"

class FbBookPanel;

class FbMasterInfo : public wxObject
{
	public:
		FbMasterInfo(FbBookFields order)
			: m_order(order) {}
		FbMasterInfo(const FbMasterInfo & info)
			: m_order(info.m_order) {}
		virtual ~FbMasterInfo()
			{}
		virtual FbThread * GetThread(FbBookPanel * owner) const = 0;
		virtual FbMasterInfo * Clone() const = 0;
	protected:
		FbBookFields GetOrder();
	private:
		FbBookFields m_order;
		DECLARE_CLASS(FbMasterInfo);
};

#endif // __FBMASTERINFO_H__
