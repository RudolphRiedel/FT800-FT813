/**
 * @file	Serialport.h
 * @author	Robert Steigemann
 * @date	15 January 2017
 * @brief	Serialport communication via RS232
 *
 * This library contains functions to communicate with the PC via RS232.
 *
 * @license creative commons license CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/
 */

#ifndef SERIALPORT_H_
#define SERIALPORT_H_

// define your hardware here:
#define SERIALPORT_UART		USARTC0
#define SERIALPORT_RXC_vect USARTC0_RXC_vect
#define SERIALPORT_DRE_vect USARTC0_DRE_vect
// set pin directions
#define SERIALPORT_TXD_TO_OUTPUT (PORTC_DIRSET = PIN3_bm)
#define SERIALPORT_RXD_TO_INPUT  (PORTC_DIRCLR = PIN2_bm)

/**
 * @brief This function initializes the USART to 8N1 with any possible Baudrate and enables the RXC interrupt.
 */
uint8_t serialport_init(uint32_t baudrate);

/**
 * @brief This function send an array (string).
 *
 * It uses 64 byte buffer. It is possible to overrun the buffer, don't send more than 64 bytes at one Time!
 * Check the buffer space with the serialport_checkSpace() function, before you use this function.\n
 * Sending is interrupt driven and don't stop the controller. The function uses only the arrayLength.
 * String end (0x00) would not be recognized!
 *
 * @param *anArray The pointer to the array with the data to send.
 * @param arrayLength The number of bytes to send.
 */
void serialport_write(uint8_t *anArray, uint8_t arrayLength);

/**
 * @brief This function check how many bytes are free in the transmit buffer.
 *
 * @return Number of free bytes in the transmit buffer.
 */ 
uint8_t serialport_checkSpace(void);

/**
 * @brief This function gives you all received data to the next CR (carriage return) in string format.
 *
 * To get the string, it is necessary that your array is big enough to hold all data to the CR.
 * If is not (maxLength reached, before CR is) this function return 0 and data is lost.
 * The maxLength means all chars in the string including 0x00, so it should equals the array length!
 *
 * @param *anArray The pointer to the array assigned with the data. It have to be big enough to store all data received!
 * @param maxLength The length of the array. Correct value is necessary to prevent array overflow.
 *
 * @return The amount of characters stored in the array including the string end sign 0x00!
 *		   If 0 is returned, no CR is received or the array is to small.
 */
uint8_t serialport_getLastString(uint8_t *anArray, uint8_t maxLength);

/**
 * @brief This function is like printf.
 *
 * It uses 64 byte buffer. Your output have to be shorter than 64 chars.
 * It use serialport_checkSpace() function and wait for enough space if the buffer is full.\n
 * Sending is interrupt driven and don't stop the controller.
 */
void serialport_printf(const char *__fmt, ...);

void serialport_print(char* aString);


#endif /* SERIALPORT_H_ */