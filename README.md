# Forgotten Realms: Unlimited Adventures

This project is an **unfinished** source reconstruction of SSI's DOS version of *Forgotten Realms: Unlimited Adventures*. Most of the program has been recovered as Turbo C and assembly source, but the executable cannot yet be rebuilt from this repository, and it is not yet a byte-for-byte reconstruction. The missing piece is the linker SSI used: the right version of Pocket Soft's RTLink/Plus. See [Status](#status).

I am unable to continue this work. It is published in the hope that someone else can finish it.

## Read this first

- This source was reverse-engineered directly from the original game executable. It is **not** a clean-room reimplementation of the game, its engine or its mechanics.
- *Forgotten Realms: Unlimited Adventures*, *Forgotten Realms*, *Dungeons & Dragons*, Strategic Simulations, Inc. and all related names, trademarks, copyrights, artwork, writing, game data and other rights belong to their respective owners. I do not claim ownership of them. This project is not affiliated with or endorsed by those owners.
- This repository holds source code only. It includes no game executables and none of SSI's game data. You need to own a copy of *Forgotten Realms: Unlimited Adventures* to work with it. It is sold as part of *Forgotten Realms: The Archives — Collection Two* on [GOG](https://www.gog.com/en/game/forgotten_realms_the_archives_collection_two) and [Steam](https://store.steampowered.com/app/1882280/Forgotten_Realms_The_Archives__Collection_Two/).
- AI coding tools were used throughout the project: Anthropic's Claude Code and OpenAI Codex.
- This is a hobby and preservation project, provided as-is, without warranty or support. Please do not ask GOG, Steam, SNEG or the original rights holders for support with it.
- Because this source is derived from SSI's code, this repository does not make an open-source license grant, except where a file says otherwise (see [Third-party code](#third-party-code)). Making the source visible does not grant rights to the original game or its derived material. Anyone reusing or redistributing material from this repository is responsible for determining what they may legally use.

Laws concerning reverse engineering and derived works vary. You are responsible for complying with the laws and terms that apply to your copy of the game.

## What is here

The `v1.0` and `v1.2` tags hold the reconstruction of each DOS release; `main` holds version 1.2. The target is `CKIT.EXE` (version 1.2, 587,843 bytes, SHA-256 `eb64aa5e44b6afffcc634a3f39dea639386412e6bb50c1b1ddf06a1583237550`).

| Path | Contents |
|---|---|
| `src/c/` | 113 Turbo C source files, the bulk of the game |
| `src/asm/` | Hand-written assembly for Turbo Assembler, including the startup code and the Miles AIL sound library |
| `src/rtlink/` | Reviewed assembly for the RTLink/Plus overlay manager's startup routines |
| `src/generated/` | NASM source for every byte not yet recovered as C or TASM source, and `manifest.json` |

`src/generated/manifest.json` describes the whole executable as an ordered list of parts. For each part it records the file offset and size of its bytes in `CKIT.EXE` and the source and tool that produce them. For each C or TASM module it also records where the linker placed its segments (`place`) and the address of every external symbol it calls (`syms`). The executable's SHA-256 is recorded as `reference_sha256`.

## Status

By bytes of `CKIT.EXE`, the parts break down as:

| Produced by | Parts | Bytes | Share |
|---|---|---|---|
| Turbo C 2.01, from `src/c/` | 205 | 497,645 | 85% |
| Turbo Assembler 3.1, from `src/asm/` | 3 | 3,855 | 0.7% |
| NASM, from `src/generated/` and `src/rtlink/` | the rest | 86,343 | 15% |

Part of the NASM share is the MZ header, relocation tables and RTLink/Plus structures, which a linker writes itself. About 40 KB of it, though, is program code that has not yet been recovered as C or TASM source: chiefly `src/generated/resident-003179.asm`, `resident-01bb4b.asm` and most of overlay 27.

The game was linked with Pocket Soft's **RTLink/Plus**, an overlay linker: `CKIT.EXE` carries RTLink/Plus's overlay manager and a table of 28 sections that it loads on demand. Without the RTLink/Plus version SSI used, the compiled modules cannot be linked into the original layout. The disassembly project worked around this with its own host-side tools, which assembled the NASM parts and placed each compiled C and TASM module at the address the manifest records. Those tools are not part of this repository.

**The missing piece is the right version of RTLink/Plus.** These versions were tried, and none of them matches the overlay manager in `CKIT.EXE`:

- RTLink 2.03
- RTLink-Plus 4.00
- RTLink-Plus 4.14
- RTLink-Plus 6.10

Another version, most likely one from around the game's release, may match. With it, the remaining work is to write the link: the RTLink/Plus command or response file describing the sections, and a DOS build that compiles, assembles and links everything under DOSBox-X.

## Compiler and assembler

The compiler has been determined to be **Borland Turbo C 2.01**, run as `TCC.EXE` with its default `TURBOC.CFG`. Every C module is compiled with the same flags:

```
tcc -c -ml -O -Z -d <module>.c
```

- `-c` compiles without linking.
- `-ml` selects the large memory model (far code and far data).
- `-O` optimizes jumps.
- `-Z` suppresses redundant register loads.
- `-d` merges duplicate strings.

The assembly in `src/asm/` is assembled with **Turbo Assembler 3.1** (from Borland C++ 3.1) with `/ml` (case-sensitive symbols). `ail.asm` uses `/m /w+ /ml`: it needs TASM 3.1's multiple passes.

## Third-party code

`src/asm/ail.asm`, `ail.inc` and `ail.mac` are the IBM Audio Interface Library (AIL) by John Miles, Copyright (C) 1991, 1992 Miles Design, Inc., from his public AIL2 source release, with edits that make it the version 2.02 this game links. `src/asm/AIL2-README.md` carries that release's README, including John Miles' grant of May 26, 2000, which covers this source.

## Credits and Thanks

This work would not exist without SSI's original game and the people who have spent years documenting and exploring the Gold Box engine.

Special thanks to:

- [Gold Box Companion](https://gbc.zorbus.net/) for its character tools, automapping and years of practical Gold Box knowledge.
- [Gold Box Explorer](https://github.com/bsimser/Gold-Box-Explorer) for making the games' files and resources much easier to inspect and understand.
- Simeon Pilgrim's [CoAB project](https://github.com/simeonpilgrim/coab), an earlier reverse-engineering effort for *Curse of the Azure Bonds* that provided valuable reference work.
- John Miles, for releasing the Audio Interface Library's source.
- GOG and Steam for continuing to sell these games and giving new players a way to find them.
- SNEG for handling the rights work and negotiations that allowed the Gold Box games to return to sale.
