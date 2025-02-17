/***************************************************************************************************************
* David's Super Special Awesome Macro Keyboard
* Based on Ryan Bates sample code written for MacroKeyboardV2
* Removed all references to LCD and to Encoder B, customized macros
* 
* Ryan's Info:
* github project: https://github.com/retrobuiltRyan/MacroKeyboardV2
* web: www.retrobuiltgames.com
* youtube: https://www.youtube.com/c/ryanbatesrbg
* twitter: @retrobuiltgames
* Buy Ryan's PCB at https://www.tindie.com/products/19957/
* 
*Ryan's Pro Tips:
*=============== Keyboard Control================================================================================
*Keyboard.write();   Sends a keystroke to a connected computer. 
*                    This is similar to pressing and releasing a key on your keyboard.
*                    Will send a shift command if applicable. Example: Keyboard.write('K') will 
*                    automatically do SHIFT + k. 
*                    Can also accept ASCII code like this:
*                    //Keyboard.write(32); // This is space bar (in decimal)
*                    Helpful list of ASCII + decimal keys http://www.asciitable.com/
*                    
*
*Keyboard.press();   Best for holding down a key with multi-key commands; like copy/ paste
*                    This example is [ctrl] + [shift] + [e] 
*                      //Keyboard.press(KEY_LEFT_CTRL);
*                      //Keyboard.press(KEY_LEFT_SHIFT);
*                      //Keyboard.press('e'); 
*                      //delay(100); Keyboard.releaseAll();
*                
*
*Keyboard.print();   Sends a keystroke(s)
*                    Keyboard.print("stop using aim bot"); // types this in as a char or int! (not a string)!
*
*
*Keyboard.println(); Sends a keystroke followed by a newline (carriage return)
*                     Very practical if you want to type a password and login in one button press!
*                     
*SOMETIMES, applications are coded to recognize Keyboard.press() and not Keyboard.write() and vice versa.
*You might have to experiment. 
*
*=============== Mouse Control================================================================================
*Mouse.move(x, y, wheel);  Moves the mouse and or scroll wheel up/down left/right.
*                          Range is -128 to +127. units are pixels 
*                          -number = left or down
*                          +number = right or up
*
*Mouse.press(b);         Presses the mouse button (still need to call release). Replace "b" with:
*                        MOUSE_LEFT   //Left Mouse button
*                        MOUSE_RIGHT  //Right Mouse button
*                        MOUSE_MIDDLE //Middle mouse button
*                        MOUSE_ALL    //All three mouse buttons
*                        
*Mouse.release(b);       Releases the mouse button.
*
*Mouse.click(b);         A quick press and release.
***************************************************************************************************************/

//=============== Setup  =====================================================
//this section imports libraries and sets up the macro pad matrix
//===============================================================================

#include <HID-Project.h>
#include <Keypad.h>

const byte ROWS = 3;  //four rows
const byte COLS = 4;  //four columns

#include <Encoder.h>
//Library for simple interfacing with encoders (up to two)
//low performance ender response, pins do not have interrupts
Encoder RotaryEncoderA(14, 15);  //the LEFT encoder (encoder A)

#include <Adafruit_NeoPixel.h>  //inclusion of Adafruit's NeoPixel (RBG addressable LED) library

#define PIN A2
#define NUMPIXELS 13

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int colorUpdate = 0;  //setting a flag to only update colors once when the mode is switched.
const int b = 3;      // Brightness control variable. Used to divide the RBG vales set for the RGB LEDs. full range is 0-255. 255 is super bright
                      // In fact 255 is obnoxiously bright, so this use this variable to reduce the value. It also reduces the current draw on the USB


char keys[ROWS][COLS] = {
  { '1', '2', '3', '4' },
  { '5', '6', '7', '8' },
  { '9', '0', 'A', 'B' },
};
// The library will return the character inside this array when the appropriate
// button is pressed then look for that case statement. This is the key assignment lookup table.
// Layout(key/button order) looks like this
//     |----------------------------|
//     |                  [2/3]*    |     *TRS breakout connection. Keys 5 and 6 are duplicated at the TRS jack
//     |      [ 1] [ 2] [ 3] [ 4]   |     * Encoder location = key[1]
//     |      [ 5] [ 6] [ 7] [ 8]   |
//     |      [ 9] [10] [11] [12]   |      NOTE: The mode button is not row/column key, it's directly wired to A0!!
//     |----------------------------|




// Variables that will change:
int modePushCounter = 0;  // counter for the number of button presses
int buttonState = 0;      // current state of the button
int lastButtonState = 0;  // previous state of the button

long positionEncoderA = -999;  //encoderA LEFT position variable

const int ModeButton = A0;  // the pin that the Modebutton is attached to
const int pot = A1;         // pot for adjusting attract mode demoTime or mouseMouse pixel value
//const int Mode1= A2;
//const int Mode2= A3; //Mode status LEDs

