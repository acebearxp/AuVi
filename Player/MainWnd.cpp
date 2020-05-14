#include "stdafx.h"
#include "MainWnd.h"
#include "App.h"

CMainWnd::CMainWnd()
{
    m_wstrClsName.assign(L"MainWnd");
}


CMainWnd::~CMainWnd()
{
}

ATOM CMainWnd::Register()
{
    HINSTANCE hInstance = CApp::GetApp()->GetInstance();

    WNDCLASS wcls;
    ZeroMemory(&wcls, sizeof(WNDCLASS));
    wcls.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcls.lpfnWndProc = CXWnd::WindowProc;
    wcls.hInstance = hInstance;
    wcls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcls.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcls.lpszClassName = m_wstrClsName.c_str();
    wcls.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_APP));
    wcls.lpszMenuName = MAKEINTRESOURCE(IDR_MENU_MAIN);


    return RegisterClass(&wcls);
}

BOOL CMainWnd::Create()
{
    int cxScreen = GetSystemMetrics(SM_CXSCREEN);
    int cyScreen = GetSystemMetrics(SM_CYSCREEN);
    int width = cxScreen > 800 ? 800 : cxScreen;
    int height = cyScreen > 600 ? 600 : cyScreen;

    HINSTANCE hInstance = CApp::GetApp()->GetInstance();

    WNDCLASS cls;
    if (!GetClassInfo(hInstance, m_wstrClsName.c_str(), &cls)) Register();

    wstring wstrTitle(L"FFmpeg");
    m_hwnd = CreateWindow(m_wstrClsName.c_str(), wstrTitle.c_str(), WS_OVERLAPPEDWINDOW|WS_VISIBLE,
        (cxScreen - width) / 2, (cyScreen - height) / 2, width, height,
        NULL, NULL, hInstance, this);
    
    if (!m_hwnd) {
        MessageBox(NULL, L"Create window failed", L"Error", MB_OK);
        return FALSE;
    }

    ShowWindow(m_hwnd, SW_SHOW);

    return TRUE;
}

BOOL CMainWnd::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    BOOL bOK = CXWnd::OnCreate(hwnd, lpCreateStruct);
    
    return bOK;
}

void CMainWnd::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    RECT rc;
    GetClientRect(m_hwnd, &rc);
}

void CMainWnd::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case ID_FILE_OPEN:
        OpenFile();
        break;
    case ID_FILE_EXIT:
        PostQuitMessage(0);
        break;
    default:
        CXWnd::OnCommand(hwnd, id, hwndCtl, codeNotify);
    }
}

void CMainWnd::OpenFile()
{
    CComPtr<IFileOpenDialog> spOpenFileDlg;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&spOpenFileDlg));

    FILEOPENDIALOGOPTIONS options;
    hr = spOpenFileDlg->GetOptions(&options);
    hr = spOpenFileDlg->SetOptions(options | FOS_FILEMUSTEXIST);

    COMDLG_FILTERSPEC desc[2];
    desc[0].pszName = L"Video Files";
    desc[0].pszSpec = L"*.mp4;*.mov;*.mkv;*.wmv";
    desc[1].pszName = L"All Files";
    desc[1].pszSpec = L"*.*";
    hr = spOpenFileDlg->SetFileTypes(2, desc);
    hr = spOpenFileDlg->SetFileTypeIndex(0);

    CComPtr<IShellItem> spItems;
    hr = spOpenFileDlg->Show(m_hwnd);
    if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED)) return;
    hr = spOpenFileDlg->GetResult(&spItems);

    PWSTR wszPath = NULL;
    hr = spItems->GetDisplayName(SIGDN_FILESYSPATH, &wszPath);
    if (FAILED(hr)) {
        MessageBox(m_hwnd, L"打开文件失败", L"ERROR", MB_OK | MB_ICONERROR);
        return;
    }

    wstring wstrPath(wszPath);
    CoTaskMemFree(wszPath);

    m_ffmpeg.SetTarget(m_hwnd);
    int nRet = m_ffmpeg.Open(wstrPath);
    if (nRet != 0) {
        wstring wstrError = m_ffmpeg.GetLastError();
        MessageBox(m_hwnd, wstrError.c_str(), L"ERROR", MB_OK | MB_ICONERROR);
    }
}