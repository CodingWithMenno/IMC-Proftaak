//Header include guard for the extension file (lcd-menu-elaboration.c)
#ifndef LCD_MENU_C
#define LCD_MENU_C

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "smbus.h"
#include "i2c-lcd1602.h"
#include "lcd-menu.h"
#include "lcd-menu-elaboration.h"

//ID's of every lcd menu (is also the number in the lcdMenus array)
#define MAIN_MENU_ID 0
#define ECHO_MENU_ID 1
#define RADIO_MENU_ID 2
#define CLOCK_MENU_ID 3

#define INVALID 99

//Static functions
static void doFancyAnimation(i2c_lcd1602_info_t*);
static int displayMenu(i2c_lcd1602_info_t*, unsigned int);
static int refreshMenu(i2c_lcd1602_info_t*, unsigned int, unsigned int);
static int displayCursorOn(i2c_lcd1602_info_t*, unsigned int);

//Create menu functions
int createMenu(int, int, char*, int, void(), void(), void());
void createMenuItem(int, int, char*, int, int, void (*onClick)());

//Variable to all the lcd menu's
static LCD_MENU *lcdMenus;
//Variable to the current lcd menu
static unsigned int currentLcdMenu;
//The current selected menu item in the current menu
static unsigned int currentMenuItem;


int menu_updateMenu(i2c_lcd1602_info_t *lcd_info, void *p)
{
    if (lcdMenus[currentLcdMenu].update == NULL)
        return LCD_MENU_ERROR;
    
    lcdMenus[currentLcdMenu].update(p);
    return refreshMenu(lcd_info, currentLcdMenu, currentMenuItem);
}

int menu_goToParentMenu(i2c_lcd1602_info_t *lcd_info)
{
    if (lcdMenus[currentLcdMenu].parent == INVALID)
        return LCD_MENU_ERROR;
    
    return displayMenu(lcd_info, lcdMenus[currentLcdMenu].parent);
}

int menu_onClick(i2c_lcd1602_info_t *lcd_info)
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
    if (itemToSelect > MAX_ITEMS_ON_MENU - 1 || newItem.id == INVALID)
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
    //Get the menu to display
    LCD_MENU newMenu = lcdMenus[menuToDisplay];

    //Perform the exit function of the old menu
    if (currentLcdMenu != INVALID && lcdMenus[currentLcdMenu].menuExit != NULL)
        lcdMenus[currentLcdMenu].menuExit();

    doFancyAnimation(lcd_info);

    //Perform the init function of the new menu
    if (newMenu.menuEnter != NULL)
        newMenu.menuEnter();

    currentMenuItem = newMenu.items[0].id;
    currentLcdMenu = newMenu.id;
    return refreshMenu(lcd_info, menuToDisplay, currentMenuItem);
}

static int refreshMenu(i2c_lcd1602_info_t *lcd_info, unsigned int menuToDisplay, unsigned int selectedItem)
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
        if (newMenu.items[i].id == INVALID) 
            break;

        //Write the item on the screen
        i2c_lcd1602_move_cursor(lcd_info, newMenu.items[i].xCoord, newMenu.items[i].yCoord);
        i2c_lcd1602_write_string(lcd_info, newMenu.items[i].text);
    }

    return displayCursorOn(lcd_info, selectedItem);
}