byte rowPins[ROWS] = { 4, 5, A3 };    //connect to the row pinouts of the keypad
byte colPins[COLS] = { 6, 7, 8, 9 };  //connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  pixels.begin();
  Consumer.begin();
  Serial.begin(9600);
}

//=============== Main Loop =====================================================
//this section includes the main loop and defines the key presses in each mode
//===============================================================================

void loop() {
  char key = keypad.getKey();
  checkModeButton();

  switch (modePushCounter) {  // switch between keyboard configurations:
    case 0:                   //Numpad
      encoderA_Mode1();       //custom function for encoder A
      setColorsMode0();       //indicate what mode is loaded by changing the key colors

      if (key) {
        switch (key) {
          case '1':
            Keyboard.press(KEY_DELETE);
            delay(100);
            Keyboard.release(KEY_DELETE);
            break;
          case '2': Keyboard.print('7'); break;
          case '3': Keyboard.print('8'); break;
          case '4': Keyboard.print('9'); break;
          case '5':
            Keyboard.press(KEY_RETURN);
            delay(100);
            Keyboard.release(KEY_RETURN);
            break;
          case '6': Keyboard.print('4'); break;
          case '7': Keyboard.print('5'); break;
          case '8': Keyboard.print('6'); break;
          case '9': Keyboard.print('0'); break;
          case '0': Keyboard.print('1'); break;
          case 'A': Keyboard.print('2'); break;
          case 'B': Keyboard.print('3'); break;
        }

        delay(100);
        Keyboard.releaseAll();  // this releases the buttons
      }
      break;

    case 1:              //Browsing
      encoderA_Mode0();  //custom function for encoder A
      setColorsMode1();  //indicate what mode is loaded by changing the key colors

      if (key) {
        switch (key) {
          case '1':
            Consumer.write(MEDIA_VOLUME_MUTE);
            break;
          case '2':
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.print('t');
            delay(100);
            Keyboard.release(KEY_LEFT_CTRL);
            delay(100);
            Keyboard.println("www.youtube.com");
            break;
          case '3':
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.print('t');
            delay(100);
            Keyboard.release(KEY_LEFT_CTRL);
            delay(100);
            Keyboard.println("www.reddit.com");
            break;
          case '4':
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.print('t');
            delay(100);
            Keyboard.release(KEY_LEFT_CTRL);
            delay(100);
            Keyboard.println("10.0.0.10:8123");
            break;
          case '5':
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.print('t');
            delay(100);
            Keyboard.release(KEY_LEFT_CTRL);
            delay(100);
            Keyboard.println("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
            break;
          case '6':
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.print('t');
            delay(100);
            Keyboard.release(KEY_LEFT_CTRL);
            delay(100);
            Keyboard.println("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
            break;
          case '7':
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.print('t');
            delay(100);
            Keyboard.release(KEY_LEFT_CTRL);
            delay(100);
            Keyboard.println("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
            break;
          case '8':
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.print('t');
            delay(100);
            Keyboard.release(KEY_LEFT_CTRL);
            delay(100);
            Keyboard.println("https://www.youtube.com/watch?v=dQw4w9WgXcQ");
            break;
          case '9':
            Keyboard.press(KEY_LEFT_GUI);
            delay(100);
            Keyboard.print('3');
            delay(100);
            Keyboard.release(KEY_LEFT_GUI);
            break;
          case '0':
            Keyboard.press(KEY_LEFT_GUI);
            delay(100);
            Keyboard.print('2');
            delay(100);
            Keyboard.release(KEY_LEFT_GUI);
            break;
          case 'A':
            Keyboard.press(KEY_LEFT_GUI);
            delay(100);
            Keyboard.print('5');
            delay(100);
            Keyboard.release(KEY_LEFT_GUI);
            break;
          case 'B':
            Keyboard.press(KEY_LEFT_GUI);
            delay(100);
            Keyboard.print('7');
            delay(100);
            Keyboard.release(KEY_LEFT_GUI);
            break;
        }

        delay(100);
        Keyboard.releaseAll();  // this releases the buttons
      }
      break;
  }
}

//=============== Sub Routine Section ===========================================
//this section sets up sub routines that the keypad may need to function
//===============================================================================

void setColorsModeClear() {
  if (colorUpdate == 0) {                  // have the neopixels been updated?
    for (int i = 0; i < NUMPIXELS; i++) {  //  Red,Green,Blue        // pixels.Color takes RGB values; range is (0,0,0) to (255,255,255)
      pixels.setPixelColor(i, pixels.Color(255, 255, 255));
      pixels.show();  // This pushes the updated pixel color to the hardware.
      delay(50);      // Delay for a period of time (in milliseconds).
    }

    colorUpdate = 1;  // Mark the color flag so neopixels are no longer updated in the loop
  }
}

void checkModeButton() {
  buttonState = digitalRead(ModeButton);
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      modePushCounter++;
      Serial.println("pressed");
      Serial.print("number of button pushes: ");
      Serial.println(modePushCounter);
      colorUpdate = 0;  // set the color change flag ONLY when we know the mode button has been pressed. Saves processor resources from updating the neoPixel colors all the time
    }
    delay(50);  // Delay a little bit to avoid bouncing
  }
  lastButtonState = buttonState;
  if (modePushCounter > 1) {  //reset the counter after 2 presses CHANGE THIS FOR MORE MODES
    modePushCounter = 0;
  }
}

void encoderA() {
  long newPos = RotaryEncoderA.read() / 4;  //When the encoder lands on a valley, this is an increment of 4.

  if (newPos != positionEncoderA && newPos > positionEncoderA) {
    positionEncoderA = newPos;
    Keyboard.press(KEY_LEFT_ARROW);
    Keyboard.release(KEY_LEFT_ARROW);
  }

  if (newPos != positionEncoderA && newPos < positionEncoderA) {
    positionEncoderA = newPos;
    Keyboard.press(KEY_RIGHT_ARROW);
    Keyboard.release(KEY_RIGHT_ARROW);
  }
}

//=============== LED Assignments ===============================================
//this section allows a unique LED color function for each mode.
//===============================================================================

void setColorsMode0() {
  if (colorUpdate == 0) {                               // have the neopixels been updated?
    pixels.setPixelColor(0, pixels.Color(80, 0, 200));  //gradient mix
    pixels.setPixelColor(1, pixels.Color(10, 0, 200));  //gradient mix
    pixels.setPixelColor(2, pixels.Color(20, 0, 200));
    pixels.setPixelColor(3, pixels.Color(40, 0, 200));
    pixels.setPixelColor(4, pixels.Color(60, 0, 200));
    pixels.setPixelColor(5, pixels.Color(80, 0, 200));
    pixels.setPixelColor(6, pixels.Color(100, 0, 200));
    pixels.setPixelColor(7, pixels.Color(120, 0, 200));
    pixels.setPixelColor(8, pixels.Color(140, 0, 200));
    pixels.setPixelColor(9, pixels.Color(160, 0, 200));
    pixels.setPixelColor(10, pixels.Color(180, 0, 200));
    pixels.setPixelColor(11, pixels.Color(200, 0, 200));
    pixels.setPixelColor(12, pixels.Color(220, 0, 200));
    pixels.show();    // neoPixels have been updated.
    colorUpdate = 1;  // Set the flag to 1; so they are not updated until a Mode change
  }
}

void setColorsMode1() {
  if (colorUpdate == 0) {
    pixels.setPixelColor(0, pixels.Color(51, 102, 0));
    pixels.setPixelColor(1, pixels.Color(0, 0, 150));
    pixels.setPixelColor(2, pixels.Color(0, 150, 0));
    pixels.setPixelColor(3, pixels.Color(150, 0, 0));
    pixels.setPixelColor(4, pixels.Color(220, 0, 200));
    pixels.setPixelColor(5, pixels.Color(150, 0, 150));
    pixels.setPixelColor(6, pixels.Color(150, 0, 150));
    pixels.setPixelColor(7, pixels.Color(80, 102, 0));
    pixels.setPixelColor(8, pixels.Color(80, 102, 0));
    pixels.setPixelColor(9, pixels.Color(5, 5, 100));
    pixels.setPixelColor(10, pixels.Color(5, 5, 100));
    pixels.setPixelColor(11, pixels.Color(102, 5, 0));
    pixels.setPixelColor(12, pixels.Color(80, 102, 0));
    pixels.show();
    colorUpdate = 1;
  }
}

//=============== Encoder Assignments ===========================================
//this section allows a unique encoder function for each mode.
//===============================================================================

void encoderA_Mode0() {  //volume
  long newPos = RotaryEncoderA.read() / 2;
  if (newPos != positionEncoderA && newPos < positionEncoderA) {
    positionEncoderA = newPos;
    //Decrease Volume
    Keyboard.print('a');
  }

  if (newPos != positionEncoderA && newPos > positionEncoderA) {
    positionEncoderA = newPos;
    //Increase Volume
    //Consumer.write(MEDIA_VOL_UP);
    Keyboard.print('b');
  }
}

void encoderA_Mode1() {
  long newPos = RotaryEncoderA.read() / 2;
  if (newPos != positionEncoderA && newPos < positionEncoderA) {
    positionEncoderA = newPos;
    //Zoom -
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('-');
    Keyboard.release('-');
    Keyboard.release(KEY_LEFT_CTRL);
  }

  if (newPos != positionEncoderA && newPos > positionEncoderA) {
    positionEncoderA = newPos;
    //Zoom +
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('=');
    Keyboard.release('=');
    Keyboard.release(KEY_LEFT_CTRL);
  }
}