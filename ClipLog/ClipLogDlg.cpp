
// ClipLogDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "ClipLog.h"
#include "ClipLogDlg.h"
#include "afxdialogex.h"
#include "Database.h"
#include "afxrich.h"
#include "EditViewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool CClipLogDlg::s_bIgnoreNextClipboardUpdate = false;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CClipLogDlg dialog



CClipLogDlg::CClipLogDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CLIPLOG_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CClipLogDlg::~CClipLogDlg()
{
	m_bExitThread = true;
	m_ClipboardCV.notify_all();
	if (m_ClipboardThread.joinable())
	{
		m_ClipboardThread.join();
	}
}

void CClipLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
	DDX_Control(pDX, IDC_EDIT1, m_searchEdit);
	DDX_Control(pDX, IDC_BUTTON1, m_searchButton);
}

BEGIN_MESSAGE_MAP(CClipLogDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_DRAWCLIPBOARD, &CClipLogDlg::OnDrawClipboard)
	ON_MESSAGE(WM_CHANGECBCHAIN, &CClipLogDlg::OnChangeCbChain)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CClipLogDlg::OnListDoubleClick)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CClipLogDlg message handlers

BOOL CClipLogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	AfxInitRichEdit2();

	// TODO: Add extra initialization here
	m_listCtrl.ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

	m_listCtrl.InsertColumn(0, _T("No"), LVCFMT_LEFT, 60);
	m_listCtrl.InsertColumn(1, _T("Content"), LVCFMT_LEFT, 445);
	m_listCtrl.InsertColumn(2, _T("Time"), LVCFMT_LEFT, 160);

	m_listCtrl.SetColumnWidth(0, 0);
	m_listCtrl.SetColumnWidth(2, 0);

	m_hWndNextViewer = SetClipboardViewer();

	m_ClipboardThread = std::thread(&CClipLogDlg::ClipboardThreadFunc, this);

	LoadDataToListCtrl();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClipLogDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClipLogDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClipLogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CClipLogDlg::PreTranslateMessage(MSG* pMsg)
{
	// 拦截回车键消息
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		
		//HWND hWndClipboardOwner = ::GetClipboardOwner();
		//if (hWndClipboardOwner == hWndEditViewDlg) {
		//	TRACE(_T("AAAAAAAAAAAAAAAAAAA"));
		//	return TRUE;
		//}
		// 确认焦点在编辑框中
		//CWnd* pFocusWnd = GetFocus();
		//if (pFocusWnd && pFocusWnd->GetDlgCtrlID() == IDC_EDIT1)
		//{
		//	// 获取编辑框内容
		//	CString searchText;
		//	GetDlgItemText(IDC_EDIT1, searchText);

		//	// 执行搜索操作
		//	FilterListItems(searchText);

		//	// 返回 TRUE 表示消息已处理
		//	return TRUE;
		//}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

CString TrimLeadingWhitespace(const CString& str)
{
	CString trimmedStr = str;
	trimmedStr.TrimLeft(_T(" \t\n\r"));
	if (trimmedStr.IsEmpty())
	{
		return _T(" ");
	}

	return trimmedStr;
}

LRESULT CClipLogDlg::OnDrawClipboard(WPARAM wParam, LPARAM lParam)
{
	CDialogEx::OnDrawClipboard();

	if (s_bIgnoreNextClipboardUpdate)
	{
		s_bIgnoreNextClipboardUpdate = false;
	}
	else
	{
		m_bClipboardUpdated = true;
		m_ClipboardCV.notify_all();
	}

	if (m_hWndNextViewer != NULL)
	{
		::SendMessage(m_hWndNextViewer, WM_DRAWCLIPBOARD, 0, 0);
	}

	return 0;
}

LRESULT CClipLogDlg::OnChangeCbChain(WPARAM wParam, LPARAM lParam)
{
	HWND hWndRemove = (HWND)wParam;
	HWND hWndNext = (HWND)lParam;

	if (hWndRemove == m_hWndNextViewer)
	{
		m_hWndNextViewer = hWndNext;
	}
	else if (m_hWndNextViewer)
	{
		::SendMessage(m_hWndNextViewer, WM_CHANGECBCHAIN, wParam, lParam);
	}
	return 0;
}

void CClipLogDlg::ClipboardThreadFunc()
{
	while (!m_bExitThread)
	{
		std::unique_lock<std::mutex> lock(m_ClipboardQueueMutex);
		m_ClipboardCV.wait(lock, [this]() { return m_bClipboardUpdated || m_bExitThread; });

		if (m_bExitThread)
			break;

		m_bClipboardUpdated = false;
		lock.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(32));

		ProcessClipboard();
	}
}

