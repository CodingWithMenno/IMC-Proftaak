#include <stdlib.h>
#include <stdio.h>
#include "smbus.h"
#include "i2c-lcd1602.h"
#include "lcd-menu.h"

//Static functions
static int displayMenu(i2c_lcd1602_info_t*, unsigned int);


//Extern variable to all the menu's in the application
LCD_MENU (*lcdMenus)[TOTAL_MENUS];

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

    //Item to fill up the items if there are not enough items
    LCD_MENU_ITEM nullItem;
    nullItem.id = 99;

    //Main menu
    LCD_MENU *mainMenu = (LCD_MENU*) malloc(sizeof(LCD_MENU));
    if (mainMenu == NULL)
        return LCD_MENU_ERROR;

    mainMenu->id = MAIN_MENU_ID;
    mainMenu->text = "MENU";
    mainMenu->xCoord = 8;
    LCD_MENU_ITEM mainMenu_Radio;
    mainMenu_Radio.id = 0;
    mainMenu_Radio.text = "RADIO";
    mainMenu_Radio.xCoord = 2;
    mainMenu_Radio.yCoord = 2;
    LCD_MENU_ITEM mainMenu_Clock;
    mainMenu_Clock.id = 1;
    mainMenu_Clock.text = "KLOK";
    mainMenu_Clock.xCoord = 9;
    mainMenu_Clock.yCoord = 2;
    LCD_MENU_ITEM mainMenu_Echo;
    mainMenu_Echo.id = 2;
    mainMenu_Echo.text = "ECHO";
    mainMenu_Echo.xCoord = 15;
    mainMenu_Echo.yCoord = 2;
    LCD_MENU_ITEM mainMenuItems[MAX_ITEMS_ON_MENU] = {mainMenu_Radio, mainMenu_Clock, mainMenu_Echo, nullItem};
    mainMenu->items = &mainMenuItems;


    //Set all the menu's in an array
    LCD_MENU menusArray[TOTAL_MENUS] = {*mainMenu};
    //Point the lcdMenus to the array of menu's
    lcdMenus = &menusArray;

    currentLcdMenu = lcdMenus[MAIN_MENU_ID]->id;
    currentMenuItem = lcdMenus[MAIN_MENU_ID]->items[0]->id;

    //Show the current menu
    displayMenu(lcd_info, currentLcdMenu);

    return LCD_MENU_OKE;
}

//Displays the given menu to the lcd
static int displayMenu(i2c_lcd1602_info_t *lcd_info, unsigned int menuToDisplay)
{
    //Activate the cursor
    i2c_lcd1602_set_cursor(lcd_info, true);
    //Clear the display
    i2c_lcd1602_clear(lcd_info);

    //Get the menu to display
    LCD_MENU newMenu = *lcdMenus[menuToDisplay];

    //Write the title of the menu to the screen
    i2c_lcd1602_move_cursor(lcd_info, newMenu.xCoord, 0);
    i2c_lcd1602_write_string(lcd_info, newMenu.text);

    //Write each item on the screen
    for (int i = 0; i < MAX_ITEMS_ON_MENU; i++)
    {
        if ((*newMenu.items)[i].id == 99) 
            break;

        i2c_lcd1602_move_cursor(lcd_info, (*newMenu.items)[i].xCoord, (*newMenu.items)[i].yCoord);
        i2c_lcd1602_write_string(lcd_info, (*newMenu.items)[i].text);
    }
    
    //Hide the cursor
    i2c_lcd1602_set_cursor(lcd_info, false);

    return LCD_MENU_OKE;
}

