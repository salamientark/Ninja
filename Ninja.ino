#include "config.h"
#include "Button.h"
#include "menu.h"

/* ************************************************************************** */
/*                                GLOBAL & CONSTANTS                          */
/* ************************************************************************** */
// OBJ_NBR and pin/difficulty constants are defined in config.h


// Global variables
int   _difficulty = 2;
int   obj_list[OBJ_NBR];
byte  MAGNET_REGISTER     = 0b00000000;  // Chip 1: electromagnet outputs
byte  MAGNET_LED_REGISTER = 0b00000000;  // Chip 2: magnet indicator LEDs


unsigned long T1 = 0, T2 = 0;
uint8_t TimeInterval = 5; // 5ms

/* ************************************************************************** */
/*                                     SETUP                                  */
/* ************************************************************************** */

Button upButton(UP_BUTTON_PIN);
Button downButton(DOWN_BUTTON_PIN);
Button startButton(START_BUTTON_PIN);

void setup() {
  // SERIAL
  Serial.begin(9600);
  Serial.println("=== Ninja Game Starting ===");

  // RANDOM SEED
  randomSeed(analogRead(0));

  // INPUTS
  upButton.begin();
  downButton.begin();
  startButton.begin();

  // OUTPUTS
  pinMode(DATA_LOCK_PIN, OUTPUT);
  pinMode(DATA_SHIFT_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(DATA_OUTPUT_PIN, OUTPUT);

  pinMode(DIFFICULTY_5_LED_PIN, OUTPUT);
  pinMode(DIFFICULTY_4_LED_PIN, OUTPUT);
  pinMode(DIFFICULTY_3_LED_PIN, OUTPUT);
  pinMode(DIFFICULTY_2_LED_PIN, OUTPUT);
  pinMode(DIFFICULTY_1_LED_PIN, OUTPUT);

  // Variables initialization
  for (int i = 0; i < OBJ_NBR; i++)
    obj_list[i] = i + 1; // Fill with values 1 to OBJ_NBR
                         //
  // 74HC595 OUTPUTS
  outputDisable(); // Keep outputs disabled until data is ready
  digitalWrite(DATA_LOCK_PIN, LOW);
  digitalWrite(DATA_SHIFT_PIN, LOW);
  digitalWrite(DATA_PIN, LOW);

  magnetWrite(MAGNET_1_PIN, HIGH);
  magnetWrite(MAGNET_2_PIN, HIGH);
  magnetWrite(MAGNET_3_PIN, HIGH);
  magnetWrite(MAGNET_4_PIN, HIGH);
  magnetWrite(MAGNET_5_PIN, HIGH);
  magnetWrite(MAGNET_6_PIN, HIGH);
  magnetWrite(MAGNET_7_PIN, HIGH);
  outputEnable(); // Data is latched, now enable outputs
}

/* ************************************************************************** */
/*                                 MAIN LOOP                                  */
/* ************************************************************************** */

void loop() {
  // 1. Setup initial state
  init_game();

  // 2. Show menu and allow difficulty selection
  menu_loop();

  // 3. Switch off menu LED
  for (int i = 0; i < DIFFICULTY_MAX; i++)
    digitalWrite(DIFFICULTY_1_LED_PIN + i, LOW);

  delay(1000); // Small delay to avoid bouncing issues when starting the game

  // 4. Game loop
  game_loop();
  // extendedPinSequence();
}

/* ************************************************************************** */
/*                              CUSTOM FUNCTIONS                              */
/* ************************************************************************** */


void  init_game() {
  // Initialize game state, reset variables, etc.
  for (int i = 0; i < OBJ_NBR; i++)
    obj_list[i] = i + 1; // Reset the object list to default values
  // shuffleList(obj_list, OBJ_NBR); // Shuffle the list for a new game

  _difficulty = 2; // Reset difficulty to default
  Serial.println("--- New game initialized ---");

  // Activate magnets
  MAGNET_REGISTER = 0b11111111; // All magnets on
  MAGNET_LED_REGISTER = 0b11111111; // All LEDs on
  sendRegisters();
}

void  game_loop() {
  int obj_index = 0;

  // 1. Shuffle the list of objects
  shuffleList(obj_list, OBJ_NBR);

  // 2. Display the objects in a random order (e.g., using LEDs or a screen)
  while (obj_index < OBJ_NBR) {
    // Display obj_list[obj_index] (e.g., light up corresponding LED)
    // For demonstration, we'll just print it to the Serial Monitor
    Serial.print("Object: ");
    Serial.println(obj_list[obj_index]);
    offMagnet(obj_list[obj_index] - 1); // Turn off the magnet corresponding to the displayed object (assuming obj_list values are 1-indexed)
    offMagnetLED(obj_list[obj_index] - 1); // Turn off the corresponding LED
    delay(1000); // Wait for a moment before showing the next object
    obj_index++;
  }
}

bool debounce(int pin)
{
  static uint16_t btnState = 0;
  btnState = (btnState<<1) | (!digitalRead(pin));
  return (btnState == 0xFFF0);
}

// 4. CUSTOM FUNCTION
void shuffleList(int arrayToShuffle[], int size) {
  // Loop backward through the array
  for (int i = size - 1; i > 0; i--) {
    // Pick a random index from 0 to i
    int j = random(0, i + 1); 
    
    // Swap the elements
    int temp = arrayToShuffle[i];
    arrayToShuffle[i] = arrayToShuffle[j];
    arrayToShuffle[j] = temp;
  }
}

void extendedPinSequence() {
  Serial.println("Sequence: pin1 ON");
  magnetWrite(MAGNET_1_PIN, HIGH);
  delay(1000);
  magnetWrite(MAGNET_1_PIN, LOW);

  Serial.println("Sequence: pin2 ON");
  magnetWrite(MAGNET_2_PIN, HIGH);
  delay(1000);
  magnetWrite(MAGNET_2_PIN, LOW);

  Serial.println("Sequence: pin3 ON");
  magnetWrite(MAGNET_3_PIN, HIGH);
  delay(1000);
  magnetWrite(MAGNET_3_PIN, LOW);

  Serial.println("Sequence: done");
}

void magnetWrite(byte pin, bool state) {
  bitWrite(MAGNET_REGISTER, pin, state);
  sendRegisters();
}

void magnetLedWrite(byte pin, bool state) {
  bitWrite(MAGNET_LED_REGISTER, pin, state);
  sendRegisters();
}

void  offMagnet(byte magnetPin) {
  bitClear(MAGNET_REGISTER, magnetPin);
  sendRegisters();
}

void  offMagnetLED(byte magnetLEDPin) {
  bitClear(MAGNET_LED_REGISTER, magnetLEDPin);
  sendRegisters();
}


void outputEnable() {
  digitalWrite(DATA_OUTPUT_PIN, LOW);  // OE is active LOW
}

void outputDisable() {
  digitalWrite(DATA_OUTPUT_PIN, HIGH);
}

// ==================================
// Fonction : sendRegisters
// Sends both chip registers in one latch cycle.
// Chip 2 (LED) is shifted first so it cascades through chip 1's Q7'.
// Chip 1 (magnets) is shifted second and stays in chip 1.
// ==================================
void sendRegisters() {
  digitalWrite(DATA_LOCK_PIN, LOW);
  shiftOut(DATA_PIN, DATA_SHIFT_PIN, MSBFIRST, MAGNET_LED_REGISTER);  // Chip 2 (cascaded)
  shiftOut(DATA_PIN, DATA_SHIFT_PIN, MSBFIRST, MAGNET_REGISTER);      // Chip 1
  digitalWrite(DATA_LOCK_PIN, HIGH);  // Single latch — both chips update simultaneously
}
