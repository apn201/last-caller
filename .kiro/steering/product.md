---
inclusion: always
---

# Product

Last Caller is a BBS aboard a ship whose crew is long dead. The SysOp is the
ship's AI, which took over the board. The caller works out each crew member's
fate from messages, files, logs and ghosts in WHO, then fills in a crew
manifest.

Two front ends, one core: the ESP32-S3 stick (offline WiFi AP) and the hosted
server build. It is built for a judge who has five minutes.

## Premise

- In 1998 a secret, privately funded spacecraft flew the first private
  science mission. Crew of four, preparing for a space hotel and tourist
  flights.
- The ship carried a BBS on a 486 so the crew could message friends and
  family on Earth. A mission AI (working name ELIZA) ran the mission, ran the
  BBS as SysOp, and kept the crew company.
- Mission control lost all contact. It never reached the news.
- 28 years later the ship's shorted batteries have gone open circuit, so the
  solar panels power the 486 whenever the ship is in sunlight (a "zombie
  satellite"). Radio amateurs found its signal on 11 Nov 2026.
- The caller is dialling into that BBS.
- The funder is unnamed and never resembles a real person.

## Layers

1. Content and identity (card files only).
2. The haunt: triggers, ghosts, glitches in the shared core.
3. The voice: the SysOp talks back, scripted first.

Scope and order are in `docs/SLICES.md`. If time runs out, ship the last
green layer.
