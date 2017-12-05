What is SCSMD?
-------------
This is "Single Coil Switch Machine Driver", based on Arduino and designed to control accessories equipped with a single coil and controlled by polarity reversal.<br />
The SCSMD have two modes of operation - "DCC" and "MANUAL".<br />
For switching between modes, used the push button.<br />
Controlling in "MANUAL" mode carried out by toggle switches or by means of digital outputs from Arduino, for example with DCC++ command station (https://github.com/DccPlusPlus).<br />
For configure DCC address, SCSMD has a "LEARNING MODE".<br />

What do you need in addition?
-------------------------
In addition, you need to install libraries Bounce2 and DCC Decoder<br />
https://github.com/thomasfredericks/Bounce2<br />
https://github.com/MynaBay/DCC_Decoder<br />

How to configure
----------------
You do not need to configure anything in this time. This functional be added in future.<br />
For first time run, do reset procedure.<br />

How to reset decoder to default values
--------------------------------------
For reset decoder:<br />
* Press and hold "Reset" button on Arduino board.
* Press and hold SW1 push button on PCB.
* Release "Reset" button.
* After the controlling LED twice flashing - release SW1 push button.<br />

How to use
----------
In "MANUAL" operating mode, the switching is performed by supplying or disconnecting the low level (ground) to the corresponding control inputs.<br />
At the same time, the SCSMD ignores commands received from the DCC input.<br />
In the "DCC" operation mode, the switching is performed from the command received in DCC input to switch the corresponding output.<br />
In the "DCC" operating mode, SCSMD does not react to the signal level change at the inputs of the manual control.<br />
Also, in the “DCC” operating mode, the turnout positions are stored in the EEPROM and upon SCSMD power-up they switch to the position they were in the power turning off.<br />

How to switch operating mode
----------------------------
By default, the SCSMD operates in the DCC mode.<br />
In order to switch between the operating modes:<br />
* Press and hold SW1 push button more when 1,5 sec.
* The controlling LED start single flashing. It means that SCSMD switched to "LEARNING MODE".
* Within 15 seconds after switching on "LEARNING MODE", press and hold again more than 1.5 seconds the SW1 button. The LED will also stop blinking, indicating that the SCSMD has exited from "LEARNING MODE".<br />
The operating modes are switched in turn. If the mode was DCC, it will be switched to MANUAL and vice versa.<br />

How to set DCC address
----------------------
To set the decoder address, the SCSMD must be in the "LEARNING MODE".<br />
* Press and hold SW1 push button more when 1,5 sec.
* The controlling LED start single flashing. It means that SCSMD switched to "LEARNING MODE".
* After this, send any command from DCC command station to necessary accessory address. 
* The controlling LED stops flashing. It means that SCSMD stored a new decoder address.<br />
**_NOTE_**: SCSMD store the Decoder-Address, it's not the same as the Output-Address.<br />
For example:<br />
  if you send a command to Accessory address 5, SCSM store Decoder-Address 2, as well as if you send a command to Accessory address 6, 7 or 8.<br />
  In this case, the output #1 will have an Output-Address 5, the output #2 will have an Output-Address 6, the output #3 will have an Output-Address 7 and the output #4 will have an Output-Address 8.<br />
  More information about Decoder-Address and Output-Address see in NMRA Standards S-9.2.2, Chapters 350-365 on page 9. <br />
If within 30 seconds after entering the "LEARNING MODE", the SCSMD does not receive a command to switch the accessory, it will exit the "LEARNING MODE", the controlling LED will stop blinking, and the decoder address will remain the same.<br />
You can also exit from "LEARNING MODE" if, within 15 seconds after switching on "LEARNING MODE", press and hold again more than 1.5 seconds the SW1 button. The LED will also stop blinking, indicating that the SCSMD has exited the "LEARNING MODE".<br />

Change Log
----------
2017-11-24 version 1.0.1b<br />
 * mapping enable pin to PWM for future using
 * change control pin processing
 * extended turnout structure<br />
 
2017-11-26 version 1.0.2b<br />
 * added procedure for resetting settings
 * changed the procedure for switching between operating modes<br />
 
2017-12-05 version 1.0.2b<br />
 * fixed bug when switching between operating modes<br />
