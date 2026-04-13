$ErrorActionPreference = 'Stop'
$base = 'http://localhost:8080/api'
$ts = [DateTimeOffset]::UtcNow.ToUnixTimeSeconds()
$email = "smoke.$ts@swifttrack.test"
$pass = 'SmokeTest@123'

function BodyToString($resp) {
    if ($resp.Content -is [byte[]]) {
        return [System.Text.Encoding]::UTF8.GetString($resp.Content)
    }
    return [string]$resp.Content
}

Write-Output "TEST_USER=$email"

$h = Invoke-WebRequest -UseBasicParsing -Uri "$base/health"
Write-Output "health_status=$($h.StatusCode)"

$registerBody = @{ full_name='Smoke User'; email=$email; phone='9876543210'; password=$pass; city='Delhi'; state='Delhi'; pincode='110001'; address='Connaught Place' } | ConvertTo-Json
$r = Invoke-WebRequest -UseBasicParsing -Method POST -Uri "$base/auth/register" -ContentType 'application/json' -Body $registerBody
Write-Output "register_status=$($r.StatusCode)"

$loginBody = @{ email=$email; password=$pass } | ConvertTo-Json
$l = Invoke-WebRequest -UseBasicParsing -Method POST -Uri "$base/auth/login" -ContentType 'application/json' -Body $loginBody
$loginJson = (BodyToString $l) | ConvertFrom-Json
Write-Output "login_status=$($l.StatusCode)"

$token = $null
if ($loginJson.token) { $token = $loginJson.token }
if ($loginJson.data -and $loginJson.data.token) { $token = $loginJson.data.token }
Write-Output "token_present=$(-not [string]::IsNullOrWhiteSpace($token))"

if ([string]::IsNullOrWhiteSpace($token)) {
    Write-Output 'VERDICT=PARTIAL'
    exit 1
}

$headers = @{ Authorization = "Bearer $token" }
$me = Invoke-WebRequest -UseBasicParsing -Method GET -Uri "$base/auth/me" -Headers $headers
Write-Output "me_status=$($me.StatusCode)"

$ships = Invoke-WebRequest -UseBasicParsing -Method GET -Uri "$base/shipments" -Headers $headers
Write-Output "shipments_status=$($ships.StatusCode)"

$priceBody = @{
    sender_name='Smoke User'; sender_phone='9876543210'; sender_email=$email
    sender_address='Connaught Place'; sender_city='Delhi'; sender_state='Delhi'; sender_pincode='110001'
    recipient_name='Receiver User'; recipient_phone='9876501234'; recipient_email='receiver@test.com'
    recipient_address='Andheri'; recipient_city='Mumbai'; recipient_state='Maharashtra'; recipient_pincode='400001'
    package_type='PARCEL'; weight_grams=1200; length_cm=20; width_cm=15; height_cm=10
    package_description='Docs'; declared_value=1500; service_type='STANDARD'
    is_fragile=$false; requires_signature=$false; is_cod=$false; cod_amount=0
    special_instructions=''; distance_km=1400
} | ConvertTo-Json
$pe = Invoke-WebRequest -UseBasicParsing -Method POST -Uri "$base/shipments/price-estimate" -Headers $headers -ContentType 'application/json' -Body $priceBody
Write-Output "price_estimate_status=$($pe.StatusCode)"

Write-Output 'VERDICT=WORKING'
