**OutDated**

Instead of having to submit the score by pausing right before the end of the map, then turning the internet on, and submitting, you can buffer the submission by hitting the results screen, connecting to the internet, then pressing the further results in the bottom of the screen. Also, certain memory locations don't work anymore.

# osu-pause

osu-pause is a Windows application that pauses osu!.exe (while in game) just before the final object. This was created for offline players who want to submit their scores without being online, or having the playcount number get higher.

## Usage

Open up `cmd.exe`, and go to the directory where `osu-pause.exe`. Once there, write `.\osu-pause.exe PAUSE_KEY_HERE`, where `PAUSE_KEY_HERE` is the pause key you've set you're game to (use a letter).

## Installation

You can download the executable from the [Releases](https://github.com/K3VRAL/osu-pause/releases) tab in GitHub.

You can also compile the project as shown below.

## Compiling

Before you can make the project, you will need `make` and `g++`/`x86_64-w64-mingw32-g++`/`i686-w64-mingw32-g++`. Once they have been installed, for the 64-bit version, write `make 64`, for the 32-bit version, write `make 32`, and a new executable should be created called `osu-pause.exe`.

## Special Thanks

If it weren't for you're help, I would've died trying to get the right data using Cheat Engine by now.

- [UnnamedOrange / osu-memory](https://github.com/UnnamedOrange/osu-memory) For being the entry point and a massive help of my journey in learning more about how memory works and how to find them easily (though other projects did teach me in other aspects).

- [l3lackShark / gosumemory](https://github.com/l3lackShark/gosumemory) for providing a lot more memory signatures, a lot I experimented, and a lot didn't seemed to work (this could be the fault of mine though), I'm especially thankful for the Song Time memory signature.

- [OsuSync / OsuRTDataProvider](https://github.com/OsuSync/OsuRTDataProvider) for provided a lot of working memory signatures (compared to gosumemory) that I had fun playing with, I'm especially thankful for the Client Status memory signature.

- [Piotrekol / ProcessMemoryDataFinder](https://github.com/Piotrekol/ProcessMemoryDataFinder) for teaching me about getting data in an array, and providing a lot of other memory signatures that helped me when trying to solve a problem that I stopped using, which was getting the beatmap's full file/folder path in unicode, then using my own `libosu` to extract the last object's time (but I later thought it was overkill).

- [Azukee / osu-rx](https://github.com/Azukee/osu-rx) for providing the final memory signatures, that being the Last Object Time, which I'm really thankful (probably one of the hardest parts for me in the entire project when trying to find the data).

- and everyone else I forgot to mention!
