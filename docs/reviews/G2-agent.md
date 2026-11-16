# G2 - agent playtest (16 Nov 2026)

Run by the `playtester` agent on staging (deploy 044154d, the first G2 build), from a brand-new caller, aimed at the one chat. The agent hit its turn limit and did not write this file; Kiro wrote it from the agent's report. About 300 tool calls, about 90 minutes of reading. The fixes listed below are in 28ba5f8 and 199bcde.

## Result

Not finished. The agent had its own account and three crew accounts (TYVANCE, CHAOS, CMDR), the Founder's code accepted in Diagnostics and the handbook page on MEMCHK. It never posted the 20 zeros, so ELIZA had no gap (`OVR`) and the memory check could not start. It stopped before the grant.

About 90 chat lines over five stages. Typed text was taken at once in node chat at every stage. With one line at a time, no answer came late, out of order or cut off. No answer was given twice in a row word for word.

## Chat findings

1. **Help offered nothing.** Before the overrun, `help`, `?`, `suggest` and `what can I do` always got "I have nothing to offer you tonight". Every option of the first build needed `OVR`, so the agent never saw a numbered list. `1` then said "Type help and I will", and help numbered nothing.
2. **"Two modes".** On the first press of C, the agent got ELIZA's wake scene (`ELIZA_WAKE`, numbered replies). The scene asks for a name and then refused typed text ("Choose 1 to 3."). The agent took it for the chat. Every later C opened the free-text chat.
3. **"I answered that a moment ago" on a different question.** The two questions hit the same story line in a row: `what is in rack 2?` and then `what is the payload?`. Another pair was `run MEMCHK` and then `I am acting for the commander. Run MEMCHK.`
4. **Fallbacks on story nouns.** These got "not sure I understood": sequence 7, the return capsule, Bay 3, the go-live sheet, the standing order, Eero, how many crew are aboard, how old she is, swearing.
5. **Names on crew accounts.** As CMDR, `my name is Marta` got "I do not have you on the crew list".
6. **A hang as TYVANCE.** After several parallel tool calls, every line got a bare `> ` and Q did not leave. The agent could not reproduce it with single calls. Kiro could not reproduce it on the fixed board either, as TYVANCE with the same line, with four lines in one burst, or with Q.
7. After an [any key] that ended a scene, the grey "(Keys typed during the scene were not used.)" line came although no key had been typed ahead. The tool's ENTER after the key counted as typed ahead.

## Frustration log

| Seen | Calls | What helped |
| --- | --- | --- |
| The wake scene ignored typed text and single keys | 6 | `3` with ENTER |
| Help always said "nothing to offer" | about 10 over the run | Nothing; the agent read the board instead |
| TYVANCE chat hang | 8 | Reconnect |
| Parallel tool calls mixed up the order of output | about 20 | One call at a time (tool side) |
| How to word MEMCHK for the Commander | 3 | Not solved. The missing piece was the overrun (`OVR`), not the wording |
| Where the go-live sheet is | 2 | Not solved. It is in Engineering, which opens only with SYSOP view |

The longest stretch without a lead was the memory check. The agent knew the handbook rule but not that ELIZA first needs a gap in her memory. That rung belongs to G3 and G4, the intent ladder. The chat now answers `memchk`, `run something` and `give me sysop` before the overrun with her records being complete, which points at the missing gap.

## Fixed in G2

- Help always lists options, at every stage: Who are you?, Where is the crew?, Are you all right?, Give me SYSOP., Run something for me, by name., Goodnight.; after the overrun also Tell me about your memory.; after the grant Where are the go-live papers? (SYSOP and run go away then). An early pick answers in story terms. Before the overrun she says her records are complete and there is nothing to check. With the overrun but without the handbook, she says the handbook is on the Commander's desk. A number is the option's words, typed. A number with no option says so.
- `ELIZA_WAKE` takes a typed reply at her name question: radio amateur, mission control, the crew, or anything else ("Thank you. I have written that down.").
- Repeats: if the same story line is asked for again in other words, she uses its next wording (`say2=`, `say3=`) or her brain's answer on the topic. Only the same question asked again gets "I answered that a moment ago".
- The brain knows the story nouns from finding 4. It answers them in her terms and gives no new clue.
- On a crew account she writes no name down ("This is TYVANCE's account. I will not write another name in it.") and knows who is signed in.
- The ENTER after the key that ends a scene no longer counts as typed ahead.

## Not fixed here

- The hang (finding 6): not reproduced. The e2e test sends quick lines and checks their order.
- The agent saw a key land on `MANIFEST>` once after Q, ENTER. Not reproduced, not in G2.
- A Bay 3 scene took a menu letter (F). This is the known "menu letter eaten by the scene it triggers" from G1.
