// Author: linjian
#include "ProcessData.h"
#include "IEDef.h"
typedef std::map<DWORD, ProcessData> ProcessMap;

#define MAKESURE_IN_MAINUIUTIL_THREAD()		TuoAssert(::GetCurrentThreadId() == GetWindowThreadProcessId(Common::Module::GetMainUIUtil(), NULL))

class CProcessManager
{
public:


	CProcessManager();
	~CProcessManager();

	void Init();

	BEGIN_MSG_MAP_EX(CProcessManager)
		MESSAGE_HANDLER(WM_MAINUITIL_CREATEPAGE, OnCreatePage)
		MESSAGE_HANDLER(WM_MAINUIUTIL_NOTIFY_AXUI_CREATED, OnAxUICreate)
	END_MSG_MAP()

	LRESULT OnCreatePage(UINT, WPARAM, LPARAM, BOOL&);
	LRESULT OnAxUICreate(UINT, WPARAM, LPARAM, BOOL&);

private:

	ProcessMap m_ProcessMap;
};
