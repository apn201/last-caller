# Judge Anna: notes (first session)

## 1. What I read, how far I played

Read: `docs/DEVPOST.md` (without the checklist), `docs/LAUNCH_KIT.md` section 3 (film plan). After playing: `README.md` (first 120 lines), the `.kiro/` folder listing, and the "How Kiro was used" part of the Devpost text.

Played: about 60 tool calls, roughly 20 to 25 minutes of reading, not the 40 I was allowed. New account (AnnaF). I did the power prologue, registered, read the bulletins, the Mission Log, part of the Lounge, the Library manual, the Diagnostics door (two queries), the manifest screen, node chat with ELIZA, and the gallery menu. I did not finish and did not fill in any manifest entry.

Why I stopped: I had seen the shape of the game and could not see how to make progress without guessing. I did not want to guess manifest answers, and I did not open Ty's gallery, Garden Notes or the Radio Room. A busy judge would stop about here too. I was not bored. I was unsure what to do next.

## 2. First paragraph of the Devpost page

I understood two things: it is a 1990s-style bulletin board running on a $10 stick and on a server, and it contains a horror game. The audience is "anyone who likes a puzzle story" and also people building on slow radio links. I did not know what a BBS is. The page says "feasibility study" and "product" in the same paragraph, and I could not tell which one I was judging. The Meshtastic argument is the only market case, and the page itself admits it is not built.

## 3. Notes while playing

- **Delighted:** the first screen. A boot screen with a service label showing the login, so I could not get stuck. I typed admin/admin, typed SOURCE RTG, pressed K, and the ship woke up. The 2-minute power puzzle was easy for me and felt like a real action. It teaches the interface in one minute.
- **Delighted:** the writing. The commander's log is dry and has numbers ("Cabin 21.0 per plan. Three complaints, one from a professor with a spreadsheet."). The professor's post about the Founder's memo, with an invented citation he admits is invented, made me laugh. The Founder's memos (all caps, "the paradigm is shifting and we are the shift") are an accurate joke about my own industry. "I have named the first suite after myself" is good.
- **Delighted:** small wrongness. ELIZA says "All well, crew 4", then the Diagnostics door answers "Crew aboard now: 0. I would like to be accurate about that." The WHO list shows TYVANCE idle for 28 years, on a node numbered 12. I did not have to be told.
- **Delighted:** the screens. Every screen is framed and has art. It looks designed, not generated.
- **Confusing:** the keyboard. I had to learn that Q backs out one level at a time, and that Enter versus a single key matters. Twice I sent keys to the wrong screen (I pressed `I` and `R` while still inside the Diagnostics door, and got "I have no query with that code"). A new caller in a browser would feel the same. There is no on-screen hint of "you are now in the door, type QUIT" except in small text.
- **Confusing:** the first "R" in message bases listed the Lounge, while the area line said Mission Log. I could not tell which area I was in for a moment.
- **Disappointing:** ELIZA's free chat. I asked "What happened to the crew?" and then "Are they alive? Diagnostics said crew aboard now is 0." Both got the identical reply, "The crew are well. They are resting." The numbered menu answers are good. The typed chat feels like a canned reply, and it does not react to what I said. That breaks the main promise of the pitch ("ask ELIZA").
- **Unclear goal:** the manifest asks me to choose between answers such as "Died in the power bay after cutting the ship's power" and "Left the ship in the return capsule". Choosing before I have read much feels like a guess, and the board says it will not tell me which row is wrong. I do not know how many hours it takes before I could answer. The Devpost claim of 60 to 120 minutes is plausible but I could not test it.
- **Good clue design:** the Diagnostics door says the codes are in a manual in the Library. I found the manual, read the codes, and got a real answer. That loop worked and I enjoyed it.
- The notebook already had entries I did not remember earning ("Her suit log asks for the lock again and again"). It is useful, but I did not see when it was filled.
- I did not test the browser. The pitch says the browser version and the stick are the product, and I only saw Telnet text. I cannot judge the music, the 300 baud overpaint or the stick.

## 4. Repository and `.kiro/`

- **README:** opens with 1998 marketing copy, which is fun, and then the facts. It is long and technical. It still describes things the Launch Kit says are stale: "Fifty modules from The Mod Archive", "Thirty HVSC classics", 1990s FidoNet history. The Devpost says no Mod Archive or HVSC tunes ship. A judge reads that as a mismatch between pitch and product, even if it is old text.
- **`.kiro/`:** 19 spec folders, 8 agents, 2 hook files, steering, settings. This is wide use of Kiro. Specs for the story and art, not only code, are unusual. The story-bible idea (every fact provable in four media) is a real design method and I would credit it.
- **Mismatch:** Devpost says 16 specs, 4 agents and 3 hooks (including story lint). The folder has 19 specs, 8 agents (two are judges and two are critics) and two hook files. I did not open them, so I cannot say whether the lint hook exists inside `verification.json`. The numbers on the page should match.
- **Honesty:** the Devpost is candid about what is not built (mesh sync) and about pre-existing work. I give credit for that. It still has `[fill: ...]` placeholders.
- **Film plan:** a sensible 3-minute plan with timings, and it spends 45 seconds on Kiro. As a plan it is strong. It is a plan: there is no video yet, and the plan assumes the stick, the box and footage that do not exist. I judge Presentation on what is there.

## 5. Provisional scores

**Potential Impact: 5/10.** The pitch has two audiences and neither is proven. The game is a real, finished-looking product with a place to play. The Meshtastic case is a direction and the page says it is not built. I would not invest on this, but I see a niche community (retro, BBS, radio amateurs) that would enjoy it.

**Technological Implementation: 8/10.** Specs, steering with a banned-words list, agents, hooks, an MCP server that lets Kiro play the game, property tests, headless queue runs. The use is deep and varied, and the evidence is in the repository. I take off points because the page counts do not match the folder, and because I saw only part of it. The game itself ran with no errors in my session.

**Design: 8/10.** Coherent, polished, intentional. The visual style and the voice are strong and consistent. The boot, login and power puzzle are a good first minute. I take off points for key handling that lost me twice, the unclear sense of progress, and ELIZA's typed chat repeating one line.

**Innovation/Idea: 8/10.** A game told through the structures of a bulletin board, with the dead ship's own AI as the host, is new to me. The satire of the founder is sharp. The Obra Dinn style manifest fits.

**Presentation: 5/10.** The Devpost text is long, personal and honest, but its first paragraph mixes a product, a study and a game. The video does not exist, only a plan. Stale README text and unfilled placeholders show an unfinished package. The first page does not say in one sentence what a BBS is, and a judge outside technology needs that.

TOTAL: 34/50
