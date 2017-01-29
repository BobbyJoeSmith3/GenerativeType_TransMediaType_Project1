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

#define KEY_0   0x30
#define KEY_1   0x31
#define KEY_2   0x32
#define KEY_3   0x33
#define KEY_4   0x34
#define KEY_5   0x35
#define KEY_6   0x36
#define KEY_7   0x37
#define KEY_8   0x38
#define KEY_9   0x39

//---------

#define LED_PIN 13
//#define BOUNCE_LOCK-OUT //activate the alternative debouncing method. This method is a lot more responsive, but does not cancel noise.

//========== CONFIGURATION SETTINGS ==========
#define BOUNCE_WAIT 10
#define BOUNCE_COUNT 1

  //pins 2-9
  //for buttons 1-2, up, down, left and right
int buttonPins[] = {
  2,  //button 1 (yellow)
  3,  //button 2 (orange)
  4,  //button 3 (blue)
  5,  //button 4 (green)
  6,  //button 5 (yellow)
  7,  //button 6 (orange)
  8,  //button 7 (blue)
  9   //button 8 (green)
};

char buttonPresets[] = { 
  KEY_2, //button 1 (yellow)
  KEY_3, //button 2 (orange)
  KEY_4, //button 3 (blue)
  KEY_5, //button 4 (green)
  KEY_6, //button 5 (yellow)
  KEY_7, //button 6 (orange)
  KEY_8, //button 7 (blue)
  KEY_9  //button 8 (green)
  
};

//========== END CONFIGURATION SETTINGS ==========

// Instantiate button state array
boolean buttonPressed[8];
// Instantiate a Bounce object array to store each debouncer in
Bounce debouncers[8];
//Instantiate a counter array for each button to debounce count timer
int debounceCount[8];

void setup() {
  // put your setup code here, to run once:

 // open the serial port at 115200 baud rate
 Serial.begin(115200);
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
     
}

void loop() {
  
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
