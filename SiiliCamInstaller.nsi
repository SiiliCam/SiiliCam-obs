!include "MUI2.nsh"

!define MUI_ABORTWARNING
!define MUI_ICON "${ICO_PATH_PREFIX}siilicam.ico"  ; Replace with the path to your application's icon

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

OutFile "${OUTPUT_DIR}\SiiliCam-obs-installer-${PROJECT_VERSION}.exe"

Function .onInit
    ClearErrors
    SetRegView 64
    ReadRegStr $INSTDIR HKLM "SOFTWARE\OBS Studio" ""
    IfErrors 0 +3
    StrCpy $INSTDIR "$INSTDIR"
    Goto done

    SetRegView 32
    ReadRegStr $INSTDIR HKLM "SOFTWARE\OBS Studio" ""
    StrCpy $INSTDIR "$INSTDIR"

    done:
FunctionEnd

Section "Uninstall"
    ; Uninstallation commands
    Delete "$INSTDIR\obs-plugins\64bit\Processing.NDI.Lib.x64.dll"
    Delete "$INSTDIR\obs-plugins\64bit\SiiliCam-obs.dll"
    Delete "$INSTDIR\obs-plugins\64bit\SiiliCam-obs.dll.manifest"
    Delete "$INSTDIR\obs-plugins\64bit\SiiliCam-obs.exp"
    Delete "$INSTDIR\obs-plugins\64bit\SiiliCam-obs.lib"

    Delete "$INSTDIR\Uninstall SiiliCam-obs.exe"

    ; Remove the uninstall information from the registry
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SiiliCam OBS Plugin"
SectionEnd

Section "Install SiiliCam OBS Plugin"

    valid_path:
    SetOutPath "$INSTDIR\obs-plugins\64bit"
    ; Copy each file to the installation directory
    File "${FILE_PATH_PREFIX}Processing.NDI.Lib.x64.dll"
    File "${FILE_PATH_PREFIX}SiiliCam-obs.dll"
    File "${FILE_PATH_PREFIX}SiiliCam-obs.dll.manifest"
    File "${FILE_PATH_PREFIX}SiiliCam-obs.exp"
    File "${FILE_PATH_PREFIX}SiiliCam-obs.lib"
    IfErrors extraction_failed

    MessageBox MB_OK "Extraction succeeded"
    Goto done

    extraction_failed:
    MessageBox MB_ICONSTOP "Extraction failed."
    Abort

    not_valid:
    MessageBox MB_ICONSTOP "OBS Studio installation path is not valid."
    Abort

    not_installed:
    MessageBox MB_ICONSTOP "OBS Studio is not installed. Please install OBS Studio first."
    Abort

    done:
    ; Rest of your script
    end:

    WriteUninstaller "$INSTDIR\Uninstall SiiliCam-obs.exe"

    ; Register the uninstaller in Windows
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SiiliCam OBS Plugin" "DisplayName" "SiiliCam OBS Plugin"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SiiliCam OBS Plugin" "UninstallString" "$\"$INSTDIR\Uninstall SiiliCam-obs.exe$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SiiliCam OBS Plugin" "InstallLocation" "$INSTDIR"  ; Add this line
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SiiliCam OBS Plugin" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SiiliCam OBS Plugin" "NoRepair" 1

SectionEnd
