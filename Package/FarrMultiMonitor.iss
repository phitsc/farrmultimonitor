[Setup]
AppName=FarrMultiMonitor
AppVersion=1.1.0
DefaultDirName={reg:HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Find and Run Robot_is1,InstallLocation|{pf}\FindAndRunRobot}\Plugins\FarrMultiMonitor
OutputBaseFilename=FarrMultiMonitorSetup
Uninstallable=no
Compression=lzma2
SolidCompression=yes
DirExistsWarning=no

[Files]
Source: "..\Deploy\CloseApp2.dll"; DestDir: "{tmp}"; Flags: onlyifdoesntexist
Source: "..\Deploy\FarrMultiMonitor.dll"; DestDir: "{app}";
Source: "..\Deploy\MultiMonitor.ico"; DestDir: "{app}"
Source: "..\Deploy\FarrMultiMonitor.dcupdate"; DestDir: "{app}"
Source: "..\Deploy\readme.txt"; DestDir: "{app}"; Flags: isreadme

[Code]
// importing DLL functions from closeapp2.dll
procedure SendMsgUnloadPlugins_OnInstall(farrApplicationName: String);
external 'SendMsgUnloadPlugins@files:CloseApp2.dll cdecl setuponly';

procedure SendMsgLoadPlugins_OnInstall(farrApplicationName: String);
external 'SendMsgLoadPlugins@files:CloseApp2.dll cdecl setuponly';

//
procedure CurStepChanged(CurStep: TSetupStep);
begin
    case CurStep of
        ssInstall: SendMsgUnloadPlugins_OnInstall('FindAndRunRobot.exe');
        ssPostInstall: SendMsgLoadPlugins_OnInstall('FindAndRunRobot.exe');
    end;
end;
