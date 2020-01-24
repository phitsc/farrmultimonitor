# FARR MultiMonitor plugin

https://www.donationcoder.com/forum/index.php?topic=13760.0

# Description

The FARR MultiMonitor plugin extends FARR to work better with multiple monitors. FARR's main window currently always shows up on the monitor you moved it to. With the FARR MultiMonitor plugin enabled, FARR's main window will show up on the monitor that contains the currently active window (usually the window of the application you're working in, and also where you're currently looking at) or on the monitor where the mouse currently is.

Additionally, the FARR MultiMonitor plugin can center FARR's main window on the screen.

# Installation

Copy all files into the FarrMultiMonitor plugin directory (usually `C:\Program Files\FindAndRunRobot\Plugins\FarrMultiMonitor`) and restart FARR.

# Options

- Display FARR on active monitor: enables / disables multi monitor support
- Move relative: moves FARR relative to the screen resolution, e.g. if FARR is 1/3 from the left on monitor 1, it will be 1/3 from the left on monitor 2 as well, even if the screen resolutions of the two monitors are not the same
- Resize relative: resizes FARR relative to the screen resolution, e.g. if FARR takes 1/3 of the screen on monitor 1, it will take 1/3 on the screen of monitor 2, even if the screen resolutions of the two monitors are not the same
- Display on monitor where mouse is: displays FARR's main window on the monitor where the mouse is no matter which application is currently active

- Center FARR when it appears: will center FARR horizontally, as well as vertically, when it appears
- and whenver it changes size: will center FARR whenever its size changes

- Enable hotkeys: enable moving FARR window to next/last monitor and centering with hotkeys
    * Win + Left                  : move FARR window to monitor on the left
    * Win + Right                 : move FARR window to monitor on the right
    * Win + Home                  : center FARR window
    * Win + Ctrl + 1 .. 0         : resize FARR window to 10% .. 100% of current screen's width
    * Win + Ctrl + Shift + 1 .. 0 : resize FARR window to 10% .. 100% of current screen's height

# Credits

The icon is a modified version of one taken from here: http://www.visualpharm.com/
