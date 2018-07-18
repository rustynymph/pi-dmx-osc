// ===========================================================================
// DMXWheel - a simple program that sets the color on a DMX device using
//            the color wheel selector on the screen
// ===========================================================================


// ===========================================================================


#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <dmx.h>                              // DMX interface library
#include "tinyosc.h"

// constants and definitions

#define MacroChannel  1                         
#define MacroSettings 2                         
#define Speed         3                        
#define MasterDim     4	                        // DMX channel for dimming
#define RedChannel    5                         // DMX channel for red control
#define GrnChannel    6                         // DMX channel for green control
#define BluChannel    7                         // DMX channel for blue control

#define Whitehannel   8                         // DMX channel for white control

#define light2reserved 9
#define light2strobe   10
#define light2red      11
#define light2green    12
#define light2blue     13

#define NumChannels   16                         // # of DMX channels used

// global variables
static int      initDMX       ();
static void     setDMXColor   ( int, int, int );
static void     exitDMX       ();
static void     sigIntHandler ( int );
static void     getNumbers    ( int[] );
static volatile bool keepRunning = true;
int r = 0;
int g = 0;
int b = 0; 

// ===========================================================================
//  main program
// ===========================================================================

int main( int argc, char *argv[] )
{
  int error;

  error = initDMX();
  if ( error < 0 ) return ( error );

  // terminate
  signal(SIGINT, sigIntHandler);

  char buffer[2048]; // declare a 2Kb buffer to read packet data into
  int len = 0;

  // open a socket to listen for datagrams (i.e. UDP packets) on port 9000
  const int fd = socket(AF_INET, SOCK_DGRAM, 0);
  fcntl(fd, F_SETFL, O_NONBLOCK); // set the socket to non-blocking
  struct sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(9000);
  sin.sin_addr.s_addr = INADDR_ANY;
  bind(fd, (struct sockaddr *) &sin, sizeof(struct sockaddr_in));
  printf("tinyosc is now listening on port 9000.\n");
  printf("Press Ctrl+C to stop.\n");

  while (keepRunning) {
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(fd, &readSet);
    struct timeval timeout = {1, 0}; // select times out after 1 second
    if (select(fd+1, &readSet, NULL, NULL, &timeout) > 0) {
      struct sockaddr sa; // can be safely cast to sockaddr_in
      socklen_t sa_len = sizeof(struct sockaddr_in);
      int len = 0;
      while ((len = (int) recvfrom(fd, buffer, sizeof(buffer), 0, &sa, &sa_len)) > 0) {
        if (tosc_isBundle(buffer)) {
          tosc_bundle bundle;
          tosc_parseBundle(&bundle, buffer, len);
          const uint64_t timetag = tosc_getTimetag(&bundle);
          tosc_message osc;
          while (tosc_getNextMessage(&bundle, &osc)) {
            tosc_printMessage(&osc);
          }
        } else {
          tosc_message osc;
          tosc_parseMessage(&osc, buffer, len);
          //tosc_printMessage(&osc); // uncommenting this messes up with the "nextInt" stuff

          /*
          switch ( osc.format[0] ) {
            case 'f': 
              //printf("%g \n", tosc_getNextFloat(&osc));
              arg = tosc_getNextFloat(&osc);
              break;
            case 'i':
              //printf("%i \n", tosc_getNextInt32(&osc));
              arg = tosc_getNextInt32(&osc);
              break;
            case 's':
              //printf("%s \n", tosc_getNextString(&osc));
              arg = atoi(tosc_getNextString(&osc));
              break;
            default: continue;
          }  */

         int redarg = tosc_getNextInt32(&osc);
         int greenarg = tosc_getNextInt32(&osc);
         int bluearg  = tosc_getNextInt32(&osc);

          /* How to check OSC address */
          /*if (strncmp(tosc_getAddress(&osc), "/lights", 6) == 0){
            // do thing
          } */
         setDMXColor(redarg, greenarg, bluearg);
        }
      }
    }
  }

  close(fd);
  exitDMX();

  return (0);
}

// ===========================================================================
// initDMX -- initialize DMX interface
// ===========================================================================

int initDMX()
{
  // open DMX interface

  int success = dmxOpen();
  if ( success < 0 ) return ( success );

  // configure

  dmxSetMaxChannels ( NumChannels );

  // return valid status

  return ( 0 );
}

// ===========================================================================
// setDMXColor -- set the color values for the DMX device
// ===========================================================================
void setDMXColor ( int red, int green, int blue )
{

  ubyte redVal = (ubyte) red;
  ubyte greenVal = (ubyte) green;
  ubyte blueVal = (ubyte) blue;

  dmxSetValue ( MasterDim , 150 );
  //dmxSetValue ( light2strobe , 255 );

  // set the channel colors
  dmxSetValue ( RedChannel , redVal );
  dmxSetValue ( GrnChannel , greenVal );
  dmxSetValue ( BluChannel , blueVal );

  //dmxSetValue ( light2reserved , 0 );
  //dmxSetValue ( light2strobe , 255 );
  //dmxSetValue ( light2red , redVal );
  //dmxSetValue ( light2green , grnVal );
  //dmxSetValue ( light2blue, bluVal );
}

// ===========================================================================
// exitDMX -- terminate the DMX interface
// ===========================================================================
void exitDMX()
{
 // blackout
  for (int i = 1; i <= NumChannels; i = i + 1){
    dmxSetValue ( i , 0 );
  } 

  // close the DMX connection
  dmxClose();
}

void sigIntHandler(int sigint)
{
  exitDMX();
  keepRunning = false;
}
