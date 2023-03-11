
const int BASE_CAN_ID = 512; // base ID in ECU broadcast

// Data structure
struct sensorData
{
  String dataName;
  String units;
  float scaleMultiplier;
  int offset;
  float scaledValue;
  float minimum;
  float maximum;
  int canID;
  float alertHigh;
  float alertLow;
  int precision;
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
  0           // Number of decimal places
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
  0        // Show as int
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
  100,
  0,
  0
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
  0
};             

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
  2
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
  0
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
  0
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
  0
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
  0
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
  0
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
  0
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
  2
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
  2
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
  0
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
  0
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
  0
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
  0
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
  0
};

sensorData auxTemp1_f = {
  "Aux 1",           // Name
  "F",               // Units
  1.8,               // Multiplier for scaling
  32,             // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  200,               // Maximum Value
  (BASE_CAN_ID + 3), // Address
  100,
  10,
  0
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
  0
};

sensorData auxTemp2_f = {
  "Aux 2",           // Name
  "F",               // Units
  1.8,               // Multiplier for scaling
  32,               // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  200,               // Maximum Value
  (BASE_CAN_ID + 3), // Address
  100,
  10,
  0
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
  0
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
  0
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
  2
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
  0
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
  0
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
  2
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
  2
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
  0
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
  0
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
  2
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
  2
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
  2              // Show as integer
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
  2              // Show as integer
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
  1              // Show as integer
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
  2             // Show as integer
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
  2              // Show as integer
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
  2              // Show as integer
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
  2              // Show as integer
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
  0              // Show as integer
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
  2              // Show as integer
};

sensorData fuelTrim = {
  "FuelTrim",        // Name
  "%",               // Units
  0.001,              // Multiplier for scaling
  0,                 // Data Offset
  0,                 // Scaled value
  0,                 // Minimum Value
  0,                 // Maximum Value
  (BASE_CAN_ID + 6), // Address
  999,               // Alert High
  -999,              // Alert Low
  2              // Show as integer
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
  0            // Show as integer
};
