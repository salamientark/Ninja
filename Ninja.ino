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
byte  EXTENDED_REGISTER = 0x00000000; // This variable will hold the state of the additional outputs controlled by the 74HC595


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
  pinMode(DIFFICULTY_5_LED_PIN, OUTPUT);
  pinMode(DIFFICULTY_4_LED_PIN, OUTPUT);
  pinMode(DIFFICULTY_3_LED_PIN, OUTPUT);
  pinMode(DIFFICULTY_2_LED_PIN, OUTPUT);
  pinMode(DIFFICULTY_1_LED_PIN, OUTPUT);

  pinMode(DATA_LOCK_PIN, OUTPUT);
  pinMode(DATA_SHIFT_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(DATA_OUTPUT_PIN, OUTPUT);

  // Variables initialization
  for (int i = 0; i < OBJ_NBR; i++)
    obj_list[i] = i + 1; // Fill with values 1 to OBJ_NBR
                         //
  // 74HC595 OUTPUTS
  outputDisable(); // Keep outputs disabled until data is ready
  digitalWrite(DATA_LOCK_PIN, LOW);
  digitalWrite(DATA_SHIFT_PIN, LOW);
  digitalWrite(DATA_PIN, LOW);

  extendedDigitalWrite(MAGNET_1_PIN, HIGH);
  extendedDigitalWrite(MAGNET_2_PIN, HIGH);
  extendedDigitalWrite(MAGNET_3_PIN, HIGH);
  extendedDigitalWrite(MAGNET_4_PIN, HIGH);
  extendedDigitalWrite(MAGNET_5_PIN, HIGH);
  extendedDigitalWrite(MAGNET_6_PIN, HIGH);
  extendedDigitalWrite(MAGNET_7_PIN, HIGH);
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
}

/* ************************************************************************** */
/*                              CUSTOM FUNCTIONS                              */
/* ************************************************************************** */


void  init_game() {
  // Initialize game state, reset variables, etc.
  for (int i = 0; i < OBJ_NBR; i++)
    obj_list[i] = i + 1; // Reset the object list to default values
  shuffleList(obj_list, OBJ_NBR); // Shuffle the list for a new game

  _difficulty = 2; // Reset difficulty to default
  Serial.println("--- New game initialized ---");

  // Activate magnet

}

void  game_loop() {
  int obj_index = 0;

  // 1. Shuffle the list of objects
  shuffleList(obj_list, OBJ_NBR);

  // 2. Display the objects in a random order (e.g., using LEDs or a screen)
  while (obj_index < _difficulty) {
    // Display obj_list[obj_index] (e.g., light up corresponding LED)
    // For demonstration, we'll just print it to the Serial Monitor
    Serial.print("Object: ");
    Serial.println(obj_list[obj_index]);
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

void extendedDigitalWrite(byte additionalOutputPin, bool newState) {

  // Mise à 1 ou 0 du bit (numéro de sortie) visé
  bitWrite(EXTENDED_REGISTER, additionalOutputPin, newState);

  // Et mise à jour des sorties du 74HC595
  sendByteToRegister(EXTENDED_REGISTER);
  
}


void outputEnable() {
  digitalWrite(DATA_OUTPUT_PIN, LOW);  // OE is active LOW
}

void outputDisable() {
  digitalWrite(DATA_OUTPUT_PIN, HIGH);
}

// ==================================
// Fonction : sendByteToRegister
// ==================================
void sendByteToRegister(byte byteToSend) {

  // Mise au niveau bas de la ligne de verrouillage (car ensuite, un front montant sur celle-ci induira un "transfert + verrouillage" des données en sortie)
  digitalWrite(DATA_LOCK_PIN, LOW);

  // Envoi des données
  shiftOut(DATA_PIN, DATA_SHIFT_PIN, MSBFIRST, byteToSend);

  // Mise au niveau haut de la ligne de verrouillage, pour générer un front montant sur cette ligne,
  // et ainsi, enclencher un "transfert + verrouillage" des données en sortie du 74HC595
  digitalWrite(DATA_LOCK_PIN, HIGH);

}
