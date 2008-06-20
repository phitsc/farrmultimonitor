#pragma once

///////////////////////////////////////////////////////////////////////////////

#pragma warning(disable : 4702)
#include <vector>
#pragma warning(default : 4702)

#include <sstream>
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////

namespace util
{

///////////////////////////////////////////////////////////////////////////////

class DisplayDevice
{
    public:
        DisplayDevice(unsigned long number, const RECT& positionAndSize, bool isPrimary)
            :_number(number), _positionAndSize(positionAndSize), _isPrimary(isPrimary) {}

        unsigned long getNumber() const { return _number; }
        bool isPrimary() const { return _isPrimary; }
        const RECT& getPositionAndSize() const { return _positionAndSize; }

        //bool operator<(const DisplayDevice& dd) const { return _number < dd._number; }
        bool operator<(const DisplayDevice& dd) const { return _positionAndSize.left < dd._positionAndSize.left; }

    private:
        unsigned long _number;
        bool _isPrimary;
        RECT _positionAndSize;
};

///////////////////////////////////////////////////////////////////////////////

class DisplayDevices
{
        typedef std::vector<DisplayDevice> DisplayDeviceCollection;

    public:
        DisplayDevices()
        {
            DWORD deviceIndex = 0;

            DISPLAY_DEVICE deviceInfo = {0};
            deviceInfo.cb = sizeof(DISPLAY_DEVICE);

            while(TRUE == EnumDisplayDevices(NULL, deviceIndex, &deviceInfo, 0))
            {
                if(DISPLAY_DEVICE_ATTACHED_TO_DESKTOP == (deviceInfo.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP))
                {
                    std::string deviceName(deviceInfo.DeviceName);
                    const std::string DISPLAY("DISPLAY");
                    std::string::size_type pos = deviceName.find(DISPLAY);

                    std::string deviceNumberString = deviceName.substr(pos+DISPLAY.length());
                    unsigned long deviceNumber;
                    std::stringstream stream(deviceNumberString);
                    stream >> deviceNumber;

                    bool isPrimary = (DISPLAY_DEVICE_PRIMARY_DEVICE == (deviceInfo.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE));

                    DEVMODE deviceMode = {0};
                    deviceMode.dmSize = sizeof(DEVMODE);
                    EnumDisplaySettings(deviceInfo.DeviceName, ENUM_CURRENT_SETTINGS, &deviceMode);

                    RECT positionAndSize = { deviceMode.dmPosition.x,
                        deviceMode.dmPosition.y,
                        deviceMode.dmPosition.x + deviceMode.dmPelsWidth,
                        deviceMode.dmPosition.y + deviceMode.dmPelsHeight };

                    _displayDevices.push_back(DisplayDevice(deviceNumber, positionAndSize, isPrimary));
                }

                deviceIndex++;
            }

			std::sort(_displayDevices.begin(), _displayDevices.end());
        }

        typedef DisplayDeviceCollection::const_iterator const_iterator;
        typedef DisplayDeviceCollection::size_type size_type;

        size_type      count() const { return _displayDevices.size(); }
        const_iterator begin() const { return _displayDevices.begin(); }
        const_iterator end() const { return _displayDevices.end(); }

        const DisplayDevice* getDisplayDeviceContainingX(int x) const
        {
            const_iterator it = begin();
            const_iterator endIt = end();

            for( ; it != endIt; ++it)
            {
                const DisplayDevice& display = *it;

                const RECT& positionAndSize = display.getPositionAndSize();
                if((x >= positionAndSize.left) && (x <= positionAndSize.right))
                {
                    return &display;
                }
            }

            return 0;
        }

    private:
        DisplayDeviceCollection _displayDevices;
};

///////////////////////////////////////////////////////////////////////////////

} // namespace

///////////////////////////////////////////////////////////////////////////////
