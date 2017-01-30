#include <Mouse.h>

#include <Keyboard.h>

#include <Bounce2.h>



//#include <Bounce2.h>   //https://github.com/thomasfredericks/Bounce2/wiki

/* Pro Micro Test Code
   by: Florian Maurer
   GameThing by Cuddleburrito
   date: March 6, 2015
   license: Public Domain - please use this code however you'd like.
   It's provided as a learning tool.
   This code is provided to how to turn an arduino into an HID keyboard controller for arcade games 
*/

//---------
// USE THESE FOR THE buttonPresets ARRAY BELOW
//#define KEY_LEFT_CTRL  0x80
//#define KEY_LEFT_SHIFT 0x81
//#define KEY_LEFT_ALT   0x82
//#define KEY_LEFT_GUI   0x83  
//#define KEY_RIGHT_CTRL 0x84
//#define KEY_RIGHT_SHIFT    0x85
//#define KEY_RIGHT_ALT  0x86
//#define KEY_RIGHT_GUI  0x87

//#define KEY_UP_ARROW   0xDA
//#define KEY_DOWN_ARROW 0xD9
//#define KEY_LEFT_ARROW 0xD8
//#define KEY_RIGHT_ARROW    0xD7
//#define KEY_SPACE_BAR  0x20
//
//#define KEY_W_LOWER 0x77
//#define KEY_S_LOWER 0x73
//#define KEY_A_LOWER 0x61
//#define KEY_D_LOWER 0x64
//
//#define KEY_W_UPPER 0x57
//#define KEY_S_UPPER 0x53
//#define KEY_A_UPPER 0x41
//#define KEY_D_UPPER 0x44

//#define KEY_BACKSPACE  0xB2
//#define KEY_TAB        0xB3
//#define KEY_RETURN 0xB0
//#define KEY_ESC        0xB1
//#define KEY_INSERT 0xD1
//#define KEY_DELETE 0xD4
//#define KEY_PAGE_UP    0xD3
//#define KEY_PAGE_DOWN  0xD6
//#define KEY_HOME   0xD2
//#define KEY_END        0xD5
//#define KEY_CAPS_LOCK  0xC1
//#define KEY_F1     0xC2
//#define KEY_F2     0xC3
//#define KEY_F3     0xC4
//#define KEY_F4     0xC5
//#define KEY_F5     0xC6
//#define KEY_F6     0xC7
//#define KEY_F7     0xC8
//#define KEY_F8     0xC9
//#define KEY_F9     0xCA
//#define KEY_F10        0xCB
//#define KEY_F11        0xCC
//#define KEY_F12        0xCD

//#define KEY_0   0x30
//#define KEY_1   0x31
//#define KEY_2   0x32
//#define KEY_3   0x33
//#define KEY_4   0x34
//#define KEY_5   0x35
//#define KEY_6   0x36
//#define KEY_7   0x37
//#define KEY_8   0x38
//#define KEY_9   0x39

#define KEY_S_LOWER   0x73
#define KEY_V_LOWER   0x76
#define KEY_T_LOWER   0x74
#define KEY_C_LOWER   0x63
#define KEY_G_LOWER   0x67
#define KEY_F_LOWER   0x66
#define KEY_O_LOWER   0x6f
#define KEY_R_LOWER   0x72

//---------

#define LED_PIN 13
//#define BOUNCE_LOCK-OUT //activate the alternative debouncing method. This method is a lot more responsive, but does not cancel noise.

//========== CONFIGURATION SETTINGS ==========
#define BOUNCE_WAIT 10
#define BOUNCE_COUNT 1

  //pins 2-10
  //for keys s,e,t, ,c,g,f,o,r,btn
int buttonPins[] = {
  2,  //key r (yellow)
  3,  //key f (green)
  4,  //key o (yellow)
  5,  //key c (green)
  6,  //key t (yellow)
  7,  //key v (green)
  8,  //key g (white)
  9   //key s (white)
};

char buttonPresets[] = { 
  KEY_R_LOWER, //button 2
  KEY_F_LOWER, //button 3
  KEY_O_LOWER, //button 4
  KEY_C_LOWER, //button 5
  KEY_T_LOWER, //button 6
  KEY_V_LOWER, //button 7
  KEY_G_LOWER, //button 8
  KEY_S_LOWER //button 9
};

//========== END CONFIGURATION SETTINGS ==========

// Instantiate button state array
boolean buttonPressed[8];
// Instantiate a Bounce object array to store each debouncer in
Bounce debouncers[8];
// Instantiate a counter array for each button to debounce count timer
int debounceCount[8];
// Instantiate a Bounce object to store mouseButton in
Bounce mouseBtnDebouncer;
// Instantiate mouseButton state
boolean mouseBtnPressed;
// Instantiate a count timer for mouseButton
int mouseDebounceCount;

// set pin numbers for switch, joystick axes, and LED:
const int switchPin = 11; // switch to turn on and off mouse control
const int mouseButton = 10; // input pin for the mouse pushButton
const int ledPin = 12;    // Mouse control LED
const int xAxis = A0;     // joystick X axis
const int yAxis = A1;     // joystick Y axis

