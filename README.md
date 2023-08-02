# osu-tdm

osu-tdm is a Windows application that sends a sound on missing a Tiny Droplet. This was created because osu!client doesn't report the misses of Tiny Droplets, only through implicit visuals with the accuracy meter, whereas this application communicates when a Tiny Droplet is missed with auditory communication.

## Usage

Open up `cmd.exe`, and go to the directory where `osu-tdm.exe`.

## Compiling

Before you can make the project, you will need `make` and `g++`/`x86_64-w64-mingw32-g++`/`i686-w64-mingw32-g++`. Once they have been installed, for the 64-bit version, write `make 64`, for the 32-bit version, write `make 32`, and a new executable should be created called `osu-tdm.exe`.

## Special Thanks

If it weren't for you're help, I would've died trying to get the right data using Cheat Engine by now.

- [UnnamedOrange / osu-memory](https://github.com/UnnamedOrange/osu-memory) For being the entry point and a massive help of my journey in learning more about how memory works and how to find them easily (though other projects did teach me in other aspects).

- [l3lackShark / gosumemory](https://github.com/l3lackShark/gosumemory) for providing a lot more memory signatures, a lot I experimented, and a lot didn't seemed to work (this could be the fault of mine though), I'm especially thankful for the Song Time memory signature.

- [OsuSync / OsuRTDataProvider](https://github.com/OsuSync/OsuRTDataProvider) for provided a lot of working memory signatures (compared to gosumemory) that I had fun playing with, I'm especially thankful for the Client Status memory signature.

- [Piotrekol / ProcessMemoryDataFinder](https://github.com/Piotrekol/ProcessMemoryDataFinder) for teaching me about getting data in an array, and providing a lot of other memory signatures that helped me when trying to solve a problem that I stopped using, which was getting the beatmap's full file/folder path in unicode, then using my own `libosu` to extract the last object's time (but I later thought it was overkill).

- [Azukee / osu-rx](https://github.com/Azukee/osu-rx) for providing the final memory signatures, that being the Last Object Time, which I'm really thankful (probably one of the hardest parts for me in the entire project when trying to find the data).

- and everyone else I forgot to mention!
