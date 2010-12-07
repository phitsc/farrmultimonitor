@ECHO off
SET SevenZipPath="C:\Program Files\7-Zip\7z.exe"
Set InnoSetupPath="C:\Program Files\Inno Setup 5\ISCC.exe"

%PHI_PROJECTS%\UpdateVersion\Deploy\UpdateVersion.exe -s %PHI_PROJECTS%\FarrMultiMonitor\Deploy\FarrMultiMonitor.dll -t %PHI_PROJECTS%\FarrMultiMonitor\Deploy\FarrMultiMonitor.dcupdate -t %PHI_PROJECTS%\FarrMultiMonitor\Package\versioninfo.xml -t %PHI_PROJECTS%\FarrMultiMonitor\Package\FarrMultiMonitor.iss
%SevenZipPath% a Output\FarrMultiMonitor.zip %PHI_PROJECTS%\FarrMultiMonitor\Deploy\*.* -r -xr!.svn -tzip -mx7
%InnoSetupPath% FarrMultiMonitor.iss