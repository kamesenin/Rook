# Rook v 0.0.11
*"This is the journey into sound, sound sculpture in space"*
##Rook - Audio plugin for Unreal 4
Rook is simple audio plugin for Unreal 4 Engine which usese OpenAL Soft for 3D sounds and Unreal 4 Audio Component for multichannel.
Project itself will be develop over time and main focus on:
- Usebility - a KISS method (Keep It Simple Stupid). So every new feature can be access via C++ or Blueprints (no coding skill requierd)
- Optimization - memory and CPU usage is very imporatant
- Nowdays features

##Quick Setup
If You want to start working with Rook just grab latest version and under Your project place it under Plugins Folder (ex. MyProject/Plugins) - if You don't have one just create it. Thats it. Rook is already configurated to be enabled by default. 
######!Imporatant
- Rook comes only with OpenAL Soft version 1.17.2 for Win **64bit** version. If You want to used it on Win32, Linux or Mac grab source from [OpenAL Soft git](http://repo.or.cz/w/openal-soft.git).
- Remember: if You want to make it work in **Shipped** version of Your project, You need to place OpenAL Soft DLL under: *UE4\Engine\Binaries\ThirdParty\OpenAL\1.17.2* and proper folder for Win64, Win32, Linux or Mac

##Documentation
Check Rook github Wiki. It's still work in progress and will be updated over time. Offline version will be included when all basic wiki entries will be finished.

##TODO
In implementation order:
- Check if possible to play multichannel with OpenAL Soft without virtualizing
- Mixer
- HDR

##ChangeLog
0.0.11
- Refactor codes base
- Fixed small bugs
- Made work for Unreal 4.21

##ChangeLog
0.0.10
- Refactor codes base
- Fixed memory leak on audio source
- Fixed Audio Listener on first play

0.0.9
- EAX volume boxes - now eax can by overlaping volume box
- fixed renewing audio playback when listener steps outside distance and returning. Now user can set if it should play outside audibility or not
- refactor a bit Rook Utils 

0.0.8
- Delegates: finsish, new loop, new sequence 

0.0.7
- Fade in and out for 2D and 3D audio
- bug fixes + fixing wrong functionality

0.0.61
- first documentation - WIP

0.0.6
- Checks if audio asset has proper number of channels
- Additional function which will choose best audio device based on channels number
- Rook DLL for non C++ users. Now You can work with Rook in blueprint project. 

0.0.59
BugFX version
- refactor code
- fixed issues with playing audio 

0.0.55
- Fixed listener orientation

## License
MIT
