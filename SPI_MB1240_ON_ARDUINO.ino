#include <SPI.h>

// Track if digital pins are set as input or output (saves reading registers) 
// Pins 0 to 9 can be used, pin 10 can be used for SS (but can be used if that's not required) 
bool pin_output[11];
double distance; 
double calibration = 0.0;

void setup() 
{
  // By default, all pins are inputs
  for (int i = 0; i < 11; i++) 
  {
    pin_output[i] = false;  
  }

  // Set the MISO pin as an output (Secondary Output)
  pinMode(MISO, OUTPUT);
  
  // Turn on SPI as a secondary device
  // Set appropriate bit in SPI Control Register
  SPCR |= _BV(SPE);
}

void loop() 
{
  char print_text[50];
  byte in_byte;

  // SPIF indicates transmission complete (byte received)
  if ((SPSR & (1 << SPIF)) != 0)
  {
    in_byte = SPDR;

    // If no action bit sent, then return the same value
    if (in_byte & 0x80) 
    {
      SPDR = in_byte;
    }
    // If write set
    else if (in_byte & 0x20) 
    {
      // Set digital pin output - use mask to extract pin and output
      if (set_digital_pin(in_byte & 0x0F, in_byte & 0x40) != 0xFF) 
      {
        SPDR = in_byte;
      }
    }    
    // If value is only pin numbers, perform digital read
    else if (in_byte < 0x10) 
    {
      byte return_val = read_digital_pin(in_byte);
      SPDR = return_val;
    }
    // Check for analog read (0x10 bit set and no write bit)
    else if ((in_byte & 0x10) && !(in_byte & 0x20)) 
    {
      byte return_val = read_analog_pin(in_byte & 0x0F);  // Address corresponds to analog pin
      SPDR = return_val;
    }
    else // Otherwise an error, return 0xFF
    {
      SPDR = 0xFF;
    }
  }
}

// Function to set a digital pin to high or low
byte set_digital_pin(byte address, bool high_value) 
{
  if (address < 0 || address > 10) return 0xFF;

  // If it's not set as output, set to output
  if (!pin_output[address]) 
  {
    pinMode(address, OUTPUT);
    pin_output[address] = true;
  }

  // Set to low if 0, otherwise set high
  if (high_value == 0) 
    digitalWrite(address, LOW);
  else 
    digitalWrite(address, HIGH);
}

// Function to read a digital pin (returns 1 for high, 0 for low)
byte read_digital_pin(byte address)
{
  if (address < 0 || address > 10) return 0xFF;

  // If it’s currently set as output, change it to input
  if (pin_output[address]) 
  {
    pinMode(address, INPUT);
    pin_output[address] = false;
  }

  return digitalRead(address);
}

// Function to read an analog pin (returns 0-255)
byte read_analog_pin(byte address)
{
  if (address < 0 || address > 5) return 0x00;  // Valid addresses are from 0 to 5 (A0 to A5)
  double voltage = analogRead(A3)*.0049;
  distance = voltage / 0.0049;  // Convert voltage to distance in cm
  // Analog value is between 0 and 1023, divide by 4 to map it to 0-255
  return (distance+calibration);
}
