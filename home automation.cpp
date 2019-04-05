
//RFID IRQ->   Not used. Leave open
//RFID MISO->   SPI4_MISO=PA_6
//RFID MOSI->   SPI4_MOSI=PA_7
//RFID SCK=->   SPI4_SCLK =PA_5
//RFID SDA ->   SPI4_SSEL =PA_4
//RFID RST=pin7    ->  NRST
//3.3V and Gnd to the respective pins

#include "mbed.h"
#include "MFRC522.h"
#include "Servo.h"


#define SPI4_MISO PA_6
#define SPI4_MOSI PA_7
#define SPI4_SCLK PA_5
#define SPI4_SSEL PA_4
#define RESET PC_15

DigitalOut Match(PG_2);
DigitalOut NoMatch(PG_5);
Servo servoPin(PA_8);
AnalogIn LDR(PF_5);
DigitalOut batti(PF_2);
DigitalOut buzzer(PE_9);

MFRC522    RfChip(SPI4_MOSI, SPI4_MISO, SPI4_SCLK, SPI4_SSEL,RESET);

int cardNo[]= {4,208,5,5};
int newCard[4];
int check=0;
int doorOpen=0;


int main(void)
{
    printf("The program has started...\n");

    // Init. RC522 Chip
    RfChip.PCD_Init();

    while (true) {
        //  LedGreen = 1;

        // Look for new cards
        if ( ! RfChip.PICC_IsNewCardPresent()) {
            wait_ms(500);
            continue;
        }

        // Select one of the cards
        if ( ! RfChip.PICC_ReadCardSerial()) {
            wait_ms(500);
            continue;
        }

        //LedGreen = 0;

        // Print Card UID
        printf("Card UID: ");
        for (uint8_t i = 0; i < RfChip.uid.size; i++) {
            newCard[i]=RfChip.uid.uidByte[i];
            //printf(" %d ", RfChip.uid.uidByte[i]);
            printf("%d",newCard[i]);
        }
        for(int a=0; a<4; a++) {
            if(newCard[a]==cardNo[a]) {

                check=1;
            } else {
                check=0;
                break;
            }

        }
        printf("\n\r");
        if(check==1) {
            Match=1;
            doorOpen++;
            printf("%s %d","match=",doorOpen);
            wait(1);
            Match=0;
        } else {
            NoMatch=1;
            printf("%s","no match");
            wait(1);
            NoMatch=0;
        }
        printf("\n\r");
        if(doorOpen!=0 && (doorOpen%2)==1 && check==1) {
            printf("%s","door opened");
            printf("\n\r");
            for(float p=0.0; p<1.0; p += 0.1) {
                servoPin = p;
                // wait(0.2);
            }
            int temp=LDR.read_u16()/64;
            printf("\r %s %u\n","ldr value=", temp);
            if(temp>10) {
                batti=1;
            } else {
                batti=0;
            }
        } else if(doorOpen!=0 && (doorOpen%2)==0 && check==1) {
            for(float p=1.0; p>0.0; p -= 0.2) {
                servoPin = p;
                //wait(0.2);
                batti=0;
            }
            printf("%s","door closed");
        }

        else {
            printf("%s","Wrong card");
            int on = 1, off = 0;

            buzzer = on;
            wait(.5);
            buzzer = off;
            wait(.5);
            buzzer = on;
            wait(.5);
            buzzer = off;
            wait(.5);
            buzzer = on;
            wait(.5);
            buzzer = off;
            wait(.5);
        }
        printf("\n\r");

        wait_ms(1000);
    }
}