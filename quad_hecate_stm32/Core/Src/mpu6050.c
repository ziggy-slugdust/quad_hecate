#include <mpu6050.h>
#include <main.h>
#include <stdio.h>

#include "ssd1306.h"
#include "ssd1306_fonts.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;

void led_blink(uint16_t led, uint16_t repeat, uint16_t delay);

uint8_t count = 0;

void mpu6050_init()
{
	HAL_StatusTypeDef ret = HAL_I2C_IsDeviceReady(&hi2c1, (DEVICE_ADDRESS <<1) + 0, 2, 200);

	if (ret == HAL_OK)
	{
      HAL_GPIO_WritePin(GPIOA,  LED_PIN_BAD, GPIO_PIN_SET); // LED OFF
	  printf(" THE DEVICE IS READY \n");
	  led_blink(LED_PIN,10,50);
	}

		else
		{
		  printf(" THE DEVICE IS NOT READY \n");
		  led_blink(LED_PIN_BAD,10,50);
		}

	uint8_t temp_data = FS_GYRO_500;
	ret = HAL_I2C_Mem_Write(&hi2c1, (DEVICE_ADDRESS <<1) + 0, REG_CONFIG_GYRO, 1, &temp_data, 1, 100);

	if (ret == HAL_OK)
	{
	  printf("Configuring gyroscope\n");
	  led_blink(LED_PIN,10,50);
	}

		else
		{
			printf("Failed to configure gyroscope\n");
			  led_blink(LED_PIN_BAD,10,50);
		}

	temp_data = FS_ACC_2G;
	ret = HAL_I2C_Mem_Write(&hi2c1, (DEVICE_ADDRESS <<1) + 0, REG_CONFIG_ACC, 1, &temp_data, 1, 100);

	if (ret == HAL_OK)
	{
	  printf("Configuring accelerometer\n");
	  led_blink(LED_PIN,10,50);
	}

		else
		{
			printf("Failed to configure accelerometer\n");
			led_blink(LED_PIN_BAD,10,50);
		}

	temp_data = 0;
	ret = HAL_I2C_Mem_Write(&hi2c1, (DEVICE_ADDRESS <<1) + 0, REG_USR_CTRL, 1, &temp_data, 1, 100);

	if (ret == HAL_OK)
	{
	  printf("Exiting from sleep mode\n");
	  led_blink(LED_PIN,10,50);
	}

		else
		{
			printf("Failed to exit from sleep mode\n");
			led_blink(LED_PIN_BAD,10,50);
		}
}


void mpu6050_read()
{
	uint8_t len = 14;
	uint8_t data[len];


	int16_t x_acc;
	int16_t y_acc;
	int16_t z_acc;

	int16_t tempature;

	int16_t x_gyro;
	int16_t y_gyro;
	int16_t z_gyro;


	HAL_I2C_Mem_Read(&hi2c1, (DEVICE_ADDRESS <<1) + 1, REG_DATA, 1, data, len, 1);
	x_acc = ((int16_t)data[0] << 8) | data[1];
	float x_acc_cal = (float)x_acc/16384.0;


	y_acc = ((int16_t)data[2] << 8) | data[3];
	float y_acc_cal = (float)y_acc/16384.0;


	z_acc = ((int16_t)data[4] << 8) | data[5];
	float z_acc_cal = (float)z_acc/16384.0;

	tempature = ((int16_t)data[6] << 8) | data[7];
	float tempature_cal = (float)tempature/340 + 36;

	x_gyro = ((int16_t)data[8] << 8) | data[9];
	float x_gyro_cal = (float)x_gyro/16384.0;


	y_gyro = ((int16_t)data[10] << 8) | data[11];
	float y_gyro_cal = (float)y_gyro/16384.0;


	z_gyro = ((int16_t)data[12] << 8) | data[13];
	float z_gyro_cal = (float)z_gyro/16384.0;

    ssd1306_Fill(Black);


//	char buff[64];
//	sprintf(buff,  "xaccl: %f", x_acc_cal);
//
//	ssd1306_SetCursor(0,0);
//	ssd1306_WriteString(buff, Font_11x18, White);
//
//	ssd1306_Line(1,SSD1306_HEIGHT,SSD1306_WIDTH-1,SSD1306_HEIGHT*(1-x_acc_cal) - 1,White);
//
//	ssd1306_UpdateScreen();


    count++;

	if(count >50)
	{
		uint8_t xacc_buff[20];
		uint8_t new_buff[] = "\n";
		sprintf(xacc_buff, "xaccl: %fg", x_acc_cal);
		HAL_UART_Transmit(&huart1, xacc_buff, 12, 100);
		HAL_UART_Transmit(&huart1, new_buff, 12, 100);
		count = 0;

	}


    char buff[64];
    sprintf(buff,  "temp: %f", tempature_cal);
    ssd1306_SetCursor(0,0);
    ssd1306_WriteString(buff, Font_11x18, White);




	ssd1306_DrawCircle(64-64*y_acc_cal, 32-32*x_acc_cal, 10, White);
	ssd1306_DrawCircle(64-64*x_gyro_cal, 32+32*y_gyro_cal, 5, White);


	ssd1306_UpdateScreen();
	return;

}

void led_blink(uint16_t led, uint16_t repeat, uint16_t delay){
	uint8_t i;
	repeat = repeat/2;

	for (i = 0; i<repeat; i++){
		HAL_GPIO_WritePin(GPIOA,  led, GPIO_PIN_RESET); // turn on
		HAL_Delay(delay);
		HAL_GPIO_WritePin(GPIOA,  led, GPIO_PIN_SET); // turn off
		HAL_Delay(delay);
	}

}







