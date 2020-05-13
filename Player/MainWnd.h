#pragma once
#include "XWnd.h"

class CMainWnd : public CXWnd
{
public:
    CMainWnd();
    virtual ~CMainWnd();

    virtual ATOM Register();
    BOOL Create();
protected:
    virtual void OnSize(HWND hwnd, UINT state, int cx, int cy);
private:
};

