## Bloodworks

 Bloodworks is top-down survival-action game.

You play as a soldier who tries to survive against endless waves of monsters he faces. He has many weapons at his arsenal, bonuses or perks that will help him in his fight.

The game is pretty much entirely moddable. Adding new weapons, bonuses, monsters, perks, missions or tweaking the existing ones are possible with a little bit of lua scripting (which you can do simply by editing text files!). There is also support for uploading or downloading game mods.

Bloodworks is still in development. Once it is finished, it will be completely free and open source. If you get a crash, the game will ask you to confirmation to send a crash report. It would greatly help me if you do so! Nothing personal is being sent to my servers, it is just console logs and any other information you provide in the crash report. You can also open a ticket or send feedback using my website if you prefer it.

If you want to contribute to the project or simply give feedback, my email address is (mail <at> enginmercan.com). Source code can be found in my Github Page if you are interested.

Controls

    WASD: Move Around
    Mouse: Aim
    Left Click: Shoot
    Right Click: Ultimate
    R: Reload
    P: Pause
    Tab: Open perk menu (if leveled up)
    F10 - Toggle full screen

You can find more information in readme.txt *cough*andcheats*cough*.

Controller support is there, but not heavily tested. It is on my todo list.

Enjoy!

## Sneak peak

<img src="https://raw.githubusercontent.com/shultays/bloodworks/master/0.gif" width="350">

<a href="https://www.youtube.com/watch?v=Fjxbh8k15z8">Gameplay video (YouTube)</>

## Documentation

Documentation for scripting part will be here.

## Features

- C++ game engine. 
- OpenGL for rendering. 
- SDL2/glew to make things easier and cross-platform.
- Lua for scripting. Heavy work is done on C++ side while gameplay is mostly on Lua.
- json reader of nlohmann (https://github.com/nlohmann/json)
- sol2 for C++ & Lua interaction (https://github.com/ThePhD/sol2/)
- soloud for sounds (http://sol.gfxile.net/soloud/)
- Art is mostly from http://opengameart.org/. Check credits for more information.

## License

Bloodworks is distributed with a GPLv3 License. You can see [LICENSE.md](https://raw.githubusercontent.com/shultays/bloodworks/master/LICENCE.md) for more information.
