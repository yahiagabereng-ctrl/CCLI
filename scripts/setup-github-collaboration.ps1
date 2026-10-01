# Setup private GitHub repo and invite alirezavzand as collaborator.
# Prerequisites: gh auth login (run once), git user.name/email configured.

$ErrorActionPreference = "Stop"
$Collaborator = "alirezavzand"
$RepoName = "CCLI"

Set-Location (Split-Path -Parent $PSScriptRoot)

Write-Host "Checking GitHub CLI auth..."
gh auth status 2>&1 | Out-Null
if ($LASTEXITCODE -ne 0) {
    Write-Host "Not logged in. Run: gh auth login -h github.com -p https -w"
    exit 1
}

$User = (gh api user -q .login)
Write-Host "GitHub user: $User"

if (-not (git rev-parse --verify HEAD 2>$null)) {
    $Email = gh api user/emails -q '.[] | select(.primary==true).email' 2>$null
    if (-not $Email) { $Email = "$User@users.noreply.github.com" }
    $env:GIT_AUTHOR_NAME = $User
    $env:GIT_COMMITTER_NAME = $User
    $env:GIT_AUTHOR_EMAIL = $Email
    $env:GIT_COMMITTER_EMAIL = $Email
    git add -A
    git commit -m "Initial commit: CCLI central plant controller project." `
        -m "Exclude build artifacts, OpenWrt SDK trees, and TLS lab credentials from version control."
}

$Remote = git remote get-url origin 2>$null
if (-not $Remote) {
    Write-Host "Adding remote for existing repo $User/$RepoName..."
    git remote add origin "https://github.com/$User/$RepoName.git"
}
Write-Host "Pushing to origin/main..."
git config http.postBuffer 524288000
git push -u origin main

Write-Host "Inviting collaborator $Collaborator (write access)..."
gh api -X PUT "repos/$User/$RepoName/collaborators/$Collaborator" -f permission=push

Write-Host ""
Write-Host "Done."
Write-Host "  Repo:    https://github.com/$User/$RepoName"
Write-Host "  Invite:  $Collaborator must accept at https://github.com/notifications"
Write-Host ""
Write-Host "For live pair sessions: use VS Code Live Share or screen share + PR review."
