#pragma once

class CMainFrame : public CWindowImpl<CMainFrame>
{
public:

	CMainFrame();
	~CMainFrame();

private:
	
	BEGIN_MSG_MAP_EX(CMainFrame)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
	END_MSG_MAP()
	BOOL OnEraseBkgnd(CDCHandle dc);

};