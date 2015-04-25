
#pragma once

class SetupLoaderDialog : public CDialogEx
{
	CString m_sFilePath;

public:
	SetupLoaderDialog(CWnd *pParent = NULL);

	enum { IDD = IDD_SETUPLOADER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange *pDX);

private:
	void ShowError(TCHAR *pszMessage, unsigned int uiError = ERROR_SUCCESS);

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonBrowse();
	afx_msg void OnBnClickedButtonLaunch();
};
