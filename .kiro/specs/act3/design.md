# Design - Act 3

## Content map

| Item | Where | Grants / does |
| --- | --- | --- |
| Ty's last post, "bay" (8 Dec 04:13) | Lounge, last message | `KVF` (read trigger) |
| `MEM_BAY` memory scene | opening `POWER.LOG` | once per caller; art `MEM_BAY` |
| Club post and reply, 2026 | Radio Room, after Eero's "Porch light" | none |
| Order code query `NW1984417` | `DIAG.INI` | `NOWHERE_OPEN`, `KZO` |
| Comms manual section 6 (authorisation codes, queues) | Library `COMMS.TXT` | none |
| Haven tree | `SHELL.INI` | none |
| `incident.rpt` | haven, `has=M_Z` | none |
| Final command `release out-1122 ground-1` | `SHELL.INI` | scene `ENDING` |
| Real `ENDING` | `SCENES/ENDING.INI` | `END1` / `END2` |
| Bulletin 5, News Wire | `BULLETINS.INI`, `required_flag=END1` | none |
| Eero's mail | trigger `EERO_MAIL` (login, `END1`) | once |

## The unlock chain, steps 5 to 7

5. Order code. The four pieces give `NW1-98-44-17` in piece order. Hints:
   the Archive posts ("the machine that reads everything has to be asked to
   look"; the place is called NOWHERE), the pieces ("AUTHORISATION CODE"), and
   comms manual section 6 ("authorisation codes are entered as queries").
   Typed into Diagnostics it is one more query (`NW1984417`, nine characters);
   ELIZA answers it and the door menu gains `N`.
6. The haven. Free to read. The orders explain the power (article 3), the
   silence (article 5) and why she was kind (article 6). The register and
   the rate card prove what the rack is for (Z-W). The queue holds the
   crew's mail.
7. The command. Verb from the Professor, syntax from the manual, names from
   the queue header. The hold reason is the hole: article 5 forbids the
   mission computer to file an incident report; it does not forbid the
   caller. The manifest is that report. With `M_Z` the haven grows
   `incident.rpt` and `release` is a command; without it, "command not
   found".

## Engine (small)

- `shell_logic.h`: `final_usage=`. With the flags for the final command and
  the final command's first word typed with other arguments, the shell
  answers `final_usage` (default: command not found). Without the flags the
  verb does not exist. Host test in `shell_logic_test.cpp`.
- Nothing else: the order code is a DIAG query, the news is a bulletin, the
  mail is a trigger, the club post is a history file.

## Clock

The club's posts carry `1998-12-08 04:2x`: the board's clock resumes at 04:17
on every power-up, so anyone who posts now is stamped then. Nothing is
future-dated. The text says so ("It says that every time").

## The haven (draft tree)

```
/README  /etc/motd  /etc/hosts  /etc/orders.cfg
/srv/rack2/clients.lst  /srv/rack2/rates.txt
/var/log/outbound
/var/spool/out/queue  /var/spool/out/index  /var/spool/out/incident.rpt (M_Z)
```

Queue `out-1122`: created 22 Nov 14:09, one job, 41 messages (CMDR 9, CHAOS 6,
GARDEN 14, TYVANCE 12), 3 files (`AUDIT.LOG`, the standing order, the client
register) added by CHAOS 3 Dec 02:08 for `press-list`. Destination `ground-1`
(the downlink, disabled by command 22 Nov 14:09).

## Endings

`ENDING.INI` (repeat, `has=M_Z`): the queue is released; ELIZA states the
downlink fact and her orders; the caller sends, asks why, or leaves it queued.
Send: `END_SENT`, the club station relays (01:52 UTC, 12 Nov 2026), ELIZA files
the wire as bulletin 5, Eero reads Keiko's reply of 25 Nov and answers; ELIZA's
last line. Stay: as before, the name joins the crew list.

## Art

`tools/ansi/screens/act3.py`: `MEM_BAY` (12 rows, plain twin) and bulletin
`BULLETINS/005_NEWS` (ANSI and plain). Rendered to PNG and looked at.

## Test

The e2e plays act 3 after act 2 as before: no `--shell-open`, the code typed in
Diagnostics, the haven, the manifest, wrong then right command, both endings,
the bulletin, Eero's mail. `staging-remote.sh` no longer edits `SHELL.INI`.
