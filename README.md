# This repository has been migrated to the self-hosted ari-web Forgejo instance: <https://git.ari.lt/ari/badapplefetch>
# Bad Apple Fetch

> Bad apple... As Neofetch

As the creator of Neofetch has [picked up farming](https://github.com/dylanaraps/dylanaraps/commit/811599cc564418e242f23a11082299323e7f62f8) thought why not combing one of the most infamous memes on the internet, and one of the most influential scripts by Dylan Araps.

# Requirements

-   `make` (for compilation)
-   FFMpeg (for video and audio processing)
-   jp2a (for ASCII frames generation)
-   Most likely Linux, haven't tested it anywhere else
-   yt-dlp or youtube-dl (for downloading videos off YouTube, you can change that with `YT=...`)
-   LibMPV & Libpthread (if you have `LIBMPV=1`)
-   BASH

# Compiling

-   Make sure you have all the dependencies
-   If you want audio support, type `export LIBMPV=1`
-   If you want more or less FPS, type `export FPS=<FPS>` (by default 24)
-   If you want to use youtube-dl instead of yt-dlp, type `export YT=yt-dlp`
-   Now, compile! Type `make -j$(nproc --all)` (`nproc --all` will just turn into your core count)

So, basically:

    LIBMPV=1 make -j$(nproc --all)

Should be enough for most people. For more optimized flags see [CONTRIBUTING.md](/CONTRIBUTING.md).

# Forks

- "Skibidi Toilet... In a fetch script" by Zayd: https://git.inamatrix.xyz/zayd/skibidifetch

# Credits

-   The video used for audio and video: <https://www.youtube.com/watch?v=FtutLA63Cp8>
-   Logos mainly extracted from: <https://github.com/dylanaraps/pfetch>

# Logo

-   If you want a specific logo to show up, supply it in the command line of BadAppleFetch, such as `./badapplefetch ubuntu`
-   If you want to add a logo, please see [CONTRIBUTING.md](/CONTRIBUTING.md)

# Screenshot

![Screenshot of BadAppleFetch](/screenshot.jpg)

Live demonstration: <https://www.youtube.com/watch?v=C9wIdshX__g>