void CClipLogDlg::ProcessClipboard()
{
	if (OpenClipboard())
	{
		HANDLE hClipboardData = GetClipboardData(CF_TEXT);
		if (hClipboardData)
		{
			char* pchData = (char*)GlobalLock(hClipboardData);
			if (pchData)
			{
				CString text(pchData);
				GlobalUnlock(hClipboardData);
				{
					std::lock_guard<std::mutex> lock(m_ClipboardQueueMutex);
					m_ClipboardQueue.push(text);
				}
			}
		}
		CloseClipboard();
	}

	while (!m_ClipboardQueue.empty())
	{
		CString text;
		{
			std::lock_guard<std::mutex> lock(m_ClipboardQueueMutex);
			text = m_ClipboardQueue.front();
			m_ClipboardQueue.pop();
		}
		//SetDlgItemText(IDC_EDIT1, text);
		std::chrono::system_clock::time_point currentTimestamp = std::chrono::system_clock::now();

		//auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTimestamp - m_lastClipboardData.timestamp).count();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTimestamp - m_lastClipboardData.timestamp).count();

		if (text == m_lastClipboardData.text && duration <= 512) {
			continue;
		}

		m_lastClipboardData.text = text;
		m_lastClipboardData.timestamp = std::chrono::system_clock::now();

		CTime currentTime = CTime::GetCurrentTime();
		CString timeStr = currentTime.Format(_T("%Y-%m-%d %H:%M:%S"));

		//////////////////
		std::string dbname = "clipboard.db";

		CT2CA pszConvertedAnsiString1(text);
		std::string strStd(pszConvertedAnsiString1);

		CT2CA pszConvertedAnsiString2(timeStr);
		std::string timeStrStd(pszConvertedAnsiString2);

		InitializeDatabase(dbname);
		int id = SaveDataToDatabase(dbname, strStd, timeStrStd);

		////////////////////

		CString newStrClipboard = TrimLeadingWhitespace(text);
		CString line;
		int index = 0;
		CString noStr;
		AfxExtractSubString(line, newStrClipboard, index, '\n');

		line = TrimLeadingWhitespace(line);

		noStr.Format(_T("%d"), m_listCtrl.GetItemCount() + 1);

		int itemIndex = m_listCtrl.InsertItem(index, noStr);
		m_listCtrl.SetItemText(itemIndex, 1, line);
		m_listCtrl.SetItemText(itemIndex, 2, timeStr);
		m_listCtrl.SetItemData(itemIndex, id);
	}

	int itemCount = m_listCtrl.GetItemCount();
	for (int i = 0; i < itemCount; i++)
	{
		CString strNumber;
		strNumber.Format(_T("%d"), itemCount - i);
		m_listCtrl.SetItemText(i, 0, strNumber);
	}
}

void CClipLogDlg::LoadDataToListCtrl()
{
	std::string dbname = "clipboard.db";
	auto dataVector = LoadAllDataFromDatabase(dbname);

	m_listCtrl.DeleteAllItems();

	size_t totalItems = dataVector.size();
	for (int i = 0; i < totalItems; ++i)
	{
		const auto& record = dataVector[i];

		CString noStr;
		noStr.Format(_T("%lld"), (totalItems - i));
		CString dataStr = CA2T(record.data.c_str());
		CString timestampStr = CA2T(record.created_at.c_str());

		int nItem = m_listCtrl.InsertItem(i, noStr);
		m_listCtrl.SetItemText(nItem, 1, dataStr.Mid(0, 64));
		m_listCtrl.SetItemText(nItem, 2, timestampStr);

		m_listCtrl.SetItemData(nItem, record.id);
	}
}

void CClipLogDlg::OnListDoubleClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int itemIndex = pNMItemActivate->iItem;

	if (itemIndex != -1)
	{
		std::string dbname = "clipboard.db";
		int id = static_cast<int>(m_listCtrl.GetItemData(itemIndex));
		auto record = GetRecordById(dbname, id);

		CString data = CA2T(record.data.c_str());
		CString timestamp = CA2T(record.created_at.c_str());

		EditViewDlg editDlg;
		editDlg.SetEditData(data);
		if (editDlg.DoModal() == IDCANCEL) {
			hWndEditViewDlg = editDlg.m_hWnd;
			TRACE(_T("editDlg IDCANCEL\n"));
		}
	}

	*pResult = 0;
}