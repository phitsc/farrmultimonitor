#include "resource.h"

#include "OptionsFile.h"

#include <sstream>

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
        _thresholdCheck = GetDlgItem(IDC_CHECK_THRESHOLD);
        _thresholdEditBox = GetDlgItem(IDC_EDIT_THRESHOLD);
        
        _enableMultiMonitorCheck.SetCheck(_optionsFile.getValue("EnableMultiMonitor", true) ? BST_CHECKED : BST_UNCHECKED);
        _moveRelativeCheck.SetCheck(_optionsFile.getValue("MoveRelative", true) ? BST_CHECKED : BST_UNCHECKED);
        _resizeRelativeCheck.SetCheck(_optionsFile.getValue("ResizeRelative", true) ? BST_CHECKED : BST_UNCHECKED);
        _centerCheck.SetCheck(_optionsFile.getValue("Center", false) ? BST_CHECKED : BST_UNCHECKED);
        _thresholdCheck.SetCheck(_optionsFile.getValue("EnableThreshold", false) ? BST_CHECKED : BST_UNCHECKED);

        std::stringstream stream;
        stream << _optionsFile.getValue("Threshold", 0L);
        _thresholdEditBox.SetWindowText(stream.str().c_str());

        return 0;
    }

    LRESULT onOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        _optionsFile.setValue("EnableMultiMonitor", (_enableMultiMonitorCheck.GetCheck() == BST_CHECKED));
        _optionsFile.setValue("MoveRelative", (_moveRelativeCheck.GetCheck() == BST_CHECKED));
        _optionsFile.setValue("ResizeRelative", (_resizeRelativeCheck.GetCheck() == BST_CHECKED));
        _optionsFile.setValue("Center", (_centerCheck.GetCheck() == BST_CHECKED));
        _optionsFile.setValue("EnableThreshold", (_thresholdCheck.GetCheck() == BST_CHECKED));

        char thresholdAsString[100];
        _thresholdEditBox.GetWindowText(thresholdAsString, 100);

        std::stringstream stream(thresholdAsString);
        long threshold = 0;
        stream >> threshold;

        _optionsFile.setValue("Threshold", threshold);

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
    CButton _thresholdCheck;
    CEdit   _thresholdEditBox;
};

//-----------------------------------------------------------------------
