//*****************************************************************************
/*

File: FIFO.h
Hardware: Generic
Project: UART0_SWFifo
Author: Simon
Date: 30.04.2016
Revison: Variante 2
Info: Generic SW-Fifo

	Die Software verf�gt �ber einen Buffer sowie einen Schreib- und einen Leseindex (_Put und _Get). Die L�nge des Fifo muss ein Vielfaches von 2 sein!

	_fifo_put:
		legt Daten im Buffer ab und inkementiert _Put. Die Routine liefert eine negative Antwort wenn keine
		Daten mehr im Buffer abgelegt werden k�nnen (voll) und eine positive Antwort, wenn das Speichern erfolgreich war

	_fifo_get:
		holt Daten aus den Buffer und inkrementiert _Get. Die Routine liefert eine negative Antwort wenn keine Daten abgeholt werden
		k�nnen (leer) und eine positive Antwort, wenn ein Byte erfolgreich aus dem Fifo geholt wurde.

	Zugriffe auf _fifo_put und _fifo_get sollten nach M�glichkeit atomar erfolgen

	1) Anzeige ob Daten im Fifo vorhanden:
	Ob Daten abholbereit im jeweiligen FIFO vorhanden sind kann z.B. �ber eine Flag
	der aufrufenden Routine angezeit werden. Diese Flag wird einmalig gesetzt wenn der R�ckgabewert
	von fifo_put positiv ist. Diese kann wiederum negiert werden, wenn fifo_get eine negative Antwort zur�ckliefert.

	2) Anzeige ob der Fifo voll ist:
	Ebenso kann �ber eine Flag angezeit werden ob der Fifo voll ist wenn fifo_put eine negative Antwort zur�ckliefert
	Diese Flag kann dann bei Aufrufen der Funktion fifo_read negiert werden da Daten aus dem Puffer abgeholt werden.

	3) Anzeige der Gr��e des Fifo
	Die Funktion NAME ## fifo_size() gibt die Anzahl der derzeitigen Elemente im Fifo zur�ck

	Optional k�nnten die Daten des jeweilgen Fifos �ber DMA an den UART �bertragen bzw. vom UART geholt werden.

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
