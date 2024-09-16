# Zapper+2xControllers-Arduino-Adapter
NES Zapper+Controllers USB adapter, Arduino script and build instructions.

This NES Zapper USB adapter is meant to be used with a CRT display (CRTEmudriver for PC, RGBPi Scart hat for Pi4), the RetroArch's FCEumm core and ROMs modified with Alexey's delay patches: https://neslcdmod.com/roms/

This doesn't seem to work consistently with standalone FCEUX, seems to be related to emulation refresh mismatches causing inconsistencies, RA gets around this by adjusting either game speed or CRT refreshrate (CRTSwitchres) to match ~60.1Hz. If the photo-diode is replaced with one that isn't picky about the target light needing to strobe at ~15kHz it's possible to make the Zapper work with modern TVs as well.

Video of the Zapper adapter in action: https://i.imgur.com/D6HUQmy.mp4
The results are perfect and have been tested extensively, the zapper never misses the target it's shooting at, multi targets work without a hitch.

About the .ino script:
There's a hold function for the light sensor to smooth out <1 frame jittering, it holds the light output for a few ms as soon the Zapper diode senses light emitted from a CRT. The de-bounce also only applies to the trigger. There's also a failsafe that prevents the adapter from outputting a constantly held input when the Zapper is unplugged from the adapter. Making it possible to have the same adapter port work with NES controllers.  

In RetroArch you'll want to optimize lag down to 1 frame, with CRTEmudriver you'll want to follow the tutorial in the description of this video to set it up: https://youtu.be/Fdo5z1mQ748
configs for RA are included in there. If setting the configs yourself you'll likely want the video driver to be set to d3d11, ```video_max_swapchain_images = "2"```, ```video_swap_interval = "1"```, ```video_waitable_swapchains = "true"```, the goal is to bring down the input lag down to a single frame, without the use of runahead.

The Zapper outputs "r" for the trigger and "t" for the light sensor and is recognized as a keyboard, in RA you'll have to map ```input_player2_gun_trigger = "r"``` and ```input_player2_gun_aux_a = "t"``` (can also be set by editing the retroarch.cfg directly).
The device is at the same time recognized as two separate gamepad devices for converting regular NES controllers.

With the patched NES ROMs you'll set 1.0 frames of delay on the title screen, like shown in the video example. If the emulation input lag of your setup isn't optimized down to 1 frame you'll end up shooting the duck you weren't pointing at in Duck Hunt's Game B, or simply miss.
In the FCEumm core options the Zapper mode has to be set to "Sequential Targets", "Invert" enabled for both the "Zapper Trigger signal" and "Sensor Signal". Then in the RA Quick Menu Controls set the Port 2 controller device to Zapper.

Build instructions for the Arduino Pro Micro: ![Wiring Pro Micro](https://github.com/riggles1/Zapper-Arduino/assets/28061895/2d115be7-c98b-47f6-8111-9f38a2a70c5d)
When flashing the .ino to the Arduino the Joystick2.h and Keyboard.h libraries have to be installed first.

The part of the script that converts NES controllers was taken from here https://github.com/mcgurk/Arduino-USB-HID-RetroJoystickAdapter
The NES ports get recognized as their own controller devices in Windows and Linux. For Linux (or Raspberry Pi) in order for both NES controllers to get
recognized you'll have to edit cmdline.txt and add ```usbhid.quirks=0x2341:0x8037:0x040``` to the very end of the line, without adding more lines of script.
This is explained in detail in the above HID RetroJoystickAdapter Git.

For use on a Raspberry Pi4 with an RGBPi hat, use the RGBPi image together with the RGBPi-Extras toolset https://github.com/forkymcforkface/RGBPi-Extra
to get the NES Zapper functionality (as well as many other features). Everything there is preconfigured to work with this adapter, just install the Zapper package in Extras. (Works with Pi5, however as it has noisy GPIO, so wait for an HDMI DAC solution for it)







