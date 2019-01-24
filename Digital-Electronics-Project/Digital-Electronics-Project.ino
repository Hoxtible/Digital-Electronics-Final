
// include the library code:
#include <LiquidCrystal.h>// Library for LCD
#include <Stepper.h> // Library for Stepper
#include <IRremote.h> // Library for IRremote


// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;// Setting Up LCD Pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); //Setting up LCD
const int stepsPerRevolution = 2048;// Setting Up stepsPerRevoluton, how many steps per one rotation
Stepper myStepper(stepsPerRevolution, 7,8,9,10);// Setting up Stepper Motor
int maxLength, minLength, currentLength;// Setting Up Nessecary Ints

int receiver = 53;
IRrecv irrecv(receiver); // Setting Up IR Receiver
decode_results results; 
int resetTimer = 0;

void setup() {
  Serial.begin(9600);// Opening Serial Port For Serial Monitior
  
  myStepper.setSpeed(4);/ Setting Stepper Speed
  
  lcd.begin(16, 2);// set up the LCD's number of columns and rows:
  
  lcd.print("Curtain Length");// Print a message to the LCD.
  
  maxLength = 100;// Configuring Maximums and Minimums for Curtain Lengths
  minLength = 10;
  currentLength = 60;
  irrecv.enableIRIn();// Starting IR receiver
  irrecv.blink13(true);
  pinMode(49, OUTPUT);// Enabling LED
 
  
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  digitalWrite(49, HIGH);// Turning off LED
  Serial.print(currentLength);// Displaying Length of Curtain in Console
  currentLengthChecks();// Checks the Current Length of the Curtain
  remoteTurns(); // Detects input coming from the remote
  joystickTurns();// Detects input coming from the Joystick
  screenConfig();// Displays text on the screen
  
}
void currentLengthChecks(){
  // Checks Bounds of the length, stops it if its too big or small.
  if(currentLength<minLength){
    currentLength = minLength;
  }
  if(currentLength>maxLength){
    currentLength = maxLength;
  }
  
}
void screenConfig(){
  //Displays text on screen
  lcd.setCursor(0,0);
  //Displays header
  lcd.print("Curtain Length");
  lcd.setCursor(0, 1);
  // print the curtain Length and where it is
  // Displays where the curtain is.
  lcd.print("[");
  digitalWrite(49, HIGH);
   
  
  
  for (int i = 0; i < (currentLength-minLength)/ (maxLength / 15);  i++) {
    lcd.setCursor(i+1, 2);
    lcd.print("~");
    
    
  }
  //Resets the screen every 1/10th of a second
  if(resetTimer == 100){
    resetTimer = 0;
    lcd.clear();
  }
  //Completes the display
  lcd.setCursor(15, 2);
  lcd.print("]");
  //Adds another second to the timer
  resetTimer++;
}
void rotate(int numberOfRotations) {
  //Translates number of rotations into number of Steps
  Serial.println(numberOfRotations*stepsPerRevolution);
  myStepper.step(numberOfRotations*stepsPerRevolution);
  //Turns on Warning LED
  digitalWrite(49, LOW);
  // Adds to counter
    currentLength = numberOfRotations*16 + currentLength;
  //Delays any other Input
  delay(15000*numberOfRotations);
  
}
void remoteTurns(){
   
  if (irrecv.decode(&results)) // have we received an IR signal?

  {
    Serial.println(results.value, HEX);// Prints IR signal Received
    switch(results.value) 

    {

      case 0xFF30CF: // UP button pressed
                      myStepper.setSpeed(4);// Sets the speed
                      rotate(1);// Rotates the motor
                      Serial.println("Moving Up");
                     
                      break;

      case 0xFF18E7: // DOWN button pressed
                      myStepper.setSpeed(4);// Sets the speed
                      rotate(-1);// Rotates the motor
                      Serial.println("Moving Down");// Prints the status
                      break;
                      
    }
    
      irrecv.resume(); // receive the next value
  }  
}
void joystickTurns(){
  //Detects input from Joystick
  int x = analogRead(11);
  Serial.println(x);
  //Turns Joystick motion into curtain motion
  if(x>600){
    myStepper.step(1);
    currentLength = 1/128 +currentLength;
  }
  //Deadzone is between 600 and 400
  if(x<400){
    myStepper.step(-1);
    currentLength = -1/128 +currentLength;
    
  }
  
}
