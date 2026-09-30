# Run in administrator PowerShell. No automatic reboot.
$ErrorActionPreference = 'Stop'
$taskIdentity = [Security.Principal.WindowsIdentity]::GetCurrent()
$taskPrincipal = [Security.Principal.WindowsPrincipal]::new($taskIdentity)
if (-not $taskPrincipal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    throw 'Open PowerShell as administrator and run this script again.'
}
$taskMsi = Join-Path $PSScriptRoot 'installers\wsl.x64.msi'
if (-not (Test-Path -LiteralPath $taskMsi)) { throw 'Microsoft WSL installer missing.' }
$taskSignature = Get-AuthenticodeSignature $taskMsi
if ($taskSignature.Status -ne 'Valid' -or $taskSignature.SignerCertificate.Subject -notmatch 'Microsoft Corporation') {
    throw 'Microsoft installer signature verification failed.'
}
$taskProcess = Start-Process msiexec.exe -ArgumentList @('/i', ('"' + $taskMsi + '"'), '/qn', '/norestart') -WindowStyle Hidden -Wait -PassThru
if ($taskProcess.ExitCode -notin @(0, 3010)) { throw "WSL installer failed: $($taskProcess.ExitCode)" }
$taskWsl = Enable-WindowsOptionalFeature -Online -FeatureName Microsoft-Windows-Subsystem-Linux -All -NoRestart
$taskVm = Enable-WindowsOptionalFeature -Online -FeatureName VirtualMachinePlatform -All -NoRestart
if ($taskWsl.RestartNeeded -or $taskVm.RestartNeeded -or $taskProcess.ExitCode -eq 3010) {
    Write-Output 'Restart Windows when ready, then run: wsl --install -d Ubuntu-24.04 --no-launch'
} else {
    wsl --install -d Ubuntu-24.04 --no-launch
    if ($LASTEXITCODE -ne 0) { throw 'Ubuntu installation failed; inspect WSL output.' }
}
Write-Output 'Open Ubuntu afterward and create your Linux username/password. Then follow documents/setup.md.'
