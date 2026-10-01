# Expose HiTEKS CCI website (+ chatbot via /api/rag proxy) on ngrok.
# Prerequisites: npm run dev (port 5177), Telematry RAG on :8001, ngrok authed.

$ErrorActionPreference = "Stop"
$port = 5177

try {
  Invoke-WebRequest "http://127.0.0.1:$port/" -UseBasicParsing -TimeoutSec 3 | Out-Null
} catch {
  Write-Error "Vite not reachable on :$port. In website/: npm run dev"
}

try {
  $h = Invoke-RestMethod "http://127.0.0.1:8001/health" -TimeoutSec 3
  if ($h.status -ne "ok") { Write-Warning "RAG health not ok" }
} catch {
  Write-Warning "RAG not reachable on :8001 - chatbot will fail until Telematry is up."
}

Get-Process ngrok -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue
Start-Sleep 1

Write-Host "Starting ngrok http $port ..." -ForegroundColor Cyan
Start-Process -FilePath "ngrok" -ArgumentList @("http", "$port") -WindowStyle Minimized
Start-Sleep 3

$tunnels = Invoke-RestMethod "http://127.0.0.1:4040/api/tunnels"
$https = $tunnels.tunnels | Where-Object { $_.public_url -like "https://*" } | Select-Object -First 1
if (-not $https) { Write-Error "No https tunnel. Open http://127.0.0.1:4040" }

Write-Host ""
Write-Host "Public site:  $($https.public_url)" -ForegroundColor Green
Write-Host "Chatbot lab:  $($https.public_url)/lab" -ForegroundColor Green
Write-Host "ngrok UI:     http://127.0.0.1:4040" -ForegroundColor DarkGray
Write-Host ""
Write-Host "On phone: open the URL, tap Visit Site if ngrok shows a warning page." -ForegroundColor Yellow
