#include <cc1101.h>
#include <cc1101_custom_config.h>
#include <ccpacket.h>

#define RADIO_CHANNEL             16
#define THIS_DEVICE_ADDRESS       22
#define DESINATION_DEVICE_ADDRESS 3
#define GDO0_INTERRUPT_PIN 3

#define SEND_STUFF_PERIODICALLY     1
//#define SHOW_CC_STATUS_PERIODICALLY 1


CC1101 radio;

unsigned long lastSend = 0;
unsigned int sendDelay = 10000;

unsigned long lastStatusDump = 0;
unsigned int statusDelay = 5000;

String rec_payload;

void setup() {
  // put your setup code here, to run once:
  // Start Serial
    delay (100);
    Serial.begin(115200);
    delay (100);
    Serial.println("Starting...");

    //radio.enableSerialDebug();

    // Start RADIO
    while ( !radio.begin(CFREQ_868, KBPS_250, /* channel num */ 16, /* address */ THIS_DEVICE_ADDRESS, GDO0_INTERRUPT_PIN /* Interrupt */) ) // channel 16! Whitening enabled 
    {
        delay(5000); // Try again in 5 seconds
    }   
    radio.printCConfigCheck();
    Serial.println(F("CC1101 radio initialized."));
    rec_payload.reserve(100);

    // IMPORTANT: Kick the radio into receive mode, otherwise it will sit IDLE and be TX only.
    radio.setRxState();
}

void loop() {
  // put your main code here, to run repeatedly:

}