int menu_initMenus(i2c_lcd1602_info_t *lcd_info)
{
    //Hides the cursor
    i2c_lcd1602_set_cursor(lcd_info, false);

    //Custom white rectangle
    uint8_t rectangle[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_0 , rectangle);

    //Allocate memory for each menu
    lcdMenus = (LCD_MENU*) malloc(sizeof(LCD_MENU) * TOTAL_MENUS);
    if (lcdMenus == NULL)
        return LCD_MENU_ERROR;
    
    //Main menu
    // lcdMenus[MAIN_MENU_ID].id = MAIN_MENU_ID;
    // strcpy(lcdMenus[MAIN_MENU_ID].text, "MENU");
    // lcdMenus[MAIN_MENU_ID].xCoord = 8;
    // lcdMenus[MAIN_MENU_ID].parent = INVALID;
    // lcdMenus[MAIN_MENU_ID].menuEnter = NULL;
    // lcdMenus[MAIN_MENU_ID].update = NULL;
    // lcdMenus[MAIN_MENU_ID].menuExit = NULL;
    // lcdMenus[MAIN_MENU_ID].items = (LCD_MENU_ITEM*) malloc(sizeof(LCD_MENU_ITEM) * MAX_ITEMS_ON_MENU);
    // if (lcdMenus[MAIN_MENU_ID].items == NULL)
    // {
    //     free(lcdMenus);
    //     return LCD_MENU_ERROR;
    // }
    
    // LCD_MENU_ITEM *itemsMainMenu = lcdMenus[MAIN_MENU_ID].items;
    // //Radio item
    // itemsMainMenu[0].id = 0;
    // strcpy(itemsMainMenu[0].text, "RADIO");
    // itemsMainMenu[0].xCoord = 2;
    // itemsMainMenu[0].yCoord = 2;
    // itemsMainMenu[0].onClick = &onClickMainRadio;
    // //Clock item
    // itemsMainMenu[1].id = 1;
    // strcpy(itemsMainMenu[1].text, "KLOK");
    // itemsMainMenu[1].xCoord = 9;
    // itemsMainMenu[1].yCoord = 2;
    // itemsMainMenu[1].onClick = &onClickMainClock;
    // //Echo item
    // itemsMainMenu[2].id = 2;
    // strcpy(itemsMainMenu[2].text, "ECHO");
    // itemsMainMenu[2].xCoord = 15;
    // itemsMainMenu[2].yCoord = 2;
    // itemsMainMenu[2].onClick = &onClickMainEcho;
    // //Fill-up item
    // itemsMainMenu[3].id = INVALID;


    // //Echo menu
    // lcdMenus[ECHO_MENU_ID].id = ECHO_MENU_ID;
    // strcpy(lcdMenus[ECHO_MENU_ID].text, "MENU");
    // lcdMenus[ECHO_MENU_ID].xCoord = 8;
    // lcdMenus[ECHO_MENU_ID].parent = MAIN_MENU_ID;
    // lcdMenus[ECHO_MENU_ID].menuEnter = NULL;
    // lcdMenus[ECHO_MENU_ID].update = NULL;
    // lcdMenus[ECHO_MENU_ID].menuExit = NULL;
    // lcdMenus[ECHO_MENU_ID].items = (LCD_MENU_ITEM*) malloc(sizeof(LCD_MENU_ITEM) * MAX_ITEMS_ON_MENU);
    // if (lcdMenus[ECHO_MENU_ID].items == NULL)
    // {
    //     free(lcdMenus);
    //     free(lcdMenus[MAIN_MENU_ID].items);
    //     return LCD_MENU_ERROR;
    // }

    // LCD_MENU_ITEM *itemsEchoMenu = lcdMenus[ECHO_MENU_ID].items;
    // //Record item
    // itemsEchoMenu[0].id = 0;
    // strcpy(itemsEchoMenu[0].text, "RECORD");
    // itemsEchoMenu[0].xCoord = 2;
    // itemsEchoMenu[0].yCoord = 2;
    // itemsEchoMenu[0].onClick = NULL;
    // //Clips item
    // itemsEchoMenu[1].id = 1;
    // strcpy(itemsEchoMenu[1].text, "CLIPS");
    // itemsEchoMenu[1].xCoord = 13;
    // itemsEchoMenu[1].yCoord = 2;
    // itemsEchoMenu[1].onClick = NULL;
    // //Fill-up item
    // itemsEchoMenu[2].id = INVALID;


    // //Radio menu
    // lcdMenus[RADIO_MENU_ID].id = RADIO_MENU_ID;
    // strcpy(lcdMenus[RADIO_MENU_ID].text, "RADIO");
    // lcdMenus[RADIO_MENU_ID].xCoord = 7;
    // lcdMenus[RADIO_MENU_ID].parent = MAIN_MENU_ID;
    // lcdMenus[RADIO_MENU_ID].menuEnter = &onEnterRadio;
    // lcdMenus[RADIO_MENU_ID].update = NULL;
    // lcdMenus[RADIO_MENU_ID].menuExit = &onExitRadio;
    // lcdMenus[RADIO_MENU_ID].items = (LCD_MENU_ITEM*) malloc(sizeof(LCD_MENU_ITEM) * MAX_ITEMS_ON_MENU);
    // if (lcdMenus[RADIO_MENU_ID].items == NULL)
    // {
    //     free(lcdMenus);
    //     free(lcdMenus[MAIN_MENU_ID].items);
    //     free(lcdMenus[ECHO_MENU_ID].items);
    //     return LCD_MENU_ERROR;
    // }

    // LCD_MENU_ITEM *itemsRadioMenu = lcdMenus[RADIO_MENU_ID].items;
    // //538
    // itemsRadioMenu[0].id = 0;
    // strcpy(itemsRadioMenu[0].text, "538");
    // itemsRadioMenu[0].xCoord = 2;
    // itemsRadioMenu[0].yCoord = 2;
    // itemsRadioMenu[0].onClick = &onClickRadio538;
    // //Q
    // itemsRadioMenu[1].id = 1;
    // strcpy(itemsRadioMenu[1].text, "Qmusic");
    // itemsRadioMenu[1].xCoord = 7;
    // itemsRadioMenu[1].yCoord = 2;
    // itemsRadioMenu[1].onClick = &onClickRadioQ;
    // //Sky
    // itemsRadioMenu[2].id = 2;
    // strcpy(itemsRadioMenu[2].text, "SKY");
    // itemsRadioMenu[2].xCoord = 15;
    // itemsRadioMenu[2].yCoord = 2;
    // itemsRadioMenu[2].onClick = &onClickRadioSky;
    // //Fill-up item
    // itemsRadioMenu[3].id = INVALID;


    // //Klok menu
    // lcdMenus[CLOCK_MENU_ID].id = CLOCK_MENU_ID;
    // strcpy(lcdMenus[CLOCK_MENU_ID].text, "KLOK");
    // lcdMenus[CLOCK_MENU_ID].xCoord = 8;
    // lcdMenus[CLOCK_MENU_ID].parent = MAIN_MENU_ID;
    // lcdMenus[CLOCK_MENU_ID].menuEnter = &onEnterClock;
    // lcdMenus[CLOCK_MENU_ID].update = &onUpdateClock;
    // lcdMenus[CLOCK_MENU_ID].menuExit = &onExitClock;
    // lcdMenus[CLOCK_MENU_ID].items = (LCD_MENU_ITEM*) malloc(sizeof(LCD_MENU_ITEM) * MAX_ITEMS_ON_MENU);
    // if (lcdMenus[CLOCK_MENU_ID].items == NULL)
    // {
    //     free(lcdMenus);
    //     free(lcdMenus[MAIN_MENU_ID].items);
    //     free(lcdMenus[ECHO_MENU_ID].items);
    //     free(lcdMenus[RADIO_MENU_ID].items);
    //     return LCD_MENU_ERROR;
    // }

    // LCD_MENU_ITEM *itemsClockMenu = lcdMenus[CLOCK_MENU_ID].items;
    // //Clock item
    // itemsClockMenu[0].id = 0;
    // strcpy(itemsClockMenu[0].text, "tijd");
    // itemsClockMenu[0].xCoord = 8;
    // itemsClockMenu[0].yCoord = 2;
    // itemsClockMenu[0].onClick = NULL;
    // //Fill-up item
    // itemsClockMenu[1].id = INVALID;

    createMenu(MAIN_MENU_ID, INVALID, "MENU", 8, NULL, NULL, NULL);
    createMenuItem(MAIN_MENU_ID, 0, "RADIO", 2, 2,&onClickMainRadio);
    createMenuItem(MAIN_MENU_ID, 1, "KLOK", 9, 2, &onClickMainClock);
    createMenuItem(MAIN_MENU_ID, 2, "ECHO", 15, 2, &onClickMainEcho);
    createMenuItem(MAIN_MENU_ID, INVALID, "", 0, 0, NULL);

    createMenu(RADIO_MENU_ID, MAIN_MENU_ID, "RADIO", 7, &onEnterRadio, NULL, &onExitRadio);
    createMenuItem(RADIO_MENU_ID, 0, "538", 2, 2, NULL);
    createMenuItem(RADIO_MENU_ID, 1, "Qmusic", 7, 2, NULL);
    createMenuItem(RADIO_MENU_ID, 2, "SKY", 15, 2, NULL);
    createMenuItem(RADIO_MENU_ID, INVALID, "", 0, 0, NULL);

    createMenu(ECHO_MENU_ID, MAIN_MENU_ID, "ECHO", 8, NULL, NULL, NULL);
    createMenuItem(ECHO_MENU_ID, 0, "RECORD", 2, 2, NULL);
    createMenuItem(ECHO_MENU_ID, 1, "CLIPS", 13, 2, NULL);
    createMenuItem(ECHO_MENU_ID, INVALID, "", 0, 0, NULL);

    createMenu(CLOCK_MENU_ID, MAIN_MENU_ID, "KLOK", 7, &onEnterClock, &onUpdateClock, &onExitClock);
    createMenuItem(CLOCK_MENU_ID, 0, "TIME", 8, 2, NULL);
    createMenuItem(CLOCK_MENU_ID, INVALID, "", 0, 0, NULL);

    printf("DOET HET ! 4");

    //Display the main menu
    currentLcdMenu = INVALID;
    return displayMenu(lcd_info, MAIN_MENU_ID);
}

