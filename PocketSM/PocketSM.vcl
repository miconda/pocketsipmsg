<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: PocketSM - Win32 (WCE ARM) Release--------------------
</h3>
<h3>Command Lines</h3>
Creating command line "rc.exe /l 0x409 /fo"ARMRel/PocketSM.res" /d UNDER_CE=300 /d _WIN32_WCE=300 /d "UNICODE" /d "_UNICODE" /d "NDEBUG" /d "WIN32_PLATFORM_PSPC" /d "ARM" /d "_ARM_" /d "_AFXDLL" /r "D:\Fokus\PocketSM\PocketSM.rc"" 
Creating temporary file "C:\DOCUME~1\dcm\LOCALS~1\Temp\RSP44F.tmp" with contents
[
/nologo /W3 /D "ARM" /D "_ARM_" /D UNDER_CE=300 /D _WIN32_WCE=300 /D "WIN32_PLATFORM_PSPC" /D "UNICODE" /D "_UNICODE" /D "NDEBUG" /D "_AFXDLL" /FR"ARMRel/" /Fp"ARMRel/PocketSM.pch" /Yu"stdafx.h" /Fo"ARMRel/" /Oxs /MC /c 
"D:\Fokus\PocketSM\SetupDlg.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\dcm\LOCALS~1\Temp\RSP44F.tmp" 
Creating temporary file "C:\DOCUME~1\dcm\LOCALS~1\Temp\RSP450.tmp" with contents
[
/nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"wWinMainCRTStartup" /incremental:no /pdb:"ARMRel/PocketSM.pdb" /out:"ARMRel/PocketSM.exe" /subsystem:windowsce,3.00 /align:"4096" /MACHINE:ARM 
.\ARMRel\md5.obj
.\ARMRel\PocketSM.obj
.\ARMRel\PocketSMDlg.obj
.\ARMRel\rfc2617.obj
.\ARMRel\SetupDlg.obj
.\ARMRel\SIPMsg.obj
.\ARMRel\StdAfx.obj
.\ARMRel\PocketSM.res
]
Creating command line "link.exe @C:\DOCUME~1\dcm\LOCALS~1\Temp\RSP450.tmp"
<h3>Output Window</h3>
Compiling resources...
Compiling...
SetupDlg.cpp
Linking...



<h3>Results</h3>
PocketSM.exe - 0 error(s), 0 warning(s)
</pre>
</body>
</html>
