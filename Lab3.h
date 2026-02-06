#pragma once

class CControl;

char lab3_input_checker(char input);
void lab3_print_menu(void);
void lab3_loop(void);
bool user_exit();

void test_digital(CControl& board);
void test_button(CControl& board);
void test_analog(CControl& board);
void test_servo(CControl& board);