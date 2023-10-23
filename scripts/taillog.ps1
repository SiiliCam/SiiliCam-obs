# Define the path to the log file
$logFilePath = "C:/Users/Simo/AppData/Roaming/log.log"

# Check if the log file exists
if (Test-Path $logFilePath) {
    # Convert the Windows path to a WSL path
    $wslPath = "/mnt/c/" + $logFilePath.Substring(2).Replace(":", "").Replace("\", "/")
    
    # Use WSL's tail command to tail the log file
    wsl tail -f $wslPath -n 100 ---disable-inotify
} else {
    Write-Output "Log file not found at $logFilePath"
}
