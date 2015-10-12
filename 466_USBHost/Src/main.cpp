/*
 Example sketch for the PS4 Bluetooth library for the STM32F4 platform - developed by Kristian Sloth Lauszus
 For more information visit my blog: http://blog.tkjelectronics.dk/ or
 send me an e-mail: lauszus@gmail.com
 */

#include "stm32f4xx_hal.h"
#include <PS4BT.h>
#include <usbhub.h>

SPI_HandleTypeDef SPI_Handle;
UART_HandleTypeDef UART_Handle;

SerialClass Serial(&UART_Handle); // Arduino style Serial class

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so

/* You can create the instance of the PS4BT class in two ways */
// This will start an inquiry and then pair with the PS4 controller - you only have to do this once
// You will need to hold down the PS and Share button at the same time, the PS4 controller will then start to blink rapidly indicating that it is in paring mode
//PS4BT PS4(&Btd, PAIR);
// After that you can simply create the instance like so and then press the PS button on the device
PS4BT PS4(&Btd);

static bool printAngle, printTouch;
static uint8_t oldL2Value, oldR2Value;
static bool buttonPressed;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);

int main(void) {
	HAL_Init(); // Reset of all peripherals, Initializes the Flash interface and the Systick
	SystemClock_Config(); // Configure the system clock

	// Initialize all configured peripherals
	MX_GPIO_Init();
	MX_SPI1_Init();
	MX_USART2_UART_Init();

	if (Usb.Init() == -1) {
		Serial.print(F("\r\nOSC did not start"));
		while (1); // Halt
	}
	Serial.print(F("\r\nPS4 Bluetooth Library Started"));

	while (1) {
		Usb.Task();

		if (PS4.connected()) {
			if (PS4.getAnalogHat(LeftHatX) > 137 || PS4.getAnalogHat(LeftHatX) < 117 || PS4.getAnalogHat(LeftHatY) > 137 || PS4.getAnalogHat(LeftHatY) < 117 || PS4.getAnalogHat(RightHatX) > 137 || PS4.getAnalogHat(RightHatX) < 117 || PS4.getAnalogHat(RightHatY) > 137 || PS4.getAnalogHat(RightHatY) < 117) {
				Serial.print(F("\r\nLeftHatX: "));
				Serial.print(PS4.getAnalogHat(LeftHatX));
				Serial.print(F("\tLeftHatY: "));
				Serial.print(PS4.getAnalogHat(LeftHatY));
				Serial.print(F("\tRightHatX: "));
				Serial.print(PS4.getAnalogHat(RightHatX));
				Serial.print(F("\tRightHatY: "));
				Serial.print(PS4.getAnalogHat(RightHatY));
			}

			if (PS4.getAnalogButton(L2) || PS4.getAnalogButton(R2)) { // These are the only analog buttons on the PS4 controller
				Serial.print(F("\r\nL2: "));
				Serial.print(PS4.getAnalogButton(L2));
				Serial.print(F("\tR2: "));
				Serial.print(PS4.getAnalogButton(R2));
			}
			if (PS4.getAnalogButton(L2) != oldL2Value || PS4.getAnalogButton(R2) != oldR2Value) // Only write value if it's different
				PS4.setRumbleOn(PS4.getAnalogButton(L2), PS4.getAnalogButton(R2));
			oldL2Value = PS4.getAnalogButton(L2);
			oldR2Value = PS4.getAnalogButton(R2);

			if (PS4.getButtonClick(PS)) {
				Serial.print(F("\r\nPS"));
				PS4.disconnect();
			} else {
				if (PS4.getButtonClick(TRIANGLE)) {
					Serial.print(F("\r\nTraingle"));
					PS4.setRumbleOn(RumbleLow);
				}
				if (PS4.getButtonClick(CIRCLE)) {
					Serial.print(F("\r\nCircle"));
					PS4.setRumbleOn(RumbleHigh);
				}
				if (PS4.getButtonClick(CROSS)) {
					Serial.print(F("\r\nCross"));
					PS4.setLedFlash(10, 10); // Set it to blink rapidly
				}
				if (PS4.getButtonClick(SQUARE)) {
					Serial.print(F("\r\nSquare"));
					PS4.setLedFlash(0, 0); // Turn off blinking
				}

				if (PS4.getButtonClick(UP)) {
					Serial.print(F("\r\nUp"));
					PS4.setLed(Red);
				}
				if (PS4.getButtonClick(RIGHT)) {
					Serial.print(F("\r\nRight"));
					PS4.setLed(Blue);
				}
				if (PS4.getButtonClick(DOWN)) {
					Serial.print(F("\r\nDown"));
					PS4.setLed(Yellow);
				}
				if (PS4.getButtonClick(LEFT)) {
					Serial.print(F("\r\nLeft"));
					PS4.setLed(Green);
				}

				if (PS4.getButtonClick(L1))
					Serial.print(F("\r\nL1"));
				if (PS4.getButtonClick(L3))
					Serial.print(F("\r\nL3"));
				if (PS4.getButtonClick(R1))
					Serial.print(F("\r\nR1"));
				if (PS4.getButtonClick(R3))
					Serial.print(F("\r\nR3"));

				if (PS4.getButtonClick(SHARE))
					Serial.print(F("\r\nShare"));
				if (PS4.getButtonClick(OPTIONS)) {
					Serial.print(F("\r\nOptions"));
					printAngle = !printAngle;
				}
				if (PS4.getButtonClick(TOUCHPAD)) {
					Serial.print(F("\r\nTouchpad"));
					printTouch = !printTouch;
				}

				if (printAngle) { // Print angle calculated using the accelerometer only
					Serial.print("\r\nPitch: "); // As I have set "-specs=nano.specs" in the linker flags, printf does not support printing floating point number
					Serial.print(PS4.getAngle(Pitch));
					Serial.print("\tRoll: ");
					Serial.print(PS4.getAngle(Roll));
				}

				if (printTouch) { // Print the x, y coordinates of the touchpad
					if (PS4.isTouching(0) || PS4.isTouching(1)) // Print newline and carriage return if any of the fingers are touching the touchpad
						Serial.print(F("\r\n"));
					for (uint8_t i = 0; i < 2; i++) { // The touchpad track two fingers
						if (PS4.isTouching(i)) { // Print the position of the finger if it is touching the touchpad
							Serial.print(F("X")); Serial.print(i + 1); Serial.print(F(": "));
							Serial.print(PS4.getX(i));
							Serial.print(F("\tY")); Serial.print(i + 1); Serial.print(F(": "));
							Serial.print(PS4.getY(i));
							Serial.print(F("\t"));
						}
					}
				}
			}
		} else if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) {
		    if (!buttonPressed)
		        PS4.pair(); // Start paring routine if user button was just pressed
		    buttonPressed = true;
		} else
		    buttonPressed = false;
	}
}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	__PWR_CLK_ENABLE()
	;

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = 16;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	RCC_OscInitStruct.PLL.PLLR = 2;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000000); // NOTE: Edited, so it increments every us

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0); // SysTick_IRQn interrupt configuration
}

