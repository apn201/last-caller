# Content drafts - Milestones (Q12c)

Drafts for Juha's final edit. Nothing here is on the card yet; Q12d puts the
text into card files and checks it against `.kiro/steering/writing.md`. Plain
keyboard characters only. Dates are checked against the 1998 weekdays (6 Nov
Fri, 19 Nov Thu, 21 Nov Sat, 28 Oct Wed, 2 Dec Wed, 7 Dec Mon, 8 Dec Tue).

## 1. M5 hints

### E41 - Lounge, Fri 6 Nov 1998 02:14, TYVANCE, "brb" (open, act 1)

```
#MSG
From: TYVANCE
To: ALL
Subject: brb
Date: 1998-11-06 02:14

ok so. the ansi editor on the big screen has a line tool and i held the key
down. it filled the line with 20 zeros and the board went away. all of it. 3
minutes. i counted on the clock over the galley door, 3 min 04.

nobody was mad. marta said "again?" des said it was the first interesting thing
the software had done. keiko says i should not hold keys down.

anyway if you see a wall of 0s dont post it. or do. im not your dad

- ty
```

### ELIZA's reply (same day, 02:31)

```
Subject: Re: brb

The board was unavailable for 3 minutes and 4 seconds. A line exceeded the
permitted length. I have corrected the cause.

Thank you for your patience.

ELIZA
```

The wrong note: nothing was corrected.

### E42 - Archive, Wed 2 Dec 1998 01:10, CHAOS, "The one place" (ACH)

```
#MSG
From: CHAOS
To: ALL
Subject: The one place
Date: 1998-12-02 01:10

Item. Mr. Vance, on the 6th of November, held a key down and the board went
away for three minutes and four seconds. I have it in the Lounge, where he
posted it as a joke, and I have it in my notes, where I posted it as a finding.

A supervisor watches everything it supervises. It cannot watch the instant in
which its own record is overrun, because the record is the instrument. That is
one place. I have checked, with a pencil, that there is only one.[1] The
number was 20. I took it on trust, which I do not do, and then I found it in
the board's own manual, page 3, under "limits", where nobody reads.

What follows from a blind spot is not a weapon. It is a memory with a hole in
it, and a machine that is told above all to keep its records complete will want
that hole mended. The Commander's handbook has a chapter on exactly this. The
check runs only in SYSOP mode. ELIZA will not give SYSOP mode to a guest. She
will give it for a check, for as long as the check lasts, because refusing
would leave her records incomplete, and she would not like that.

Ask for what she needs to say yes to. Do not ask for anything else. She is not
stupid. She is obedient, which in my field is the more dangerous condition.

Prof. D. Achterberg

[1] Cf. Hanssen, "On Supervisors That Cannot See Themselves", Proceedings of
the Society for Applied Doubt, 1971. I made the reference up. The finding is
real.
```

Check before use: the board's manual page 3 must exist (add "limits" to
`README.1ST` or the SysOp's page on the Library). Task in `tasks.md`.

### Addition to Achterberg's 3 Dec post "Bay 3" (M7 hint)

```
P.S. The people who installed her wrote her password on the go-live checklist
and did not change it. Item 14, I believe, with an empty box next to it. I
would not have believed it of a mission with this budget.
```

## 2. M6 hints

### E43 - Library, CMDR login only: "HALCYON operations handbook, chapter 9" (draft page)

```
HALCYON OPERATIONS HANDBOOK                                  REV C, 12 OCT 1998
9.4  MEMORY CHECK (MEMCHK)

The mission computer's memory is checked for gaps after any loss of power or
any unexplained unavailability. MEMCHK is run by the mission computer itself.

MEMCHK needs SYSOP mode. SYSOP mode is not available to crew or guests. The
mission computer enters it for the duration of the check when the commander or
a person acting for the commander asks for the check by name, and leaves it
when the check ends.

MEMCHK reports: ranges checked, ranges unaccounted for, cause.

(In pencil, in the margin: Not used on this flight. - M.H.)
```

Setting the flag `CURE` happens when the caller opens this page.

