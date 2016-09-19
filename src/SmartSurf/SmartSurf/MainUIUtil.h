#pragma once
#include "IE/ProcessManager.h"

class CMainUIUtil : public CWindowImpl<CMainUIUtil>
{
public:

	static CMainUIUtil* Inst();

	CMainUIUtil();
	~CMainUIUtil() {}

	void Init(HWND hWnd);

	BEGIN_MSG_MAP_EX(CMainUIUtil)
		MSG_WM_CREATE(OnCreate)
		CHAIN_MSG_MAP_MEMBER(m_processMgr)
	END_MSG_MAP()

	int OnCreate(LPCREATESTRUCT lpCreateStruct);

private:

	CProcessManager m_processMgr;
	HWND			m_hAttachWnd;
};
