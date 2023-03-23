/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Santiago Valencia Roldan
 * @brief          : Main program body
 ******************************************************************************
 *******************************************************************************/
#include <stdint.h>

int main (void){
	//Ejemplo punteros

	//Definimos variables

	uint8_t dato = 124;

	//Creamos un puntero con la direccion de la variable

	uint8_t *pDato = &dato;

	//Guardar el valor que está en esa direccion de memoria

	uint8_t valorDato = *pDato;

	//Castero básico

	uint16_t casteoDato = (uint16_t) dato;

	//Crear un puntero en una direccion específica

	uint16_t *punteroDireccion = (uint16_t *) 0x20000001;

	//Cambiar la direccion de memoria del puntero

	punteroDireccion = (uint16_t *) 0x20000002;

	//Guardar la direccion de memoria d eun puntero

	uint32_t direccionPuntero = (uint32_t) pDato;

	//Cambiar el valor almacenado en el puntero

	*pDato = 200;

	//Aumentar 8 bits el puntero

	pDato++;

	//Ejemplo Arreglos


	#define sizeOfarray 4
	uint8_t miPrimerArreglo[sizeOfArray] = {5, 0xAE, 'a', 254};

	//Recorrer un arreglo con ciclos
	uint8_t contenido = 0;

	for(uint8_t i=0; i<sizeOfArray; i++){
		contenido = miPrimerArreglo[i];

	}

	for(uint8_t i=0; i<sizeOfArray; i++){
		contenido = *(miPrimerArreglo+i);
	}

	//Cambiar elementos de un arreglo

	miPrimerArreglo[1] = 12;

	*(miPrimerArreglo + 1) = 12;

