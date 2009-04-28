#include "stdafx.h"
#include "MultiMonitorPlugin.h"

#include "DisplayDevices.h"
#include "OptionsDialog.h"
#include "OptionsFile.h"

#include "JrPlugin_GenericShell.h"

#include <algorithm>
#include <sstream>

//-----------------------------------------------------------------------

Options::Options(const OptionsFile& optionsFile)
{
    update(optionsFile);
}

//-----------------------------------------------------------------------

void Options::update(const OptionsFile& optionsFile)
{
    enableMultiMon = optionsFile.getValue("EnableMultiMonitor", true);
    showOnMonitorWithMouse = optionsFile.getValue("ShowOnMonitorWithMouse", false);
    moveRelative = optionsFile.getValue("MoveRelative", true);
    resizeRelative = optionsFile.getValue("ResizeRelative", true);
    center = optionsFile.getValue("Center", false);
    alwaysCenter = optionsFile.getValue("CenterAlways", false);
    enableHotkeys = optionsFile.getValue("EnableHotkeys", true);
    resizePercent = optionsFile.getValue("ResizePercent", false);
    resizePercentValue = optionsFile.getValue("ResizePercentValue", 80L);
}

//-----------------------------------------------------------------------

MultiMonitorPlugin::MultiMonitorPlugin(HWND farrWindowHandle, const std::string& modulePath)
:_farrWindowHandle(farrWindowHandle),
 _optionsFile(modulePath + "\\FARR_MultiMonitor.ini"),
 _options(_optionsFile),
 _isVisible(false)
{
}

//-----------------------------------------------------------------------

const util::DisplayDevice* MultiMonitorPlugin::getDisplayDevice(const util::DisplayDevices& displayDevices, HWND hWnd)
{
    RECT windowRect = { 0 };
    GetWindowRect(hWnd, &windowRect);
    int x = windowRect.left + (windowRect.right - windowRect.left) / 2;

    return displayDevices.getDisplayDeviceContainingX(x);
}

//-----------------------------------------------------------------------

const util::DisplayDevice* MultiMonitorPlugin::getDisplayDeviceContainingMouse(const util::DisplayDevices& displayDevices)
{
    POINT mousePosition = { 0 };
    GetCursorPos(&mousePosition);

    return displayDevices.getDisplayDeviceContainingX(mousePosition.x);
}

//-----------------------------------------------------------------------

void MultiMonitorPlugin::handleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    //std::stringstream stream;
    //stream << "Message: " << std::hex << cwp->message << "\n";
    //OutputDebugString(stream.str().c_str());

    if(message == WM_SHOWWINDOW)
    {
        _isVisible = (wParam == TRUE);

        if(_isVisible)
        {
            handleShowWindow(getLastWindowHandle());
        }
    }
    else if(message == WM_WINDOWPOSCHANGING)
    {
        if(_isVisible)
        {
            // ignore when moving or resizing with the mouse
            bool leftMouseButtonPressed = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
            if(!leftMouseButtonPressed)
            {
                WINDOWPOS* windowPos = (WINDOWPOS*)lParam;
                handleWindowPosChanging(*windowPos);
            }
        }
    }
}

//-----------------------------------------------------------------------

#define VK_0    0x30
#define VK_1    0x31
#define VK_2    0x32
#define VK_3    0x33
#define VK_4    0x34
#define VK_5    0x35
#define VK_6    0x36
#define VK_7    0x37
#define VK_8    0x38
#define VK_9    0x39

