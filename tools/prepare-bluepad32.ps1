$ErrorActionPreference = 'Stop'
$taskGitUsr = 'C:\Program Files\Git\usr\bin'
if (Test-Path -LiteralPath $taskGitUsr) { $env:PATH = $taskGitUsr + ';' + $env:PATH }
$taskRoot = Split-Path -Parent $PSScriptRoot
$taskVendor = Join-Path $taskRoot 'Code\firmware\vendor\bluepad32'
git -C $taskRoot submodule update --init --recursive
if ($LASTEXITCODE -ne 0) { throw 'Submodule update failed' }
$taskBtstack = Join-Path $taskVendor 'external\btstack'
foreach ($taskPatch in Get-ChildItem (Join-Path $taskVendor 'external\patches') -Filter '*.patch') {
    $taskTemp = [IO.Path]::GetTempFileName()
    try {
        [IO.File]::WriteAllText($taskTemp, [IO.File]::ReadAllText($taskPatch.FullName).Replace("`r`n", "`n"))
        git -C $taskBtstack apply --reverse --check $taskTemp 2>$null
        if ($LASTEXITCODE -eq 0) { Write-Output "Already applied: $($taskPatch.Name)"; continue }
        git -C $taskBtstack apply --check $taskTemp
        if ($LASTEXITCODE -ne 0) { throw "Patch check failed: $($taskPatch.Name)" }
        git -C $taskBtstack apply $taskTemp
        if ($LASTEXITCODE -ne 0) { throw "Patch failed: $($taskPatch.Name)" }
    } finally { Remove-Item -LiteralPath $taskTemp }
}
$taskComponent = Join-Path $taskVendor 'src\components\btstack'
if (-not (Test-Path -LiteralPath $taskComponent)) {
    $taskOldIdf = $env:IDF_PATH
    try {
        $env:IDF_PATH = Join-Path $taskVendor 'src'
        python (Join-Path $taskVendor 'external\btstack\port\esp32\integrate_btstack.py')
        if ($LASTEXITCODE -ne 0) { throw 'BTstack component generation failed' }
    } finally { $env:IDF_PATH = $taskOldIdf }
}
