
#include "stdafx.h"
#include "SetupLoader.h"
#include "SetupLoaderDialog.h"
#include "afxdialogex.h"

#include <string>
#include <thread>

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
	GetDlgItem(IDC_EDIT_SETUPFILE)->SetWindowText(m_sFilePath);
	
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
	TCHAR szFileFilter[] = { "Setup Files (*.exe;*.msi)|*.exe;*.msi|All Files (*.*)|*.*||" };
	CFileDialog openFileDialog(TRUE, "*.exe", NULL, 0, szFileFilter, NULL); // TODO: hWnd?
	
	if (openFileDialog.DoModal() == IDOK)
	{
		m_sFilePath = openFileDialog.GetPathName();
		GetDlgItem(IDC_EDIT_SETUPFILE)->SetWindowText(openFileDialog.GetFileName());
	}
	else
	{
		m_sFilePath = "";
		GetDlgItem(IDC_EDIT_SETUPFILE)->SetWindowText("");
	}

	GetDlgItem(IDC_BUTTON_LAUNCH)->EnableWindow(m_sFilePath.GetLength() != 0);
}

void SetupLoaderDialog::ShowError(TCHAR *pszMessage, unsigned int uiError /*= ERROR_SUCCESS*/)
{
	if (uiError)
	{
		TCHAR szBuffer[1024] = {0};
		sprintf_s(szBuffer, "%s [ErrorCode: 0x%08x]", pszMessage, uiError);
		MessageBox(szBuffer, "Error Occurred!", MB_ICONEXCLAMATION | MB_ICONERROR | MB_TOPMOST);
		return;
	}

	MessageBox(pszMessage, "Error Occurred!", MB_ICONEXCLAMATION | MB_ICONERROR | MB_TOPMOST);
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
				pszArchitecture = "x86";
				break;
			case SCS_64BIT_BINARY:
				pszArchitecture = "x64";
				break;
			default:
				ShowError("Selected binary file is not supported!");
				return;
		}
	}
	
	STARTUPINFO startupInfo = {0};
	PROCESS_INFORMATION processInformation = {0};
	SECURITY_ATTRIBUTES securityAttributes = {0};
	char szTempOutputBuffer[1024] = {0};
	_TCHAR szCurrentPath[MAX_PATH] = {0};
	_TCHAR szDllPath[MAX_PATH] = {0};
	_TCHAR szLoaderPath[MAX_PATH] = {0};
	_TCHAR szCommandLine[MAX_PATH * 3] = {0};
	HANDLE hChildOutputRead = INVALID_HANDLE_VALUE;
	HANDLE hChildOutputWrite = INVALID_HANDLE_VALUE;
	DWORD dwExitCode = 0;
	DWORD dwBytesRead;
	DWORD dwBytesAvailable;
	DWORD dwBufferSpaceRemainder = sizeof(szTempOutputBuffer) - 1;
	char * pszWriteOffset = szTempOutputBuffer;
	
	memset(&securityAttributes, 0, sizeof(securityAttributes));
	securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	securityAttributes.bInheritHandle = TRUE;
	securityAttributes.lpSecurityDescriptor = NULL;

	if (!::CreatePipe(&hChildOutputRead, &hChildOutputWrite, &securityAttributes, 0))
	{
		ShowError("Could not create pipe!", ::GetLastError());
		return;
	}

	startupInfo.cb			= sizeof(startupInfo);
	startupInfo.hStdInput	= NULL;
	startupInfo.hStdOutput	= hChildOutputWrite;
	startupInfo.hStdError	= hChildOutputWrite;
	startupInfo.dwFlags		|= STARTF_USESTDHANDLES;
	startupInfo.wShowWindow	|= SW_HIDE;

	::GetModuleFileName(NULL, szCurrentPath, sizeof(szCurrentPath));
	strrchr(szCurrentPath, '\\')[1] = 0;
	
	sprintf_s(szDllPath, "%sAutoSetup_%s.dll", szCurrentPath, pszArchitecture);
	sprintf_s(szLoaderPath, "%sSetupLoader_%s.exe", szCurrentPath, pszArchitecture);
	sprintf_s(szCommandLine, "\"%s\" -d \"%s\" \"%s\"", szLoaderPath, szDllPath, m_sFilePath);

	if (!::CreateProcess(szLoaderPath, szCommandLine, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS|CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &processInformation))
	{
		_TCHAR szErrorMessage[MAX_PATH * 3] = {0};
		sprintf_s(szErrorMessage, "Unable to start the SetupLoader.\r\nPlease verify whether the file exists.\r\nCommandLine: %s\r\n", szCommandLine);
		ShowError(szErrorMessage, ::GetLastError());
		::CloseHandle(hChildOutputRead);
		::CloseHandle(hChildOutputWrite);
		return;
	}

	while (true)
	{
		dwBytesAvailable = 0;

		::WaitForSingleObject(processInformation.hProcess, 25);
		::GetExitCodeProcess(processInformation.hProcess, &dwExitCode);
		::PeekNamedPipe(hChildOutputRead, NULL, 0, NULL, &dwBytesAvailable, NULL);

		if (dwBytesAvailable > 0 &&
			dwBufferSpaceRemainder != 0)
		{
			dwBytesRead = 0;

			::ReadFile(hChildOutputRead, pszWriteOffset, dwBufferSpaceRemainder, &dwBytesRead, NULL);

			dwBufferSpaceRemainder -= dwBytesRead;
			pszWriteOffset += dwBytesRead;
		}
		else
		{
			if (dwExitCode != STILL_ACTIVE)
			{
				break;
			}
		}
	}

	if (dwExitCode == 1)
	{
		ShowError(szTempOutputBuffer);
	}
	
	::CloseHandle(hChildOutputRead);
	::CloseHandle(hChildOutputWrite);
}
