
const int BASE_CAN_ID = 512; // base ID in ECU broadcast

// Data structure
struct sensorData
{
  char dataName[16];
  String units;
  float scaleMultiplier;
  int offset;
  float scaledValue;
  float minimum;
  float maximum;
  int canID;
  float alertHigh;
  float alertLow;
  bool useInt;
};



sensorData testData = {
  "Test", // Name
  "%@#",       // Units
  1,           // Multiplier for scaling
  0,           // Data Offset
  0,           // Scaled value
  10,           // Minimum Value
  100,         // Maximum Value
  (0),         // Address
  75,          // Alert High
  25,          // Alert Low
  false        // Show as int
};

sensorData noData = {
  "None",           // Name
  "",           // Units
  0,           // Multiplier for scaling
  0,           // Data Offset
  0,           // Scaled value
  0,           // Minimum Value
  100,         // Maximum Value
  (0),         // Address
  100,         // Alert High
  -100,         // Alert Low
  false        // Show as int
};

// -- ID 512 --
sensorData warningStatus = {
  "Warning",         // Name
  "",                // Units
  1,                 // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  100,               // Maximum Value
  (BASE_CAN_ID + 0), // Address
  0,
  0,
  false
};

int warningCounter;
int lastError;
int revLimit;
int mainRelay;
int fuelPump;
int checkEngine;
int egoHeater;

// -- ID 513 --

sensorData rpm = {
  "RPM",             // Name
  "",                // Units
  1,                 // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  7000,              // Maximum Value
  (BASE_CAN_ID + 1), // Address
  6800,              // Alert High
  -100,              // Alert Low
  true
};             // Show as int

sensorData ignitionTiming = {
  "Ign Timing",      // Name
  "o",               // Units
  0.02,              // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  -35,               // Minimum Value
  35,                // Maximum Value
  (BASE_CAN_ID + 1), // Address
  50,
  -50,
  true
};

sensorData injectorDuty = {
  "Injector Duty",   // Name
  "%",               // Units
  0.5,               // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Recieved value
  0,                 // Minimum Value
  100,               // Maximum Value
  (BASE_CAN_ID + 1), // Address
  90,
  -100,
  true
};

sensorData vss_mph = {
  "Speed",           // Name
  "mph",             // Units
  0.6213711922,      // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Recieved value
  0,                 // Minimum Value
  120,               // Maximum Value
  (BASE_CAN_ID + 1), // Address
  255,
  -255,
  true
};

sensorData vss = {
  "Speed",           // Name
  "kph",             // Units
  1,                 // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Recieved value
  0,                 // Minimum Value
  255,               // Maximum Value
  (BASE_CAN_ID + 1), // Address
  255,
  -255,
  true
};

// -- ID 514 --

sensorData accelerator = {
  "Accel",           // Name
  "%",               // Units
  0.01,              // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  100,               // Maximum Value
  (BASE_CAN_ID + 2), // Address
  200,
  -100,
  true
};

sensorData throttle1 = {
  "Throttle 1",      // Name
  "%",               // Units
  0.01,              // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  100,               // Maximum Value
  (BASE_CAN_ID + 2), // Address
  200,
  -200,
  true
};

sensorData throttle2 = {
  "Throttle 2",      // Name
  "%",               // Units
  0.01,              // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  100,               // Maximum Value
  (BASE_CAN_ID + 2), // Address
  200,
  -200,
  true
};

// -- ID 515 --

sensorData manifoldPressure_psi = {
  "MAP",                       // Name
  "psi",                       // Units
  ((0.033333) * 0.1450377377), // Multiplier for scaling
  0,                           // Scaled value
  0,                           // Data Offset
  0,                           // Minimum Value
  25,                          // Maximum Value
  (BASE_CAN_ID + 3),           // Address
  999,
  -999,
  false
};

sensorData manifoldPressure = {
  "MAP",             // Name
  "kpa",             // Units
  (0.033333),        // Multiplier for scaling
  0,                 // Scaled value
  0,                 // Data Offset
  0,                 // Minimum Value
  250,               // Maximum Value
  (BASE_CAN_ID + 3), // Address
  999,
  -999,
  false
};

