/*
 * Author: Aurelio Colosimo, 2021
 * Copyright: Elo System srl
 */

void lcd_init();

void lcd_write(u8 ch, int isdata);

void lcd_write_string(const char *str, int line);

void lcd_cursor(int line, int pos, int show);

void lcd_backlight(int en);
