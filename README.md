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

Configuration Variables (CV)
----------------------------
  * **514** - Output control and inverse mode<br />
          The lower 4 bytes define the output control mode. 0 = DCC, 1 = MANUAL.<br />
          The upper 4 bytes define the inverse output polarity. 0 = NORMAL, 1 = INVERSE.<br />
          Default 0<br />
  * **515** - Output #1 pulse time. X*100ms.<br />
          Default 10 (1000ms)<br />
  * **516** - Output #2 pulse time. X*100ms.<br />
        Default 10 (1000ms)<br />
  * **517** - Output #3 pulse time. X*100ms.<br />
          Default 10 (1000ms)<br />
  * **518** - Output #4 pulse time. X*100ms.<br />
          Default 10 (1000ms)<br />
  * **555** - Output #1 Address (LSB).<br />
          Contains the low-order address bits for Output #1<br />
          Default 1<br />
  * **556** - Output #2 Address (LSB).<br />
          Contains the low-order address bits for Output #2<br />
          Default 2<br />
  * **557** - Output #3 Address (LSB).<br />
          Contains the low-order address bits for Output #3<br />
          Default 3<br />
  * **558** - Output #4 Address (LSB).<br />
          Contains the low-order address bits for Output #4<br />
          Default 4<br />
  * **563** - Output #1 Address (MSB).<br />
          Contains the high-order address bits for Output #1<br />
          Default 0<br />
  * **564** - Output #2 Address (MSB).<br />
          Contains the high-order address bits for Output #2<br />
          Default 0<br />
  * **565** - Output #3 Address (MSB).<br />
          Contains the high-order address bits for Output #3<br />
          Default 0<br />
  * **566** - Output #4 Address (MSB).<br />
          Contains the high-order address bits for Output #4<br />
          Default 0<br />
The values contained in CV555 and CV563 correspond to the bits in the Output #1 packets as follows: <br />
Accessory-Output = (CV555 + (CV563 *256)) - 1<br />
**_NOTE_**:Please, don't configure CV555-558 & CV563-566. Use LEARNING mode, for set the Output Address<br />

How to configure
----------------
For first time run, do reset procedure.<br />
To configure the CV, the serial port is used.<br />
To change the value of the CV, send the appropriate command via the port monitor.<br />
Available command:<br />
  * **&lt;R CV&gt;**
    reads a Configuration Variable from the decoder<br />
    **CV**: the number of the Configuration Variable memory location in the decoder to read from<br />
    **returns**: &lt;r CV VALUE&gt; where VALUE is a number from 0-255 as read from the requested CV<br />
  * **&lt;W CV VALUE&gt;**<br />
    writes, without any verification, a Configuration Variable to the decoder<br />
    **CV**: the number of the Configuration Variable memory location in the decoder to write to<br />
    **VALUE**: the value to be written to the Configuration Variable<br />
    **returns**: &lt;r CV VALUE&gt;<br />
  * **&lt;B CV BIT VALUE&gt;**<br />
    writes, without any verification, a single bit within a Configuration Variable to the decoder<br />
    **CV**: the number of the Configuration Variable memory location in the decoder to write to<br />
    **BIT**: the bit number of the Configurarion Variable regsiter to write (0-7)<br />
    **VALUE**: the value of the bit to be written (0-1)<br />
    **returns**: &lt;r CV BIT VALUE&gt;<br />

How to reset decoder to default values
--------------------------------------
For reset decoder:<br />
* Press and hold "Reset" button on Arduino board.
* Press and hold SW1 push button on PCB.
* Release "Reset" button.
* After the controlling LED twice flashing - release SW1 push button.<br />

How to use
----------
If output set in "MANUAL" operating mode, the switching is performed by supplying or disconnecting the low level (ground) to the corresponding control inputs.<br />
At the same time, SCSMD ignores commands received for the Output-Address if this output set to "MANUAL" operating mode.<br />
If output set the "DCC" operation mode, the switching is performed from the command received in DCC input to switch the corresponding output.<br />
At the same time, SCSMD does not react to the signal level change at the input of the manual control for this output.<br />
Also, in the “DCC” operating mode, the turnout positions are stored in the EEPROM and upon SCSMD power-up they switch to the position they were in the power turning off.<br />

How to switch operating mode
----------------------------
By default, the all outputs operates in the DCC mode.<br />
In order to switch between the operating modes:<br />
* Press and hold SW1 push button more when 1,5 sec.
* The controlling LED start single flashing. It means that SCSMD switched to "LEARNING MODE" for output #1.
* To select the "LEARNING MODE" for output #2, briefly press the SW1 button. The controlling LED will flash twice.
* For the third output - press the SW1 button again, the controlling LED will flash three times, etc.
* Within 15 seconds after switching on "LEARNING MODE" for the desired output, press and hold again more than 1.5 seconds the SW1 button. The LED will also stop blinking, indicating that the SCSMD has exited from "LEARNING MODE".<br />
The operating modes for selected output are switched in turn. If the mode was DCC, it will be switched to MANUAL and vice versa.<br />

How to set DCC address
----------------------
To set the Output address, the SCSMD must be in the "LEARNING MODE".<br />
* Press and hold SW1 push button more when 1,5 sec.
* The controlling LED start single flashing. It means that SCSMD switched to "LEARNING MODE" for output #1.
* To select the "LEARNING MODE" for output #2, briefly press the SW1 button. The controlling LED will flash twice.
* For the third output - press the SW1 button again, the controlling LED will flash three times, etc.
* After the desired output, send any command from DCC command station to necessary accessory address. 
* The controlling LED stops flashing. It means that SCSMD stored a new Accessory-address for selected output.<br />
If within 15 seconds after entering the "LEARNING MODE", the SCSMD does not receive a command to switch the accessory, it will exit the "LEARNING MODE", the controlling LED will stop blinking, and the Output-Address will remain the same.<br />

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

2017-12-29 version 2.0.1b<br />
 * added Configuration Variables<br />
 * СV setting is possible via serial port<br />
 * the operating mode can be set for each output separately<br />
 * the DCC address is set for each output separately<br />
 