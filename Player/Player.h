#pragma once
#include "App.h"
#include "MainWnd.h"

using namespace std;

class CPlayer : public CApp
{
public:
    CPlayer();
    virtual ~CPlayer();
    
    virtual int Run(PWSTR pCmdLine, int nCmdShow);
private:
    CMainWnd m_wndMain;
};