int createMenu(int menuId, int parentId, char* displayName, int xCoord, void (*onEnter)(), void (*onUpdate)(), void (*onExit)())
{
    lcdMenus[menuId].id = menuId;
    strcpy(lcdMenus[menuId].text, displayName);
    lcdMenus[menuId].xCoord = xCoord;
    lcdMenus[menuId].parent = parentId;
    lcdMenus[menuId].menuEnter = (*onEnter);
    lcdMenus[menuId].update = (*onUpdate);
    lcdMenus[menuId].menuExit = (*onExit);
    lcdMenus[menuId].items = (LCD_MENU_ITEM*) malloc(sizeof(LCD_MENU_ITEM) * MAX_ITEMS_ON_MENU);
    if(lcdMenus[menuId].items == NULL)
    {
        // for(int i = 0; i < menuId; i++)
        //     free(lcdMenus[i].items);
        // free(lcdMenus);
        // return LCD_MENU_ERROR;
    }
    return LCD_MENU_OKE;
}

void createMenuItem(int menuId, int id, char* displayName, int xCoord, int yCoord, void (*onClick)())
{
    LCD_MENU_ITEM *itemsMenu = lcdMenus[menuId].items;
    itemsMenu[id].id = id;
    strcpy(itemsMenu[id].text, displayName);
    itemsMenu[id].xCoord = xCoord;
    itemsMenu[id].yCoord = yCoord;
    itemsMenu[id].onClick = (*onClick);
}

