#include "stdafx.h"
#include "Player.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrev, PWSTR pCmdLine, int nCmdShow)
{
    CPlayer*pApp = dynamic_cast<CPlayer*>(CPlayer::GetApp());

    if (!pApp->Init(hInstance)) return GetLastError();
    int nRet = pApp->Run(pCmdLine, nCmdShow);
    pApp->UnInit();

    return nRet;
}