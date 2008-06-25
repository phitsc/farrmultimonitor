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
    void initializeCheckBox(CButton& button, int id, const char* optionName, bool defaultValue)
    {
        button = GetDlgItem(id);
        button.SetCheck(_optionsFile.getValue(optionName, defaultValue) ? BST_CHECKED : BST_UNCHECKED);
    }

    void storeCheckBoxValue(const CButton& button, const char* optionName)
    {
        _optionsFile.setValue(optionName, (button.GetCheck() == BST_CHECKED));
    }

    LRESULT onInitDialog(UINT, WPARAM, LPARAM, BOOL&)
    {
        initializeCheckBox(_enableMultiMonitorCheck, IDC_CHECK_USE_MULTIMON, "EnableMultiMonitor", true);
        initializeCheckBox(_moveRelativeCheck, IDC_CHECK_MOVE_RELATIVE, "MoveRelative", true);
        initializeCheckBox(_resizeRelativeCheck, IDC_CHECK_RESIZE_RELATIVE, "ResizeRelative", true);
        initializeCheckBox(_moveToMonitorWithMouse, IDC_CHECK_MOVE_TO_MOUSE, "ShowOnMonitorWithMouse", false);
        initializeCheckBox(_centerCheck, IDC_CHECK_CENTER, "Center", false);
        initializeCheckBox(_thresholdCheck, IDC_CHECK_THRESHOLD, "EnableThreshold", false);
        
        _thresholdEditBox = GetDlgItem(IDC_EDIT_THRESHOLD);
        
        std::stringstream stream;
        stream << _optionsFile.getValue("Threshold", 0L);
        _thresholdEditBox.SetWindowText(stream.str().c_str());

        return 0;
    }

    LRESULT onOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        storeCheckBoxValue(_enableMultiMonitorCheck, "EnableMultiMonitor");
        storeCheckBoxValue(_moveRelativeCheck, "MoveRelative");
        storeCheckBoxValue(_resizeRelativeCheck, "ResizeRelative");
        storeCheckBoxValue(_moveToMonitorWithMouse, "ShowOnMonitorWithMouse");
        storeCheckBoxValue(_centerCheck, "Center");
        storeCheckBoxValue(_thresholdCheck, "EnableThreshold");

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
    CButton _moveToMonitorWithMouse;
    CButton _centerCheck;
    CButton _thresholdCheck;
    CEdit   _thresholdEditBox;
};

//-----------------------------------------------------------------------
