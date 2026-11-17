# G2 - Juha's test, 17 Nov 2026 (public board; build not known, see item 1)

Written down by Claude Code from Juha's message. His words in quotes.

He played to the end. "Quite good. Could be longer, could have
'sidequests'. Ending is boring."

## Fix in G2 (small)

1. **Build on screen.** He could not tell whether he was testing G1 or G2.
   The browser's top bar (`HALCYON // CONNECT 28800/ARQ`) gets a dim build
   tag after it, for example `G2 2c450d4`: the slice and the short commit,
   put in at build time (PlatformIO and the server build), also in
   `/api/board`. Telnet: the same tag dim on the login front page. It is a
   tester's aid; one line in `SYSTEM.INI` (`build_tag=0`) hides it for a
   release.
2. **After the ending the board does not know the game is over.** Bulletins
   and Status still say "Antenna fault" (`BULLETINS/004_STATUS.ANS`). After
   END1 or END2 the status bulletin and the main-menu status show the
   aftermath (queue sent, downlink restored, or ELIZA stayed: END2), from the
   card, keyed on the end flags.
3. **Bulletins take a number without ENTER**, like every other short list
   (contract C1).
4. **Reset or delete my account.** In the caller's own settings: `R` reset my
   game (keeps the handle and password, wipes story flags, notebook,
   private story posts, ELIZA's memory of the caller; asks to type the handle
   to confirm) and `D` delete my account (asks to type the handle; frees the
   name). Never for crew accounts, the SysOp or the maintenance account. The
   ending tells the caller about both (see the end demo, step 8).

## A new slice: the end demo (Juha: "Most important is a cool ending")

"Oldskool end demo. When you release the queue, it starts the demo, with
music, animation, credits etc. Should be rather long, 1-3 minutes. Could show
snippets of the aftermath. News reports. Flashbacks. Images. Animation." Then
"CARRIER LOST. PRESS ANY KEY TO RECONNECT", then the board says you can still
explore, and to play again reset or make a new account.

Plan in `docs/briefs/Q23-Q24-SLICES.md`, slice **GE**.

## For later slices

- "Could be longer, could have sidequests": G3 design adds optional side
  threads (not on the main chain, each with its own small payoff), next to
  the new verbs of G5 to G7.
