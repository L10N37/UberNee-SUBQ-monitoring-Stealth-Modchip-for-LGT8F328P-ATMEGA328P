# tehUberChip_Another_PSX_Modchip
Another PSX modchip made in 2021 for arduino

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





========================================================================
Optional reset: connection 4 off MM3 diagrams to the RST on the arduino.
Without this, you can reset but you'd have to open the drive lid and close it again to get the game disc to boot again.

Just checked this connection and it has a logic high of 3.xxV and gets pulled low on reset which should trigger the arduinos reset
pin simulataneously without an issue.
