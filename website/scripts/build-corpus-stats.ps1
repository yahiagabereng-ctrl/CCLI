# Rebuild website/public/corpus-stats.json from live Telematry RAG.
# Requires RAG at http://localhost:8001
param(
  [string] $RagBase = "http://localhost:8001",
  [string] $TenantId = "telematry",
  [string] $BotId = "default"
)

$ErrorActionPreference = "Stop"
$websiteRoot = Split-Path -Parent $PSScriptRoot
# Re-run the same logic by invoking inline — keep script self-contained for re-use

Write-Host "Building corpus stats from $RagBase ..."
$summary = Invoke-RestMethod "$RagBase/v1/classification/summary?tenant_id=$TenantId&bot_id=$BotId" -TimeoutSec 120
$ccliPattern = '^(ccli-|cei-0-16-|eu-2018-2001-red|eu-2024-2847-cra$)'
$ccliSources = @($summary.sources | Where-Object { $_.source_id -match $ccliPattern })

$tel = "C:\Yahia\projects\Telematry System\documents"
$pdfPaths = @()
$pdfPaths += @(Get-ChildItem "$tel\projects\ccli" -Recurse -Filter *.pdf -File -EA SilentlyContinue)
$pdfPaths += @(Get-ChildItem "$tel\regulations\cei-0-16" -Recurse -Filter *.pdf -File -EA SilentlyContinue)
$pdfPaths += @(Get-ChildItem "$tel\regulations\eu" -Recurse -Filter *.pdf -File -EA SilentlyContinue)
$mdPaths = @(Get-ChildItem "$tel\projects\ccli" -Recurse -Filter *.md -File -EA SilentlyContinue)

function Grade([double]$mean) {
  if ($mean -ge 0.90) { return "A" }
  if ($mean -ge 0.80) { return "B" }
  if ($mean -ge 0.70) { return "C" }
  if ($mean -ge 0.50) { return "D" }
  return "F"
}

$perSource = @()
$allScores = New-Object System.Collections.Generic.List[double]
$bands = [ordered]@{ "A" = 0; "B" = 0; "C" = 0; "D" = 0; "F" = 0 }
$ocrHigh = 0; $ocrMed = 0; $ocrLow = 0; $ocrUnk = 0
$needsReview = 0
$scoredChunks = 0

foreach ($src in ($ccliSources | Sort-Object { -$_.total_chunks })) {
  $sid = $src.source_id
  Write-Host "  $sid ($($src.total_chunks))"
  $offset = 0
  $limit = 500
  $scores = New-Object System.Collections.Generic.List[double]
  $filenames = @{}
  do {
    $url = "$RagBase/v1/classification/chunks?tenant_id=$TenantId&bot_id=$BotId&source_ids=$([uri]::EscapeDataString($sid))&chunk_type=all&limit=$limit&offset=$offset"
    $page = Invoke-RestMethod $url -TimeoutSec 120
    foreach ($it in $page.items) {
      if ($it.metadata.source_filename) { $filenames[$it.metadata.source_filename] = $true }
      $q = $it.metadata.chunk_quality_score
      if ($null -ne $q) {
        $qd = [double]$q
        [void]$scores.Add($qd)
        [void]$allScores.Add($qd)
        $scoredChunks++
        $bands[(Grade $qd)]++
        switch ($it.metadata.ocr_quality) {
          "high" { $ocrHigh++ }
          "medium" { $ocrMed++ }
          "low" { $ocrLow++ }
          default { $ocrUnk++ }
        }
        if ($it.needs_review -or $it.metadata.needs_review) { $needsReview++ }
      }
    }
    $offset += $limit
  } while ($offset -lt [int]$page.total)

  $mean = if ($scores.Count) { ($scores | Measure-Object -Average).Average } else { $null }
  $sorted = @($scores | Sort-Object)
  $median = $null
  if ($sorted.Count -gt 0) {
    $mid = [int][math]::Floor(($sorted.Count - 1) / 2)
    $median = if ($sorted.Count % 2 -eq 1) { $sorted[$mid] } else { ($sorted[$mid] + $sorted[$mid + 1]) / 2 }
  }

  $perSource += [ordered]@{
    source_id = $sid
    chunks = [int]$src.total_chunks
    scored_chunks = $scores.Count
    mean_quality = if ($null -ne $mean) { [math]::Round($mean, 4) } else { $null }
    median_quality = if ($null -ne $median) { [math]::Round($median, 4) } else { $null }
    grade = if ($null -ne $mean) { Grade $mean } else { "-" }
    other_ratio = [math]::Round([double]$src.other_ratio, 3)
    filenames = @($filenames.Keys | Sort-Object)
  }
}

