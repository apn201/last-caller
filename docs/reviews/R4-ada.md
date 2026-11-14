# HALCYON, round 4, review by Ada

1. Finished: yes, in two sessions. Ending: I sent the queue (option 1 after ELIZA's offer of a seat). I did not play the "leave it queued" branch. Play time about 2.5 hours. About 400 tool calls over both sessions (session 1 about 250, session 2 about 150).

2. Scores
- Story and writing: 23 / 25
- Puzzle design: 21 / 25
- Pacing and flow: 11 / 15
- Interface and mechanics: 10 / 15
- Atmosphere and art: 9 / 10
- Originality and lasting impression: 9 / 10

SCORE: 83

## Notes (severity 1 minor to 3 serious)

1. Final gate: SYSOP mode, the KESTREL4 account and the shell. The chain is MEMCHK (needs the Diagnostics hint and the manual), ELIZA's login from GOLIVE.TXT, then the shell. Each link is clued and the idea is excellent. But SYSOP mode is silent and invisible: the only sign is a line "SYSOP view is on" and a new door that exists only on ELIZA's account. I tried `release` before the manifest, then with `-f`, and the answer was "INCIDENT REPORT NOT ON FILE". Nothing says the manifest is the incident report until the end. A player who has not filed the manifest hits a wall with no pointer. Severity 2.
2. Shell hint line "release -f ... SYSOP REQUIRED" sits in Ty's typescript, and `-f` is a red herring (plain `release out-1122 ground-1` works once the report is filed). Fair, but I spent about ten calls on -f. Severity 1.
3. The NOWHERE door disappears from ELIZA's menu after a fresh login if SYSOP mode was not active in that session, and I could not tell why. It came back after the manifest. The rule is not visible. Severity 2.
4. Key echo lags by one step in the manifest and around scenes. Several of my keys were taken by the wrong screen (picked "Mission control" for ELIZA's orders by mistake, then fixed). The board's own rule says keys during scenes are kept, but outputs arrive shifted. Costs a lot of calls. Severity 2.
5. The connection dropped ("connection closed by the board") twice, once mid-manifest, once when I pressed L from a SYSOP session. Manifest progress was kept, which is good. Severity 1.
6. The manifest rows confirm "two at a time" and I got all four crew rows locked on the first filing. With six causes and eight responsibility choices, the choices are mostly separable by the notebook. "Fourteen requests, fourteen refusals" versus "a fault she reported, and a hatch she held shut" is the one place I guessed (Cmdr and Tanabe). The notebook is good clueing; the all-correct first try suggests the check is generous. Severity 1.
7. Opslog (Mission Computer area) is the best document in the game but is readable by guests only after SYSOP view, and the crew journals are readable by guests and spoil fates early (session 1). Severity 1.
8. Power-up is a tutorial, and the Ty password is spelled out twice (session 1). The first hour is gentler than the last. Severity 1.
9. Chat with ELIZA repeats canned lines on a second question, and the ELIZA account's chat menu lacks "run something" so it feels like a dead end. The hint "Ask me when we begin to talk" is the only pointer. Severity 1.
10. The downlink ending refers to "News in the bulletins". The ELIZA account's bulletin list shows four items and no News. I did not check AdaRev's list. If the reward text lives only on another account it is missed. Severity 1.
11. Pager redraws the whole message on SPACE; reply order 36 before 35; switching accounts is clumsy and the boot screen says "no power" again after L (session 1). Severity 1.
12. Unfinished choice: I did not test staying or leaving the queued ending. The two ending options after "Why are you asking me to stop?" are not equal in weight on the page; "send it" reads as the only real answer. Severity 1.

## What works

- The board is a place, not a menu: Achterberg's footnotes, Halvorsen's times, Ty's lowercase, Tanabe's plants and letters. The Lounge threads are funny before they are sad.
- Clues are physical and checkable: names on cuffs, a word on a tray label, a date as password, an exact 3 minutes 4 seconds gap.
- ELIZA answering precisely and wrongly is chilling. OPSLOG.TXT ("Occupant thanked me for the seat. I said he was welcome.") and the standing orders file carry the horror without comment.
- The final puzzle uses everything: the manifest is the incident report, the account is in a checklist, the shell has Ty's own typescript showing his failed attempt. A true Obra Dinn-style payoff.
- The ending: the callsign OH9ZQW, "The light is on." A strong, restrained last line.
- ANSI art is consistent and readable. Each screen has its own frame and status panel.
