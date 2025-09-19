# Run all OBJ tests against the built hw1.exe
$exe = "..\build\hw1.exe"
if (-not (Test-Path $exe)) {
    Write-Error "Executable not found at $exe. Build the project first (from build folder: make)."
    exit 2
}

$tests = Get-ChildItem -Path . -Filter "*.obj" | Sort-Object Name
foreach ($t in $tests) {
    Write-Host "=== Running $($t.Name) ==="
    $startInfo = New-Object System.Diagnostics.ProcessStartInfo
    $startInfo.FileName = (Resolve-Path $exe).Path
    $startInfo.Arguments = "`"$($t.FullName)`""
    $startInfo.RedirectStandardOutput = $true
    $startInfo.RedirectStandardError = $true
    $startInfo.UseShellExecute = $false
    $proc = [System.Diagnostics.Process]::Start($startInfo)
    $out = $proc.StandardOutput.ReadToEnd()
    $err = $proc.StandardError.ReadToEnd()
    $proc.WaitForExit()

    if ($out) { Write-Host $out }
    if ($err) { Write-Host "[stderr]"; Write-Host $err }
    Write-Host "Exit code: $($proc.ExitCode)" -ForegroundColor Yellow
    Write-Host ""
}
Write-Host "All tests run." -ForegroundColor Green
