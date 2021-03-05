#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "smbus.h"
#include "i2c-lcd1602.h"
#include "lcd-menu.h"
#include "esp_log.h"

//ID's of every lcd menu (is also the number in the lcdMenus array)
#define MAIN_MENU_ID 0
#define ECHO_MENU_ID 1

//Static functions
static int displayMenu(i2c_lcd1602_info_t*, unsigned int);
static int displayCursorOn(i2c_lcd1602_info_t*, unsigned int);
static void onClickMainEcho(i2c_lcd1602_info_t*);

//Extern variable to all the menu's in the application
LCD_MENU *lcdMenus;

//Variable to the current lcd menu
static unsigned int currentLcdMenu;
//The current selected menu item in the current menu
static unsigned int currentMenuItem;


int menu_doActionCurrentItem(i2c_lcd1602_info_t *lcd_info)
{
    if (lcdMenus[currentLcdMenu].items[currentMenuItem].onClick == NULL)
        return LCD_MENU_ERROR;
    
    lcdMenus[currentLcdMenu].items[currentMenuItem].onClick(lcd_info);
    return LCD_MENU_OKE;
}

int menu_goToNextItem(i2c_lcd1602_info_t *lcd_info)
{
    return displayCursorOn(lcd_info, currentMenuItem + 1);
}

int menu_goToPreviousitem(i2c_lcd1602_info_t *lcd_info)
{
    return displayCursorOn(lcd_info, currentMenuItem - 1);
}

//Sets the user selector (cursor) on the given menu item
static int displayCursorOn(i2c_lcd1602_info_t *lcd_info, unsigned int itemToSelect)
{
    LCD_MENU displayedMenu = lcdMenus[currentLcdMenu];
    LCD_MENU_ITEM currentItem = displayedMenu.items[currentMenuItem];
    LCD_MENU_ITEM newItem = displayedMenu.items[itemToSelect];

    //Check if itemToSelect is valid
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
    LCD_MENU newMenu = lcdMenus[menuToDisplay];

    //Write the title of the menu to the screen
    i2c_lcd1602_move_cursor(lcd_info, newMenu.xCoord, 0);
    i2c_lcd1602_write_string(lcd_info, newMenu.text);

    //Write each item on the screen
    for (int i = 0; i < MAX_ITEMS_ON_MENU; i++)
    {
        //Check if item is valid
        if (newMenu.items[i].id == 99) 
            break;

        //Write the item on the screen
        i2c_lcd1602_move_cursor(lcd_info, newMenu.items[i].xCoord, newMenu.items[i].yCoord);
        i2c_lcd1602_write_string(lcd_info, newMenu.items[i].text);
    }
    
    currentMenuItem = newMenu.items[0].id;
    currentLcdMenu = newMenu.id;
    return displayCursorOn(lcd_info, currentMenuItem);
}

int menu_initMenus(i2c_lcd1602_info_t *lcd_info)
{
    //Hides the cursor
    i2c_lcd1602_set_cursor(lcd_info, false);


    //Allocate memory for each menu
    lcdMenus = (LCD_MENU*) malloc(sizeof(LCD_MENU) * TOTAL_MENUS);
    if (lcdMenus == NULL)
        return LCD_MENU_ERROR;
    
    //Main menu
    lcdMenus[MAIN_MENU_ID].id = MAIN_MENU_ID;
    strcpy(lcdMenus[MAIN_MENU_ID].text, "MENU");
    lcdMenus[MAIN_MENU_ID].xCoord = 8;
    lcdMenus[MAIN_MENU_ID].items = (LCD_MENU_ITEM*) malloc(sizeof(LCD_MENU_ITEM) * MAX_ITEMS_ON_MENU);
    if (lcdMenus[MAIN_MENU_ID].items == NULL)
    {
        free(lcdMenus);
        return LCD_MENU_ERROR;
    }
    
    LCD_MENU_ITEM *itemsMainMenu = lcdMenus[MAIN_MENU_ID].items;
    //Radio item
    itemsMainMenu[0].id = 0;
    strcpy(itemsMainMenu[0].text, "RADIO");
    itemsMainMenu[0].xCoord = 2;
    itemsMainMenu[0].yCoord = 2;
    itemsMainMenu[0].onClick = NULL;
    //Clock item
    itemsMainMenu[1].id = 1;
    strcpy(itemsMainMenu[1].text, "KLOK");
    itemsMainMenu[1].xCoord = 9;
    itemsMainMenu[1].yCoord = 2;
    itemsMainMenu[1].onClick = NULL;
    //Echo item
    itemsMainMenu[2].id = 2;
    strcpy(itemsMainMenu[2].text, "ECHO");
    itemsMainMenu[2].xCoord = 15;
    itemsMainMenu[2].yCoord = 2;
    itemsMainMenu[2].onClick = &onClickMainEcho;
    //Fill-up item
    itemsMainMenu[3].id = 99;


    //Echo menu
    lcdMenus[ECHO_MENU_ID].id = ECHO_MENU_ID;
    strcpy(lcdMenus[ECHO_MENU_ID].text, "MENU");
    lcdMenus[ECHO_MENU_ID].xCoord = 8;
    lcdMenus[ECHO_MENU_ID].items = (LCD_MENU_ITEM*) malloc(sizeof(LCD_MENU_ITEM) * MAX_ITEMS_ON_MENU);
    if (lcdMenus[ECHO_MENU_ID].items == NULL)
    {
        free(lcdMenus);
        free(lcdMenus[MAIN_MENU_ID].items);
        return LCD_MENU_ERROR;
    }

    LCD_MENU_ITEM *itemsEchoMenu = lcdMenus[ECHO_MENU_ID].items;
    //Record item
    itemsEchoMenu[0].id = 0;
    strcpy(itemsEchoMenu[0].text, "RECORD");
    itemsEchoMenu[0].xCoord = 2;
    itemsEchoMenu[0].yCoord = 2;
    itemsEchoMenu[0].onClick = NULL;
    //Clips item
    itemsEchoMenu[1].id = 1;
    strcpy(itemsEchoMenu[1].text, "CLIPS");
    itemsEchoMenu[1].xCoord = 13;
    itemsEchoMenu[1].yCoord = 2;
    itemsEchoMenu[1].onClick = NULL;
    //Fill-up item
    itemsEchoMenu[2].id = 99;


    //Set the current lcd menu and display it on the lcd
    currentLcdMenu = MAIN_MENU_ID;
    return displayMenu(lcd_info, currentLcdMenu);
}



//From here onClick functions

static void onClickMainEcho(i2c_lcd1602_info_t* lcd_info)
{
    printf("Clicked on Echo button in Main menu\n");
    displayMenu(lcd_info, ECHO_MENU_ID);
}