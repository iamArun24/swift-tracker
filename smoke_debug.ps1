$ErrorActionPreference = 'Stop'
$base='http://localhost:8080/api'
$ts=[DateTimeOffset]::UtcNow.ToUnixTimeSeconds()
$email="smoke.$ts@swifttrack.test"
$pass='SmokeTest@123'

$rb=@{full_name='Smoke User';email=$email;phone='9876543210';password=$pass;city='Delhi';state='Delhi';pincode='110001';address='Connaught Place'}|ConvertTo-Json
Invoke-WebRequest -UseBasicParsing -Method POST -Uri "$base/auth/register" -ContentType 'application/json' -Body $rb | Out-Null
$lb=@{email=$email;password=$pass}|ConvertTo-Json
$l=Invoke-WebRequest -UseBasicParsing -Method POST -Uri "$base/auth/login" -ContentType 'application/json' -Body $lb

Write-Output ("content_type=" + $l.Content.GetType().FullName)
Write-Output ("content_raw=" + $l.Content)