$allSorted = @($allScores | Sort-Object)
$globalMean = if ($allScores.Count) { ($allScores | Measure-Object -Average).Average } else { 0 }
$globalMedian = 0
if ($allSorted.Count -gt 0) {
  $mid = [int][math]::Floor(($allSorted.Count - 1) / 2)
  $globalMedian = if ($allSorted.Count % 2 -eq 1) { $allSorted[$mid] } else { ($allSorted[$mid] + $allSorted[$mid + 1]) / 2 }
}

$out = [ordered]@{
  generated_at = (Get-Date).ToUniversalTime().ToString("o")
  tenant_id = $TenantId
  bot_id = $BotId
  telematry_total = [ordered]@{
    sources = $summary.sources.Count
    chunks = [int]$summary.total_chunks
    unreviewed_chunks = [int]$summary.unreviewed_chunks
    other_chunks = [int]$summary.other_chunks
    other_ratio = [math]::Round([double]$summary.other_ratio, 3)
  }
  ccli = [ordered]@{
    sources_ingested = $ccliSources.Count
    chunks = [int]($ccliSources | Measure-Object -Property total_chunks -Sum).Sum
    scored_chunks = $scoredChunks
    pdf_files_on_disk = $pdfPaths.Count
    md_files_on_disk = $mdPaths.Count
    project_files_md_pdf = $pdfPaths.Count + $mdPaths.Count
    quality = [ordered]@{
      mean = [math]::Round($globalMean, 4)
      median = [math]::Round($globalMedian, 4)
      min = if ($allScores.Count) { [math]::Round(($allScores | Measure-Object -Minimum).Minimum, 4) } else { $null }
      max = if ($allScores.Count) { [math]::Round(($allScores | Measure-Object -Maximum).Maximum, 4) } else { $null }
      grade = Grade $globalMean
      bands = [ordered]@{ A = [int]$bands.A; B = [int]$bands.B; C = [int]$bands.C; D = [int]$bands.D; F = [int]$bands.F }
      needs_review = $needsReview
      ocr = [ordered]@{ high = $ocrHigh; medium = $ocrMed; low = $ocrLow; unknown = $ocrUnk }
    }
    pdf_inventory = @($pdfPaths | Sort-Object Name | ForEach-Object {
      [ordered]@{
        name = $_.Name
        path = $_.FullName.Replace("C:\Yahia\projects\Telematry System\documents\", "").Replace("\", "/")
        kb = [math]::Round($_.Length / 1KB, 1)
      }
    })
    sources = $perSource
  }
}

$dest = Join-Path $websiteRoot "public\corpus-stats.json"
$out | ConvertTo-Json -Depth 8 | Set-Content $dest -Encoding utf8
Copy-Item $dest "c:\Yahia\projects\CCLI\knowledge-base\08-engineering\corpus-stats.json" -Force
Write-Host "Wrote $dest"
Write-Host "CCLI: $($out.ccli.sources_ingested) sources, $($out.ccli.chunks) chunks, $($out.ccli.pdf_files_on_disk) PDFs, grade $($out.ccli.quality.grade)"
