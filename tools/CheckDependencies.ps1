# Checks that the modules (folders under src/) only depend on modules below them.
#
#   Game     may include Engine, Physics and Math
#   Engine   may include Physics and Math
#   Physics  may include Math
#   Math     may include nothing from src/
#
# Run from anywhere:  powershell -File tools/CheckDependencies.ps1
# Exit code is 0 when the dependencies are respected, 1 otherwise.

param(
    [string]$Root = (Join-Path $PSScriptRoot "..")
)

# Lowest layer first. A module may only include modules at a lower position (or its own).
$Layers = @("Math", "Physics", "Engine", "Game")

$srcRoot = Join-Path (Resolve-Path $Root) "src"
if (-not (Test-Path $srcRoot)) {
    Write-Host "No src folder found at $srcRoot"
    exit 1
}
$srcRoot = (Resolve-Path $srcRoot).Path

function Get-Module($path) {
    $relative = $path.Substring($srcRoot.Length).TrimStart('\', '/')
    $parts = $relative -split '[\\/]'
    if ($parts.Count -lt 2) { return $null }   # file directly in src/ (not part of a module)
    return $parts[0]
}

# Finds the file an #include "..." refers to: next to the including file first, then from src/
function Resolve-Include($fromFile, $includeName) {
    $candidates = @(
        (Join-Path (Split-Path $fromFile) $includeName),
        (Join-Path $srcRoot $includeName)
    )
    foreach ($candidate in $candidates) {
        if (Test-Path -LiteralPath $candidate -PathType Leaf) {
            return (Resolve-Path -LiteralPath $candidate).Path
        }
    }
    return $null
}

$edges = @{}        # "From>To" -> first "file:line" that creates it
$violations = @()

$files = Get-ChildItem $srcRoot -Recurse -Include *.h, *.hpp, *.cpp
foreach ($file in $files) {
    $fromModule = Get-Module $file.FullName
    if (-not $fromModule) { continue }

    $lineNumber = 0
    foreach ($line in Get-Content $file.FullName) {
        $lineNumber++
        if ($line -notmatch '^\s*#\s*include\s*[<"]([^>"]+)[>"]') { continue }

        $target = Resolve-Include $file.FullName $Matches[1]
        if (-not $target) { continue }               # standard library
        if (-not $target.StartsWith($srcRoot)) { continue }   # outside src/ (external/ libraries)
        $toModule = Get-Module $target
        if (-not $toModule -or $toModule -eq $fromModule) { continue }

        $relativeFile = $file.FullName.Substring($srcRoot.Length - 3)   # starts with "src"
        $where = "${relativeFile}:$lineNumber"
        $key = "$fromModule>$toModule"
        if (-not $edges.ContainsKey($key)) { $edges[$key] = $where }

        $fromRank = $Layers.IndexOf($fromModule)
        $toRank = $Layers.IndexOf($toModule)
        if ($fromRank -ge 0 -and $toRank -ge 0 -and $toRank -gt $fromRank) {
            $violations += "$where  $fromModule must not include $toModule  ($($Matches[1]))"
        }
    }
}

# Cycles between modules, including folders that are not in $Layers
$graph = @{}
foreach ($key in $edges.Keys) {
    $from, $to = $key -split '>'
    if (-not $graph.ContainsKey($from)) { $graph[$from] = @() }
    $graph[$from] += $to
}

$cycles = @()
function Find-Cycles($node, $stack) {
    if ($stack -contains $node) {
        $start = $stack.IndexOf($node)
        $cycle = $stack[$start..($stack.Count - 1)] + $node
        $script:cycles += ($cycle -join " -> ")
        return
    }
    foreach ($next in $graph[$node]) { Find-Cycles $next ($stack + $node) }
}
foreach ($module in $graph.Keys) { Find-Cycles $module @() }
$cycles = $cycles | ForEach-Object {
    # Same cycle found from several starting points: keep one spelling
    $names = ($_ -split ' -> ' | Select-Object -SkipLast 1) | Sort-Object
    [pscustomobject]@{ Key = ($names -join ','); Text = $_ }
} | Sort-Object Key -Unique | ForEach-Object Text

$unknown = $graph.Keys + ($graph.Values | ForEach-Object { $_ }) | Sort-Object -Unique | Where-Object { $Layers -notcontains $_ }

Write-Host "Module dependencies found:"
foreach ($key in ($edges.Keys | Sort-Object)) {
    Write-Host ("  {0,-20} {1}" -f ($key -replace '>', ' -> '), $edges[$key])
}
if ($edges.Count -eq 0) { Write-Host "  (none)" }
Write-Host ""

foreach ($module in $unknown) {
    Write-Host "Note: '$module' is not in the layer list at the top of this script, so only cycles are checked for it."
}

if ($violations.Count -eq 0 -and $cycles.Count -eq 0) {
    Write-Host "OK: no dependency goes the wrong way and there are no cycles."
    exit 0
}

if ($violations.Count -gt 0) {
    Write-Host "Wrong-way includes ($($violations.Count)):"
    $violations | ForEach-Object { Write-Host "  $_" }
}
if ($cycles.Count -gt 0) {
    Write-Host "Cycles:"
    $cycles | ForEach-Object { Write-Host "  $_" }
}
exit 1
