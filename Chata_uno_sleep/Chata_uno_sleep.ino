/*
 * Low power libraries
 */
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

// Seconds to wait before a new sensor reading is logged.
#define WAKEUP_INTERVAL_SECONDS   120
#define MAX_SLEEP_ITERATIONS   WAKEUP_INTERVAL_SECONDS / 8
#define KEEP_AWAKE_FOR_SECONDS 120*60 // 2 hours

volatile bool watchdogActivated = false;
int sleepIterations = 0;

#define SETUP_PIN_OUT 12
#define SETUP_PIN_IN 13
#define POWER_PIN 7

/* 
 * 
 * Define watchdog timer interrupt. 
 */
ISR(WDT_vect)
{
  // Set the watchdog activated flag.
  // Note that you shouldn't do much work inside an interrupt handler.
  watchdogActivated = true;
}

void sleep()
{
  Serial.println("Going to sleep.");
  delay(500); // to give serial time to print
  
  // Set sleep to full power down.  Only external interrupts or 
  // the watchdog timer can wake the CPU!
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // Turn off the ADC while asleep.
  power_adc_disable();

  // Enable sleep and enter sleep mode.
  sleep_mode();

  // CPU is now asleep and program execution completely halts!
  // Once awake, execution will resume at this point.
  
  // When awake, disable sleep mode and turn on all devices.
  sleep_disable();
  power_all_enable();
}

void setup_sleep() {
  // Setup the watchdog timer to run an interrupt which
  // wakes the Arduino from sleep every 8 seconds.
  
  // Note that the default behavior of resetting the Arduino
  // with the watchdog will be disabled.
  
  // This next section of code is timing critical, so interrupts are disabled.
  // See more details of how to change the watchdog in the ATmega328P datasheet
  // around page 50, Watchdog Timer.
  noInterrupts();
  
  // Set the watchdog reset bit in the MCU status register to 0.
  MCUSR &= ~(1<<WDRF);
  
  // Set WDCE and WDE bits in the watchdog control register.
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  // Set watchdog clock prescaler bits to a value of 8 seconds.
  WDTCSR = (1<<WDP0) | (1<<WDP3);
  
  // Enable watchdog as interrupt only (no reset).
  WDTCSR |= (1<<WDIE);
  
  // Enable interrupts again.
  interrupts();
  
  Serial.println("Setup complete.");
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(POWER_PIN, OUTPUT);
  pinMode(SETUP_PIN_OUT, OUTPUT);
  pinMode(SETUP_PIN_IN, INPUT);

  digitalWrite(POWER_PIN, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(SETUP_PIN_OUT,LOW);

  setup_sleep();
}

void loop() {
  // 1 = floating output, now connected
  // 0 = connected with SETUP_PIN_OUT
  int shouldSleep = digitalRead(SETUP_PIN_IN);

  if (shouldSleep) {
    // run the regular sleep code
    // put your main code here, to run repeatedly:
    // Don't do anything unless the watchdog timer interrupt has fired.
    if (watchdogActivated)
    {
      watchdogActivated = false;
      // Increase the count of sleep iterations and take a sensor
      // reading once the max number of iterations has been hit.
      sleepIterations += 1;
      if (sleepIterations >= MAX_SLEEP_ITERATIONS) {
        // Reset the number of sleep iterations.
        sleepIterations = 0;
        // Do stuff now
        run();
      }
    }
    
    // Go to sleep!
    sleep();
  } else {
    // turn the power on
    Serial.println("Keeping lights on.");
    digitalWrite(POWER_PIN, LOW); 
    delay(10000);
  }
}

void run() {
  Serial.println("Awaken.");
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on 
  digitalWrite(POWER_PIN, LOW);
  
  Serial.println("Do stuff...");
  // keep awake for a few seconds
  for (int i =0; i < KEEP_AWAKE_FOR_SECONDS; i++) {
    // delay 1s
    delay(1000);
    Serial.println("Doing things...");
  }
  Serial.println("Done!");

  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off 
  digitalWrite(POWER_PIN, HIGH);
}
