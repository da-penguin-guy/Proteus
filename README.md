# Virgil Controller

The Virgil Controller is a modular collection of PCBs that will be used to develop the Virgil sound Protocol

## Overview

The Virgil Controller doesn't fall into a good product class, primarily because this hasn't ever been done before. The best analog would be some midi controller, like the [Behringer X-touch](https://www.behringer.com/en/products/0808-AAE). However, this varies from that in several cases. Firstly, it will be fully open source, modular, and future proof. The other main difference is that it won't be using the MIDI protocol. I will be using this as a hardware prototype to develop the [Virgil](https://github.com/da-penguin-guy/Virgil) live sound protocol. 

The Virgil protocol is a companion protocol to Audinate's Dante protocol, the industry standard for live audio transfer via a network. However, there is a significant gap in Dante: It only encompasses the audio transportation, not any additional communication. This is especially important for digital stageboxes, where the mixer *needs* control over physical hardware controls such as gain and phantom power. Because Dante doesn't allow for control packets, every popular audio company has created their own proprietary standard for transporting the information. This means that, despite Dante's goal of allowing for equipment from different companies to work together, productions are still limited to a single brand if they want a functional ecosystem. 

The Virgil protocol solves this by being a universal language, allowing a variety of interconnectivity.

## Modules

The controller itself has 16 module slots, 8 on the bottom and 8 on the top. Electrically, the slots are identical, but the slots have different lengths.

All modules (at this time) must have an rp2040/2350 microcontroller in them, and all communication over SPI.

Currently, the only module for the top slot is a display module that has a 1.14" TFT display, a button with a neopixel, and a continuous encoder with a neopixel. The display is the same as the [Adafruit Newxie Tube](https://www.adafruit.com/product/6113), but just the panel can be found on [AliExpress](https://www.aliexpress.com/i/3256803593005387.html?gatewayAdapt=4itemAdapt#nav-description) for significantly cheaper. For more information, go to the [Virgil Screen](PCB/Virgil%20Screen/Virgil%20Screen.kicad_pro) kicad project.

Currently, the only module for the bottom slot is a motorized fader using the B103 fader. These are the same faders used in the [Behringer X-Touch](https://www.sweetwater.com/store/detail/XTOUCHFADER--behringer-mf100t-motorized-faders-set-of-5-for-by-touch-series), but they can be found for significantly cheaper on [Alibaba](https://www.alibaba.com/product-detail/Toy-Sound-Box-Variable-Resistors-Fader_1601226018682.html?spm=a2700.galleryofferlist.normal_offer.d_title.1a4e13a0jfMknk&selectedCarrierCode=SEMI_MANAGED_STANDARD%40%40STANDARD&priceId=c0e033c3b4ef4eadbab299e74c6389cc). For more information, go to the [Virgil Motor](PCB/Virgil%20Screen/Virgil%20Screen.kicad_pro) kicad project.

## Internal PCBs

The Virgil controller has 3 internal PCBs: The [Controller](#controller), The [Power](#power) Board, and The [Backplane](#backplane).

### Controller
This is the actual brain of the device.

For external ports, it has an Ethercon port for networking and a USB-C port for programming.  

For Internal ports, it has 2 50 pin IDC connectors, each with different pinouts. The first connects to the Power board and the second connects to the Backplane. The power board connects directly to the Controller, while the backplane connects via an IDC cable. There is also a microSD card connected via SDIO for icon storage.

For ICs the controller has an RP2350. It also has a W5500 for ethernet connectivity and the Raspberry Pi Radio Module 2 for wireless connectivity

### Power
The power module is designed to be completely separated and isolated from the main board. This is primarily because I want for this to eventually have a battery powered option, but I don't want to deal with a BMS yet. 

The power module is expected to supply 3.3V, 5V, and a currently unknown motor VCC (somewhere between 9-12V). Motor VCC and current requirements will be determined after testing

Power modules are given a space on the back of the enclosure to expose their own power connectors. In addition, all the required signals for POE and USB-C PD are passed from the controller to the power board, including USB-C CC lines.

Currently, the only power module that has been developed supports POE and 12V in through a barrel jack. This power module is designed to be for very early prototyping. Because of that, the current module has an RP2350 on it, combined with current and voltage sensing on all lines. In addition, the motor VCC also has a smart power fuse and adjustable voltage via a DigiPot.


### Backplane
The backplane is the PCB that the modules plug into. This is kept separate so that extra module slots may be added in the future, and for simplifying enclosure design. 

The main purpose of the backplane is to route connections from the 50 pin connector to the controller and each of the 16 modules. The signals that are being routed are various power lines, spi, SWD, and one singular GPIO. That gpio is tied directly to the controller, and was intended as an SPI interrupt. However, that will likely be removed in future iterations to allow for better scaling across more modules.

The backplane does have some ICs on it. The most notable is a CD74HC154M96, used to split the 4 CS mux lines from the controller to 16 CS lines for the modules. There are also 2 MCP23017T-E/SS I2C expanders, used for supplying reset lines to the modules and detect lines. The backplane also has 16 SN74CBTLV1G125DCKR, one for each module. This is used to switch the SWD lines on or off for each module, using the CS line as the control signal. This is so that the controller can program every module automatically.