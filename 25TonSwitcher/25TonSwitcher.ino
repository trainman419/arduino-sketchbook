#include <FlexCAN_T4.h>

#include "BMS_CAN_Dictionary.h"

const int LOOP_LED_PIN{ 13 };

const int MOTOR_CAN_TX_PIN{ 22 };
const int MOTOR_CAN_RX_PIN{ 23 };
const int MOTOR_CAN_STANDBY_PIN{ 19 };

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> motorCan;

void canCallback(const CAN_message_t& msg);

void setup() {
  Serial.printf("Setting up...\n");

  pinMode(LOOP_LED_PIN, OUTPUT);
  pinMode(MOTOR_CAN_TX_PIN, OUTPUT);
  pinMode(MOTOR_CAN_RX_PIN, INPUT);
  pinMode(MOTOR_CAN_STANDBY_PIN, OUTPUT);
  digitalWrite(MOTOR_CAN_STANDBY_PIN, LOW);  //LOW=normal, HIGH=standby
  digitalWrite(MOTOR_CAN_TX_PIN, HIGH);      //"Normal" mode

  motorCan.begin();
  motorCan.setBaudRate(500'000);
  //motorCan.setMaxMB(16);
  motorCan.enableFIFO();
  motorCan.enableFIFOInterrupt();
  motorCan.onReceive(canCallback);

  Serial.printf("Setting complete!\n");
}

void canCallback(const CAN_message_t &message) {
  Serial.printf("RX@%d ID:%03x filter:%03x mailbox:%d bus:%d flags:%02x len:%d buf:",
                message.timestamp,
                message.id,
                message.idhit,
                message.mb,
                message.bus,
                message.flags,
                message.len);
  for(int i = 0; i < message.len; ++i){
    Serial.printf("%02x",message.buf[i]);
  }
  Serial.printf("\n");

  switch(message.id){
    case BATTERTY_STATUS_CAN_ID:
    {
      auto battery_status = toBatteryStatus(message);
      Serial.printf("%0.3fV %0.3fA %d%% %dh\n",
                    battery_status.voltage_mv/1000.,
                    battery_status.current_ma/1000.,
                    battery_status.state_of_charge_percent,
                    battery_status.discharge_time_h);
      break;
    }
    case CELL_VOLTAGE_CAN_ID:
    {
      auto cell_voltage = toCellVoltage(message);
      Serial.printf("Max:%0.3fV@%d Min:%0.3fV@%d\n",
                    cell_voltage.max_cell_voltage_mv/1000.,
                    cell_voltage.highest_monomer_position,
                    cell_voltage.min_cell_voltage_mv/1000.,
                    cell_voltage.lowest_monomer_position);
      break;
    }
    default:
    {
      Serial.printf("ID unknown\n");
      break;
    }
  }
}

void commandMotorRpm(int rpm){
  uint8_t* rpm_buf = reinterpret_cast<uint8_t*>(&rpm);
  const int RPM_CAN_ID{0};
  const int SSR{0};
  const int FRAME_TYPE{0};
  const int COMMAND_SET_RPM{3};
  const int MOTOR_ID{0xC};
  CAN_message_t rpm_message;
  rpm_message.id = (RPM_CAN_ID << 18) | (SSR << 17) | (FRAME_TYPE << 18) | (COMMAND_SET_RPM << 8) | MOTOR_ID;
  rpm_message.flags.extended = 1;
  rpm_message.flags.remote = 0;
  rpm_message.flags.overrun = 0;
  rpm_message.flags.reserved = 0;
  rpm_message.len = 4;
  rpm_message.buf[0] = rpm_buf[3];
  rpm_message.buf[1] = rpm_buf[2];
  rpm_message.buf[2] = rpm_buf[1];
  rpm_message.buf[3] = rpm_buf[0];
  motorCan.write(rpm_message);
}

int loop_counter{ 0 };
void loop() {
  digitalWrite(LOOP_LED_PIN, HIGH);
  Serial.printf("Begin loop %d\n", loop_counter);

  CAN_message_t test_message;
  test_message.id = 0x7FF;
  test_message.len = 5;
  test_message.buf[0] = 0xC0;
  test_message.buf[1] = 0xFF;
  test_message.buf[2] = 0xEE;
  test_message.buf[3] = 0xCA;
  test_message.buf[4] = 0xFE;
  motorCan.write(test_message);
  commandMotorRpm(1200); // 10 gives actual ERPM of 37278

  /*if(motorCan.read(test_message)){
    Serial.printf("Read a message manually\n");
    canCallback(test_message);
  }*/

  ++loop_counter;
  delay(500);
  digitalWrite(LOOP_LED_PIN, LOW);
  delay(500);
}
