#include "stdafx.h"
#include "ProcessManager.h"
#include <Psapi.h>
#include "Module.h"


CProcessManager::CProcessManager()
{

}

CProcessManager::~CProcessManager()
{

}

void CProcessManager::Init()
{
}

LRESULT CProcessManager::OnCreatePage(UINT, WPARAM wParam, LPARAM, BOOL&)
{
	return 0;
}

LRESULT CProcessManager::OnAxUICreate(UINT, WPARAM wParam, LPARAM, BOOL&)
{
	return 0;
}