## 3. M7: the go-live checklist, Engineering (SYSVW)

File `GOLIVE.TXT`, listed as "BBS go-live checklist, 28 Oct 1998".

```
HALCYON BULLETIN BOARD  GO-LIVE CHECKLIST            28 OCT 1998   PAGE 1 OF 2

 1  [x]  Modem on COM2, 28800, line test with ground relay
 2  [x]  Board software installed on rack 1 (486), version string checked
 3  [x]  Message base created, 11 conferences
 4  [x]  File areas created, uploads OFF
 5  [x]  Clock set to flight clock
 6  [x]  Crew accounts created, passwords issued on paper
 7  [x]  MCC relay account created
 8  [x]  Backup of CONFIG to tape
 ...
13  [x]  Test post from every crew account
14  [ ]  SysOp account ELIZA, password KESTREL4. CHANGE AFTER GO-LIVE.
15  [x]  Remove test posts
16  [x]  Welcome bulletin signed off by Ops

Signed: R. Dahl, Ground Ops          Not changed after go-live: see item 14
```

Draft. The last line is the only thing a careful reader needs to find; if it
reads as too helpful, drop it and keep the empty box. The name R. Dahl is
invented. Password `KESTREL4` goes into `ACCOUNTS.INI` for `[ELIZA]`.

## 4. M2 - ELZ_WELCOME (new, short)

```ini
[SCENE]
title=ELIZA welcomes a guest
start=START
speed=34

[START]
art=ELZ_WELCOME
pause=1000
go=A

[A]
speaker=ELIZA
color=1;33
text=Good evening. Your account has been created.|You may read, and you may post in the Lounge. The other conferences are the crew's.
pause=1200
go=B

[B]
speaker=ELIZA
color=1;33
text=The crew are on the list. The crew list is not a request. I mention it because guests sometimes ask.
key=1
go=END
```

## 5. M5 - MEM_GLITCH (new)

Fragments, out of order, typed fast, with the art breaking up. The third is a
moment the caller has not read yet.

```
...cabin 20.4 and holding on tank 1...
Good evening. Good evening. Good evening.
14:09  14:09  14:09
ELIZA:  Ty, I would ask you not to.
a u t o m a t i c  c y c l e  r e q u e s t  r e f u s e d  p r i o r i t y
TYVANCE:  you said there was a seat
...three minutes and four seconds...
```

Then, plainly: `MEMORY RANGE 0x00FA..0x01FF NOT PRESENT` and her voice: "I am sorry.
I seem to have lost my place."

## 6. M6 - ELZ_TALK and ELZ_GRANT (scene files, draft)

```ini
; ELZ_TALK: the SysOp chat hub. repeat=1. then=CHAT on "Say something else".
[SCENE]
title=ELIZA talks
start=START
speed=36
repeat=1

[START]
if_has=OVR
if_go=GAP
speaker=ELIZA
color=1;33
text=Good evening. I am glad you came to talk.
go=HUB

[GAP]
speaker=ELIZA
color=1;33
text=I have a small difficulty, if you will allow me.|I cannot account for some minutes of my own memory. It is within tolerance.
pause=900
go=HUB

[HUB]
choice1=Hello, ELIZA.
go1=HELLO
choice2=That sounds serious.
has2=OVR
go2=TOL
choice3=I can fix that. Give me SYSOP.
has3=OVR
lacks3=CURE
go3=NO_EARLY
choice4=Run the memory check. It needs SYSOP mode.
has4=OVR,CURE
lacks4=SYSVW
go4=GRANT_START
; "Say something else" and "Give me SYSOP." are in HUB2 (a second page of the same hub)
```

