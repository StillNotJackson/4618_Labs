#include "stdafx.h"
#include "Lab3.h"
#include "CControl.h" 

#include <string>
#include <conio.h>//_kbhit for exit function
#include <ctype.h>
#include <iostream>

#define CHAN_S1 33
#define CHAN_S2 32
#define CHAN_LED_RED 39


void lab3_print_menu(void)
{
    std::cout << '\n';
    std::cout << std::string(40, '*') << '\n';
    std::cout << "ELEX4618 Grade System, by Jack Gillis" << '\n';
    std::cout << std::string(40, '*') << '\n';
    std::cout << "(A)nalog Test" << '\n';
    std::cout << "(B)utton Test" << '\n';
    std::cout << "(D)igtal Test" << '\n';
    std::cout << "(S)ervo Test" << '\n';
    std::cout << "(Q)uit" << '\n';
}

char lab3_input_checker(char input)
{
    bool input_check = false;
    do
    {
        input = std::toupper(input);
        switch (input)
        {
        case 'A':
        case 'B':
        case 'D':
        case 'S':
        case 'Q':
        {
            input_check = true;
            break;
        }
        case 'M':
            lab3_print_menu();
        default:
        {
            std::cout << "\nPlease enter a valid input or enter (M)enu: ";
            std::cin >> input;
        }
        }//end switch
    } while (!input_check);
    return input;
}//end lab2

void lab3_loop()
{
    char lab3_cmd;
    CControl tivaC;
    tivaC.init_com(4);

    do
    {
        lab3_print_menu();
        std::cin >> lab3_cmd;
        lab3_cmd = lab3_input_checker(lab3_cmd);

        switch (lab3_cmd)
        {
        case 'A'://analog test
        {
            test_analog(tivaC);
            break;
        }
        case 'B'://Button Test
        {
            test_button(tivaC);
            break;
        }
        case 'D'://Digital Test
        {
            test_digital(tivaC);
            break;
        }
        case 'S'://Servo Test
        {
            test_servo(tivaC);
            break;
        }
        case 'Q'://Quit
        {
            break;
        }
        }
    } while (lab3_cmd != 'Q');
}

void test_digital(CControl& board)
{
    int button_pressed = 0;

    bool led_token = false;
    do
    {
        if (user_exit()) break;

        board.get_data(DIGITAL, CHAN_S1,button_pressed);
        button_pressed = !button_pressed;
        board.set_data(DIGITAL, CHAN_LED_RED, button_pressed);

        std::cout << "\nDIGITAL TEST: CH33 " << button_pressed << "\n";
    } while (true);
}

void test_button(CControl& board)
{
    int press_total = 0, button_state = 1, button_press = 0;

    double press_time; //= GetTickCount();
    std::cout << "\nButton Test: " << press_total;
    do
    {
        if (user_exit()) 
            break;
        if (board.get_button(CHAN_S1))
        {
            press_total++;
            std::cout << "\nButton Test: " << press_total;
        }

    } while (true);
}

void test_analog(CControl& board)
{
    float horz, vert;
    while (true)
    {
        if (user_exit()) break;
        vert = board.get_analog(4);
        horz = board.get_analog(11);

        std::cout << std::fixed << std::setprecision(1) << "\nANALOG TEST: CH26 = (" << vert << "%) CH2 = (" << horz << "%)";
    }
}


void test_servo(CControl& board)
{
    const int MIN_POS = 1;   
    const int MAX_POS = 180;  

    while (true)
    {
        // Sweep up
        for (int pos = MIN_POS; pos <= MAX_POS; pos++)
        {
            if (user_exit())
                return;
            board.set_data(SERVO, 0, pos);
            std::cout << "SERVO TEST: CH0 POS" << pos << "\n";

            Sleep(30);
        }

        // Sweep down
        for (int pos = MAX_POS; pos >= MIN_POS; pos--)
        {
            if (user_exit())
                return;
            board.set_data(SERVO, 0, pos);
            std::cout << "SERVO TEST: CH0 POS " << pos << "\n";
            Sleep(30);
        }
    }
}


bool user_exit()
{
    //https://stackoverflow.com/questions/11472043/non-blocking-getch
    //This was my answer to killing the process without hitching
    if (_kbhit())
    {
        char cmd = _getch();
        if (cmd == 'q' || cmd == 'Q')
        {
            return true;
        }
    }
    return false;
}