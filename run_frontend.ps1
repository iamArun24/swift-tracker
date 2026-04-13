$ErrorActionPreference = 'Stop'

$scriptRoot = $null
if ($PSScriptRoot -and (Test-Path $PSScriptRoot)) {
	$scriptRoot = $PSScriptRoot
} elseif ($MyInvocation.MyCommand.Path) {
	$scriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
} else {
	$scriptRoot = (Get-Location).Path
}

$frontendPath = Join-Path $scriptRoot "frontend"
if (-not (Test-Path $frontendPath)) {
	throw "Frontend path not found: $frontendPath"
}

Set-Location $frontendPath

if (-not (Test-Path (Join-Path $frontendPath "package.json"))) {
	throw "package.json not found in frontend path: $frontendPath"
}

if (-not (Test-Path (Join-Path $frontendPath "node_modules"))) {
	Write-Host "Installing frontend dependencies..."
	npm install
}

$portsToClear = @(5173, 5174, 5175)
foreach ($port in $portsToClear) {
	$listeners = Get-NetTCPConnection -LocalPort $port -State Listen -ErrorAction SilentlyContinue
	foreach ($listener in $listeners) {
		$processId = $listener.OwningProcess
		$process = Get-CimInstance Win32_Process -Filter "ProcessId = $processId" -ErrorAction SilentlyContinue
		if (
			$process -and
			$process.Name -eq "node.exe" -and
			$process.CommandLine -match "vite|npm\s+run\s+dev"
		) {
			Write-Host "Stopping stale dev process PID=$processId on port $port"
			Stop-Process -Id $processId -Force -ErrorAction SilentlyContinue
		}
	}
}

Write-Host "Starting frontend at http://localhost:5173"
npm run dev -- --host --port 5173 --strictPort
