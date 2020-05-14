#pragma once
#include "XWnd.h"
#include "MediaFF4.h"

class CMainWnd : public CXWnd
{
public:
    CMainWnd();
    virtual ~CMainWnd();

    virtual ATOM Register();
    BOOL Create();
protected:
    virtual BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
    virtual void OnSize(HWND hwnd, UINT state, int cx, int cy);
    virtual void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
private:
    MediaFF4 m_ffmpeg;

    void OpenFile();
};

