#include "stdafx.h"
#include "Player.h"

CPlayer g_app;

CPlayer::CPlayer()
{
}


CPlayer::~CPlayer()
{
}

int CPlayer::Run(PWSTR pCmdLine, int nCmdShow)
{
    m_wndMain.Create();

    return CApp::Run(pCmdLine, nCmdShow);
}