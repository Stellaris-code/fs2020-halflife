# fs2020-halflife: Running Half-Life in Microsoft Simulator 2020




https://github.com/Stellaris-code/fs2020-halflife/assets/8571612/e2cec3d6-7f64-4636-9104-27d5193279dd


## Instructions

Build the VS project and copy the built .wasm file to the GaugeAircraft SDK sample.

Copy the valve/ folder from your (legal!) Half-Life install to the Packages/<package_name>/work directory. 

e.g.: this folder is located in C:\Program Files (x86)\Steam\steamapps\common\Half-Life\valve in the case of the Steam version of the game.

Controls are as follows:

* Mouseclicks and keyboard inputs are unchanged for the most part (e.g. moving around can be done with the WASD keys) 
* Hover the cursor over the gauge to use the mouse
* Numpad 7 : escape
* Numpad 5 : open console

NOTE: it is advised to use a custom input profile so that input presses are not mapped to actions in the simulator.

Controls are defined in the code and are "easily" remapped through modifiying the WasmXash3D\engine\platform\stm32\events.cpp file.

All game features are working (such as save files), with the exception of sound and music.
Disclaimer: a few graphic glitches are present and the game occasionally experiences freezes.

The rendering resolution can be adjusted by changing the gauge resolution in the corresponding panel.cfg file.

The code is based upon my port of Xash3D to the STM32H747I-DISCOVERY platform. (https://github.com/Stellaris-code/stm32h747i-xash3d).
