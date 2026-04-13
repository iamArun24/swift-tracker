$ErrorActionPreference = 'Stop'
$base = 'http://localhost:8080/api'
$ts = [DateTimeOffset]::UtcNow.ToUnixTimeSeconds()
$email = "smoke.$ts@swifttrack.test"
$pass = 'SmokeTest@123'

Write-Output "TEST_USER=$email"

$h = Invoke-WebRequest -UseBasicParsing -Uri "$base/health"
Write-Output "health_status=$($h.StatusCode)"

$registerBody = @{
    full_name = 'Smoke User'
    email = $email
    phone = '9876543210'
    password = $pass
    city = 'Delhi'
    state = 'Delhi'
    pincode = '110001'
    address = 'Connaught Place'
} | ConvertTo-Json

$r = Invoke-WebRequest -UseBasicParsing -Method POST -Uri "$base/auth/register" -ContentType 'application/json' -Body $registerBody
Write-Output "register_status=$($r.StatusCode)"
Write-Output "register_body=$($r.Content)"

$loginBody = @{ email = $email; password = $pass } | ConvertTo-Json
$l = Invoke-WebRequest -UseBasicParsing -Method POST -Uri "$base/auth/login" -ContentType 'application/json' -Body $loginBody
Write-Output "login_status=$($l.StatusCode)"
Write-Output "login_body=$($l.Content)"

$j = $l.Content | ConvertFrom-Json
$token = $null
if ($j.token) { $token = $j.token }
if ($j.data -and $j.data.token) { $token = $j.data.token }
if (-not [string]::IsNullOrWhiteSpace($token)) {
    Write-Output "token_present=true"
    $headers = @{ Authorization = "Bearer $token" }
    try {
        $me = Invoke-WebRequest -UseBasicParsing -Method GET -Uri "$base/auth/me" -Headers $headers
        Write-Output "me_status=$($me.StatusCode)"
        Write-Output "me_body=$($me.Content)"
    } catch {
        $code = [int]$_.Exception.Response.StatusCode
        Write-Output "me_status=$code"
    }
} else {
    Write-Output "token_present=false"
}
