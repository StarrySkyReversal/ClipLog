
// ClipLogDlg.h : header file
//

#pragma once
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>

struct ClipboardData
{
	CString text;
	std::chrono::system_clock::time_point timestamp;
};

// CClipLogDlg dialog
class CClipLogDlg : public CDialogEx
{
// Construction
public:
	CClipLogDlg(CWnd* pParent = nullptr);	// standard constructor
	virtual ~CClipLogDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIPLOG_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	std::atomic<bool> m_bClipboardUpdated;
	std::queue<CString> m_ClipboardQueue;
	std::mutex m_ClipboardQueueMutex;
	std::thread m_ClipboardThread;
	std::atomic<bool> m_bExitThread;
	std::condition_variable m_ClipboardCV;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	LRESULT OnDrawClipboard(WPARAM wParam, LPARAM lParam);
	LRESULT OnChangeCbChain(WPARAM wParam, LPARAM lParam);

	void ClipboardThreadFunc();
	void ProcessClipboard();

	void CClipLogDlg::LoadDataToListCtrl();

	void CClipLogDlg::OnListDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

public:
	HWND hWndEditViewDlg;
	HWND m_hWndNextViewer;
	static bool s_bIgnoreNextClipboardUpdate;

	CListCtrl m_listCtrl;
	CEdit m_searchEdit;
	CButton m_searchButton;

	ClipboardData m_lastClipboardData;
};
