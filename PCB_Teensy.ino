#include <AD75019.h>

// Define pins for the Teensy (adjust PCLK, SCLK, and SIN connections per switch)
#define PCLK 17
#define SCLK 19
#define SIN 18

// Define the number of switches
const int NUM_SWITCHES = 6;
const int Y_PINS_PER_SWITCH = 16;  // Each AD75019 has 16 Y pins

// Create an array of AD75019 instances for each switch
AD75019 xptSwitches[NUM_SWITCHES] = {
    AD75019(PCLK, SCLK, SIN),  // Adjust pins for each switch if needed
    AD75019(PCLK, SCLK, SIN),
    AD75019(PCLK, SCLK, SIN),
    AD75019(PCLK, SCLK, SIN),
    AD75019(PCLK, SCLK, SIN),
    AD75019(PCLK, SCLK, SIN)
};

// Example yMapping arrays for each switch if non-sequential (replace with actual mappings if needed)
uint8_t yMappings[NUM_SWITCHES][Y_PINS_PER_SWITCH] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}, // yMapping for switch 1
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}, // yMapping for switch 2
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}, // yMapping for switch 3
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}, // yMapping for switch 4
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}, // yMapping for switch 5
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}  // yMapping for switch 6
};

void setup() {
    Serial.begin(9600);
    delay(500);  // Allow time for the Serial connection to initialize
    Serial.println("Starting basic initialization without pin mappings...");

    bool initSuccessful = true;

    for (int i = 0; i < NUM_SWITCHES; i++) {
        Serial.print("Initializing switch ");
        Serial.println(i);
        if (!xptSwitches[i].begin()) {
            Serial.print("Initialization failed for switch ");
            Serial.println(i);
            initSuccessful = false;
            break;
        }
        xptSwitches[i].clear();  // Clear the config buffer to reset any previous state
    }

    if (initSuccessful) {
        Serial.println("All AD75019 switches initialized successfully without mappings.");
    } else {
        Serial.println("AD75019 initialization failed. Halting.");
        while (1);  // Stop execution to prevent reinitialization loop
    }
}

// Function to connect an X pin to a Y pin across multiple switches
void connectXY(int x, int y) {
    int switchIndex = y / Y_PINS_PER_SWITCH;       // Determine the switch index (0 to 5)
    int localY = y % Y_PINS_PER_SWITCH;            // Determine the Y pin within the switch (0 to 15)

    if (switchIndex >= 0 && switchIndex < NUM_SWITCHES) {
        xptSwitches[switchIndex].addRoute(x, localY);
        xptSwitches[switchIndex].flush();  // Apply the route configuration

        Serial.print("Connected X");
        Serial.print(x);
        Serial.print(" to Y");
        Serial.print(y);
        Serial.print(" on switch ");
        Serial.println(switchIndex);
    } else {
        Serial.println("Invalid Y pin. Please enter a Y value between 0 and 95.");
    }
}

void loop() {
    if (Serial.available() > 0) {
        // Read the entire input line as a single string
        String input = Serial.readStringUntil('\n');

        // Split input by each connection pair, separated by semicolons
        int lastIndex = 0;
        while (lastIndex != -1) {
            int separatorIndex = input.indexOf(';', lastIndex);

            String pair;
            if (separatorIndex == -1) {
                pair = input.substring(lastIndex);  // Last pair
                lastIndex = -1;
            } else {
                pair = input.substring(lastIndex, separatorIndex);  // Get pair between separators
                lastIndex = separatorIndex + 1;
            }

            // Trim any spaces and split by comma to get X and Y values
            pair.trim();
            int commaIndex = pair.indexOf(',');
            if (commaIndex != -1) {
                int x = pair.substring(0, commaIndex).toInt();   // Extract X value
                int y = pair.substring(commaIndex + 1).toInt();  // Extract Y value

                // Validate input range and connect
                if (x >= 0 && x < 16 && y >= 0 && y < (NUM_SWITCHES * Y_PINS_PER_SWITCH)) {
                    connectXY(x, y);  // Connect the X and Y pins
                } else {
                    Serial.println("Invalid input. Please enter X value (0-15) and Y value (0-95).");
                }
            }
        }
    }
}
