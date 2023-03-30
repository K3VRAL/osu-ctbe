# osu-pause

osu-pause is an application that pauses osu!.exe (while in game) just before the final object. This was created for offline players who want to submit their scores without being online, or having the playcount number get higher.

## Usage

Open up `cmd.exe`, and go to the directory where `osu-pause.exe`. Once there, write `.\osu-pause.exe PAUSE_KEY_HERE`, where `PAUSE_KEY_HERE` is the pause key you've set you're game to (use a letter).

## Installation

You can download the executable from the `Releases` tab in GitHub.

You can also compile the project as shown below.

## Compiling

Before you can make the project, you will need `make` and `g++`/`x86_64-w64-mingw32-g++`. Once they have been installed, write `make`, and a new executable should be created called `osu-pause.exe`.
