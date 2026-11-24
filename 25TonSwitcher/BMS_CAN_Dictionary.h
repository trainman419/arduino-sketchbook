#pragma once
#include <stdint.h>
#include <FlexCAN_T4.h>

#define ARR_8_TO_U16(arr,idx) \
          static_cast<uint16_t>(\
            static_cast<uint16_t>(arr[idx]) \
          | static_cast<uint16_t>(arr[idx+1] << 8))

#define ARR_8_TO_S16(arr,idx) \
          static_cast<int16_t>(\
            static_cast<int16_t>(arr[idx]) \
          | static_cast<int16_t>(arr[idx+1] << 8))

const int BATTERTY_STATUS_CAN_ID{0x2F4};
struct BatteryStatus_t {
  uint16_t voltage_mv;
  uint16_t current_ma;
  uint8_t state_of_charge_percent;
  uint16_t discharge_time_h;
};

BatteryStatus_t toBatteryStatus(const CAN_message_t& src){
  //Caller is expected to verify that ID and length are correct
  BatteryStatus_t dst;
  dst.voltage_mv = ARR_8_TO_U16(src.buf,0)*100;
  dst.current_ma = ARR_8_TO_U16(src.buf,2)*100 - 400'000;
  dst.state_of_charge_percent = src.buf[4];
  dst.discharge_time_h = ARR_8_TO_U16(src.buf,5);
  return dst;
}

const int CELL_VOLTAGE_CAN_ID{0x4F4};
struct CellVoltage_t {
  uint16_t max_cell_voltage_mv;
  uint8_t highest_monomer_position;
  uint16_t min_cell_voltage_mv;
  uint8_t lowest_monomer_position;
};

CellVoltage_t toCellVoltage(const CAN_message_t& src){
  //Caller is expected to verify that ID and length are correct
  CellVoltage_t dst;
  dst.max_cell_voltage_mv = ARR_8_TO_U16(src.buf,0);
  dst.highest_monomer_position = src.buf[2];
  dst.min_cell_voltage_mv = ARR_8_TO_U16(src.buf,3);
  dst.lowest_monomer_position = src.buf[5];
  return dst;
}

const int CELL_TEMPERATURE_CAN_ID{0x5F4};
struct CellTemperature_t {
  int16_t max_cell_temperature_c;
  uint8_t max_cell_temperature_position;
  int16_t min_cell_temperature_c;
  uint8_t min_cell_temperature_position;
  int16_t average_cell_temperature;
};

CellTemperature_t toCellTemperature(const CAN_message_t& src){
  //Caller is expected to verify that ID and length are correct
  CellTemperature_t dst;
  dst.max_cell_temperature_c = src.buf[0] - 50;
  dst.max_cell_temperature_position = src.buf[1];
  dst.min_cell_temperature_c = src.buf[2] - 50;
  dst.min_cell_temperature_position = src.buf[3];
  dst.average_cell_temperature = src.buf[4] - 50;
  return dst;
}

