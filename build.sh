#!/usr/bin/env bash
# build.sh -- build CKIT.EXE from this repository under DOSBox-X.
#
#   TC201=<dir> TASM31=<dir> RTLINK=<dir> ./build.sh
#
#   TC201   Turbo C 2.01: the directory holding TCC.EXE, with INCLUDE\ beneath it
#   TASM31  Turbo Assembler 3.1: the directory holding TASM.EXE (Borland C++ 3.1's BIN)
#   RTLINK  RTLink/Plus 4.14: the directory holding RTLINK.EXE and its libraries
#
# The output is OBJ/CKIT.EXE (and OBJ/CKIT.MAP, OBJ/BUILD.LOG). Exit 0 when CKIT.EXE was linked.
set -euo pipefail
root=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
for v in TC201 TASM31 RTLINK; do
  [ -n "${!v:-}" ] && [ -d "${!v}" ] || { echo "build.sh: set $v to a directory (see the header)" >&2; exit 2; }
done
[ -f "$TC201/TCC.EXE" ] || { echo "build.sh: no TCC.EXE in $TC201" >&2; exit 2; }
[ -f "$TASM31/TASM.EXE" ] || { echo "build.sh: no TASM.EXE in $TASM31" >&2; exit 2; }
[ -f "$RTLINK/RTLINK.EXE" ] || { echo "build.sh: no RTLINK.EXE in $RTLINK" >&2; exit 2; }
command -v dosbox-x >/dev/null || { echo "build.sh: dosbox-x is not on PATH" >&2; exit 2; }
mkdir -p "$root/OBJ"; rm -f "$root/OBJ/CKIT.EXE"
# Headless: no window, no quit dialog (a killed run would otherwise hang on it).
env -u DISPLAY -u WAYLAND_DISPLAY SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
  dosbox-x -silent -exit -noautoexec -set "dosbox quit warning=false" -set "cpu cycles=max" \
    -c "mount c \"$root\"" -c "mount d \"$TC201\"" -c "mount e \"$TASM31\"" -c "mount f \"$RTLINK\"" \
    -c 'path d:\;e:\;f:\' -c 'set TMP=c:\obj' -c 'c:' -c '\build\build.bat' -c 'exit' \
    >/dev/null 2>&1
log="$root/OBJ/BUILD.LOG"
tr -d '\r' < "$log" | grep -E "^(Error |Fatal |\*\*Error|\*\*Fatal|error |UNDEFINED|BUILD FAILED)" || true
if [ -f "$root/OBJ/CKIT.EXE" ]; then
  echo "built OBJ/CKIT.EXE ($(stat -c %s "$root/OBJ/CKIT.EXE") bytes); log: OBJ/BUILD.LOG"
else
  echo "build failed; see OBJ/BUILD.LOG" >&2; exit 1
fi
