##CANbus Library for Teensy 3.1, 3.2, 3.5, and 3.6

###Introduction
FlexCAN is a serial communication driver for the CAN peripherial built into the Teensy CPUs. Versions 3.1, 3.2, and 3.5 of the board support single CAN0 controller while version 3.6 supports dual CAN0/CAN1 controllers.  The driver is organized in the Arduino library format.

When the FlexCAN object is configured on Teensy 3.1/3.2, Arduino pins Digital 3 and Digital 4 are assigned to CAN functions TX and RX.

![Teensy 3.1/3.2/3.5 CAN Pins, Digital3=TX, Digital4=RX](/FlexCAN_pins.png)

Alternatively, pins 25 and 32 can be assigned to CAN0 instead:

![Teensy 3.1/3.2 Alternative CAN Pins, Digital32=TX, Digital25=RX](/FlexCAN_pins_alt.png)

Similarily on Teensy 3.5, the same Arduino pins Digital 3 and Digital 4 are assigned to CAN functions TX and RX. Pins 29 and 30 can be alternatively used for CAN0.

![Teensy 3.5 CAN Pins, Digital3=TX, Digital4=RX](/FlexCAN_pins_35.png)

All boards have an automatically created object named Can0 which can be used to setup the first CAN bus. The Teensy 3.6 adds a Can1 object which can be used to set up the second bus.

For CAN0 Arduino pins Digital 3 and Digital 4 are assigned to CAN functions TX and RX. For CAN1 Arduino pins Digital 34 and Digital 33 are assigned to CAN functions TX and RX. However, it is also possible to configure alternate pins on the Teensy 3.5 and 3.6. Can0 can be reconfigured to use pins 29 and 30 instead.

![Teensy 3.6 CAN Pins, CAN0: Digital3=TX and Digital4=RX, CAN1: Digital34=TX and Digital33=RX](/FlexCAN_pins_36.png)

CAN RX and TX pins should be wired to a 3.3V CAN transceiver TXD and RXD respectively to allow connection of the Teensy 3.1/3.2/3.5/3.6 to a CAN network.

Even though the Teensy 3.1/3.2 is operating on 3.3V, use of 5V transceivers may be an option if the system has regulated +5V available.  The CAN RXD input on the CPU is 5V tolerant and most 5V transceivers will accept the 3V TXD signal.  This is a good choice for breadboarding due to availability of thru-hole 5V transceiver parts.
**In case of Teensy 3.6 the digital pins are not 5V tolerant, so 3.3V transceivers must be used!**

Note that CAN will normally require termination resistors.  These are located at the two ends of a CAN bus to prevent reflections.  Do not add more terminators when connecting devices to an existing properly terminated CAN bus. A properly terminated CAN bus has a resistance of 60 ohms between CAN-High and CAN-Low. This is done by placing 120 ohms of resistance on both ends of the bus.

Supported baud rates are 50000, 100000, 125000, 250000, 500000, and 1000000 bits per second.  If the baud rate is not specified it will default to 250000. Other baud rates are possible but care should be taken to ensure that the desired baud rate is possible. The CAN clock is 16MHz so it must be possible to take 16MHz and divide it evenly. For example 16Mhz / 250k = 64 so that works. 16MHz / 47 = 340425 so that CAN rate would even work. But, rates not very close to evenly divisible into 16MHz will cause trouble.

###CAN Transceiver Options
Please add parts you are using successfully with Teensy 3.1 to this list.
- TI SN65HVD230D on 3.3V (1MBPS)
- TI SN65HVD232D / SN65HVD232QDQ1 on 3.3V (1MBPS)
- NXP TJA1050T/VM,118 on the same 5V supply as the Teensy. (1MBPS)
- Microchip MCP2551 on 5V (reported at 500KBPS)
- Linear LT1796 on 5V (not speedtested)
- ISO1050DW / DUB
- Microchip MCP2562 with VIO on 3.3V (tested on Teensy 3.2 and 3.6)

###Driver API
All available CAN buses have pre-created objects similarly to how the serial devices are created (Serial, Serial2, etc). In the case of these CAN buses they are called **Can0** (Teensy 3.1/3.2/3.5/3.6) and **Can1** (Teensy 3.6 only).

**begin(baud, defaultMask, txAlt, RxAlt)**
Enable the chosen CAN bus. All parameters are optional. Below is a table of parameters with defaults in bold:

