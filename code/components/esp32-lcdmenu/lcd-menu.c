#include "smbus.h"
#include "i2c-lcd1602.h"
#include "lcd-menu.h"

//Extern variable to the current lcd menu
LCD_MENU *currentLcdMenu;

//All the menu's in the application
static LCD_MENU *lcdMenus[TOTAL_MENUS];
//The current selected menu item in the current menu
static unsigned int currentMenuItem;


int menu_initMenus()
{
    //Reserve memory for all the menu's
    //Init all the menu's with the right value's
    //Store all the menu's in the lcdMenus array
    //Set the currentLcdMenu and currentMenuItem
}

