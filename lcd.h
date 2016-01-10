#ifndef TIMER_LCD_H
#define TIMER_LCD_H

#include "config.h"

unsigned long last_draw = 0;

int8_t active_frame = 0,
  prev_frame = -1,
  next_frame = -1;

int8_t get_active_frame() {
  return active_frame;
}

int8_t get_prev_frame() {
  return prev_frame;
}

int8_t get_next_frame() {
  return next_frame;
}

void set_active_frame(int8_t frame) {
  active_frame = frame;
}

void set_prev_frame(int8_t frame) {
  prev_frame = frame;
}

void set_next_frame(int8_t frame) {
  next_frame = frame;
}

void set_last_draw() {
  last_draw = millis();
}

unsigned long get_last_draw() {
  return last_draw;
}

void lcd_print_two_digit(uint8_t num) {
  if (num < 10) {
    lcd.print('0');
  }
  lcd.print(num);
}

void lcd_clear_row(uint8_t row, uint8_t col_start, uint8_t cols) {
  lcd.setCursor(col_start, row);

  uint8_t i;
  for (i = 0; i < cols; ++i) {
    lcd.print(' ');
  }
  lcd.setCursor(col_start, row);
}

void lcd_clear_row(uint8_t row, uint8_t cols) {
  lcd_clear_row(row, 0, cols);
}

void lcd_clear_row(uint8_t row) {
  lcd_clear_row(row, 0, COLS);
}

uint8_t frame_needs_redraw() {
  if (active_frame != prev_frame) {
    return 1;
  }
  return 0;
}

void switch_frame(uint8_t frame) {
  next_frame = frame;
}

void frame_blink_text(unsigned long* blink_time, uint8_t* state,
  const char* text, uint8_t col, uint8_t row) {

  /* blink text every 500ms */
  if ((millis() - *blink_time) > 500) {
    *blink_time = millis();
    if (*state == 0) {
      *state = 1;
    } else if (*state == 1) {
      *state = 0;
    }
  }

  lcd.setCursor(col, row);
  size_t textlen = strlen(text);

  if (*state == 0) {
    lcd_clear_row(row);
  } else if (*state == 1) {
    lcd.print(text);
  }
}

void frame_blink_text(unsigned long* blink_time, uint8_t* state,
  uint8_t value, uint8_t col, uint8_t row) {

  /* blink text every 500ms */
  if ((millis() - *blink_time) > 500) {
   *blink_time = millis();
   if (*state == 0) {
     *state = 1;
   } else if (*state == 1) {
     *state = 0;
   }
  }

  lcd.setCursor(col, row);
  if (*state == 0) {
   lcd.print("  ");
  } else if (*state == 1) {
   lcd_print_two_digit(value);
  }
}

uint8_t frame_button_handle_enum(uint8_t* value, uint8_t* types, uint8_t types_len,
  uint8_t col, uint8_t row, uint8_t* current_state, uint8_t next_state) {

  uint8_t selected_index = 0, i;
  for (i = 0; i < types_len; ++i) {
    if (*value == types[i]) {
      selected_index = i;
      break;
    }
  }

  int8_t btn = button_handle();

  switch (btn) {
     case 0:
     case 1:
       // UP
       if (btn == 0) {
         if (selected_index == (types_len - 1)) {
          selected_index = 0;
         } else {
          selected_index = selected_index + 1;
         }
       } else if (btn == 1) {
        // DOWN
        if (selected_index == 0) {
          selected_index = types_len - 1;
        } else {
          selected_index = selected_index - 1;
        }
       }

       *value = types[selected_index];
     break;

     case 2:
       // SET
       *current_state = next_state;
       return 1;
     break;
   }

   return 0;
}

uint8_t frame_button_handle(uint8_t* value, uint8_t min, uint8_t max,
  uint8_t col, uint8_t row, uint8_t* current_state, uint8_t next_state) {

  int8_t btn = button_handle();

  switch (btn) {
     case 0:
     case 1:
       // UP
       if (btn == 0) {
         if (*value == max) {
          *value = min;
         } else {
          (*value)++;
         }
       } else if (btn == 1) {
        // DOWN
        if (*value == min) {
          *value = max;
        } else {
          (*value)--;
        }
       }
     break;

     case 2:
       // SET
       *current_state = next_state;
       lcd.setCursor(col, row);
       lcd_print_two_digit(*value);

       return 1;
     break;
   }

   return 0;
}

#endif
