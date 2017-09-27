//*****************************************************************************
/*

File: FIFO.h
Hardware: Generic
Project: UART0_SWFifo
Author: Simon
Date: 30.04.2016
Revison: Variante 2
Info: Generic SW-Fifo

	Die Software verfügt über einen Buffer sowie einen Schreib- und einen Leseindex (_Put und _Get). Die Länge des Fifo muss ein Vielfaches von 2 sein!

	_fifo_put:
		legt Daten im Buffer ab und inkementiert _Put. Die Routine liefert eine negative Antwort wenn keine
		Daten mehr im Buffer abgelegt werden können (voll) und eine positive Antwort, wenn das Speichern erfolgreich war

	_fifo_get:
		holt Daten aus den Buffer und inkrementiert _Get. Die Routine liefert eine negative Antwort wenn keine Daten abgeholt werden
		können (leer) und eine positive Antwort, wenn ein Byte erfolgreich aus dem Fifo geholt wurde.

	Zugriffe auf _fifo_put und _fifo_get sollten nach Möglichkeit atomar erfolgen

	1) Anzeige ob Daten im Fifo vorhanden:
	Ob Daten abholbereit im jeweiligen FIFO vorhanden sind kann z.B. über eine Flag
	der aufrufenden Routine angezeit werden. Diese Flag wird einmalig gesetzt wenn der Rückgabewert
	von fifo_put positiv ist. Diese kann wiederum negiert werden, wenn fifo_get eine negative Antwort zurückliefert.

	2) Anzeige ob der Fifo voll ist:
	Ebenso kann über eine Flag angezeit werden ob der Fifo voll ist wenn fifo_put eine negative Antwort zurückliefert
	Diese Flag kann dann bei Aufrufen der Funktion fifo_read negiert werden da Daten aus dem Puffer abgeholt werden.

	3) Anzeige der Größe des Fifo
	Die Funktion NAME ## fifo_size() gibt die Anzahl der derzeitigen Elemente im Fifo zurück

	Optional könnten die Daten des jeweilgen Fifos über DMA an den UART übertragen bzw. vom UART geholt werden.

	Quelle: http://www.mikrocontroller.net/articles/FIFO
*/
//*****************************************************************************

/*****************************************************
					Precompiler
******************************************************/

#ifndef _FIFO_H
#define _FIFO_H

#include <stdint.h>

/*****************************************************
					FIFO
******************************************************/

// Version A Mikrocontroller.com


#define AddIndexFifo(NAME,SIZE,TYPE,SUCCESS,FAIL,MASK)	\
TYPE NAME ## Buffer[SIZE];	\
uint8_t NAME ## Put;	\
uint8_t NAME ## Get;	\
void NAME ## fifo_Init(void){	\
NAME ## Put = NAME ## Get = 0;	\
}	\
uint8_t NAME ## fifo_put(uint8_t data){	\
uint8_t next = ((NAME ## Put + 1) & MASK); \
if (NAME ## Get == next){	\
return(FAIL);	\
}	\
NAME ## Buffer[NAME ## Put] = data;	\
NAME ## Put = next;	\
return(SUCCESS);	\
}	\
uint8_t  NAME ## fifo_get(uint8_t *data){	\
if ( NAME ## Get == NAME ## Put){	\
return(FAIL);	\
}	\
*data = NAME ## Buffer[NAME ## Get];	\
NAME ## Get = (NAME ## Get + 1) & MASK;	\
return(SUCCESS);	\
}\
uint8_t NAME ## fifo_size(void){	\
return (NAME ## Put - NAME ## Get);	\
}

#endif
