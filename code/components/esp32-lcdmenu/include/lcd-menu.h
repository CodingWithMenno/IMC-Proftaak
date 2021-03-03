#ifndef LCDMENU_H
#define LCDMENU_H

//LCD_MENU error codes
#define LCD_MENU_OKE 0
#define LCD_MENU_ERROR 1

//ID's of every lcd menu (is also the number in the lcdMenus array)
#define MAIN_MENU_ID 0

//Some constants for the lcd menu's
#define TOTAL_MENUS 6
#define MAX_ITEMS_ON_MENU 4

//A struct for a menu item
typedef struct
{
    unsigned int id;
    char *text;
    void (*onClick)(void);
    unsigned int xCoord;  //Where to start to write the text
    unsigned int yCoord;  //Where to start to write the text
} LCD_MENU_ITEM;

//A struct for a menu
typedef struct menu
{
    unsigned int id;
    unsigned int xCoord; //Where to start to write the text
    char *text;
    void (*menuInit)(void);
    void (*menuExit)(void);
    unsigned int menuPointers[MAX_ITEMS_ON_MENU]; //Points to the id of the next possible menu's
    LCD_MENU_ITEM (*items)[MAX_ITEMS_ON_MENU]; //Pointer to an array of menu items
} LCD_MENU;

//Pointer to an array of all the menu's
extern LCD_MENU (*lcdMenus)[TOTAL_MENUS];


/*
Call this method to init all the menu's

-Returns:       a LCD_MENU error code
-Parameters:    a pointer to the lcd info
*/
int menu_initMenus(i2c_lcd1602_info_t*);

/*
Function to go back to the previous menu

-Returns:       a LCD_MENU error code
-Parameters:    a pointer to the lcd info
*/
int menu_goBackAMenu(i2c_lcd1602_info_t*);

/*
Function to start the action (the onClick() function) of
the current selected item on the current menu

-Returns:       A LCD_MENU error code
-Parameters:    a pointer to the lcd info
*/
int menu_doActionCurrentItem(i2c_lcd1602_info_t*);

/*
Function to set the cursor to the next item in the menu

-Returns:       a LCD_MENU error code
-Parameters:    a pointer to the lcd info
*/
int menu_goToNextItem(i2c_lcd1602_info_t*);

/*
Function to set the cursor to the previous item in the menu

-Returns:       a LCD_MENU error code
-Parameters:    a pointer to the lcd info
*/
int menu_goToPreviousitem(i2c_lcd1602_info_t*);

#endif