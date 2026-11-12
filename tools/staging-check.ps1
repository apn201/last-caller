# Server build and end-to-end check on the staging host (Rocky Linux, SSH).
# Pushes the committed main to the bare repo there, then runs
# tools/staging-remote.sh over SSH. Uncommitted changes are NOT tested.
# Works only under /home/lastcaller; the server binds 127.0.0.1 only.
param(
  [string]$Remote = 'lastcaller@helppox.com',
  [string]$Crew = ''   # HANDLE:PASSWORD of a crew account, once one exists
)
# Native tools (git, ssh) write progress to stderr; under 'Stop' Windows PowerShell 5.1
# turns that into an error, so check $LASTEXITCODE instead.
$ErrorActionPreference = 'Continue'
Set-Location (Join-Path $PSScriptRoot '..')

if (git status --porcelain) { Write-Warning 'Uncommitted changes are not tested: only the pushed main is built.' }
if (-not (git remote | Where-Object { $_ -eq 'staging' })) {
  git remote add staging "${Remote}:last-caller.git"
  if ($LASTEXITCODE -ne 0) { throw 'git remote add staging failed' }
}
git push staging main
if ($LASTEXITCODE -ne 0) { throw 'git push staging main failed' }

$crewEnv = if ($Crew) { "CREW='$Crew' " } else { '' }
# The script is read from the pushed repository, so it is always the committed one.
ssh $Remote "${crewEnv}bash <(git --git-dir=`$HOME/last-caller.git show main:tools/staging-remote.sh)"
exit $LASTEXITCODE
