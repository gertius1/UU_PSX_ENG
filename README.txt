Ultima Underworld PSX Rev 1 English Patch V1.2 by Gertius

TOC
- Version history
- Project description
- List of files
- Installation
- State/Known bugs
- Acknowledgements
- Links


Version history
======================
V1.2 fixes a bug in the conversation with Shak on lvl 2, when asking him to repair items
V1.1 fixed bandits dialog on lvl 3 which led to crash, added missing mantras, small fixes of other strings
V1.0 initial release

Project description
======================
This is a ROMhack/patch to translate the Ultima Underworld PSX version from Japanese to English.
The text content form the original English PC version has been adapted and put into the PSX version.
This includes: cinematics (subbed), images/menus, game world descriptions, NPC conversations. 

The PSX version has some differences to the PC version, which makes it a worthwhile experience.
- Different cinematics
- Creature models are in 3D
- Anime portraits
- Control scheme for console controller
- Fullscreen 3D display
- No typing, all spells, mantras, replies can be selected by a menu
In addition to that, the PSX version can be played in emulators, which for example makes the game available on portable systems like 2DS/3DS.


List of files
======================
The ROMhack comes with these files:
- UU_patch.xdelta
- UU_PSX_Controls.jpg
- README.txt

Installation
======================
- The ROMhack is being delivered as an xdelta patch, to be applied to the .bin of the game with xdelta or xdeltaUI, 
available from romhacking.net.

It has been tested to work with the game ROM of the name 
"Ultima Underworld - The Stygian Abyss (Japan) (Rev 1) (Track 1).bin".
File/ROM SHA-1: 937ae8cf9a65526f2622c39d34205941f22c10cb
File/ROM CRC32: 08abec11

- only "Ultima Underworld - The Stygian Abyss (Japan) (Rev 1) (Track 1).bin" needs to be altered by the xdelta patch. Tracks 2,3,4 are music tracks and will not be affected by this patch. Also the cue sheet will not have to be altered.

State/Known Bugs
=======================
The game has been tested to be finishable with this English patch (following the Japanese speedrun on Youtube).
All the NPCs have been tested to work in a Debug environment and most have been tested in the final game.
Even though it has proven stable in many hours of testing, it cannot be ruled out that there still might be issues.
It is advised to save often. Please report crashes to the thread on romhacking.net given below.

Known bugs are that some character names exceed the graphics box and may write into the dialog strings.
Also, the linebreaks, while working nicely for readables and NPC dialog, do not work for shorter dynamic strings (put together at runtime), like item descriptions.
They may or may not wrap around ugly. This is also the case, when the player´s name is mentioned in a conversation (for best results of linebreaks choose player name with length of 4 characters).
This is expected behaviour and cannot be changed within the scope of this project.
Some skill names are too long and overlap with the skill value. This has been kept, to retain clarity regarding to the skill name.

For the most part, it was possible to transfer the text contents largely unchanged from the English version.
However, some strings had to be adapted for size reasons, either for ROMsize, or display size (Japanese strings are usually much shorter than English strings).
Most prominently, the Avatar´s replies in NPC conversiations have only 5 static lines with much fewer characters than in the PC version. 
That is why the Avatar´s replies had to be edited down quite severely.
Dynamic strings were often altered, because the sentence construction/word order works differently in Japanese, and it had to be adapted for that.


Acknowledgements
=========================
Thanks to Hank Morgan and his Underworld Exporter project. Without the text strings being available from there, this project would have not been possible.
To my understanding he has also built upon existing projects in certain aspects, so also big thanks to the projects Abysmal and Underworld Adventures!
Big thanks to the romhacking.net community!! Without the tools and knowledge available on that site, this project would have also never been possible.
Thanks to dandandan5 on YouTube for providing the speedrun of the Japanese PSX version.

THANKS!!!


Links
==========================
Patch download link for Rev 1 version of the Rom (recommended): 
http://www.zaqmusic.com/UU/UU_REV1_ENG_PATCH.zip

Manuals for DOS version (most of it applies for PSX version as well):
https://www.oldgames.sk/en/game/ultima-underworld-the-stygian-abyss/download/292/

Romhacking thread (general discussion and bug reports):
https://www.romhacking.net/forum/index.php?topic=37760.0

Romhacking release page (not up-to-date anymore because of closing of Romhacking.net):
https://www.romhacking.net/translations/7062/ 

YouTube demo video:
https://youtu.be/XuXlTk50Amk 

GitHub project (software tools created to make this ROMhack):
https://github.com/gertius1/UU_PSX_ENG