// Convert from C to F
sensorData coolantTemperature_f = {
  "CLT",             // Name
  "F",               // Units
  1.8,               // Multiplier for scaling
  (32),              // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  120,               // Maximum Value
  (BASE_CAN_ID + 3), // Address
  250,
  50,
  true
};

// Convert from C to F
sensorData intakeTemperature_f = {
  "IAT",             // Name
  "F",               // Units
  1.8,               // Multiplier for scaling
  (32),              // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  55,                // Maximum Value
  (BASE_CAN_ID + 3), // Address
  120,
  50,
  true
};

sensorData coolantTemperature = {
  "CLT",             // Name
  "C",               // Units
  1,                 // Multiplier for scaling
  -40,               // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  120,               // Maximum Value
  (BASE_CAN_ID + 3), // Address
  120,
  10,
  true
};

sensorData intakeTemperature = {
  "IAT",             // Name
  "C",               // Units
  1,                 // Multiplier for scaling
  -40,               // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  55,                // Maximum Value
  (BASE_CAN_ID + 3), // Address
  100,
  10,
  true
};

sensorData auxTemp1 = {
  "Aux 1",           // Name
  "C",               // Units
  1,                 // Multiplier for scaling
  -40,               // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  200,               // Maximum Value
  (BASE_CAN_ID + 3), // Address
  100,
  10,
  true
};

sensorData auxTemp1_f = {
  "Aux 1",           // Name
  "C",               // Units
  1.8,               // Multiplier for scaling
  (-40),             // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  200,               // Maximum Value
  (BASE_CAN_ID + 3), // Address
  100,
  10,
  true
};

sensorData auxTemp2 = {
  "Aux 2",           // Name
  "C",               // Units
  1,                 // Multiplier for scaling
  -40,               // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  200,               // Maximum Value
  (BASE_CAN_ID + 3), // Address
  100,
  10,
  true
};

sensorData auxTemp2_f = {
  "Aux 2",           // Name
  "C",               // Units
  1.8,               // Multiplier for scaling
  -40,               // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  200,               // Maximum Value
  (BASE_CAN_ID + 3), // Address
  100,
  10,
  true
};

sensorData mcuTemp = {
  "MCU",             // Name
  "C",               // Units
  1,                 // Multiplier for scaling
  -40,               // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  200,               // Maximum Value
  (BASE_CAN_ID + 3), // Address
  80,
  -10,
  true
};

sensorData fuel = {
  "Fuel",            // Name
  "%",               // Units
  0.5,               // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  100,               // Maximum Value
  (BASE_CAN_ID + 3), // Address
  200,
  15,
  true
};

// -- ID 516 --
sensorData afr = {
  "AFR",            // Name
  "",                // Units
  0.001,             // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  10,                // Mimumum Value
  20,                // Maximum Value
  (BASE_CAN_ID + 4), // Address
  28,
  -10,
  false
};

sensorData oilPressure_psi = {
  "Oil",                     // Name
  "psi",                     // Units
  (0.033333) * 0.1450377377, // Multiplier for scaling
  0,                         // Data Offset
  0,                         // Scaled value
  0,                         // Minimum Value
  100,                       // Maximum Value
  (BASE_CAN_ID + 4),         // Address
  999,
  -999,
  false
};

sensorData oilPressure = {
  "Oil",             // Name
  "kpa",             // Units
  (0.033333),        // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  100,               // Maximum Value
  (BASE_CAN_ID + 4), // Address
  999,
  -999,
  false
};

sensorData vvtPosition = {
  "VVT",             // Name
  "deg",             // Units
  0.02,              // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  -50,               // Minimum Value
  50,                // Maximum Value
  (BASE_CAN_ID + 4), // Address
  100,
  -100,
  false
};

sensorData batteryVoltage = {
  "Battery",         // Name
  "V",               // Units
  0.001,             // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  8,                 // Minimum Value
  16,                // Maximum Value
  (BASE_CAN_ID + 4), // Address
  15,
  11,
  false
};

// -- ID 517 --

