/*
 * menu.h
 *
 *  Created on: Apr 10, 2024
 *      Author: hudy
 */

#ifndef INC_MENU_H_
#define INC_MENU_H_

typedef struct {
	const char *name;
	int subMenu;
} MenuItem;

typedef struct {
	int index; //obecny index
	int indexResult; //index do wchodzenia do odpowiednich menu bez submenu
	int lcdDisplayStart; //liczba od ktorej mamu wyswietlac
	int lcdIndexStart; //maksymalna liczba menu albo submenu
} Menu;

#endif /* INC_MENU_H_ */
