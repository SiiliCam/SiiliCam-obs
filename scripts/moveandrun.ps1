# Check if OBS is running and kill it
$obsProcess = Get-Process -Name "obs64" -ErrorAction SilentlyContinue
if ($obsProcess) {
    $obsProcess | Stop-Process -Force
}

Start-Sleep -Seconds 2

# Remove the DLL if it exists in the destination
$destDllPath = "C:\Users\Simo\source\repos\obs-studio\build_x64\rundir\Release\obs-plugins\64bit\SiiliCam-obs.dll"
if (Test-Path $destDllPath) {
    Remove-Item -Path $destDllPath -Force
}

# Move the DLL to the plugin folder
Copy-Item -Path "out\build\x64-release\SiiliCam-obs\SiiliCam-obs.dll" -Destination "C:\Users\Simo\source\repos\obs-studio\build_x64\rundir\Release\obs-plugins\64bit" -Force

# Copy the NDI DLL to the plugin folder
Copy-Item -Path "out\build\x64-release\SiiliCam-obs\Processing.NDI.Lib.x64.dll" -Destination "C:\Users\Simo\source\repos\obs-studio\build_x64\rundir\Release\obs-plugins\64bit" -Force

# Check if PDB exists before moving
$pdbPath = "out\build\x64-Release\SiiliCam-obs\SiiliCam-obs.pdb"
if (Test-Path $pdbPath) {
    Copy-Item -Path $pdbPath -Destination "C:\Users\Simo\source\repos\obs-studio\build_x64\rundir\Release\obs-plugins\64bit" -Force
}

# Start OBS again
Start-Process -FilePath "C:\Users\Simo\source\repos\obs-studio\build_x64\rundir\Release\bin\64bit\obs64.exe" -WorkingDirectory "C:\Users\Simo\source\repos\obs-studio\build_x64\rundir\Release\bin\64bit" -WindowStyle Minimized


# Wait for a few seconds to ensure OBS has started and is writing to the log
Start-Sleep -Seconds 2

# Get the latest log file
$latestLogFile = Get-ChildItem "C:\Users\Simo\AppData\Roaming\obs-studio\logs" | Sort-Object LastWriteTime -Descending | Select-Object -First 1

# Tail the log file using WSL's tail command
$wslPath = "/mnt/c/" + $latestLogFile.FullName.Substring(2).Replace(":", "").Replace("\", "/")
wsl tail -f $wslPath -n 200 ---disable-inotify
