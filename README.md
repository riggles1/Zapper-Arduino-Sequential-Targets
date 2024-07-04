# Zapper-Arduino
NES Zapper USB adapter, Arduino script and build instructions.

This NES Zapper USB adapter is meant to be used with a CRT (CRTEmudriver for PC, RGBScart hat for Pi4+Pi5), RetroArch's FCEumm core and modified ROMs using Alexey's delay patches: https://neslcdmod.com/roms/
This doesn't seem to work well with standalone FCEUX, seems to be related to emulation refresh mismatches causing inconsistencies, RA gets around this by adjusting either game speed or CRT refreshrate (CRTSwitchres) to match ~60.1Hz. Without modifying the Zapper itself with a new photo diode it won't react to light coming from anything but ~15kHz CRT's.

The .ino script has a hold function for the light sensor to smooth out <1 frame jittering so it holds the light output for a few ms as the Zapper diode senses light emitted from a CRT. The Trigger has an anti-bounce
and there's a failsafe that prevents the adapter from outputting a constantly held trigger if the Zapper is unplugged from the adapter. Making it possible to have the adapter port work with regular NES controllers as well.  

In RetroArch you'll want to optimize lag down to 1 frame, with CRTEmudriver you'll want to follow this tutorial to set it up: https://youtu.be/Fdo5z1mQ748
Configs for RA are included there, but in short you'll likely want d3d11, video_max_swapchain_images = "2", video_swap_interval = "1", video_waitable_swapchains = "true", the goal is to bring down the input lag down to a single frame, without the use of runahead.

The Zapper outputs "r" for the trigger and "t" for the light sensor and is recognized as a keyboard, in RA you'll have to map input_player2_gun_trigger = "r" and input_player2_gun_aux_a = "t" (can also be set by editing the retroarch.cfg directly).

With the patched NES ROMs you'll set 1.0 frames of delay on the title screen, like shown in this example: (https://i.imgur.com/D6HUQmy.mp4) 
In the FCEumm core options you'll want to set the Zapper mode to "Sequential Targets", "Invert" enabled for both the "Zapper Trigger signal" and "Sensor Signal". Then in the RA Quick Menu Controls set the Port 2 controller device to Zapper.

Build instructions for the Arduino Pro Micro: ![Wiring Pro Micro](https://github.com/riggles1/Zapper-Arduino/assets/28061895/e2a1ea58-799b-46c6-9e81-b1e442ca34de)



