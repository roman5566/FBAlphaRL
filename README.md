FB Alpha RL (Retro Loader) [by CaptainCPS-X, 2013]
----------------------------------------------------

![Image Alt](http://i1341.photobucket.com/albums/o751/CaptainCPS-X/ICON0_zps2c5f3318.png)
![Image Alt](http://i1341.photobucket.com/albums/o751/CaptainCPS-X/PIC1_small_zps511df5d7.png)
![Image Alt](http://i1341.photobucket.com/albums/o751/CaptainCPS-X/RetroArchPS3_3_zpse94767dd.png)
![Image Alt](http://i1341.photobucket.com/albums/o751/CaptainCPS-X/RetroArchPS3_4_zps2b722e7d.png)

Information:
----------------------------------------------------

As a dev from the official FB Alpha Team, 
FB Alpha RL was made with a objective in mind, to
keep the original experience from the official
FB Alpha (for Windows OS) on the PS3 system.

Even when I don't know how to implement a fancy
GUI for the PS3 system with textures and all the 
complicated stuff, I managed to use my previous 
application core (gamePKG core) to create this 
nice frontend for RetroArch's FB Alpha core.

I know that RetroArch is already a frontend, but
I like to keep things simple and straight to the 
point.

Please, read the technical info for more details.

Technical Info:
----------------------------------------------------

Signed for CFW 3.40 / 4.XX (should work for everyone).
Tested @ 1080p resolution via HDMI.

FB Alpha RL uses a custom "burn_drivers.h" header file
that was generated by modifying the official windows
version of FB Alpha with a routine to generate such
file on launch.

The header have a big structure with all the information
for all supported drivers in FB Alpha v0.2.97.28.
Information such as: 

- Romset Name (Ex. kof2002)
- Parent ROM (Ex. sf2)
- Year
- Manufacturer (Ex. Capcom)
- System (Ex. Neo Geo)
- Max players
- Game Screen Resolution
- Game Screen Aspect Ratio

Future release will have a custom generated header with 
all the ROM information as well, so the application will 
be able to verify every romset with precise CRC32 checks 
(as in the official FB Alpha for Windows).

FB Alpha RL will scan the following directories
for ZIP files:

Specific directories:

- /dev_hdd0/game/ROMS00000/USRDIR
- /dev_hdd0/game/ROMS00000/USRDIR/fba
- /dev_hdd0/game/SSNE10000/USRDIR/cores/roms

Generic directories:

- /dev_*/roms
- /dev_*/roms/fba

Devices scanned are:

- dev_hdd0 (PS3 Internal HDD)
- dev_usb000 (External USB)
- dev_usb001 (External USB)
- dev_usb002 (External USB)
- dev_usb003 (External USB)
- dev_cf (Compact Flash)
- dev_sd (SD Card)
- dev_ms (Memory Stick)

NOTE: You don't need the official RetroArch installed,
this already has my own modified build incorporated
into FB Alpha RL installation directory. So you can safely
install the official RetroArch and it won't interfere.

Usage:
----------------------------------------------------

- Use UP and DOWN to select Games.
- Press (X) to Load the selected Game.
- Press (O) to Quit and return to XMB.
- Press [SELECT] to Load "RetroArch" (FB Alpha Core).

Features:
----------------------------------------------------

- Auto configure screen Aspect Ratio on game load.
- Quick ROM(s) scanning in extensive path locations.
- Romset validation against FB Alpha v0.2.97.28 drivers library.
- Checking for duplicate ROM(s).
- Detailed info about listed ROM(s).
- Full game name display for easier navigation.
- Direct loading of ROM(s).
- Simple GUI & nice application logos(LMAO xD).

WIP / Coming on future releases:
----------------------------------------------------

- Set System Input preset on Game Load (Ex. NeoGeo.cfg / Shooters.cfg / etc).
- Simple Input Mapping feature.
- Display Extended ROM information by pressing (/\).

Changelog:
----------------------------------------------------

1.00 (January 30, 2013)

- Initial Release

Thanks to:
----------------------------------------------------
 - My buddies Treble Winner (Barry) / Kev / IQ_132 from the original FB Alpha Team <3.
 - PS3 Scene websites (in no order) such as: PS3HAX / PSX-Scene / PS3Crunch / Etc, for you support 
 - TheMaister / Twinaphex / Squarepusher / Company - Libretro/RetroArch is just amazing.
 - pete_uk - for your unofficial RetroArch builds.
 - STLcardsWS - for your support :)
 - aldostools - your ps3tools for windows are the best =)
 - deank - multiMAN helped me research / debug my application
 - fail0verfl0w, GeoHot, etc - for all you have done for the PS3 scene  
 - Annonymous - for releasing Sony SDKs
 
 Thanks to anyone else I forgot that in some way contributed
 to the PS3 scene and as a result I was able to do this 
 homebrew application.

Downloads:
----------------------------------------------------

FB_Alpha_RL_1.00_[20130130].pkg [~8.5 MB]
-[mediafire] http://www.mediafire.com/?lqqdg1i317ucqiz
-[zippyshare] http://www34.zippyshare.com/v/51407224/file.html
-[mega] https://mega.co.nz/#!Ad92FRJA!M2fB1RgrV7ZSpoe7YURowD7xKyozuCJYcPW_KA1uuCk

Source Code:
----------------------------------------------------

-FB Alpha RL - https://github.com/CaptainCPS/FBAlphaRL
-RetroArch (Modded for FB Alpha RL) - https://github.com/CaptainCPS/RetroArch (atm is just a fork of official RetroArch, but tonight I will commit my changes, so they are easier to track down :))
-FB Alpha v0.2.97.28 (Modded to generate "burn_drivers.h")(Windows) - (coming soon, Im organizing it now)