// SPI1 init function
void MX_SPI1_Init(void) {
	SPI_Handle.Instance = SPI1;
	SPI_Handle.Init.Mode = SPI_MODE_MASTER;
	SPI_Handle.Init.Direction = SPI_DIRECTION_2LINES;
	SPI_Handle.Init.DataSize = SPI_DATASIZE_8BIT;
	SPI_Handle.Init.CLKPolarity = SPI_POLARITY_LOW;
	SPI_Handle.Init.CLKPhase = SPI_PHASE_1EDGE;
	SPI_Handle.Init.NSS = SPI_NSS_SOFT;
	SPI_Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
	SPI_Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
	SPI_Handle.Init.TIMode = SPI_TIMODE_DISABLED;
	SPI_Handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
	SPI_Handle.Init.CRCPolynomial = 10;
	HAL_SPI_Init(&SPI_Handle);
}

// USART2 init function
void MX_USART2_UART_Init(void) {
	UART_Handle.Instance = USART2;
	UART_Handle.Init.BaudRate = 115200;
	UART_Handle.Init.WordLength = UART_WORDLENGTH_8B;
	UART_Handle.Init.StopBits = UART_STOPBITS_1;
	UART_Handle.Init.Parity = UART_PARITY_NONE;
	UART_Handle.Init.Mode = UART_MODE_TX_RX;
	UART_Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UART_Handle.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&UART_Handle);
}

void MX_GPIO_Init(void) {
	// GPIO Ports Clock Enable
	__GPIOA_CLK_ENABLE()
	;
	__GPIOB_CLK_ENABLE()
	;
	__GPIOC_CLK_ENABLE()
	;

	// Initialize user button
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
