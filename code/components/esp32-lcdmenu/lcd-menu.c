#include <stdlib.h>
#include <stdio.h>
#include "smbus.h"
#include "i2c-lcd1602.h"
#include "lcd-menu.h"
#include "esp_log.h"

//Static functions
static int displayMenu(i2c_lcd1602_info_t*, unsigned int);
static int displayCursorOn(i2c_lcd1602_info_t*, unsigned int);


//Extern variable to all the menu's in the application
LCD_MENU (*lcdMenus)[TOTAL_MENUS];

//Variable to the current lcd menu
static unsigned int currentLcdMenu;
//The current selected menu item in the current menu
static unsigned int currentMenuItem;


int menu_initMenus(i2c_lcd1602_info_t *lcd_info)
{
    //TODO
    //Reserve memory for all the menu's
    //Init all the menu's with the right value's
    //Store all the menu's in the lcdMenus array
    //Set the currentLcdMenu and currentMenuItem
    //Show the currentLcdMenu

    //Hides the cursor
    i2c_lcd1602_set_cursor(lcd_info, false);

    //Item to fill up the items if there are not enough items
    LCD_MENU_ITEM *nullItem = (LCD_MENU_ITEM*) malloc(sizeof(LCD_MENU_ITEM));
    nullItem->id = 99;

    //Main menu
    LCD_MENU *mainMenu = (LCD_MENU*) malloc(sizeof(LCD_MENU));
    if (mainMenu == NULL)
        return LCD_MENU_ERROR;

    mainMenu->id = MAIN_MENU_ID;
    mainMenu->text = "MENU";
    mainMenu->xCoord = 8;
    LCD_MENU_ITEM *mainMenu_Radio = (LCD_MENU_ITEM*) malloc(sizeof(LCD_MENU_ITEM));
    mainMenu_Radio->id = 0;
    mainMenu_Radio->text = "RADIO";
    mainMenu_Radio->xCoord = 2;
    mainMenu_Radio->yCoord = 2;
    LCD_MENU_ITEM *mainMenu_Clock = (LCD_MENU_ITEM*) malloc(sizeof(LCD_MENU_ITEM));
    mainMenu_Clock->id = 1;
    mainMenu_Clock->text = "KLOK";
    mainMenu_Clock->xCoord = 9;
    mainMenu_Clock->yCoord = 2;
    LCD_MENU_ITEM *mainMenu_Echo = (LCD_MENU_ITEM*) malloc(sizeof(LCD_MENU_ITEM));
    mainMenu_Echo->id = 2;
    mainMenu_Echo->text = "ECHO";
    mainMenu_Echo->xCoord = 15;
    mainMenu_Echo->yCoord = 2;
    LCD_MENU_ITEM mainMenuItems[MAX_ITEMS_ON_MENU] = {*mainMenu_Radio, *mainMenu_Clock, *mainMenu_Echo, *nullItem};
    mainMenu->items = &mainMenuItems;


    //Set all the menu's in an array
    LCD_MENU menusArray[TOTAL_MENUS] = {*mainMenu};
    //Point the lcdMenus to the array of menu's
    lcdMenus = &menusArray;

    //Set the current lcdMenu en item
    currentLcdMenu = lcdMenus[MAIN_MENU_ID]->id;
    currentMenuItem = (*lcdMenus[MAIN_MENU_ID]->items)[0].id;

    //Show the current menu
    displayMenu(lcd_info, currentLcdMenu);

    return LCD_MENU_OKE;
}

int menu_goToNextItem(i2c_lcd1602_info_t *lcd_info)
{
    return displayCursorOn(lcd_info, currentMenuItem + 1);
}

//Sets the user selector (cursor) on the given menu item
static int displayCursorOn(i2c_lcd1602_info_t *lcd_info, unsigned int itemToSelect)
{
    //Get the current menu, item and next item
    LCD_MENU displayedMenu = (*lcdMenus)[currentLcdMenu]; 
    LCD_MENU_ITEM currentItem = (*displayedMenu.items)[currentMenuItem];
    LCD_MENU_ITEM newItem = (*displayedMenu.items)[itemToSelect];
    printf("newItem x: %d\n", newItem.xCoord);
    printf("current x: %d\n", currentItem.xCoord);  

    //Check if item is valid
    if (itemToSelect > MAX_ITEMS_ON_MENU - 1 || newItem.id == 99)
        return LCD_MENU_ERROR;

    //Remove the old cursor
    i2c_lcd1602_move_cursor(lcd_info, currentItem.xCoord - 1, currentItem.yCoord);
    i2c_lcd1602_write_char(lcd_info, ' ');

    //Place the new cursor
    i2c_lcd1602_move_cursor(lcd_info, newItem.xCoord - 1, newItem.yCoord);
    i2c_lcd1602_write_char(lcd_info, '>');

    currentMenuItem = itemToSelect;

    return LCD_MENU_OKE;
}

//Displays the given menu to the lcd
static int displayMenu(i2c_lcd1602_info_t *lcd_info, unsigned int menuToDisplay)
{
    //Clear the display
    i2c_lcd1602_clear(lcd_info);

    //Get the menu to display
    LCD_MENU newMenu = (*lcdMenus)[menuToDisplay];

    //Write the title of the menu to the screen
    i2c_lcd1602_move_cursor(lcd_info, newMenu.xCoord, 0);
    i2c_lcd1602_write_string(lcd_info, newMenu.text);

    //Write each item on the screen
    for (int i = 0; i < MAX_ITEMS_ON_MENU; i++)
    {
        //Check if item is valid (not a nullItem)
        if ((*newMenu.items)[i].id == 99) 
            break;

        i2c_lcd1602_move_cursor(lcd_info, (*newMenu.items)[i].xCoord, (*newMenu.items)[i].yCoord);
        i2c_lcd1602_write_string(lcd_info, (*newMenu.items)[i].text);
    }

    currentMenuItem = 0;
    currentLcdMenu = newMenu.id;
    displayCursorOn(lcd_info, currentMenuItem);

    return LCD_MENU_OKE;
}

