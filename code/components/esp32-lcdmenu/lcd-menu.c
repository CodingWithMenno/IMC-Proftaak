#include <stdlib.h>
#include "smbus.h"
#include "i2c-lcd1602.h"
#include "lcd-menu.h"

//Static functions
static int displayMenu(i2c_lcd1602_info_t*, LCD_MENU*);
static LCD_MENU* getLcdMenu(unsigned int);


//Extern variable to all the menu's in the application
LCD_MENU *lcdMenus[TOTAL_MENUS];

//Variable to the current lcd menu
static unsigned int currentLcdMenu;
//The current selected menu item in the current menu
static unsigned int currentMenuItem;


int menu_initMenus(i2c_lcd1602_info_t *lcd_info)
{
    //Reserve memory for all the menu's
    //Init all the menu's with the right value's
    //Store all the menu's in the lcdMenus array
    //Set the currentLcdMenu and currentMenuItem
    //Show the currentLcdMenu

    //Main menu
    LCD_MENU *mainMenu = (LCD_MENU*) malloc(sizeof(LCD_MENU));
    mainMenu->id = MAIN_MENU_ID;
    mainMenu->text = "MENU";
    LCD_MENU_ITEM mainMenu_Radio;
    mainMenu_Radio.id = 0;
    mainMenu_Radio.text = "RADIO";
    mainMenu_Radio.xCoord = 1;
    mainMenu_Radio.yCoord = 2;
    LCD_MENU_ITEM mainMenu_Clock;
    mainMenu_Clock.id = 1;
    mainMenu_Clock.text = "KLOK";
    mainMenu_Clock.xCoord = 8;
    mainMenu_Clock.yCoord = 2;
    LCD_MENU_ITEM mainMenu_Echo;
    mainMenu_Echo.id = 2;
    mainMenu_Echo.text = "ECHO";
    mainMenu_Echo.xCoord = 14;
    mainMenu_Echo.yCoord = 2;
    LCD_MENU_ITEM mainMenuItems[] = {mainMenu_Radio, mainMenu_Clock, mainMenu_Echo};
    mainMenu->items = &mainMenuItems;

    lcdMenus[MAIN_MENU_ID] = mainMenu;

    //displayMenu(lcd_info, currentLcdMenu);
    return LCD_MENU_OKE;
}

//Displays the given menu to the lcd
static int displayMenu(i2c_lcd1602_info_t *lcd_info, LCD_MENU *menuToDisplay)
{
    return LCD_MENU_ERROR;
}

//Returns the lcd menu that belongs to the given id
static LCD_MENU* getLcdMenu(unsigned int menuId)
{
    return NULL;
}

