#ifndef LCDMENU_H
#define LCDMENU_H

//LCD_MENU error codes
#define LCD_MENU_OKE 0
#define LCD_MENU_ERROR 1

#define MAX_ITEMS_ON_MENU 4
#define MAX_LINES_ON_MENU 20
#define MAX_MENU_POINTERS 3
#define MAX_BUTTON_LENGTH 8

//A struct for a menu item
typedef struct
{
    unsigned int id;
    char *text[MAX_BUTTON_LENGTH];
    void (*onClick)(void);
} LCD_MENU_ITEM;

//A struct for a menu item
typedef struct menu
{
    unsigned int id;
    unsigned int menuPointers[MAX_MENU_POINTERS];
    char *text[MAX_LINES_ON_MENU];
    void (*menuInit)(void);
    void (*menuExit)(void);
    LCD_MENU_ITEM *menuItems[MAX_ITEMS_ON_MENU];
    //struct menu *subMenu[];
} LCD_MENU;


/*
Call this method to init all the menu's

-Returns:       a LCD_MENU error code
*/
int menu_initMenus();

/*
Function to switch between menu's

-Returns:       a LCD_MENU error code
-Parameters:    ID of next menu
*/
int menu_switchMenu(unsigned int);



#endif