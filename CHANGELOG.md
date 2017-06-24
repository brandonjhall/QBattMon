# Change Log
## 1.0.0 (2017-06-22)

**Classes Created**
- [Battery] (https://gitlab.com/brandonjhall/QBattMon/blob/master/battery.h)
- [MainWidget] (https://gitlab.com/brandonjhall/QBattMon/blob/master/mainwidget.h)
- [SystemTrayIcon] (https://gitlab.com/brandonjhall/QBattMon/blob/master/systemtrayicon.h)

**Headers Created**
- [GlobalHeader] (https://gitlab.com/brandonjhall/QBattMon/blob/master/globalheader.h)

**Summary**
- Designed [MainWidget] (https://gitlab.com/brandonjhall/QBattMon/blob/master/mainwidget.ui) in Qt Designer
- Created a shared QStandardItem model for all classes to read from
- Added functionality to calculate battery time remaining
- Added functionality to have SystemTrayIcon change the current icon based on battery level
- Created functions in Battery to open files in /sys/class/power_supply to read current battery capacity and power levels

## 1.1.0 (2017-06-23)

**Implemented enhancements:**
- Added functions to main to read and write settings for the application
- Added functionality to MainWidget to update the settings and Battery for a selected battery if multiple batteries are available

## [1.1.1] (https://gitlab.com/brandonjhall/QBattMon/tree/v1.1.1) (2017-06-23)

**Implemented bug fixes:**
- Fixed bug where MainWidget contained no data if the -t or --tray option was used

## [1.2.0] (https://gitlab.com/brandonjhall/QBattMon/tree/v1.2.0) (2017-06024)

**Implemented enhancements:**
- Made the system tray icon more accurately represent battery capacity
- Added warning messages when battery capacit gets below 20%, 10%, and 7%.
