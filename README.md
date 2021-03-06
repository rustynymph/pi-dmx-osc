This is an application for interfacing with the Velleman USB Controlled DMX interface and using OSC to control DMX lights.

### Credit
DMX daemon and library for writing to the different channels from: http://engeldinger.com/projects/11-dmxwheel
tinyosc: https://github.com/mhroth/tinyosc

### Prerequisites  
USB development support (libusb-dev)

### Installation  
`sudo apt-get install libusb-dev`  

### To build the source code, type make from the command line at the top level of this
distribution  
`make`  

### Operation  
The distribution contains two directories; one for the library (libdmx.a) and
deamon ( dmxd.bin ) in dmx/, and one for the application program 
( DMX-OSC.bin ) in DMX-OSC/. 

To begin, first run the deamon program. This program communicates between the
interface and the application program via a shared memory segment and updates
the interface on a periodic basis. For more information see the  README file
in the dmx directory. You can launch the deamon via command line:
`cd dmx/deamon`  
`sudo ./dmxd.bin &`  

Second, run the DMX-OSC program. This program TO DO:  
`cd DMX-OSC/bin`  
`./DMXWheel.bin`  