| Parameter   | Description          | Allowed values 
|-------------|----------------------|------------------------------------------------------------------------
| baud        | baudrate [bps]       | Teensy 3.1/3.2/3.5/3.6 - Practically anything from 20k to 1M
| defaultMask | FlexCAN interface ID | An 11 or 29 bit mask to apply to all mailboxes by default. Defaults to **0** which allows through all traffic
| txAlt       | Alternative TX pin   | Teensy 3.1/3.2: 0 (PIN3), 1 (PIN32), Teensy 3.5/3.6 CAN0: 0 (PIN3), 1 (PIN29), Teensy 3.6 CAN1: 0 (PIN33)
| rxAlt       | Alternative RX pin   | Teensy 3.1/3.2: 0 (PIN4), 1 (PIN25), Teensy 3.5/3.6 CAN0: 0 (PIN4), 1 (PIN30), Teensy 3.6 CAN1: 0 (PIN34)

**end()**
Disable the CAN from participating on the CANbus.  However, pins remain assigned to the CAN bus hardware.

**write(message)**
Send a frame of up to 8 bytes using the given identifier.  **write()** will return 0 if no buffer was available for sending.

message is a **CAN_message_t** type buffer structure.

**read(message)**
Receive a frame into "message" if available.  **read()** will return 1 if a frame was copied into the callers buffer, or 0 if no frame is available.

**available()**
Returns 0 if no frames are available. Otherwise returns the number of available frames in the buffer.

**CAN_message_t** structure:

| Parameter       | Description                    | Allowed values 
|-----------------|--------------------------------|-------------------------------------------------------------------------------------------
| id              | 11 or 29 bit ID                | Any value up to (2^(N+1) - 1) where N is either 11 or 29. For 11 bit IDs this is 0 to 4095
| flags.extended  | Specify whether 11 or 29 bit ID| 0 for standard/11 bit IDs, 1 for extended/29 bit IDs
| flags.remote    | Specify whether RTR frame      | 0 = Normal frame 1 = Remote request frame
| flags.overrun   | Was there an overrun?          | 0 = No overruns detected 1 = Message buffer overrun
| len             | # of data bytes in this frame  | 0 to 8
| timestamp       | Hardware based timestamp       | Hardware generated timestamp when frame was received (on RX frames)
| buf             | Data bytes for this frame      | Anything. These 0 to 8 bytes are the payload of the frame.

**setNumTxBoxes(boxes)**
Set the number of mailboxes used for transmit. There are 16 mailboxes in hardware. 0 to 16 of them can be set as transmission mailboxes. These mailboxes will then not be used for reception. By default, two mailboxes are automatically configured for transmission.
 
###Use of Optional RX Filtering
**setFilter(filter, number)**
Set the receive filter selected by "number", 0-15. There are 16 mailboxes in hardware but 2 are used, by default, for TX. Those cannot have filters set. Otherwise, each mailbox has its own filter and mask. The transmit boxes are always at the end. So, by default they are 14 and 15. Filters take the form of a CAN bus ID (11 bit or 29 bit).

**getFilter(filter, number)**
Retrieve the mailbox filter selected by "number", 0-15.  If the mailbox has a filter, true is returned, otherwise false is returned.  The filter is returned in the parameter "filter".

The mask and filter are **CAN_filter_t** type structures.

**CAN_filter_t** structure:

| Parameter       | Description                    | Allowed values 
|-----------------|--------------------------------|-------------------------------------------------------------------------------------------
| id              | 11 or 29 bit ID                | Any value up to (2^(N+1) - 1) where N is either 11 or 29. For 11 bit IDs this is 0 to 4095
| flags.extended  | Specify whether 11 or 29 bit ID| 0 for standard/11 bit IDs, 1 for extended/29 bit IDs
| flags.remote    | Filter for RTR frames?         | 0 for non RTR, 1 for RTR frame. Part of a long since deprecated part of the CAN standard.

**setMask(mask, number)**
Set the receive mask for the selected mailbox. Cannot be used on transmission mailboxes (The last two by default). Used along with filters to configure which messages will be accepted by each mailbox. The filtering scheme works like this: When a frame comes in the ID of the frame has a boolean AND applied with the mask for the mailbox. Then, this masked value is compared to the filter ID. If the two match then the frame is accepted. If not the next mailbox is checked. If no mailboxes accept a frame it is thrown away. Here is an example: Mask of 0x7F0, filter ID of 0x320. If a frame with id 0x322 comes in then 0x322 AND 0x7F0 = 0x320. This matches so the frame is accepted. If a frame comes in with id 0x33B then 0x33B AND 0x7F0 = 0x330 which does not match. Unless another mailbox accepts the frame it will be thrown away.