bool MultiMonitorPlugin::handleKeyboardMessage(WPARAM wParam, LPARAM lParam)
{
    if(_isVisible && _options.enableHotkeys)
    {
        bool winKeyPressed = ((GetKeyState(VK_LWIN) & 0x8000) == 0x8000);

        if(winKeyPressed)
        {
            bool keyPressed = ((lParam & 0x80000000) == 0x80000000);

            switch(wParam)
            {
            case VK_RIGHT:
                {
                    if(keyPressed)
                    {
                        moveToNextLastDisplayDevice(NextMonitor);
                    }

                    return true;
                }

            case VK_LEFT:
                {
                    if(keyPressed)
                    {
                        moveToNextLastDisplayDevice(LastMonitor);
                    }

                    return true;
                }

            case VK_HOME:
                {
                    if(keyPressed)
                    {
                        centerFarrWindow();
                    }

                    return true;
                }

            case VK_0:
            case VK_1:
            case VK_2:
            case VK_3:
            case VK_4:
            case VK_5:
            case VK_6:
            case VK_7:
            case VK_8:
            case VK_9:
                {
                    if(keyPressed)
                    {
                        const long index = wParam - VK_0;
                        const long percent = (index == 0) ? 100L : (index * 10L);

                        _options.resizePercentValue = percent;
                        _optionsFile.setValue("ResizePercentValue", percent);

                        moveToNextLastDisplayDevice(SameMonitor, percent);
                    }

                    return true;
                }
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------

void MultiMonitorPlugin::moveToNextLastDisplayDevice(Where toWhere, long percent)
{
    const util::DisplayDevices displayDevices;

    const util::DisplayDevice* currentDisplayDevice = getDisplayDevice(displayDevices, _farrWindowHandle);

    util::DisplayDevices::const_iterator displayDeviceIterator = displayDevices.begin();
    const util::DisplayDevices::const_iterator displayDevicesEnd = displayDevices.end();

    const util::DisplayDevice* newDisplayDevice = 0;

    for( ; displayDeviceIterator != displayDevicesEnd; ++displayDeviceIterator)
    {
        const util::DisplayDevice& displayDevice = *displayDeviceIterator;
        if(&displayDevice == currentDisplayDevice)
        {
            if(toWhere == NextMonitor)
            {
                ++displayDeviceIterator;
                if(displayDeviceIterator != displayDevicesEnd)
                {
                    newDisplayDevice = &(*displayDeviceIterator);
                    break;
                }
                else
                {
                    displayDeviceIterator = displayDevices.begin();
                    newDisplayDevice = &(*displayDeviceIterator);
                    break;
                }
            }
            else if(toWhere == LastMonitor)
            {
                if(displayDeviceIterator == displayDevices.begin())
                {
                    if(displayDevices.count() > 1)
                    {
                        displayDeviceIterator = displayDevices.end() - 1;
                        newDisplayDevice = &(*displayDeviceIterator);
                        break;
                    }
                }
                else
                {
                    --displayDeviceIterator;
                    newDisplayDevice = &(*displayDeviceIterator);
                    break;
                }
            }
            else if(toWhere == SameMonitor)
            {
                newDisplayDevice = currentDisplayDevice;
            }
        }
    }

    moveWindowFromSourceToTargetDisplayDevice(currentDisplayDevice, newDisplayDevice, percent);
}

//-----------------------------------------------------------------------

void MultiMonitorPlugin::handleShowWindow(HWND lastActiveWindow)
{
    if(_options.enableMultiMon)
    {
        const util::DisplayDevices displayDevices;

        const HWND foregroundWindow = lastActiveWindow;
        const HWND desktopWindow = GetDesktopWindow();
        const HWND shellWindow = GetShellWindow();

        //char title[200];
        //GetWindowText(foregroundWindow, title, 200);

        //std::stringstream stream;
        //stream << "Foreground window: 0x" << std::hex << foregroundWindow << " '" << title << "'\n";

        //OutputDebugString(stream.str().c_str());

        const bool noWindowActive = ((foregroundWindow == 0) || (foregroundWindow == desktopWindow) || (foregroundWindow == shellWindow));

        const util::DisplayDevice* currentDisplayDevice = getDisplayDevice(displayDevices, _farrWindowHandle);
        
        const util::DisplayDevice* newDisplayDevice = (noWindowActive || (_options.showOnMonitorWithMouse)) ?
            getDisplayDeviceContainingMouse(displayDevices) :
            getDisplayDevice(displayDevices, foregroundWindow);

        moveWindowFromSourceToTargetDisplayDevice(currentDisplayDevice, newDisplayDevice);
    }
}

//-----------------------------------------------------------------------

void MultiMonitorPlugin::moveWindowFromSourceToTargetDisplayDevice(const util::DisplayDevice* currentDisplayDevice, const util::DisplayDevice* newDisplayDevice, long percent)
{
    if((currentDisplayDevice != 0) && (newDisplayDevice != 0))
    {
        CRect windowRect;
        GetWindowRect(_farrWindowHandle, &windowRect);

        const CRect oldDisplayRect = currentDisplayDevice->getPositionAndSize();
        const CRect newDisplayRect = newDisplayDevice->getPositionAndSize();

        const int xOffset = windowRect.left - oldDisplayRect.left;
        const int yOffset = windowRect.top - oldDisplayRect.top;

        const double xRatio = (double)newDisplayRect.Width() / (double)oldDisplayRect.Width();
        const double yRatio = (double)newDisplayRect.Height() / (double)oldDisplayRect.Height();

        int newLeft = 0;
        int newTop = 0;
        int newRight = 0;
        int newBottom = 0;

        const long resizePercentValue = (percent == 0) ? _options.resizePercentValue : percent;
        const int newWidth = ((_options.resizePercent || (percent != 0))? (int)((newDisplayRect.Width() * resizePercentValue) / 100) : (int)((windowRect.Width() * (_options.resizeRelative ? xRatio : 1))));
        const int newHeight = (int)((windowRect.Height() * (_options.resizeRelative ? yRatio : 1)));

        if(_options.center)
        {
            newLeft = newDisplayRect.left + (newDisplayRect.Width() - newWidth) / 2;
            newTop = newDisplayRect.top + (newDisplayRect.Height() - newHeight) / 2;
        }
        else
        {
            newLeft = newDisplayRect.left + (int)(xOffset * (_options.moveRelative ? xRatio : 1));
            newTop = newDisplayRect.top + (int)(yOffset * (_options.moveRelative ? yRatio : 1));
        }

        newRight = newLeft + newWidth;
        newBottom = newTop + newHeight;

        MoveWindow(_farrWindowHandle, newLeft, newTop, newRight - newLeft, newBottom - newTop, TRUE);
    }

    if(_options.center)
    {
        _lastWindowRect = centerFarrWindow();
    }
}

//-----------------------------------------------------------------------

void MultiMonitorPlugin::handleWindowPosChanging(const WINDOWPOS& /*windowPos*/)
{
    if(_options.center) 
    {
        if(_lastWindowRect.IsRectNull() == FALSE)
        {
            CRect windowRect;
            GetWindowRect(_farrWindowHandle, &windowRect);

            if(windowRect != _lastWindowRect)
            {
                centerFarrWindow();

                _lastWindowRect = CRect();
            }
        }
        else if(_options.alwaysCenter)
        {
            centerFarrWindow();

            _lastWindowRect = CRect();
        }
    } 
}

//-----------------------------------------------------------------------

CRect MultiMonitorPlugin::centerFarrWindow()
{
    CRect windowRect;
    GetWindowRect(_farrWindowHandle, &windowRect);

    const util::DisplayDevices displayDevices;
    const util::DisplayDevice* currentDisplayDevice = getDisplayDevice(displayDevices, _farrWindowHandle);
    const CRect currentDisplayRect = currentDisplayDevice->getPositionAndSize();

    const int newWidth = windowRect.Width();
    const int newHeight = windowRect.Height();

    const int newLeft = currentDisplayRect.left + (currentDisplayRect.Width() - newWidth) / 2;
    const int newTop = currentDisplayRect.top + (currentDisplayRect.Height() - newHeight) / 2;

    const int newRight = newLeft + newWidth;
    const int newBottom = newTop + newHeight;

    MoveWindow(_farrWindowHandle, newLeft, newTop, newRight - newLeft, newBottom - newTop, TRUE);

    return CRect(newLeft, newTop, newRight, newBottom);
}

//-----------------------------------------------------------------------

void MultiMonitorPlugin::showOptions()
{
    OptionsDialog dialog(_optionsFile);
    if(dialog.DoModal() == IDOK)
    {
        _options.update(_optionsFile);
    }
}

//-----------------------------------------------------------------------

HWND MultiMonitorPlugin::getLastWindowHandle()
{
    char lastWindowHandleAsString[16];
    callbackfp_get_strval(hostptr, "resolve:%LASTHWND%", lastWindowHandleAsString, 16);

    std::stringstream stream;
    stream << lastWindowHandleAsString;

    long lastWindowHandle;
    stream >> lastWindowHandle;

    return (HWND)lastWindowHandle;
}

//-----------------------------------------------------------------------