// paramaters for reading the joystick:
int range = 12;           // output range of X or Y movement
int responseDelay = 5;    // response delay of the mouse in ms
int threshold = range /4; // resting threshold
int center = range / 2;   // resting position value

boolean mouseIsActive = false;  // whether or not to control the mouse
int lastSwitchState = LOW;      // previous switch state

void setup() {
  // put your setup code here, to run once:
  pinMode(switchPin, INPUT_PULLUP);    // the switch pin
  pinMode(ledPin, OUTPUT);      // the LED pin
  
   // open the serial port at 115200 baud rate and wait for port to open
//   Serial.begin(115200);
//   while (!Serial) {
//    ; // wait for serial port to connect. Needed for native USB 
//   }
   // initialize control over the mouse
   Mouse.begin();
   
   // initialize control over the keyboard
   Keyboard.begin();
 
   // Create debounce instances :
   for (int i = 0; i < 8; i++) {
     debouncers[i] = Bounce();
     debounceCount[i] = 0;
     pinMode(buttonPins[i],INPUT_PULLUP);
     (debouncers[i]).attach(buttonPins[i]);
     (debouncers[i]).interval(BOUNCE_WAIT);
        delay(100);
     buttonPressed[i] = false; 
   }

   // Create debounce instance for mouseButton:
   mouseBtnDebouncer = Bounce();  // instantiates a bounce object
   mouseDebounceCount = 0;
   pinMode(mouseButton, INPUT_PULLUP);  // sets pinmode for mouseButton
   // Set the pin and match the internal state to that of the pin:
   // NOTE: Only attach the pin after settin gthe pin up (internal pull-up)
   mouseBtnDebouncer.attach(mouseButton);
   mouseBtnDebouncer.interval(BOUNCE_WAIT);
   delay(100);
   mouseBtnPressed = false;  
}

void loop() {
  // put your main code here to run repeatedly:
  // read the switch:
  int switchState = digitalRead(switchPin);
  // if it's changed and it's high, toggle the mouse state:
  if (switchState != lastSwitchState) {
    if (switchState == HIGH) {
      mouseIsActive = !mouseIsActive;
      // turn on LED to indicate mouse state:
      digitalWrite(ledPin, mouseIsActive);
    }
  }
  //save switch state for next comparison:
  lastSwitchState = switchState;

  // read and scale the two axes:
  // flip x direction using mapping
  // x and y mapped opposite of directions on joystick
  int xReading = map(readAxis(A0), -1023, 1023, 1023, -1023);
  int yReading = readAxis(A1);

  // if the mouse state is active, move the mouse:
  if (mouseIsActive) {
    Mouse.move(xReading, yReading, 0);
  }

  delay(responseDelay);

  // Mouse Debounce Code:
  mouseBtnDebouncer.update(); // check current value
  int mouseValue = mouseBtnDebouncer.read();
  if ( mouseValue == LOW) { // if button pressed
    if(mouseDebounceCount == BOUNCE_COUNT && mouseBtnPressed == false) { //the button has been held down long enough and it hasn't been previously registered as pressed
      Mouse.press(MOUSE_LEFT);
      mouseBtnPressed = true;
    } else {
      if(mouseDebounceCount < BOUNCE_COUNT) {
        mouseDebounceCount++; //else increment the count
      }
    }
  } else { // button is not pressed
    if(mouseDebounceCount > 0) {
      mouseDebounceCount--; //keep decreasing count unless 0
    } else {
      Mouse.release(MOUSE_LEFT); //if 0 then release button
      mouseBtnPressed = false;
    }
  }
  
  // Debounce code:
  for (int j = 0; j < 8; j++) { //iterate over each button (pin)
    
     (debouncers[j]).update(); //check current value
     int value = (debouncers[j]).read();
     
     if ( value == LOW ) { //if button pressed
       
       if(debounceCount[j] == BOUNCE_COUNT && buttonPressed[j] == false) { //the button has been held down long enough and it hasn't been previously registered as pressed
          Keyboard.press(char(buttonPresets[j])); //Keyboard.write('1');
          buttonPressed[j] = true;
        } else {
            if(debounceCount[j] < BOUNCE_COUNT) { 
              debounceCount[j] = debounceCount[j] + 1; //else increment the count
            }
        }
        
      } else { //button is not pressed
        
        if(debounceCount[j] > 0) {
          debounceCount[j] = debounceCount[j] - 1; //keep decreasing count unless 0
        } else {
           Keyboard.release(char(buttonPresets[j])); //if 0 then release button
           buttonPressed[j] = false;
        }
        
      }

  }
}

/*
 * reads on axis (0 or 1 for x or y) and scales the
 * analog input range to a range from 0 to <range>
 */

int readAxis(int thisAxis) {
  // read the analog input:
  int reading = analogRead(thisAxis);

  // map the reading from the analog input range to the output range:
  reading = map(reading, 0, 1023, 0, range);

  // if the output reading is outside from the
  // rest position threshold, use it:
  int distance = reading - center;

  if (abs(distance) < threshold) {
    distance = 0;
  }

  // return the distance for this axis:
  return distance;
}