static void doFancyAnimation(i2c_lcd1602_info_t* lcd_info)
{
    i2c_lcd1602_move_cursor(lcd_info, 0, 0);
    for(int i = 0; i < 20; i++)
    {
        i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_0);
        vTaskDelay(15 / portTICK_RATE_MS);
    }
    i2c_lcd1602_move_cursor(lcd_info, 19, 1);
    i2c_lcd1602_set_right_to_left(lcd_info);
    for(int i = 0; i < 20; i++)
    {
        i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_0);
        vTaskDelay(15 / portTICK_RATE_MS);
    }
    i2c_lcd1602_move_cursor(lcd_info, 0, 2);
    i2c_lcd1602_set_left_to_right(lcd_info);
    for(int i = 0; i < 20; i++)
    {
        i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_0);
        vTaskDelay(15 / portTICK_RATE_MS);
    }
    i2c_lcd1602_move_cursor(lcd_info, 19, 3);
    i2c_lcd1602_set_right_to_left(lcd_info);
    for(int i = 0; i < 20; i++)
    {
        i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_0);
        vTaskDelay(15 / portTICK_RATE_MS);
    }
    i2c_lcd1602_set_left_to_right(lcd_info);
    vTaskDelay(100 / portTICK_RATE_MS);
}

#endif