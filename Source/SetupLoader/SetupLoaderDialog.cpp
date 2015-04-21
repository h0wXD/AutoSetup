
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

void SetupLoaderDialog::ShowError(TCHAR *pszMessage)
{
	MessageBox(pszMessage, L"Error Occurred!", MB_ICONEXCLAMATION | MB_ICONERROR);
}

void SetupLoaderDialog::OnBnClickedButtonLaunch()
{
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInformation;

	memset(&startupInfo, 0, sizeof(startupInfo));
	memset(&processInformation, 0, sizeof(processInformation));

	BOOL bResult = ::CreateProcess(m_sFilePath, NULL, NULL, NULL, FALSE,
        CREATE_SUSPENDED,
        NULL,
        NULL,
        &startupInfo,
        &processInformation);
	
	HANDLE processHandle = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, processInformation.dwProcessId);
	if (processHandle != NULL)
	{
		char szCurrentPath[MAX_PATH];
		const char * pszDllFile = "AutoSetup_x86.dll";
		std::size_t nLength;

		memset(szCurrentPath, 0, sizeof(szCurrentPath));
		::GetModuleFileNameA(NULL, szCurrentPath, sizeof(szCurrentPath));    
		
		for (int i = std::strlen(szCurrentPath); i > 0; i--)
		{
			if (szCurrentPath[i] == '\\')
			{
				szCurrentPath[i + 1] = 0;
				break;
			}
		}

		strcat_s(szCurrentPath, pszDllFile);
		nLength = std::strlen(szCurrentPath);

		void * pLoadLibrary = (void *)::GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");
		void * pDllEntry = (void *)::VirtualAllocEx(processHandle, NULL, nLength, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

		if (::WriteProcessMemory(processHandle, (LPVOID)pDllEntry, pszDllFile, nLength, NULL) == FALSE)
		{
			ShowError(L"Couldn't write process memory!");
		}

		HANDLE threadHandle = ::CreateRemoteThread(processHandle, NULL, NULL, (LPTHREAD_START_ROUTINE)pLoadLibrary, pDllEntry, NULL, NULL);

		if (threadHandle == INVALID_HANDLE_VALUE)
		{
			ShowError(L"Couldn't create remote thread!");
		}

		::CloseHandle(processHandle);
	}
	else
	{
		ShowError(L"Could not inject DLL!");
	}

	DWORD dwResult = ::ResumeThread(processInformation.hThread);
}
