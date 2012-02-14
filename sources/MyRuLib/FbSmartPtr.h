#ifndef __FBSMARTPTR_H__
#define __FBSMARTPTR_H__

template<class wxtype> 
class FbSmartPtr
{
public:   
	FbSmartPtr(wxtype * object = NULL): m_object(object) {}
   
	~FbSmartPtr() { wxDELETE(m_object); }
	
	wxtype * operator->() { return m_object; }
   
	wxtype & operator*() { return * m_object; }
   
	wxtype * operator&() { return m_object; }
   
	operator bool() { return m_object; }

	bool operator !() { return !m_object; }

	wxtype * Reset() 
	{ 
		wxtype * result = m_object; 
		m_object = NULL; 
		return result;
	}
	
	wxtype * operator=(wxtype * object)
	{
		if (m_object == object) return object;
		wxDELETE(m_object);
		return m_object = object;
	}

private:	
	wxtype * m_object;
};

#endif // __FBSMARTPTR_H__
