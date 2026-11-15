# Builds the committed main on the staging host, starts the board there on a
# fresh disposable card with the card's own timers, and forwards its Telnet
# port to this PC: 127.0.0.1:12323 here -> 127.0.0.1:2323 there. Leave this
# window running while the bbs-caller tool (tools/mcp/bbs_caller.py, MCP server
# "bbs-caller") plays; end it (Ctrl+C, or touch ~/lastcaller-staging/stop on the
# host) and the board there stops.
# Uncommitted changes are NOT served: only the pushed main.
# Works only under /home/lastcaller; the server binds 127.0.0.1 only.
param(
  [string]$Remote = 'lastcaller@helppox.com',
  [int]$LocalPort = 12323
)
$ErrorActionPreference = 'Continue'
Set-Location (Join-Path $PSScriptRoot '..')

if (git status --porcelain) { Write-Warning 'Uncommitted changes are not served: only the pushed main is built.' }
if (-not (git remote | Where-Object { $_ -eq 'staging' })) {
  git remote add staging "${Remote}:last-caller.git"
  if ($LASTEXITCODE -ne 0) { throw 'git remote add staging failed' }
}
git push staging main
if ($LASTEXITCODE -ne 0) { throw 'git push staging main failed' }

# -L: the tunnel; SERVE=1: staging-remote.sh leaves the server up until this session ends.
# Stop it from another window:  ssh $Remote "touch ~/lastcaller-staging/stop"
ssh -o ExitOnForwardFailure=yes -o ServerAliveInterval=20 -o ServerAliveCountMax=6 -L "127.0.0.1:${LocalPort}:127.0.0.1:2323" $Remote "SERVE=1 bash <(git --git-dir=`$HOME/last-caller.git show main:tools/staging-remote.sh)"
exit $LASTEXITCODE
