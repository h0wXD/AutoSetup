
#include "stdafx.h"
#include "SetupLoader.h"
#include "SetupLoaderDialog.h"
#include "afxdialogex.h"

#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SetupLoaderDialog::SetupLoaderDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(SetupLoaderDialog::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void SetupLoaderDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(SetupLoaderDialog, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &SetupLoaderDialog::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &SetupLoaderDialog::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_LAUNCH, &SetupLoaderDialog::OnBnClickedButtonLaunch)
END_MESSAGE_MAP()

BOOL SetupLoaderDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

#ifdef _DEBUG
	m_sFilePath = "C:/Share/Setups/Launchy2.5.exe";
	GetDlgItem(IDC_EDIT_SETUPFILE)->SetWindowTextW(m_sFilePath);
	
	GetDlgItem(IDC_BUTTON_LAUNCH)->EnableWindow(m_sFilePath.GetLength() != 0);

	if (m_sFilePath.GetLength() != 0)
	{
		GetDlgItem(IDC_BUTTON_BROWSE)->SetFocus();
	}
#else
	GetDlgItem(IDC_BUTTON_LAUNCH)->EnableWindow(FALSE);
#endif

	return FALSE;
}

void SetupLoaderDialog::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR SetupLoaderDialog::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void SetupLoaderDialog::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}

void SetupLoaderDialog::OnBnClickedButtonBrowse()
{
	CString sFileName;
	wchar_t szFileFilter[] = { L"Setup Files (*.exe;*.msi)|*.exe;*.msi|All Files (*.*)|*.*||" };
	CFileDialog openFileDialog(TRUE, L"*.exe", NULL, 0, szFileFilter, NULL); // TODO: hWnd?
	
	if (openFileDialog.DoModal() == IDOK)
	{
		m_sFilePath = openFileDialog.GetPathName();
		GetDlgItem(IDC_EDIT_SETUPFILE)->SetWindowTextW(openFileDialog.GetFileName());
	}
	else
	{
		m_sFilePath = "";
		GetDlgItem(IDC_EDIT_SETUPFILE)->SetWindowTextW(L"");
	}

	GetDlgItem(IDC_BUTTON_LAUNCH)->EnableWindow(m_sFilePath.GetLength() != 0);
}

void SetupLoaderDialog::ShowError(TCHAR *pszMessage, unsigned int uiError /*= ERROR_SUCCESS*/)
{
	if (uiError)
	{
		wchar_t szBuffer[1024] = {0};
		std::swprintf(szBuffer, sizeof(szBuffer), L"%s [ErrorCode: 0x%08x]", pszMessage, uiError);
		MessageBox(szBuffer, L"Error Occurred!", MB_ICONEXCLAMATION | MB_ICONERROR | MB_TOPMOST);
		return;
	}

	MessageBox(pszMessage, L"Error Occurred!", MB_ICONEXCLAMATION | MB_ICONERROR | MB_TOPMOST);
}

void SetupLoaderDialog::OnBnClickedButtonLaunch()
{
	DWORD dwBinaryType = 0;
	_TCHAR * pszArchitecture = nullptr;

	if (::GetBinaryType(m_sFilePath, &dwBinaryType))
	{
		switch (dwBinaryType)
		{
			case SCS_32BIT_BINARY:
				pszArchitecture = L"x86";
				break;
			case SCS_64BIT_BINARY:
				pszArchitecture = L"x64";
				break;
			default:
				ShowError(L"Selected binary file is not supported!");
				return;
		}
	}
	
	STARTUPINFO startupInfo = {0};
	PROCESS_INFORMATION processInformation = {0};
	_TCHAR szCurrentPath[MAX_PATH] = {0};
	_TCHAR szDllPath[MAX_PATH] = {0};
	_TCHAR szLoaderPath[MAX_PATH] = {0};
	_TCHAR szCommandLine[MAX_PATH * 3] = {0};
	
	::GetModuleFileName(NULL, szCurrentPath, sizeof(szCurrentPath));
	wcsrchr(szCurrentPath, '\\')[1] = 0;
	
	swprintf(szDllPath, sizeof(szDllPath) - 1, L"%sAutoSetup_%s.dll", szCurrentPath, pszArchitecture);
	swprintf(szLoaderPath, sizeof(szLoaderPath) - 1, L"%sSetupLoader_%s.exe", szCurrentPath, pszArchitecture);
	swprintf(szCommandLine, sizeof(szCommandLine) - 1, L"\"%s\" -d \"%s\" \"%s\"", szLoaderPath, szDllPath, m_sFilePath);
	
	if (!::CreateProcess(szLoaderPath, szCommandLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo, &processInformation))
	{
		_TCHAR szErrorMessage[MAX_PATH] = {0};
		swprintf(szErrorMessage, sizeof(szErrorMessage) - 1, L"Could not start loader.\r\nCommandLine: %s\r\n", szCommandLine);
		ShowError(szErrorMessage);
	}
}
