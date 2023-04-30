#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
MCP2515 mcp2515(10);

struct can_frame canMsg35;

bool speedSet = false;

canid_t SPEED_READ_ID = 0x82F83203;
canid_t SPEED_WRITE_ID = 0x85103203 | CAN_EFF_FLAG;


bool should_write = false; //change to true to write the new speed

void setup() {
  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_250KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();

  Serial.println("Setup ok");
  Serial.println();

  //Original read: 82F83203 6 C4 09 C0 2B CE 08 => C4 09 => 09C4 => 2500 => 25km/h
  //Original IVY:  82F83203 6 C4 09 C0 2B C4 08
  //Set:           85103203 6 AC 0D C0 2B CE 08 => AC 0D => 0DAC => 3500 => 35km/h
  //Set IVY        85103203 6 B8 0B C0 2B C4 08 => B8 0B => 0BB8 => 3000 => 30Km/h
  //New read:      82F83203 6 AC 0D C0 2B CE 08
  //New read IVY:  82F83203 6 B8 0B C0 2B C4 08
  
  canMsg35.can_id  = 0x85103203 | CAN_EFF_FLAG;
  canMsg35.can_dlc = 6;
  canMsg35.data[0] = 0xB8; //30km/h 
  canMsg35.data[1] = 0x0B; //30km/h
  canMsg35.data[2] = 0xC0;
  canMsg35.data[3] = 0x2B;
  canMsg35.data[4] = 0xC4; //ivy value
  canMsg35.data[5] = 0x08; //ivy value
}

uint32_t buffer = 0;
void loop() {
  unsigned long currentMillis = millis();
  
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
      if( ( canMsg.can_id == SPEED_READ_ID ) || ( canMsg.can_id == SPEED_WRITE_ID ) ){
        Serial.print(canMsg.can_id, HEX); //Print ID
        Serial.print(" "); 
        Serial.print(canMsg.can_dlc, HEX); //Print DLC
        Serial.print(" ");
        
        for (int i = 0; i<canMsg.can_dlc; i++)  { //Print the data
          Serial.print(canMsg.data[i],HEX);
          Serial.print(" ");
        }
        //olny works on seed_reed_mss
        if(canMsg.can_dlc == 6){
          buffer = 0;
          buffer = ( (uint32_t)canMsg.data[1] << 8 ) + canMsg.data[0];
          Serial.println();
          Serial.print("Speed setting: ");
          Serial.print(buffer, DEC);
          
          buffer = 0;
          buffer = ( (uint32_t)canMsg.data[3] << 8 ) + (canMsg.data[2]);
          Serial.println();
          Serial.print("Wheel Size setting: ");
          Serial.print(buffer, DEC);

          buffer = 0;
          buffer = ( (uint32_t)canMsg.data[5] << 8 ) + canMsg.data[4];
          Serial.println();
          Serial.print("Circumference setting: ");
          Serial.print(buffer, DEC);

          Serial.println();
        }
    
        Serial.println();
      }
  }

  //Write speed setting after 15 seconds waiting
  
  if(should_write && !speedSet && currentMillis > 15000){
    speedSet = true;
    Serial.println("------ WRITING SPEED ------");
    Serial.println("------ WRITING SPEED ------");
    Serial.println("------ WRITING SPEED ------");
    Serial.println("------ WRITING SPEED ------");
    Serial.println("------ WRITING SPEED ------");
    Serial.println("------ WRITING SPEED ------");
    mcp2515.sendMessage(MCP2515::TXB1, &canMsg35);
    Serial.println("----------- DONE ----------");
    Serial.println("----------- DONE ----------");
    Serial.println("----------- DONE ----------");
    Serial.println("----------- DONE ----------");
    Serial.println("----------- DONE ----------");
    Serial.println("----------- DONE ----------");
  }
  
}
