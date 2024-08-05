#include "pch.h"
#include "EditViewDlg.h"
#include "afxdialogex.h"

#include "ClipLogDlg.h"

IMPLEMENT_DYNAMIC(EditViewDlg, CDialogEx)

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

    //m_editCtrl.SetWindowTextW(m_editData);

    return TRUE;  // return TRUE unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(EditViewDlg, CDialogEx)
    ON_BN_CLICKED(IDC_EDIT_VIEW_COPY, &EditViewDlg::OnCopyButton)
    ON_BN_CLICKED(IDC_EDIT_VIEW_CANCEL, &EditViewDlg::OnCancelButton)
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
    if (pMsg->message == WM_KEYDOWN)
    {
        if ((GetKeyState(VK_CONTROL) & 0x8000) && (pMsg->wParam == 'C'))
        {
            HandleCopy();
            return TRUE;
        }
        else if ((GetKeyState(VK_CONTROL) & 0x8000) && pMsg->wParam == 'X')
        {
            HandleCut();
            return TRUE;
        }
    }

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

void EditViewDlg::OnCopyButton() {
    HandleCopy();
}

void EditViewDlg::OnCancel() {
    CDialogEx::OnCancel();
}

void EditViewDlg::OnCancelButton()
{
    CDialogEx::OnCancel();
}

void EditViewDlg::CopyRichEditContentToClipboard()
{
    // Get the length of the text in the RichEdit control
    int nTextLength = m_editCtrl.GetTextLength();

    // Allocate memory for the text
    CString strText;
    m_editCtrl.GetWindowText(strText);

    // Open the clipboard
    if (OpenClipboard())
    {
        // Clear the clipboard
        EmptyClipboard();

        // Allocate global memory for the text
        HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, (nTextLength + 1) * sizeof(TCHAR));
        if (hGlob)
        {
            // Copy the text to the global memory
            void* pGlobData = GlobalLock(hGlob);
            if (pGlobData)
            {
                memcpy(pGlobData, strText.GetBuffer(), (nTextLength + 1) * sizeof(TCHAR));
                GlobalUnlock(hGlob);
            }

            // Set the clipboard data
#ifdef _UNICODE
            SetClipboardData(CF_UNICODETEXT, hGlob);
#else
            SetClipboardData(CF_TEXT, hGlob);
#endif
        }

        // Release the buffer
        strText.ReleaseBuffer();

        // Close the clipboard
        CloseClipboard();
    }
}
