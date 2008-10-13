#include "stdafx.h"
#include "MultiMonitorPlugin.h"

#include "DisplayDevices.h"
#include "OptionsDialog.h"
#include "OptionsFile.h"

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

void MultiMonitorPlugin::handleMessage(UINT message, WPARAM wParam, LPARAM lParam, HWND lastActiveWindow)
{
    //std::stringstream stream;
    //stream << "Message: " << std::hex << cwp->message << "\n";
    //OutputDebugString(stream.str().c_str());

    if(message == WM_SHOWWINDOW)
    {
        _isVisible = (wParam == TRUE);

        if(_isVisible)
        {
            handleShowWindow(lastActiveWindow);
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

bool MultiMonitorPlugin::handleKeyboardMessage(WPARAM wParam, LPARAM lParam)
{
    if(_isVisible && _options.enableHotkeys)
    {
        bool winKeyPressed = ((GetKeyState(VK_LWIN) & 0x8000) == 0x8000);

        if(winKeyPressed)
        {
            bool keyPressed = ((lParam & 0x80000000) == 0x80000000);

            if(wParam == VK_RIGHT)
            {
                if(keyPressed)
                {
                    moveToNextLastDisplayDevice(true);
                }

                return true;
            }
            else if(wParam == VK_LEFT)
            {
                if(keyPressed)
                {
                    moveToNextLastDisplayDevice(false);
                }

                return true;
            }
            else if(wParam == VK_HOME)
            {
                if(keyPressed)
                {
                    centerFarrWindow();
                }

                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------

void MultiMonitorPlugin::moveToNextLastDisplayDevice(bool toNext)
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
            if(toNext)
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
            else
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
        }
    }

    moveWindowFromSourceToTargetDisplayDevice(currentDisplayDevice, newDisplayDevice);
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

void MultiMonitorPlugin::moveWindowFromSourceToTargetDisplayDevice(const util::DisplayDevice* currentDisplayDevice, const util::DisplayDevice* newDisplayDevice)
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

        const int newWidth = (int)((windowRect.Width() * (_options.resizeRelative ? xRatio : 1)));
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
