$ErrorActionPreference = 'Stop'
$toolBin = "C:\Users\ak232\AppData\Local\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260407-ucrt-x86_64\bin"
$backendExe = Join-Path $PSScriptRoot "build\swifttrack.exe"

if (-not (Test-Path $toolBin)) {
	throw "LLVM MinGW runtime path not found: $toolBin"
}

if (-not (Test-Path $backendExe)) {
	throw "Backend executable not found: $backendExe"
}

$env:Path = "$toolBin;$env:Path"
Set-Location $PSScriptRoot
& $backendExe

$exitCode = $LASTEXITCODE
if ($exitCode -eq -1073741515) {
	throw "Backend failed to start due missing runtime DLLs. Use run_backend.ps1 (not direct swifttrack.exe) and confirm LLVM MinGW runtime path exists."
}

if ($exitCode -ne 0) {
	throw "Backend exited with code $exitCode"
}
