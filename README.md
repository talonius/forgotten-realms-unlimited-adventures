# Forgotten Realms: Unlimited Adventures

This project is a source reconstruction of SSI's DOS version of *Forgotten Realms: Unlimited Adventures*. All of SSI's code and data has been recovered as Turbo C and Turbo Assembler source, and this repository **builds a working `CKIT.EXE`** under DOSBox-X with Turbo C 2.01, Turbo Assembler 3.1 and Pocket Soft's RTLink/Plus 4.14. It is not a byte-for-byte rebuild of the shipped executable: the RTLink/Plus version SSI used has never been found, so the overlay manager, and therefore the code addresses, are 4.14's. See [Status](#status).

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
| `src/c/` | 122 Turbo C source files: the game, and its data tables and text as data-only modules |
| `src/asm/` | Turbo Assembler source: the startup code, SSI's assembly library, the Miles AIL sound library, and the shipped RTLink/Plus run-time routines |
| `src/rtlink/` | Reviewed assembly for the shipped RTLink/Plus overlay manager's startup routines |
| `src/generated/` | NASM listings of the bytes a linker writes itself (the MZ header, relocation tables, alignment pads) and of the shipped overlay manager, plus `manifest.json` |
| `build/` | The DOS build: `BUILD.BAT`, the RTLink/Plus link file `CKIT.LNK`, and the generated DGROUP pieces |
| `build.sh` | Runs `build/BUILD.BAT` under DOSBox-X |

`src/generated/manifest.json` describes the whole shipped executable as an ordered list of parts. For each part it records the file offset and size of its bytes in `CKIT.EXE` and the source and tool that produce them. For each C or TASM module it also records where the linker placed its segments (`place`) and the address of every external symbol it calls (`syms`). The executable's SHA-256 is recorded as `reference_sha256`.

## Building

You need DOSBox-X and three DOS tools, each in its own directory:

- **Turbo C 2.01**: the directory holding `TCC.EXE`, with `INCLUDE\` beneath it.
- **Turbo Assembler 3.1**: the directory holding `TASM.EXE` (Borland C++ 3.1's `BIN`).
- **RTLink/Plus 4.14**: the directory holding `RTLINK.EXE` and its libraries, as its installer leaves it.

```
TC201=/path/to/tc201 TASM31=/path/to/bc31/bin RTLINK=/path/to/rtlink ./build.sh
```

The result is `OBJ/CKIT.EXE`, with `OBJ/CKIT.MAP` and the tools' output in `OBJ/BUILD.LOG`. Copy it over the `CKIT.EXE` of an installed copy of the game to run it.

`build/BUILD.BAT` compiles every C module as the original was compiled (below), with each module's `_DATA` and `_BSS` renamed after the module (`-zRD_<MODULE> -zDB_<MODULE>`), assembles the assembly with `/dRELINK`, and links with `build/CKIT.LNK`. Two things in that link are essential:

- **The overlay tree and `RELOAD FAR`.** `CKIT.LNK` reproduces the shipped executable's 27 overlay sections in the same tree of areas. FRUA's sections call into other sections of the same area (the design menu, the editor loop, the area editor and the module loader share one), so RTLink/Plus must reload a caller's section on return.
- **DGROUP exactly as shipped.** Some tables are indexed from 1, with their element 0 inside the previous object, and some code writes just outside its own variables; the program relies on what is next to what. Under `RELINK`, `startup.asm` includes `build/DGORDER.INC`, which declares every DGROUP segment in the shipped order, and the linker places segments in the order it first sees them. `build/DGDATA.ASM` (the alignment pads) and `build/DGBSS.ASM` (uninitialized storage no module names) supply the bytes no module owns.

The files in `build/` are generated by the disassembly project's tools from the shipped executable's section table and the module layout; they are committed so that the build needs no tools beyond the three above.

## Status

By bytes of the shipped `CKIT.EXE`, the source that reproduces each part is:

| Produced by | Parts | Bytes | Share |
|---|---|---|---|
| Turbo C 2.01, from `src/c/` | 223 | 520,221 | 88.5% |
| Turbo Assembler 3.1, from `src/asm/` | 28 | 16,582 | 2.8% |
| NASM, from `src/generated/` and `src/rtlink/` | the rest | 51,040 | 8.7% |

The NASM share holds no program code or data of SSI's: it is the MZ header, the relocation tables and alignment that a linker writes, and the shipped RTLink/Plus overlay manager.

The disassembly project's host-side tools assemble the NASM parts and place each compiled C and TASM module at the address the manifest records; that rebuild matches the shipped `CKIT.EXE` byte for byte, for version 1.0 and version 1.2. Those tools are not part of this repository.

The build in this repository links the same sources with **RTLink/Plus 4.14**. These versions were tried, and none of them matches the overlay manager in the shipped `CKIT.EXE`: RTLink 2.03, RTLink-Plus 4.00, RTLink-Plus 4.14 and RTLink-Plus 6.10. So the executable it produces is not the shipped one byte for byte. What was checked:

- Its DGROUP is the shipped DGROUP: every initialized byte is equal, apart from the segment word of each of the 1,029 far pointers.
- Driven from boot through the same keys (the title, the Rule Book copy protection, the design menu, game settings, the art gallery, the monster editor, the design list, the load-game prompt and the map editor on Dungeon 01), it shows pixel-identical screens to the shipped executable, for both versions.
- A character can be created, saved and added to a party, and *Heirs to Skull Crag* plays: its opening events, treasure, movement in the 3D view, the area map, the character sheet and camp.

The right RTLink/Plus version, if it turns up, should link the same objects into the shipped executable byte for byte.

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
