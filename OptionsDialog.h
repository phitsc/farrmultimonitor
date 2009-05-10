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
    void initializeCheckBox(CButton& button, int id, const char* optionName, bool defaultValue)
    {
        button = GetDlgItem(id);
        button.SetCheck(_optionsFile.getValue(optionName, defaultValue) ? BST_CHECKED : BST_UNCHECKED);
    }

    void storeCheckBoxValue(const CButton& button, const char* optionName)
    {
        _optionsFile.setValue(optionName, (button.GetCheck() == BST_CHECKED));
    }

    void initializeEditField(CEdit& edit, int id, const char* optionName, long defaultValue)
    {
        edit = GetDlgItem(id);
        
        std::stringstream stream;
        stream << _optionsFile.getValue(optionName, defaultValue);

        edit.SetWindowText(stream.str().c_str());
    }

    template<typename Type>
    void storeEditFieldValue(const CEdit& edit, const char* optionName, const Type& minValue, const Type& maxValue)
    {
        char buffer[1024];
        edit.GetWindowText(buffer, 1024);

        std::stringstream stream;
        stream << buffer;

        Type value;
        stream >> value;

        if(value < minValue)
        {
            value = minValue;
        }
        else if(value > maxValue)
        {
            value = maxValue;
        }

        _optionsFile.setValue(optionName, value);
    }

    LRESULT onInitDialog(UINT, WPARAM, LPARAM, BOOL&)
    {
        initializeCheckBox(_enableMultiMonitorCheck, IDC_CHECK_USE_MULTIMON, "EnableMultiMonitor", true);
        initializeCheckBox(_moveRelativeCheck, IDC_CHECK_MOVE_RELATIVE, "MoveRelative", true);
        initializeCheckBox(_resizeRelativeCheck, IDC_CHECK_RESIZE_RELATIVE, "ResizeRelative", true);
        initializeCheckBox(_moveToMonitorWithMouse, IDC_CHECK_MOVE_TO_MOUSE, "ShowOnMonitorWithMouse", false);
        initializeCheckBox(_centerCheck, IDC_CHECK_CENTER, "Center", false);
        initializeCheckBox(_alwaysCenterCheck, IDC_CHECK_ALWAYS_CENTER, "CenterAlways", false);
        initializeCheckBox(_enableHotkeysCheck, IDC_CHECK_ENABLE_HOTKEYS, "EnableHotkeys", true);
        initializeCheckBox(_resizeHCheck, IDC_CHECK_RESIZE_H, "ResizePercent", false);
        initializeCheckBox(_resizeVCheck, IDC_CHECK_RESIZE_V, "ResizePercentV", false);
        initializeEditField(_resizeHValueEdit, IDC_EDIT_RESIZE_H, "ResizePercentValue", 80L);
        initializeEditField(_resizeVValueEdit, IDC_EDIT_RESIZE_V, "ResizePercentValueV", 80L);

        return 0;
    }

    LRESULT onOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        storeCheckBoxValue(_enableMultiMonitorCheck, "EnableMultiMonitor");
        storeCheckBoxValue(_moveRelativeCheck, "MoveRelative");
        storeCheckBoxValue(_resizeRelativeCheck, "ResizeRelative");
        storeCheckBoxValue(_moveToMonitorWithMouse, "ShowOnMonitorWithMouse");
        storeCheckBoxValue(_centerCheck, "Center");
        storeCheckBoxValue(_alwaysCenterCheck, "CenterAlways");
        storeCheckBoxValue(_enableHotkeysCheck, "EnableHotkeys");
        storeCheckBoxValue(_resizeHCheck, "ResizePercent");
        storeCheckBoxValue(_resizeVCheck, "ResizePercentV");
        storeEditFieldValue<long>(_resizeHValueEdit, "ResizePercentValue", 10L, 100L);
        storeEditFieldValue<long>(_resizeVValueEdit, "ResizePercentValueV", 10L, 100L);

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
    CButton _alwaysCenterCheck;
    CButton _enableHotkeysCheck;
    CButton _resizeHCheck;
    CButton _resizeVCheck;

    CEdit   _resizeHValueEdit;
    CEdit   _resizeVValueEdit;
};

//-----------------------------------------------------------------------
