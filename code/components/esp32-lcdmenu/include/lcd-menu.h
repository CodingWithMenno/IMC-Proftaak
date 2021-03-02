#ifndef LCDMENU_H
#define LCDMENU_H

//LCD_MENU error codes
#define LCD_MENU_OKE 0
#define LCD_MENU_ERROR 1

//Some constants for the lcd menu's
#define TOTAL_MENUS 6
#define MAX_ITEMS_ON_MENU 4
#define MAX_LINES_ON_MENU 20
#define MAX_BUTTON_LENGTH 8

//The current menu
extern LCD_MENU *currentLcdMenu;

//A struct for a menu item
typedef struct
{
    unsigned int id;
    char text[MAX_BUTTON_LENGTH];
    void (*onClick)(void);
    int xCoord;
    int yCoord;
} LCD_MENU_ITEM;

//A struct for a menu
typedef struct menu
{
    unsigned int id;
    char text[MAX_LINES_ON_MENU];
    void (*menuInit)(void);
    void (*menuExit)(void);
    unsigned int menuPointers[MAX_ITEMS_ON_MENU]; //Points to the id of the next possible menu's
    LCD_MENU_ITEM *menuItems[MAX_ITEMS_ON_MENU]; //Points to all the menu items in this menu
} LCD_MENU;


/*
Call this method to init all the menu's

-Returns:       a LCD_MENU error code
*/
int menu_initMenus();

/*
Function to go back to the previous menu

-Returns:       a LCD_MENU error code
*/
int menu_goBackAMenu();

/*
Function to start the action (the onClick() function) of
the current selected item on the current menu

-Returns:       A LCD_MENU error code
*/
int menu_doActionCurrentItem();

/*
Function to set the cursor to the next item in the menu

-Returns:       a LCD_MENU error code
*/
int menu_goToNextItem();

/*
Function to set the cursor to the previous item in the menu

-Returns:       a LCD_MENU error code
*/
int menu_goToPreviousitem();



//OPTIONAL FUNCTIONS

/*
Function to switch between menu's

-Returns:       a LCD_MENU error code
-Parameters:    ID of next menu
*/
int menu_switchMenu(unsigned int);

#endif