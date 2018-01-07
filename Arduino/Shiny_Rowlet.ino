/*
Authors: Antonella Ruales and Joaquín Ruales

This program runs in the Arduino. It uses a servo motor to control the 3DS buttons and go through the game dialogs until the Rowlet naming screen appears. Then, it calls the host computer (which should be running the Python program) to ask it to take a screen capture with the webcam and determine if the Rowlet is shiny or not. If the Rowlet is shiny (this is a rare event), then the program stops. If the Rowlet is not shiny, then the Arduino performs a soft reset and loops back again.
*/

// include the Servo library
#include <Servo.h>

// create servo objects
Servo servoAButton;
Servo servoStartButton;

bool keepLooping = true;

void setup() {
  servoAButton.attach(9); // attaches the servo on pin 11 to the servo object
  servoStartButton.attach(10);  // attaches the servo on pin 13 to the servo object
  
  Serial.begin(9600); // open a serial connection to your computer
}

// Holds the A button down for the specified amount of milliseconds. pressDurationInMS should be at least 100 for best results.
void pressAButton(int pressDurationInMS) {
  servoAButton.write(140); // set servo angle
  delay(pressDurationInMS);   // wait
  servoAButton.write(117); // set servo angle
}

// Holds the Start button down for the specified amount of milliseconds. pressDurationInMS should be at least 100 for best results.
void pressStartButton(int pressDurationInMS) {
  servoStartButton.write(55); // set servo angle
  delay(pressDurationInMS);       // wait
  servoStartButton.write(73); // set servo angle
}

void loop() {
  delay(5000); // wait 5 seconds before start of program
  if (keepLooping) {
    Serial.println("Starting Loop");
    
    pressAButton(300); // skip title screen
    delay(2750);
    pressAButton(300); // select saved game
    delay(6400); // character walks to grass
    pressAButton(300); // skip text "Something is coming at you from the tall grass!"
    delay(9800); // Yungus appears
    pressAButton(300); // skip text "Yaaahhhh!!!"
    delay(11700); // starter Pokemons appear and defend character against Yungus. Camera moves to rowlet
    pressAButton(300); // skip text "Hooo!"
    delay(2500); // camera moves to Litten
    pressAButton(300); // skip text "Mraaarrr!"
    delay(2400); // camera moves to Poplio
    pressAButton(300); // skip text "Bwark! Bwark!"
    delay(2400); // camera moves to Yungus
    pressAButton(300); // skip text "Ya...yah..."
    delay(16900); // Yungus runs away. Camera shows professor
    Serial.println("Hey there, cousin!");
    pressAButton(300); // skip text "Hey there, cousin!"
    delay(2900);
    pressAButton(300); // skip text "Hang on, I know you... You're [Name], right?!"
    delay(700);
    pressAButton(300); // skip text "You OK? I saw you get attacked like that!"
    delay(300);
    pressAButton(300); // select option "Yeah, I got a bit of help"
    delay(2990);
    pressAButton(300); // skip text "Looks like you did! Though I sure didn't expect that, yeah..."
    delay(900);
    pressAButton(300); // skip text "I didn't think these little fellows would dash out on their own to help save a stranger!"
    delay(1090);
    pressAButton(300); // skip text "But I guess I'm getting ahead of myself"
    delay(720);
    pressAButton(300); // skip text "Welcome to Alola, the Pokemon paradise! That's what I'm supposed to be saying."
    delay(700);
    pressAButton(300); // skip text "I'm Kukui, yeah. The Pokemon Professor"
    delay(1420);
    pressAButton(300); // skip text "People and Pokemon live and work together here in Alola, too."
    delay(650);
    pressAButton(300); // skip text "Don't you ever forget..."
    delay(690);
    pressAButton(300); // skip text "You can go anywhere, yeah, as long as you got Pokemon in your corner!"
    delay(860);
    pressAButton(300); // skip text "Wild Pokemon could be lurking in the tall grass, yeah, just waiting to leap out and bite you, but"
    delay(600);
    pressAButton(300); // skip text "you'll be fine if you've got a partner of your own!"
    delay(700); // camera shows starter Pokemons
    pressAButton(300); // skip text "Hey there!"
    delay(670);
    pressAButton(300); // skip text "Here, let me introduce the Pokemon that helped you just now"
    delay(4690);
    pressAButton(300); // skip text "This is the Grass-type Pokemon Rowlet"
    delay(2590);
    Serial.println("Hoo!");
    pressAButton(300); // skip text "Hoo!"
    delay(1900);
    pressAButton(300); // skip text "And the Fire-type Pokemon Litten"
    delay(2220);
    Serial.println("Mrawr.");
    pressAButton(300); // skip text "Mrawr."
    delay(1680);
    pressAButton(300); // skip text "And the Water-type Pokemon Popplio"
    delay(2250);
    Serial.println("Bwark?");
    pressAButton(300); // skip text "Bwark?"
    delay(750);
    pressAButton(300); // skip text "Y'see, the fact of the matter is..."
    delay(690);
    pressAButton(300); // skip text "one of these Pokemon will become your first partner, woo!"
    delay(3140);
    pressAButton(300); // skip text "Heh... Maybe that's why they wanted to save you?"
    delay(800);
    pressAButton(300); // skip "Go on and choose the one you'd like to keep as your partner, cousin!"
    delay(5000); // camera moves to show 3 Pokemon
    pressAButton(300); // skip "Why don't you go ahead and choose the one you'd like to keep as your partner, cousin?"
    delay(300);
    pressAButton(300); // choose Rowlet as starter Pokemon
    delay(1320);
    pressAButton(300); // skip text "Hoo?"
    delay(1010);
    pressAButton(300); // skip text "Rowlet can fly up on its enemies real quiet like, yeah, and catch 'em by surprise"
    delay(830);
    pressAButton(300); // skip text "So, you wanna go with the Grass-type Pokemon Rowlet?"
    delay(300);
    pressAButton(300); // select "yes"
    delay(26660);
    pressAButton(300); // skip text "You chose Rowlet"
    delay(3000);
    
    Serial.println("Flushing leftover data");
    
    // flush the input buffer in case there is any leftover data
    while (Serial.available() != 0) {
      char t = Serial.read();
    }
    
    // Send message to Python
    Serial.println("Command: checkIfShiny");
    
    // Wait until we receive a message from Python
    while (Serial.available() == 0) {
      delay(200);
    }
    
    Serial.println("Reading response from Python");
    
    // Read response from Python
    char inByte = Serial.read();
    
    // Check if shiny
    if (inByte == 'y') {
      // it's shiny! Stop looping
      
      Serial.println("It's shiny!");
      keepLooping = false;
    } else {
      // not shiny. Do a soft reset and keep on looping!
      
      Serial.println("Not shiny. Doing soft reset");
      keepLooping = true;
      pressStartButton(300);
      
      // wait 10 seconds after soft reset
      delay(10000);
    }
  }
}