sensorData airMass = {
  "Air",             // Name
  "mg",              // Units
  1,                 // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  1000,              // Maximum Value
  (BASE_CAN_ID + 5), // Address
  9999,
  -9999,
  false
};

sensorData estimatedAirflow = {
  "Air",             // Name
  "kg/h",            // Units
  0.01,              // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  100,               // Maximum Value
  (BASE_CAN_ID + 5), // Address
  999,
  -999,
  false
};

sensorData injectorPulse = {
  "Injector",        // Name
  "ms",              // Units
  0.003,             // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  100,               // Maximum Value
  (BASE_CAN_ID + 5), // Address
  999,
  -999,
  false
};

// NEW - Fueling 3
sensorData lambda1 = {
  "Lambda",          // Name
  "",                // Units
  (0.0001),          // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0.7,               // Minimum Value
  1.3,               // Maximum Value
  (BASE_CAN_ID + 7), // Address
  10,               // Alert High
  -10,              // Alert Low
  false
};

sensorData lambda2 = {
  "Lambda 2",        // Name
  "",                // Units
  (0.0001),          // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0.7,               // Minimum Value
  1.3,               // Maximum Value
  (BASE_CAN_ID + 7), // Address
  100,               // Alert High
  -100,              // Alert Low
  false              // Show as integer
};

sensorData fuelPressureLow = {
  "Fuel",            // Name
  "kpa",             // Units
  (0.033333),        // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  0,                 // Maximum Value
  (BASE_CAN_ID + 7), // Address
  999,               // Alert High
  -999,              // Alert Low
  false              // Show as integer
};

sensorData fuelPressureHigh = {
  "Fuel",            // Name
  "bar",             // Units
  0.1,               // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  0,                 // Maximum Value
  (BASE_CAN_ID + 7), // Address
  999,               // Alert High
  -999,              // Alert Low
  false              // Show as integer
};

// NEW CAMs
sensorData vvtIntake1 = {
  "InVVT 1",         // Name
  "o",               // Units
  0.02,              // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  0,                 // Maximum Value
  (BASE_CAN_ID + 8), // Address
  999,               // Alert High
  -999,              // Alert Low
  false              // Show as integer
};

sensorData vvtIntake2 = {
  "InVVT 2",         // Name
  "o",               // Units
  0.02,              // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  0,                 // Maximum Value
  (BASE_CAN_ID + 8), // Address
  999,               // Alert High
  -999,              // Alert Low
  false              // Show as integer
};

sensorData vvtExhaust1 = {
  "ExVVT 1",         // Name
  "o",               // Units
  0.02,              // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  0,                 // Maximum Value
  (BASE_CAN_ID + 8), // Address
  999,               // Alert High
  -999,              // Alert Low
  false              // Show as integer
};

sensorData vvtExhaust2 = {
  "ExVVT 2",         // Name
  "o",               // Units
  0.02,              // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  0,                 // Maximum Value
  (BASE_CAN_ID + 8), // Address
  999,               // Alert High
  -999,              // Alert Low
  false              // Show as integer
};

// NEW fueling 2

sensorData fuelConsumed = {
  "Fuel",            // Name
  "g",               // Units
  1,                 // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  0,                 // Maximum Value
  (BASE_CAN_ID + 6), // Address
  999,               // Alert High
  -999,              // Alert Low
  false              // Show as integer
};

sensorData fuelConsumption = {
  "Fuel",            // Name
  "g/s",             // Units
  0.005,             // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  0,                 // Maximum Value
  (BASE_CAN_ID + 6), // Address
  999,               // Alert High
  -999,              // Alert Low
  false              // Show as integer
};

sensorData fuelTrim = {
  "FuelTrim",        // Name
  "%",               // Units
  1,                 // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  0,                 // Maximum Value
  (BASE_CAN_ID + 6), // Address
  999,               // Alert High
  -999,              // Alert Low
  false              // Show as integer
};

sensorData ethanol = {
  "Ethanol",        // Name
  "%",               // Units
  1,                 // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  100,                 // Maximum Value
  (BASE_CAN_ID + 1), // Address
  999,               // Alert High
  -999,              // Alert Low
  true              // Show as integer
};
