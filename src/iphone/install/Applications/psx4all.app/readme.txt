psx4iphone v0.2.0
Port By ZodTTD
Based on psx4all by Unai, Chui, Tinnus, and ZodTTD

Visit www.zodttd.com for the official homepage of psx4iphone. Chat, get support, and more at the ZodTTD forums!

psx4iphone is an unofficial Sony Playstation 1 emulator for the Apple iPhone and iPod Touch.

Changelog:
v0.2.0 -
- Compiled with saurik's toolchain
- Improved detection of when frameskip is allowed within a frame.
- Performance increases from recompiler work, frameskip, and the new toolchain.
- Runs most games around 30 FPS still.
v0.1.0 - 
- Public release
v0.0.9 -
- Added save state support!
- Fixed graphic bugs introduced in v0.0.8.
- Fixed a dynarec bug related to the graphics bug.
- Fixed memory card in-game saving.
- Fixed delete rom bug. Should delete .table files as well now.
- Default settings have autosave enabled, interlace disabled, frameskip disabled. Adjust as needed.
- Changed icon to kmacleod's icon. Thanks! :)
v0.0.8 -
- Changed the PSX CYCLE to 4.0 for a balance of performance and compatibility.
- Improved the dynarec code (better optimising for the MIPS zero register). Better performance.
- Changed default settings to mute sound and turn interlacing off.
- Included new icons and controller design by those at ipodtouchfans.com. Thanks everyone!
- Fixed memory card in-game saves. Better testing needed to confirm though.
v0.0.7 -
- Fixed a lot of compatibility issues due to an ISO caching bug. Games such as FF7 now work!
- Changed the default PSX CYCLE increment back to 2.5 for compatibility reasons (FF Tactics now works).
- Improved performance slightly.
- Added new icon donated by a friendly beta tester. (Author: Please contact ZodTTD if you would like to be in the readme credits).
v0.0.6 - 
- A quick but important bug fix that should improve compatibility even further.
v0.0.5 - 
- Added support for BIOS for compatibility. BIOS support on by default.
- Added support for ZNX compression.
- Fixed various dynarec bugs to improve compatibility.
- Switched to portrait mode for reasons of performance increase.
- Added fixed icon by rokrboy and toomuchcaffiene
v0.0.1 - Initial version

To install:
1. Unzip and extract psx4all.app to /Applications/ on your iPhone or iTouch.
2. You should now have a /Applications/psx4all.app/ directory with the supplied application files.
3. psx4iphone support the following game formats:
.ISO
.BIN ("RAW" rip from tools such as CDRDAO and CloneCD)
.IMG (such as from CloneCD)
.Z   (from PocketISO compressed BIN files)
.ZNX (from PocketISO compressed BIN files)
Currently NOT supported but will be soon:
.BZ
4. v0.0.5 and above allow for a BIOS to improve compatibility. 
The BIOS file scph1001.bin should be placed in /Applications/psx4all.app/
5. Games go in /var/root/Media/ROMs/PSX/
6. If that directory is not there, create it.
7. IMPORTANT! YOU MUST SET FILE PERMISSIONS OF THE FILE psx4all TO 755 OR YOU WILL GET A WHITE SCREEN WHEN TRYING TO RUN PSX4IPHONE!

To use:
1. Run psx4iphone and choose a ROM or goto Settings.
2. In Settings you can choose frameskip options, mute sound, and turn off the default interlacing.
3. After choosing a ROM, it will run. If a game is not compatible it will most likely crash to Springboard right about then.
4. Assuming the game format is correct and the game is compatible things will run now.
5. The default skin has L1 L2 MENU R2 R1 at the top. The bottom left has the dpad. The bottom right has the 4 PS1 hardbuttons in an 8way configuration so combos of the buttons can be used. The bottom middle has SELECT and START.
6. There is currently no support for save states. This will be fixed shortly. You CAN save via in-game saves to memory cards though!

Known issues:
1. Performance can be too slow. The dynarec is being worked on and performance should keep increasing.
2. Sound can be bad. This can be due to a few different issues, including poor performance.
3. Glitches in graphics can be due to some performance tweaks done by Unai of psx4all. Depending on reports from testers, they can be fixed.
