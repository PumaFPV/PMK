//Define Trackballer Breakout pin connections to Arduino
#define PIN_TRACKBALL_RIGHT     37
#define PIN_TRACKBALL_UP        33
#define PIN_TRACKBALL_LEFT      35
#define PIN_TRACKBALL_DOWN      18

#define PIN_TRACKBALL_BUTTON    16

#define PIN_TRACKBALL_LED_WHITE 39
#define PIN_TRACKBALL_LED_GREEN 36
#define PIN_TRACKBALL_LED_RED   38
#define PIN_TRACKBALL_LED_BLUE  40

//Define variables used in sketch
bool trackballButtonCurrentState = 0;
bool trackballLeftCurrentState = 0;
bool trackballRightCurrentState = 0;
bool trackballUpCurrentState = 0;
bool trackballDownCurrentState = 0;

//Previous state
bool trackballButtonPreviousState = 0;
bool trackballLeftPreviousState = 0;
bool trackballRightPreviousState = 0;
bool trackballUpPreviousState = 0;
bool trackballDownPreviousState = 0;

int xPosition;
int yPosition;
int xDistance; 
int yDistance; 

int range = 10;              // output range of X or Y movement; affects movement speed


struct Func
{
  uint16_t counter;
  unsigned long beginTime;
  unsigned long startCounterTime;
  unsigned long endTime;
  unsigned long duration;
  const    long interval;
  uint8_t frequency;
  unsigned long inBetweenTime;
};

Func gpioTask = {0, 0, 0, 0, 0, 10000, 0, 0};
Func trackballTask = {0, 0, 0, 0, 0, 10000, 0, 0};
Func pmkTask = {0, 0, 0, 0, 0, 10000, 0, 0};


