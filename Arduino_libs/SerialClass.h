#ifndef __serial_h__
#define __serial_h__

#include "Stream.h"
#include "stm32f4xx_hal.h"

class SerialClass : public Stream {
public:
	SerialClass(UART_HandleTypeDef *UART_Handle) : pUART_Handle(UART_Handle) {
	};

	size_t write(uint8_t data);
	size_t write(const uint8_t *buffer, size_t size);
	using Print::write; // Pull in write(const char *str) from Print
	int read();

	// These are NOT implemented!
	int available();
	int peek();
	void flush();

private:
	UART_HandleTypeDef *pUART_Handle;
};

extern SerialClass Serial; // Create this constructor in your main.cpp

#endif
