#include "stdafx.h"
#include "Player.h"
#include "resource.h"

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

    HACCEL hAccel = LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator((HWND)m_wndMain, hAccel, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    DestroyAcceleratorTable(hAccel);

    return static_cast<int>(msg.wParam);
}