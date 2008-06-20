#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlctrls.h>

#include "resource.h"

#include "OptionsFile.h"

//-----------------------------------------------------------------------

class OptionsDialog : public CDialogImpl<OptionsDialog>
{
public:
    enum { IDD = IDD_DIALOG_OPTIONS };

    OptionsDialog(OptionsFile& optionsFile)
        :_optionsFile(optionsFile)
    {}

    BEGIN_MSG_MAP(OptionsDialog)
        MESSAGE_HANDLER(WM_INITDIALOG, onInitDialog)
        COMMAND_ID_HANDLER(IDOK, onOk)
        COMMAND_ID_HANDLER(IDCANCEL, onCancel)
    END_MSG_MAP()

private:
    LRESULT onInitDialog(UINT, WPARAM, LPARAM, BOOL&)
    {
        _enableMultiMonitorCheck = GetDlgItem(IDC_CHECK_USE_MULTIMON);
        _moveRelativeCheck = GetDlgItem(IDC_CHECK_MOVE_RELATIVE);
        _resizeRelativeCheck = GetDlgItem(IDC_CHECK_RESIZE_RELATIVE);
        _centerCheck = GetDlgItem(IDC_CHECK_CENTER);
        
        _enableMultiMonitorCheck.SetCheck(_optionsFile.getValue("EnableMultiMonitor", true) ? BST_CHECKED : BST_UNCHECKED);
        _moveRelativeCheck.SetCheck(_optionsFile.getValue("MoveRelative", true) ? BST_CHECKED : BST_UNCHECKED);
        _resizeRelativeCheck.SetCheck(_optionsFile.getValue("ResizeRelative", true) ? BST_CHECKED : BST_UNCHECKED);
        _centerCheck.SetCheck(_optionsFile.getValue("Center", false) ? BST_CHECKED : BST_UNCHECKED);

        return 0;
    }

    LRESULT onOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        _optionsFile.setValue("EnableMultiMonitor", (_enableMultiMonitorCheck.GetCheck() == BST_CHECKED));
        _optionsFile.setValue("MoveRelative", (_moveRelativeCheck.GetCheck() == BST_CHECKED));
        _optionsFile.setValue("ResizeRelative", (_resizeRelativeCheck.GetCheck() == BST_CHECKED));
        _optionsFile.setValue("Center", (_centerCheck.GetCheck() == BST_CHECKED));

        EndDialog(IDOK);

        return 0;
    }
    LRESULT onCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        EndDialog(IDCANCEL);

        return 0;
    }

private:
    OptionsFile& _optionsFile;

    CButton _enableMultiMonitorCheck;
    CButton _moveRelativeCheck;
    CButton _resizeRelativeCheck;
    CButton _centerCheck;
};

//-----------------------------------------------------------------------
