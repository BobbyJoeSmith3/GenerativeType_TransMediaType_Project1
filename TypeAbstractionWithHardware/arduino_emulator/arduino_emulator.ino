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
#define KEY_E_LOWER   0x65
#define KEY_T_LOWER   0x74
#define KEY_SPACEBAR  0x20
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
  7,  //key e (green)
  8,  //key g (white)
  9,  //key s (white)
  10  //key   (white)
};

char buttonPresets[] = { 
  KEY_R_LOWER, //button 2
  KEY_F_LOWER, //button 3
  KEY_O_LOWER, //button 4
  KEY_C_LOWER, //button 5
  KEY_T_LOWER, //button 6
  KEY_E_LOWER, //button 7
  KEY_G_LOWER, //button 8
  KEY_S_LOWER, //button 9
  KEY_SPACEBAR //button 10 
};

//========== END CONFIGURATION SETTINGS ==========

// Instantiate button state array
boolean buttonPressed[9];
// Instantiate a Bounce object array to store each debouncer in
Bounce debouncers[9];
//Instantiate a counter array for each button to debounce count timer
int debounceCount[9];

int Xin = A0; // X Input Pin
int Yin = A1; // Y Input Pin
int Xpos;     // Store value of analog pin reading for x coordinate
int Ypos;     // Store value of analog pin reading for y coordinate
byte Xmap;     // Mapped value of Xpos
byte Ymap;     // Mapped value of Ypos
byte mappedValues[2]; // Array with mapped x and y values

void setup() {
  // put your setup code here, to run once:
  
   // open the serial port at 115200 baud rate and wait for port to open
   Serial.begin(115200);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB 
   }
   // initialize control over the keyboard
   Keyboard.begin();
 
  // Create debounce instances :
   for (int i = 0; i < 9; i++) {
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
  // put your main code here to run repeatedly:
  Xpos = analogRead(Xin);
  Ypos = analogRead(Yin);
  
  // Note: need to map analog x an y position readings to byte size
  // Serial will only transfer bytes and strings
  Xmap = byte(map(Xpos, -1023, 0, 1023, 255));
  Ymap = byte(map(Ypos, -1023, 0, 1023, 255));
  mappedValues[0] = Xmap;
  mappedValues[1] = Ymap;
  Serial.write(mappedValues, sizeof(mappedValues));
  
  // Debounce code:
  for (int j = 0; j < 9; j++) { //iterate over each button (pin)
    
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
