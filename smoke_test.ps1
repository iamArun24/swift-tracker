$ErrorActionPreference = 'Stop'

function Invoke-Api {
    param(
        [string]$Method,
        [string]$Uri,
        [hashtable]$Headers,
        $BodyObj
    )

    try {
        $params = @{ Method = $Method; Uri = $Uri; UseBasicParsing = $true }
        if ($Headers) { $params.Headers = $Headers }
        if ($null -ne $BodyObj) {
            $params.Body = ($BodyObj | ConvertTo-Json -Depth 10)
            $params.ContentType = 'application/json'
        }

        $res = Invoke-WebRequest @params
        $parsed = $null
        try { $parsed = $res.Content | ConvertFrom-Json -Depth 20 } catch { $parsed = $res.Content }

        [pscustomobject]@{ ok = $true; status = [int]$res.StatusCode; body = $parsed }
    } catch {
        $status = 0
        $raw = $_.Exception.Message
        if ($_.Exception.Response) {
            $status = [int]$_.Exception.Response.StatusCode
            $reader = New-Object System.IO.StreamReader($_.Exception.Response.GetResponseStream())
            $raw = $reader.ReadToEnd()
            $reader.Close()
        }

        $parsed = $raw
        try { $parsed = $raw | ConvertFrom-Json -Depth 20 } catch {}
        [pscustomobject]@{ ok = $false; status = $status; body = $parsed }
    }
}

$base = 'http://localhost:8080/api'
$ts = [DateTimeOffset]::UtcNow.ToUnixTimeSeconds()
$email = "smoke.$ts@swifttrack.test"
$pass = 'SmokeTest@123'
$results = @()

$r1 = Invoke-Api -Method GET -Uri "$base/health"
$results += [pscustomobject]@{ step='health'; pass=($r1.ok -and $r1.status -eq 200 -and $r1.body.status -eq 'healthy'); status=$r1.status; body=$r1.body }

$registerBody = @{
    full_name='Smoke User'; email=$email; phone='9876543210'; password=$pass
    city='Delhi'; state='Delhi'; pincode='110001'; address='Connaught Place'
}
$r2 = Invoke-Api -Method POST -Uri "$base/auth/register" -BodyObj $registerBody
$results += [pscustomobject]@{ step='register'; pass=($r2.ok -and ($r2.status -eq 200 -or $r2.status -eq 201)); status=$r2.status; body=$r2.body }

$loginBody = @{ email=$email; password=$pass }
$r3 = Invoke-Api -Method POST -Uri "$base/auth/login" -BodyObj $loginBody
$token = $null
if ($r3.body -and $r3.body.data -and $r3.body.data.token) { $token = [string]$r3.body.data.token }
$results += [pscustomobject]@{ step='login'; pass=($r3.ok -and $r3.status -eq 200 -and -not [string]::IsNullOrWhiteSpace($token)); status=$r3.status; body=$r3.body }

$authHeaders = @{}
if ($token) { $authHeaders['Authorization'] = "Bearer $token" }

$r4 = Invoke-Api -Method GET -Uri "$base/auth/me" -Headers $authHeaders
$results += [pscustomobject]@{ step='me'; pass=($r4.ok -and $r4.status -eq 200); status=$r4.status; body=$r4.body }

$r5 = Invoke-Api -Method GET -Uri "$base/shipments" -Headers $authHeaders
$results += [pscustomobject]@{ step='shipments_list'; pass=($r5.ok -and $r5.status -eq 200); status=$r5.status; body=$r5.body }

$priceBody = @{
    sender_name='Smoke User'; sender_phone='9876543210'; sender_email=$email
    sender_address='Connaught Place'; sender_city='Delhi'; sender_state='Delhi'; sender_pincode='110001'
    recipient_name='Receiver User'; recipient_phone='9876501234'; recipient_email='receiver@test.com'
    recipient_address='Andheri'; recipient_city='Mumbai'; recipient_state='Maharashtra'; recipient_pincode='400001'
    package_type='PARCEL'; weight_grams=1200; length_cm=20; width_cm=15; height_cm=10
    package_description='Docs'; declared_value=1500; service_type='STANDARD'
    is_fragile=$false; requires_signature=$false; is_cod=$false; cod_amount=0
    special_instructions=''; distance_km=1400
}
$r6 = Invoke-Api -Method POST -Uri "$base/shipments/price-estimate" -Headers $authHeaders -BodyObj $priceBody
$results += [pscustomobject]@{ step='price_estimate'; pass=($r6.ok -and $r6.status -eq 200); status=$r6.status; body=$r6.body }

$summary = [pscustomobject]@{
    email = $email
    token_present = (-not [string]::IsNullOrWhiteSpace($token))
    total = $results.Count
    passed = (@($results | Where-Object { $_.pass }).Count)
    failed = (@($results | Where-Object { -not $_.pass }).Count)
    results = $results
}

$summary | ConvertTo-Json -Depth 20