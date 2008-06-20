#include "stdafx.h"
#include "MultiMonitorPlugin.h"

#include "DisplayDevices.h"
#include "OptionsDialog.h"
#include "OptionsFile.h"

#include <algorithm>

//-----------------------------------------------------------------------

Options::Options(const OptionsFile& optionsFile)
{
    update(optionsFile);
}

//-----------------------------------------------------------------------

void Options::update(const OptionsFile& optionsFile)
{
    enableMultiMon = optionsFile.getValue("EnableMultiMonitor", true);
    moveRelative = optionsFile.getValue("MoveRelative", true);
    resizeRelative = optionsFile.getValue("ResizeRelative", true);
    center = optionsFile.getValue("Center", false);
    enableThreshold = optionsFile.getValue("EnableThreshold", false);
    threshold = optionsFile.getValue("Threshold", 0L);
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

    const util::DisplayDevice* newDisplayDevice = displayDevices.getDisplayDeviceContainingX(x);

    return newDisplayDevice;
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
            handleShowWindow();
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

void MultiMonitorPlugin::handleShowWindow()
{
    if(_options.enableMultiMon)
    {
        const util::DisplayDevices displayDevices;

        HWND foregroundWindow = GetForegroundWindow();

        const util::DisplayDevice* currentDisplayDevice = getDisplayDevice(displayDevices, _farrWindowHandle);
        const util::DisplayDevice* newDisplayDevice = getDisplayDevice(displayDevices, foregroundWindow);

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
    }
}

//-----------------------------------------------------------------------

void MultiMonitorPlugin::handleWindowPosChanging(const WINDOWPOS& /*windowPos*/)
{
    if(_options.center) 
    {
        const util::DisplayDevices displayDevices;
        const util::DisplayDevice* currentDisplayDevice = getDisplayDevice(displayDevices, _farrWindowHandle);
        const CRect currentDisplayRect = currentDisplayDevice->getPositionAndSize();

        CRect windowRect;
        GetWindowRect(_farrWindowHandle, &windowRect);

        const int heightDelta = std::abs(windowRect.Height() - _oldWindowRect.Height());
        const bool doCenter = _options.enableThreshold ? (heightDelta >= _options.threshold) : true;

        if(doCenter)
        {
            const int newWidth = windowRect.Width();
            const int newHeight = windowRect.Height();

            const int newLeft = currentDisplayRect.left + (currentDisplayRect.Width() - newWidth) / 2;
            const int newTop = currentDisplayRect.top + (currentDisplayRect.Height() - newHeight) / 2;

            const int newRight = newLeft + newWidth;
            const int newBottom = newTop + newHeight;

            MoveWindow(_farrWindowHandle, newLeft, newTop, newRight - newLeft, newBottom - newTop, TRUE);

            _oldWindowRect = CRect(newLeft, newTop, newRight, newBottom);
        }
    }
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
