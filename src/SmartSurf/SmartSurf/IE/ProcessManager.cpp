#include "stdafx.h"
#include "ProcessManager.h"
#include <Psapi.h>
#include "Module.h"

#define TIMER_MONITOR_PROCESS	0x100

// ²âÊÔ£¬ÁÙÊ±¸Ä³É3M
#define MAX_WOKING_SET			300 * 1024 * 1024

CProcessManager::CProcessManager()
{

}

CProcessManager::~CProcessManager()
{

}

void CProcessManager::Init()
{
	::SetTimer(Common::Module::GetMainUIUtil(), TIMER_MONITOR_PROCESS, 1000, NULL);
}

LRESULT CProcessManager::OnCreatePage(UINT, WPARAM wParam, LPARAM, BOOL&)
{
	HWND hChildFrame = (HWND)wParam;
	bool bForceMainProcess = false;
	bool bCreateNewProcess = false;

	if (m_ProcessMap.empty())
	{
		ProcessData mainProcessData;
		mainProcessData.hProcess = GetCurrentProcess();
		mainProcessData.dwProcessID = GetCurrentProcessId();
		m_ProcessMap[mainProcessData.dwProcessID] = mainProcessData;
	}

	m_ProcessMap.begin()->second.CreateNewPageInProcess(hChildFrame);

	return 0;
}

LRESULT CProcessManager::OnAxUICreate(UINT, WPARAM wParam, LPARAM, BOOL&)
{
	HWND hAxui = (HWND)wParam;
	DWORD dwPid = 0;
	GetWindowThreadProcessId(hAxui, &dwPid);

	if (m_ProcessMap.empty())
	{
		ProcessData mainProcessData;
		mainProcessData.hProcess = GetCurrentProcess();
		mainProcessData.dwProcessID = GetCurrentProcessId();
		m_ProcessMap[mainProcessData.dwProcessID] = mainProcessData;
	}

	ProcessMap::iterator it = m_ProcessMap.find(dwPid);
	TuoAssert(it != m_ProcessMap.end());
	if (it == m_ProcessMap.end())
		return 0;
	it->second.OnSetAxUIWnd(hAxui);
	
	return 0;
}