###In-order Transmission
By default two transmission mailboxes are configured. Ordinarily two mailboxes still allow for in-order transmission as they'll ping-pong while loading frames and send them out in order on the bus. However, there are still some scenarios where it might be possible for both mailboxes to get loaded and the wrong frame to go out causing a single frame out of order situation. For strict in-order transmission the library should be set to use a single transmission box like so: **Can0.setNumTxBoxes(1);**

###Out of Order Reception
The driver configuration uses multiple mailboxes to receive the incoming messages.  The FlexCAN hardware sequentially searches the configured mailboxes for an empty mailbox, an incoming message is stored in the first empty mailbox found.  As messages are copied from the FlexCAN mailboxes to the incoming ring buffer, the driver sequentially clears the mailboxes. This creates the possibility that the FlexCAN hardware may fill the mailboxes out of order; a recently cleared mailbox may be filled along with a mailbox following the already filled mailboxes.

This may result in messages being delivered out of order by the **read()** function.  The timestamp within the message will correctly indicate the time of arrival and should be used to order the messages.

###CAN Bus Statistics

**CAN_stats_t** structure:

| Parameter          | Description                          | Allowed values 
|--------------------|--------------------------------------|-------------------------------------------------------------------------------------------
| enabled            | Enable collecting stats              | True/False
| ringRxMax          | Number of entries in the ring buffer | Set at compile time. Here for reference
| ringRxHighWater    | Max entries used in ring buffer      | 0 to ringRxMax
| ringRxFramesLost   | Total number of frames lost          | Lost frames count
| ringTxMax          | Number of entries in the ring buffer | Set at compile time. Here for reference
| ringTxHighWater    | Max entries used in ring buffer      | 0 to ringTxMax
| mb[#].refCount     | Mailbox use count                    | 0 to 4 billion
| mb[#].overrunCount | Mailbox overrun count                | 0 to 4 billion
 
 
**startStats()**
Start capturing statistics (those from the table above)

**stopStats()**
Quit updating CAN statistics

**clearStats()**
Reset all statistics back to zero / default state
  
**getStats()**
Returns a CAN_stats_t structure with the current statistic values. 


###Object Oriented Callback Interface
This library has the capability to allow C++ objects to register themselves with the library such that they will automatically be sent any incoming frames that were accepted by a mailbox. This allows user code to no longer have to do any polling to receive messages. In order to take advantage of this, base your class off of the **CANListener** class. Then, tell either Can0 or Can1 about your class with:

**attachObj(*yourClass)**
Pass a pointer to your object to this function. This will register your class object with the library.

**detachObj(*yourClass)**
Pass a pointer to your class object to de-register it with the library and to quit receiving frames via the callback interface.

But, another step is necessary. You must tell the library which mailboxes your class object was interested in receiving messages from. This is done with the following functions which are part of CANListener and thus should be called on your class object itself:

**attachMBHandler(mailbox)**
Tell the library you want a callback any time a message comes into the passed mailbox (0 - 15)

**detachMBHandler(mailbox)**
Tell the library which mailbox you are no longer interested in (0 - 15)
 
**attachGeneralHandler()**
Tell the library to pass messages from any mailbox to your object.

**detachGeneralHandler()**
Tell the library to quit passing any mailbox messasges to your object.

It should be noted, however, that attachMBHandler calls override the general handler. This means that if you remove the general handler you'll still be registered to get the per-mailbox messages from boxes you attached to. 

Once you've attached your object to the library and set up which mailboxes the object is interested in you will begin receiving callbacks as messages come in. Callbacks are done via a virtual function you must override in your class:

**frameHandler(frame, mailbox, controller)**
A function you create in your class that gets called anytime a new message comes in. The message is passed in the first parameter. The second parameter shows which mailbox sent this
message. But, the general handler always sets the mailbox number to -1. The third parameter indicates which CAN controller the frame arrived from.

You are allowed to have both per-mailbox handlers and the general handler configured at the same time. If a message arrives at a mailbox being monitoring then it's mailbox number will be passed to this function.  If a general handler is configured, messages arriving at mailboxes that do not have a per-mailbox handler will result in the general handler being called.  When the general handler is called the mailbox is identified as -1.

The frame handler returns *true* if the frame was processed, otherwise *false* is returned.  If none of the installed handlers process the frame, the frame will be added to the ring buffer for processing with the *read()* function.  Processed frames are not added to the ring buffer.

Note, the handlers are called in the context of the interrupt.  Care should be taken to avoid long processing times in the handler, mailbox overruns by the FlexCAN controller may occur if the handlers introduce long latencies.

By default each CAN bus can have 4 objects attached for callbacks.

(All images in this README courtesy of Pawelsky)
