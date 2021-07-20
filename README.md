# tehUberChip_Another_PSX_Modchip
Another PSX modchip made in 2021 for arduino

PU18 = done, should cover launch model 1000 series as well - in full stealth - well, I would assume anyway, - when some lasers arrive I will try one out because launch
models that don't have busted lasers are basically non existent as they all broke about 20 years ago. It won't be staying in that sucker though cos Xstation baby!

PU22 = in dev, semi working with old crow link wire method - fussy on what games it wants, rewrote injection method, spaced out string delays to 120 (didn't capture that on the logic analyser, usually around + ~4-5m. Worked out a neat bitmask comparator method (well, technically it's straight off the arduino bitmask example) for streaming bits from bytes so could store the string in hex and convert on the fly, but haven't worked out the timing aspect - at the end of the day neat code is pretty to look at but functionality wise makes zero difference. Plus being a noob it's more research to understand (i.e 2 lines of complex code could be easily understood in 6 lines of noobish code.)

References: !!!

Pretty much just this 

"When outputting a '1', the pin is simply pulled low
When outputting a '0', the pin emits the WFCK signal (same phase as the reference)

Unlike the modes used on the older boards, the output pin is low-Z for the entire message.

Also note that I'm using '1' to mean a high bit in the SCEx message - I.E. the state that corresponds to the DATA pin being pulled low on the older modchips.

[from: http://www.psxdev.net/forum/viewtopic.php?t=1265&start=20]

& Interesting information here [https://www.obscuregamers.com/threads/playstation-scph-7502-pu-22-pcb-modchip-question-and-modchip-diagrams-request.1311/]
            + chips i removed with the link where i was like "huh?"
            

Next: Take that link as an input and pass it out as an output temporarily so we can go full stealth on this b*tch like the chip was never there....!
This should also cover PU23 and then onto BIOS patching and PSone method. Interim release posted for anyone interested.




UberChip - by Vajskids Consoles 2021

An Open Source arduino full stealth modchip developed on an SCPH5502/ PU18 with an Arduino Nano. 
Multi-disc support and a full serial monitor output which shows you exactly what's
happening as well as acting like a built in debugger.

No Audio CD delay- will boot straight to audio CD player

Currently only PAL string bits are included, but this is easily changed. 
Thanks to all the other open source modchip writers and especially thanks to OldCrows old articles and for
releasing the first of the mighty PSX modchips!

For stand-alone version (internal installations, no debug/ serial monitor) run a link from D4 to 3.3v pin

A clone arduino nano will set you back a whopping 6.60AUD free post at the moment.
Flash with arduino IDE with old bootloader (if clone) and CH340 drivers (if clone)
picture attached

PU18 diagrams done
...For now if you want to use other strings (my consoles are PAL, so cbf) i suggest using find and replace in notepad or similar
to switch out the bit strings with the delays or re-write it in a fancier manner (the injection method)





Optional reset: connection 4 off MM3 diagrams to the RST on the arduino.
Without this, you can reset but you'd have to open the drive lid and close it again to get the game disc to boot again.

Just checked this connection and it has a logic high of 3.xxV and gets pulled low on reset which should trigger the arduinos reset
pin simultaneously without an issue.
