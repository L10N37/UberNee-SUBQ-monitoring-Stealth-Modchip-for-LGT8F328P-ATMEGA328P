╔╗░╔╦══╗╔═══╦═══╦═══╦╗░╔╦══╦═══╗░░░░░░░░░░░░░░░░░░░░░░░░░
║║░║║╔╗║║╔══╣╔═╗║╔═╗║║░║╠╣╠╣╔═╗║░░░░░░░░░░░░░░░░░░░░░░░░░
║║░║║╚╝╚╣╚══╣╚═╝║║░╚╣╚═╝║║║║╚═╝║░░░░░░░░░░░░░░░░░░░░░░░░░
║║░║║╔═╗║╔══╣╔╗╔╣║░╔╣╔═╗║║║║╔══╝░░░░░░░░░░░░░░░░░░░░░░░░░
║╚═╝║╚═╝║╚══╣║║╚╣╚═╝║║░║╠╣╠╣║░░░░░░░░░░░░░░░░░░░░░░░░░░░░
╚═══╩═══╩═══╩╝╚═╩═══╩╝░╚╩══╩╝░░░░░░░░░░░░░░░░░░░░░░░░░░░░

# UberChip

The Supreme PSX Modchip (C) 2021

LGT8F328P PU20/ PU22/ PU23 version (c) 2022

VajskiDs Consoles

**'The Black Screen of Injection Acception'**
**For 3.3v compatible Arduino / clones & PIC 12F675**

## Credit where it's Due

The delays used in the PIC ASM version were generated by 'PICLOOPS'
The WFCK Frequency was calculated with RTM Timer Calculator

## References for informations:

- http://www.psxdev.net/forum/viewtopic.php?t=1265&start=20
- https://www.obscuregamers.com/threads/playstation-scph-7502-pu-22-pcb-modchip-question-and-modchip-diagrams-request.1311/
- http://www.psxdev.net/forum/viewtopic.php?t=1266
- PM41 with BIOS patch for PAL consoles Demo: https://youtu.be/Ahy8XMkAvQc 

## Descriptions

An Open Source arduino "full stealth" modchip developed for each model of PS1. The PU18 Is assumed to be working

The chips include multi-disc support and initially started with a full serial monitor output which shows you exactly what's
happening, as well as acting like a built in debugger. There are of course stand-alone internal install versions with out a debugger.

No Audio CD delay- will boot straight to audio CD player

Thanks to all the other open source modchip writers and especially thanks to OldCrows old articles and for
releasing the first of the mighty PSX modchips! (though, I did write this from scratch before looking at other source code!)

A clone arduino nano will set you back a whopping 6.60AUD free post at the moment, please check compatible arduinos folder.
This will be updated when possible, but should work with many (if not all) 3.3v arduinos.

Optional reset: PIC leg 4 off MM3 diagrams to the RST on the arduino.
Without this, you can reset but you'd have to open the drive lid and close it again to get the game disc to boot again. 

## How it works

Precise timing of injections on each model and cutting them off in time for anti-mod discs not to detect them (Stealth).

For PU8/ PU18 the gate is tied low for entire duration of console power on. 
Later models require a WFCK frequency instead.

The chip only kicks back into the magic key injection routine when the drive lid is opened / closed again (for Multi-Disc games), or if it's reset (optional wire) or powered off/ on.

The timing isn't written in stone, but I'd suggest leaving it as is, as I've tested them thoroughly. Try and use high quality CD-R’s


## To do

- ATTINY85 version would be cool to write.
- May have identified a bug in PAL PSOne version, multidisc swap doesn't like BIOS patch routine. For arduino version, likely a separate injection routine to call, minus the BIOS patch after lid open/ close. For PIC version, simply not resetting the BIOS patch routine counters after the initial injection routine should fix it. The counters should just count backwards from 255, minus 1 per injection- after hitting zero. The counters hitting zero trigger the BIOS patch routine to kick in and to end.
- Uber-D-D (PM41 bios patch version) has being ported across in raw assembly for PIC12F675. 
It would be nice to port the others across as well, though the WFCK frequency can’t be generated on the PIC chips without an external 20Mhz clock. You might be able to drive the PIC off the reference clock in the clock gen circuit on later phat models, this should allow the 7.22khz WFCK output on the PIC 12F675 version of Uber-D-D

## Some notes from the listed source in relation to WFCK frequency on PIC version

Source: https://pic-microcontroller.com/pic12f675-pwm-code-and-proteus-simulation/
" Then PWM is initialized using InitPWM() function. After that, PWM variable is assigned a value of 127, which corresponds to a duty cycle of 50% (as shown in figure 1). You can change duty cycle of PWM by just changing the value of PWM variable.
You can change PWM frequency by changing the CPU frequency of PIC12F675 microcontroller. In other words, currently I am using internal oscillator of 4MHz value which generates a PWM frequency of 1.8KHz. But you can use external crystal of upto 20MHz value to generate a maximum PWM frequency of 9KHz. Also, you can change PWM frequency by changing the frequency Pre-Scalar of timer0 in the code."


##  PU22+ notes
A 'Genuine Mode' (Chip disable) originally existed for early revision of the chip. You would start the console with the Lid open, then close it a few seconds later to boot genuine games. This would disable the chip completely as the magic keys would collide and genuine discs would not orginally boot.
Chip disable mode was left in place up to V2.1J though, it would never be required. Later versions output the WFCK frequency required. Chip disable mode was removed on the LGT8F328P version (V2.1Jx)  & the Data pin moved from D12 to D7 on this version.

## 4.43361875 MHz Sub Carrier Output
July 2022: So after updating a version to work with the LGT8F328P I realised we can now output the PAL Sub Carrier Frequency on the PWM CLK pins on this chip.
The other nanos used prior had a limit of 4Mhz output due to the 16Mhz they ran at. 
These run @ 32Mhz so we should be able to output up to 8Mhz.
PAL Sub Carrier Target = 4.43361875 Mhz


This would be easy to get close to if you spent some time next to a scope with a tiny sample of code and a calculator.
This would be like the old crow chips (I have amassed a small collection) that have the colour burst crystal (not the oscillator, just the small xtal that need supporting
circuitry) and act as a 2 in one by forcing NTSC games to use PAL Sub Carrier (and play in colour on PAL machines) and by injecting magic key to get your CD-R's and Imports to run.

This would only be useful in PU8/ PU18 machines - The sub carrier is not step locked to the GPU frequency in later revisions, it is a separate rail altogether off the onboard clock synth. It already outputs a fixed sub carrier regardless of game region (so don't even ask me why people were doing the link wire hacks back in the day).
