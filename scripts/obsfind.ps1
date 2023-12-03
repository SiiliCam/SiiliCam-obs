# PowerShell Script to Get OBS Studio Install Path

# Path to the OBS Studio registry key
$regPath = 'HKLM:\SOFTWARE\OBS Studio'

# Check if the registry path exists
if (Test-Path $regPath) {
    # Get the install path from the registry
    $obsInstallPath = (Get-ItemProperty -Path $regPath).installPath
    Write-Host "OBS Studio is installed at: $obsInstallPath"
} else {
    Write-Host "OBS Studio is not installed or the registry key does not exist."
}
