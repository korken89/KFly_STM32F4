/* *
 *
 * Hardware Communication Layer for the IC bus
 *
 * TODO:
 * Everything...
 *
 * */


#include "i2c.h"

/* Gobal variable defines */

/* Private Typedefs */

/* *
 * The I2C read on the STM32F4xx is not symetric so special care
 * must be taken if the number of bytes to be read are one, two
 * or more than two.
 *
 * This enum identifies, for the interrupt handeler, which size
 * of read that shall be performed.
 * */
typedef enum
{
	I2C_READ_ONE = 0,
	I2C_READ_TWO,
	I2C_READ_MORE_THAN_TWO
} I2C_READ_LENGTH_Type;

/* Private function defines */

/* Private external functions */
