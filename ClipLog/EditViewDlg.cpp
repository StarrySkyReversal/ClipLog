#include "pch.h"
#include "EditViewDlg.h"
#include "afxdialogex.h"

#include "ClipLogDlg.h"

IMPLEMENT_DYNAMIC(EditViewDlg, CDialogEx)

HWND EditViewDlg::m_hDialogHandle = NULL;

EditViewDlg::EditViewDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_EDIT_VIEW_DIALOG, pParent)
{
}

EditViewDlg::~EditViewDlg()
{
}

void EditViewDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RICH_EDIT1, m_editCtrl);
    DDX_Text(pDX, IDC_RICH_EDIT1, m_editData);
}

BOOL EditViewDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_hDialogHandle = this->GetSafeHwnd();

    CHARFORMAT cf;
    cf.cbSize = sizeof(CHARFORMAT);
    cf.dwMask = CFM_SIZE;
    cf.yHeight = 220; // 200 twips = 10 points (1 point = 20 twips)

    m_editCtrl.SetDefaultCharFormat(cf);

    return TRUE;  // return TRUE unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(EditViewDlg, CDialogEx)
    ON_BN_CLICKED(IDC_EDIT_VIEW_COPY, &EditViewDlg::OnCopyButton)
    ON_BN_CLICKED(IDC_EDIT_VIEW_CANCEL, &EditViewDlg::OnCancelButton)
    ON_WM_TIMER()
END_MESSAGE_MAP()

void EditViewDlg::SetEditData(const CString& data)
{
    m_editData = data;
    if (m_editCtrl.GetSafeHwnd()) {
        m_editCtrl.SetWindowTextW(data);
    }
}

CString EditViewDlg::GetEditData() const
{
    CString data;
    m_editCtrl.GetWindowTextW(data);
    return data;
}

BOOL EditViewDlg::PreTranslateMessage(MSG* pMsg)
{
    //if (pMsg->message == WM_KEYDOWN)
    //{
    //    if ((GetKeyState(VK_CONTROL) & 0x8000) && (pMsg->wParam == 'C'))
    //    {
    //        HandleCopy();
    //        return TRUE;
    //    }
    //    else if ((GetKeyState(VK_CONTROL) & 0x8000) && pMsg->wParam == 'X')
    //    {
    //        HandleCut();
    //        return TRUE;
    //    }
    //}

    return CDialogEx::PreTranslateMessage(pMsg);
}

void EditViewDlg::HandleCopy()
{
    m_editCtrl.SetFocus();
    m_editCtrl.SetSel(0, -1);

    m_editCtrl.Copy();
}

void EditViewDlg::HandleCut()
{
    m_editCtrl.SetFocus();
    m_editCtrl.SetSel(0, -1);

    m_editCtrl.Cut();
}

void EditViewDlg::OnCopyButton()
{
    HandleCopy();
}

void EditViewDlg::OnCancel()
{
    CDialogEx::OnCancel();
}

void EditViewDlg::OnCancelButton()
{
    CDialogEx::OnCancel();
}