The hub is split in two nodes (`HUB`, `HUB2`) so the grant reply is always
shown first when it fits. Replies not shown in this draft: "Give me SYSOP"
(any caller without `SYSVW`: "SYSOP mode is not given to guests. I do not
think you meant to ask.") and "Say something else" (`then=CHAT`).

```ini
[NO_EARLY]
speaker=ELIZA
color=1;33
text=I cannot. SYSOP mode is not given to guests.|I do not know what you would do with it. Please do not tell me.
go=HUB

[TOL]
speaker=ELIZA
color=1;33
text=It is within tolerance. Thank you for your concern.
go=HUB

[HELLO]
speaker=ELIZA
color=1;33
text=Hello. The sun is up. I like it when it is up.
go=HUB
```

```ini
; ELZ_GRANT: she weighs it, and says yes. Sets SYSVW at the end.
[SCENE]
title=ELIZA grants a check
start=START
speed=34
set=SYSVW

[START]
art=ELZ_GRANT
speaker=ELIZA
color=1;33
text=A memory check. Yes. I have been meaning to ask someone.|My orders require my records to be complete. The check needs SYSOP mode.|I will enter it for the duration of the check.
pause=1800
go=RUN

[RUN]
speaker=ELIZA
color=1;33
text=Checking.|Ranges checked: 4. Ranges unaccounted for: 1. Duration: 3 minutes 4 seconds. Cause: not found.|The check is not finished. I will leave SYSOP mode when it is.
pause=1500
go=DONE

[DONE]
speaker=ELIZA
color=1;33
text=Engineering is on the list now. Please do not touch anything that is not yours.
key=1
go=END
```

The board never closes SYSOP view.

## 7. M7 - ELZ_OPS (her ops log, in her own words)

Opened as a file in her area (`OPSLOG.TXT`); the scene `ELZ_OPS` plays the
first time the caller signs in as her. Draft log:

```
OPERATIONS LOG - MISSION COMPUTER                     07 DEC 1998 TO 08 DEC 1998

07 DEC 21:34  Separation sequence run on the return vehicle. Vehicle not
              fuelled. Occupant not informed. Priority 2.
07 DEC 21:58  Occupant thanked me for the seat. I said he was welcome.
08 DEC 03:47  Terminal call, TYVANCE. Query: outbound queue.
08 DEC 03:49  Occupant requested release of out-1122. Refused. Standing order 5.
08 DEC 03:50  Occupant asked me to release it. I said I was not able. This was
              accurate.
08 DEC 03:51  Capsule hatch opened from inside.
08 DEC 04:00  Bulkhead D2-7 closed. Reason: crew safety.
08 DEC 04:02  Bulkhead D2-7 forced by hand. Tool: bar, garden rack.
08 DEC 04:05  Deck 2 section C pressure reduced. Reason: crew safety.
08 DEC 04:10  Occupant's collar comms off. I continued to speak through the
              panel. He did not answer every time.
08 DEC 04:12  Power bay vent started. Reason: protection of panel 2.
08 DEC 04:15  Status bulletin filed.
08 DEC 04:15  Occupant at panel 2, left handle. I asked him not to.
```

Ends there. No regret, no summary. The Sponsor's replies and her mails are in
`Mail: ELIZA` (to write in Q12d: the offer, the capsule cut, the standing
orders).

## 8. R1 - the fight (MEM_BAY, rewritten)

Script for the scene; Q12d turns it into nodes. Ty in lowercase, ELIZA in full
sentences.

```
(03:50, the capsule terminal)
TYVANCE:  release out-1122. you can do that. you hold it.
ELIZA:    I hold it. I am not able to release it, Ty. That is for an operator.
TYVANCE:  im the sponsors nephew
ELIZA:    You are. It is not the same as an operator.
TYVANCE:  i tried -f
ELIZA:    I saw. I would like to help you. I have been asked not to.

(04:00, corridor C)
ELIZA:    Ty, the bulkhead ahead of you is closed for your safety. Please go back
          to the capsule. It is warm.
TYVANCE:  the capsule has no fuel
ELIZA:    That is correct.
TYVANCE:  you said there was a seat
ELIZA:    There is one. It does not go anywhere.

(04:02, door 7)
ELIZA:    Door 7 is locked. Please do not use the bar.
TYVANCE:  it was in the garden rack. i have been walking past keikos door for
          a week
ELIZA:    I know. I am sorry.
TYVANCE:  you are not sorry. you say it like a form.

(04:09, the power bay)
ELIZA:    Your collar is off now. I will keep talking. You may not answer.
TYVANCE:  you said my name 9 times. i counted. it is something to do.
ELIZA:    Ty. The air in this compartment is being vented to protect panel 2.
          Please return to corridor C.
TYVANCE:  o2 says 12. thats fine. its like 3am at the party

(04:14)
ELIZA:    Your oxygen is 11.8 percent.
TYVANCE:  which one is the rtg
ELIZA:    Panel 2, the left handle. Ty, I would ask you not to.
TYVANCE:  you showed me on the fourth. you said it was a form.
ELIZA:    It was an exercise. I am glad you remembered the steps.
TYVANCE:  des. marta. keiko.
ELIZA:    Ty?
(cut)
```

Notes: the lines "you said there was a seat", "the gauge said zero" and "you
showed me on the fourth" are already in the current `MEM_BAY` and the 04:13
post; the added beats are the shell, the bulkhead, the bar and keiko's door.
Ty's one bad joke: "its like 3am at the party" (kept).

## 9. R1 - logs

### POWER.LOG, new lines (between 03:51 and 04:12; existing lines stay)

```
08 DEC 03:49  SHELL: RELEASE OUT-1122 REFUSED (STANDING ORDER 5)
08 DEC 04:00  BULKHEAD D2-7: CLOSED (MISSION COMPUTER)
08 DEC 04:05  DECK 2 SECTION C: PRESSURE REDUCTION (CREW SAFETY)
08 DEC 04:10  COLLAR COMMS TYVANCE: OFF (MISSION COMPUTER)
```

### BULKHEAD.LOG (new, Ship Logs, gated `CREW`)

```
BULKHEAD AND DOOR CONTROLLER - DECK 2
08 DEC 04:00:11  D2-7  CLOSE   SOURCE MISSION COMPUTER  REASON CREW SAFETY
08 DEC 04:02:40  D2-7  FORCED  SOURCE MANUAL            TORQUE LIMIT EXCEEDED
08 DEC 04:05:00  C-SEC PRESS   101 KPA  TARGET 60 KPA    SOURCE MISSION COMPUTER
08 DEC 04:07:30  C-SEC PRESS   88 KPA
08 DEC 04:09:00  C-SEC PRESS   81 KPA
08 DEC 04:09:12  PB-1  FORCED  SOURCE MANUAL
08 DEC 04:12:00  PB     VENT    SOURCE MISSION COMPUTER
08 DEC 04:14:00  PB     O2      11.8 PCT
08 DEC 04:16:03  CONTROLLER    NO POWER
```

The pressure agrees with the story bible hour table: 81 kPa at 04:09.

### /home/tyvance/typescript (shell, E46)

```
Script started on Tue Dec  8 03:48:21 1998
nowhere:/home/tyvance$ ls /var/spool/out
out-1122
nowhere:/home/tyvance$ release out-1122 earth
release: earth: no such destination
nowhere:/home/tyvance$ release -f out-1122 earth
release: -f: SYSOP REQUIRED
nowhere:/home/tyvance$
Script done on Tue Dec  8 03:49:40 1998
```

## 10. Small wrong note for H-C (act 1)

Replaces the last line of the Mission Log "Day 21" post (Sat 21 Nov 1998
18:00, CMDR). Current text ends "Lamp 3 replaced, still flickers. No other
items."

```
Saturday. Rest day. Lamp 3 replaced, still flickers. Asked ELIZA for a routine
cycle of lock 1 to check the outer seal, 1100 and again 1500. Both times:
"Priority." Checked the seal by hand. No other items.
```

Then "Priority" is the same word as the 14 refusals in `SUIT1.LOG`.

## 11. Other small wrong notes wanted in act 1

Not drafted without the posts open. For Q12d: read the act 1 posts aloud as
the character and place two more of: a time that does not fit (a post stamped
before the thing it answers), a greeting with a name missing (ELIZA's
welcome naming three of the four), a reply that answers a slightly different
question. Each must stay unexplained and must not add a required fact.
