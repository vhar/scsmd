What is SCSMD?
-------------
This is "Single Coil Switch Machine Driver", based on Arduino and designed to control accessories equipped with a single coil and controlled by polarity reversal
The SCSMD have two modes of operation - "DCC" and "MANUAL".
For switching between modes, used the push button.
Controlling in "MANUAL" mode carried out by toggle switches or by means of digital outputs from Arduino, for example with DCC++ command station (https://github.com/DccPlusPlus).
For configure DCC address, SCSMD has a "LEARNING MODE".

What do you need in addition?
-------------------------
In addition, you need to install libraries Bounce2 and DCC Decoder
https://github.com/thomasfredericks/Bounce2
https://github.com/MynaBay/DCC_Decoder

How to configure
----------------
If you make SCSMD as in electrical scheme (file smsmd.pdf in Documentation folder), then you do not need to configure anything.
Otherwise, you may need to make changes to the scsmd_config.h file.
The switching pulse time sets in the fifth structure element of each array of output and defaults set as 1 second.
More detailed information in the comments to the lines of the scsmd_config.h and  scsmd.h files.

How to use
----------
In "MANUAL" operating mode, the switching is performed by supplying or disconnecting the low level (ground) to the corresponding control inputs.
At the same time, the SCSMD ignores commands received from the DCC input.
In the "DCC" operation mode, the switching is performed from the command received in DCC input to switch the corresponding output.
In the "DCC" operating mode, SCSMD does not react to the signal level change at the inputs of the manual control.
Also, in the “DCC” operating mode, the turnout positions are stored in the EEPROM and upon SCSMD power-up they switch to the position they were in the power turning off 

How to switch operating mode
----------------------------
By default, the SCSMD operates in the DCC mode.
In order to switch the operating mode:
* Press and hold "Reset" button on Arduino board.
* Press and hold SW1 push button on SCSMD board.
* Release "Reset" button.
* Release SW1 push button.
The controlling LED flashes twice if operating mode switch to "MANUAL" and the controlling LED remains off or thrice if operating mode switch to "DCC" and the controlling LED will burn.

How to set DCC address
----------------------
To set the decoder address, the SCSMD must be in the DCC operating mode.
* Press and hold SW1 push button more when 1,5 sec.
* The controlling LED start single flashing. It means that SCSMD switched to Learning Mode.
* After this, send any command from DCC command station to necessary accessory address. 
* The controlling LED stops flashing. It means that SCSMD stored a new decoder address.
NOTE: SCSM store the Decoder-Address, it's not the same as the Output-Address.
For example:
  if you send a command to Accessory address 5, SCSM store Decoder-Address 2, as well as if you send a command to Accessory address 6, 7 or 8.
  In this case, the output #1 will have an Output-Address 5, the output #2 will have an Output-Address 6, the output #3 will have an Output-Address 7 and the output #4 will have an Output-Address 8.
  Mode information about Decoder-Address and Output-Address see in NMRA Standards S-9.2.2, Chapters 350-365 on page 9. 
If within 30 seconds after entering the "LEARNING MODE", the SCSMD does not receive a command to switch the accessory, it will exit the "LEARNING MODE", the controlling LED will stop blinking, and the decoder address will remain the same.
You can also exit from "LEARNING MODE" if, within 15 seconds after switching on "LEARNING MODE", press and hold again more than 1.5 seconds the SW1 button. The LED will also stop blinking, indicating that the SCSMD has exited the "LEARNING MODE".
