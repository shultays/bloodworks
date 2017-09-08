Hola!

Bloodworks is top down 2D shooter that I have been working for almost a year now. It is heavily inspired by some of my favourite childhood games: "Crimsonland". I decided to work on a fun side project and that is how Bloodworks was born.

Name "Bloodworks" comes from one of my favourite games: "The Elder Scrolls IV: Oblivion". It is name of a place in the game (http://elderscrolls.wikia.com/wiki/Arena_Bloodworks) and I thought that it would be a cool name for a project.

Bloodworks uses a free software licence (GNU GENERAL PUBLIC LICENSE Version 3) and it will be always "free", both as in "free beer" and "free speech". The source code is always available on my github page and you are free to use as you wishes. I try to put latest Windows binaries on my github page too but sometimes they might be behind the source code. I tried to make it easy to port other platforms (I am using cross platform libraries) but there might be some bits of code only available for Windows. Once the game released, I will try to port it on other platforms.

I am always looking for help and feedback. I don't know how active I will be once the game is fully released but I am wishing that the people will create mods for it or contribute to it. You can contact me using my email adress (mail<at>enginmercan.com) if you want to help, to ask questions about the code base or to simply give feedback.

If you don't want to bother with git hub, you can always upload mods to my server which will be available for downloading. The system is not fully mature yet but it is functional. You can read modding_help.html to get some idea how scripting works and there are some binaries that come with the game to upload mods to my server.

Thanks for reading so far and playing my game! And again, please give some feedback. If you are feeling shy or want to remain anonymous, you can use this link to give feedback:
http://bloodworks.enginmercan.com/feedback.html

Engin Mercan


----------------------------------------------

Version Information:

0.8 - second public beta

Known issues:

There might be a problem with explosion particles, but I don't know If I was able to fix them or not. You can disable explosion particles by editing userconfig.txt if you get a performance drop off once things blow up.

My shader-fu is not so strong, the shaders may not work on all machines and likely to crash the game if they don't. I fixed some non-standard things by there are still some deprecated stuff I should fix.

If the game crashes by any means, it will ask you to confirm for sending a crash report. There is no personal information in this report, it only includes a stack trace and console logs and any other information you provide in your crash message. It would be very helpful if you send these reports to me! The game will not post any information without your confirmation first. 

If you don't want to send a report, please send me an email or open a ticket in github with details and attach stdout.txt along with it.

Cheats:

There are some cheats I implemented to make testing stuff easier. You can enable them by setting cheats to 1 in userconfig.txt (which is created once you run the game.). Here are some of the cheats that you can use while playing the game, some of them are more cheaty, some of them are for debugging:

n               toggle FPS
numpad +-       Increase/Decrease volume
1               toggle monster movement
2 + space       take all perks (it takes a single perk without space, but I change it all the time to test different perks)
3               test a bonus (again, this one is changed all the time)
4               spawn every bonus and weapon available
5               level upload
6               spawn random bonuses/weapons on the map
7               create blood for closest monster
8-9             create monster bits/parts for closest monster
0               clear bloods
z               slowdown game
x               remove slowdown
f1-f2-f3        some debug renderers
g               toggle invincibility
q               kill player
c               get an ultimate gun
insert          spawn a bunch of monsters
delete          kill all monsters
page up-down    increase/decrease difficulty
q + p + space   crashes the game (really)


