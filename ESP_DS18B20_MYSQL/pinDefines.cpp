//////////////////////////////////////////////////
//////////////////////////////////////////////////
////////////// Autor:Ignacio Islas 	//////////////
//////////////		  4/21/22			    //////////////
//////////////	    EcoControl		  //////////////
//////////////	   pinDefines.cpp   //////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////

#include "pinDefines.h"
#include "esp32-hal-gpio.h"

// Class Reless
Reless::Reless(uint8_t PIN_INIT) //Constructor class
{
	this->PIN=PIN_INIT;
	pinMode(this->PIN, OUTPUT);
	digitalWrite(this->PIN, LOW);
}

void Reless::High() //Set Pin to HIGH
{
	digitalWrite(this->PIN, HIGH);
	this->status=HIGH;
}

void Reless::Low() //Set Pin to LOW
{
	digitalWrite(this->PIN, LOW);
	this->status=LOW;
}


void Reless::toggle() //Toggle PIN status
{
	if (this->status==HIGH)
	{
		digitalWrite(this->PIN, LOW);
		this->status=LOW;
	} 
	else if(this->status==LOW)
	{
		digitalWrite(this->PIN, HIGH);
		this->status=HIGH;
	}
}

// Class Reless

Opto::Opto(uint8_t PIN_INIT) //Constructor class
{
	this->PIN=PIN_INIT;
	pinMode(this->PIN, INPUT);
  this->status=digitalRead(this->PIN);
}

void Opto::enableInterrupt(void (*functionHandler)(), int modo) 
//! Enable Interrupt
//! Function callback = functionHandler[IRAM_ATTR atribute for ESP32]
//! modo=(LOW, HIGH, CHANGE,RISING,FALLING)
//!LOW to trigger the interrupt whenever the pin is low,
//!CHANGE to trigger the interrupt whenever the pin changes value
//!RISING to trigger when the pin goes from low to high,
//!FALLING for when the pin goes from high to low.
//!HIGH to trigger the interrupt whenever the pin is high.
{
  this->funcionCallback = &functionHandler;
  attachInterrupt(digitalPinToInterrupt(this->PIN), functionHandler,  modo);
}

uint8_t Opto::ReadOpto() //Leer valor de los opto
{
  this->status=digitalRead(this->PIN);
  return this->status;
}

void Opto::disableInterrupt() //deshabilito la interrupcion
{
  detachInterrupt(this->PIN);
}

