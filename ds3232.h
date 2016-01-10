#ifndef TIMER_RTC_H
#define TIMER_RTC_H

//http://github.com/JChristensen/DS3232RTC
#include <DS3232RTC.h>

static uint8_t __attribute__ ((noinline)) bcd2dec(uint8_t n) {
  return n - 6 * (n >> 4);
}

const char* alarm2s(ALARM_TYPES_t alarm) {
  switch (alarm) {
    case ALM2_EVERY_MINUTE:
      return "Every minute";
    case ALM2_MATCH_MINUTES:
      return "Match minute";
    case ALM2_MATCH_HOURS:
      return "Hour and minute";
  }

  return "Unknown";
}

uint8_t is_alarm_active(uint8_t alarm_number) {
  uint8_t controlReg, mask;

  controlReg = RTC.readRTC(RTC_CONTROL);
  mask = _BV(A1IE) << (alarm_number - 1);

  if (controlReg & mask) {
    return 1;
  }

  return 0;
}

void setAlarm(uint8_t alarm_number, alarm_t* alarm) {
  RTC.setAlarm(alarm->type, alarm->seconds, alarm->minutes, alarm->hours, 0);
  RTC.alarmInterrupt(alarm_number, true);
}

/*
  assuming 24 hour clock.
  does not fetch day/date
 */
void readAlarm(uint8_t alarm_number, alarm_t* alarm) {
  uint8_t addr;

  if (alarm_number == ALARM_1) {
    addr = ALM1_SECONDS;
  } else {
    addr = ALM2_MINUTES;
  }

  uint8_t seconds = 0, minutes, hour, date, alarm_type = 0;

  if (alarm_number == ALARM_1) {
    RTC.readRTC(addr++, &seconds, 1);
  }

  RTC.readRTC(addr, &minutes, 1);
  RTC.readRTC(addr + 1, &hour, 1);
  RTC.readRTC(addr + 2, &date, 1);

  /* get the 8th bit and shift it right to 2^4 */
  alarm_type = (date & (1 << 7)) >> 4;
  /* get the 8th bit and shift it right to 2^3 */
  alarm_type = alarm_type | ((hour & (1 << 7)) >> 5);
  /* get the 8th bit and shift it right to 2^2 */
  alarm_type = alarm_type | ((minutes & (1 << 7)) >> 6);
  /* get the 8th bit */
  alarm_type = alarm_type | ((seconds & (1 << 7)) >> 7);

  if (alarm_number == ALARM_2) {
    /* shift one bit to the right, removing seconds */
    alarm_type = alarm_type >> 1;
  }

  /* does not fetch DY/DT, thus ignoring ALM2_MATCH_DAY and ALM2_MATCH_DATE */
  switch (alarm_type) {
    case 16:
      alarm->type = ALM1_MATCH_DAY;
    case 15:
      alarm->type = ALM1_EVERY_SECOND;
      break;
    case 14:
      alarm->type = ALM1_MATCH_SECONDS;
      break;
    case 12:
      alarm->type = ALM1_MATCH_MINUTES;
      break;
    case 8:
      if (alarm_number == ALARM_1) {
        alarm->type = ALM1_MATCH_HOURS;
      } else {
        alarm->type = ALM2_MATCH_DAY;
      }
      break;
    case 7:
      alarm->type = ALM2_EVERY_MINUTE;
      break;
    case 6:
      alarm->type = ALM2_MATCH_MINUTES;
      break;
    case 4:
      alarm->type = ALM2_MATCH_HOURS;
      break;
    case 0:
      if (alarm_number == ALARM_1) {
        alarm->type = ALM1_MATCH_DATE;
      } else {
        alarm->type = ALM2_MATCH_DATE;
      }
      break;
  }

  alarm->alarm = alarm_number;
  /* ignoring 8th bit */
  alarm->seconds = bcd2dec(seconds & ~(1 << 7));
  alarm->minutes = bcd2dec(minutes & ~(1 << 7));
  /* ignoring 8th and 7th bit */
  alarm->hours = bcd2dec(hour & ((1 << 6) - 1));
}

#endif
