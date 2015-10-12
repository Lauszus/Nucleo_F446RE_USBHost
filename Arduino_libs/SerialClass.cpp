#include "SerialClass.h"

size_t SerialClass::write(uint8_t data) {
	return write(&data, 1);
}

size_t SerialClass::write(const uint8_t *buffer, size_t size) {
	uint8_t *pBuffer = (uint8_t*)buffer;
	HAL_UART_Transmit(pUART_Handle, pBuffer, size, HAL_MAX_DELAY);
	return size;
}

int SerialClass::read() {
	uint8_t data;
	HAL_UART_Receive(pUART_Handle, &data, 1, HAL_MAX_DELAY);
	return data;
}

int SerialClass::available() {
	return -1;
}

int SerialClass::peek() {
	return -1;
}

void SerialClass::flush() {
}
