===============================================================================

FARR MultiMonitor plugin - 0.2.0 - 3. July 2008

===============================================================================

Description
================
The FARR MultiMonitor plugin extends FARR to work better with multiple monitors. FARR's main window currently always shows up on the monitor you moved it to. With the FARR MultiMonitor plugin enabled, FARR's main window will show up on the monitor that contains the currently active window (usually the window of the application you're working in, and also where you're currently looking at) or on the monitor where the mouse currently is.

Additionally, the FARR MultiMonitor plugin can center FARR's main window on the screen.


Installation
================
Copy all files into the FarrMultiMonitor plugin directory (usually C:\Program Files\FindAndRunRobot\Plugins\FarrMultiMonitor) and restart FARR.


Options
================
- Display FARR on active monitor: enables / disables multi monitor support
- Move relative: moves FARR relative to the screen resolution, e.g. if FARR is 1/3 from the left on monitor 1, it will be 1/3 from the left on monitor 2 as well, even if the screen resolutions of the two monitors are not the same
- Resize relative: resizes FARR relative to the screen resolution, e.g. if FARR takes 1/3 of the screen on monitor 1, it will take 1/3 on the screen of monitor 2, even if the screen resolutions of the two monitors are not the same
- Display on monitor where mouse is: displays FARR's main window on the monitor where the mouse is no matter which application is currently active

- Center FARR when it appears: will center FARR horizontally, as well as vertically, when it appears
- and whenver it changes size: will center FARR whenever its size changes

- Enable hotkeys: enable moving to next/last monitor and centering with hotkeys
    Win - Left : move FARR window to monitor on the left
	Win - Right : move FARR window to monitor on the right
	Win - Home : center FARR window


Credits
================
The icon is from FamFamFam's silk icon set: http://www.famfamfam.com/lab/icons/silk/


Disclaimer
================
This plugin is provided 'as is'. No warranty of any kind is expressed or implied. Use at your own risk.


Version history
================
0.2.0
- added support for moving to next/last monitor and centering with hotkeys

0.1.0
- added option to display FARR on monitor where mouse is, instead of on monitor with active window
- added option to center FARR window only when it appears
- fixed DLL unload problem
- added .dcupdate file

0.0.0.1
- initial release