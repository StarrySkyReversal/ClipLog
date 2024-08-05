#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "afxrich.h" // 添加这一行
#include "resource.h"
#include "ClipLogDlg.h"

#define WM_TRAY_NOTIFY (WM_USER + 1)

class EditViewDlg : public CDialogEx
{
    DECLARE_DYNAMIC(EditViewDlg)

public:
    EditViewDlg(CWnd* pParent = nullptr);
    virtual ~EditViewDlg();

    void SetEditData(const CString& data);
    CString GetEditData() const;

    static HWND m_hDialogHandle;
    static HWND GetDialogHandle() { return m_hDialogHandle; }

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG1 };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog(); // 确保初始化函数声明
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    virtual void OnCancel(); // 覆盖 OnCancel 以处理对话框关闭


    DECLARE_MESSAGE_MAP()

private:
    CString m_editData;
    CRichEditCtrl m_editCtrl;

    void HandleCopy();
    void HandleCut();

    afx_msg void OnCopyButton();
    afx_msg void OnCancelButton();

};
