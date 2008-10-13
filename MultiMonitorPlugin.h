#pragma once

#include "OptionsFile.h"

#include <string>

//-----------------------------------------------------------------------

namespace util
{
    class DisplayDevice;
    class DisplayDevices;
}

//-----------------------------------------------------------------------

struct Options
{
    Options(const OptionsFile& optionsFile);
    void update(const OptionsFile& optionsFile);

    bool enableMultiMon;
    bool showOnMonitorWithMouse;
    bool moveRelative;
    bool resizeRelative;
    bool center;
    bool alwaysCenter;
    bool enableHotkeys;
};

//-----------------------------------------------------------------------

class MultiMonitorPlugin
{
public:
    MultiMonitorPlugin(HWND farrWindowHandle, const std::string& modulePath);

    void handleMessage(UINT message, WPARAM wParam, LPARAM lParam, HWND lastActiveWindow);
    bool handleKeyboardMessage(WPARAM wParam, LPARAM lParam);

    void showOptions();

private:
    void handleShowWindow(HWND lastActiveWindow);
    void handleWindowPosChanging(const WINDOWPOS& windowPos);
    void moveToNextLastDisplayDevice(bool toNext);
    void moveWindowFromSourceToTargetDisplayDevice(const util::DisplayDevice* source, const util::DisplayDevice* target);

    static const util::DisplayDevice* getDisplayDevice(const util::DisplayDevices& displayDevices, HWND hWnd);
    static const util::DisplayDevice* getDisplayDeviceContainingMouse(const util::DisplayDevices& displayDevices);

    // returns new RECT
    CRect centerFarrWindow();

    HWND _farrWindowHandle;

    OptionsFile _optionsFile;
    Options     _options;

    bool        _isVisible;
    CRect       _lastWindowRect;
};

//-----------------------------------------------------------------------
