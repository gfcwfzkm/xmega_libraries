# Driver / Library Interface Functions
In order to interface with the various peripheral drivers for different microcontrollers and the many different IC-drivers, I had to come up with a way of connecting different libraries to different controllers and peripherals with as little code-changes as possible.

The solution to this is a set of I/O functions that are as generic as possible, in order to interface with many different kind of peripheral libraries - while also making it easy enough to add these I/O functions to HAL-drivers with just a few, basic glue-functions.

## Basic Overview
The I/O functions are kept as simple as possible, consisting only of two functions to initialise or close the interface and three functions to send the actual data through. 

While the peripheral drivers provides the functions themself, the IC drivers need a way to hold the function pointer and the potential structure pointer to the peripheral driver.
<table><tr>
<td> Description </td><td> Peripheral Driver Side </td> <td> IC Driver Side </td>
</tr>
<tr><td>
Driver Structure and slave-IC related info<br>(For example /CS I/O in SPI)
</td>
<td>

```c
typedef struct {...} Driver_t;
```
</td><td>

```c
void *ioInterface;
```
</tr><tr><td>
Preparing the peripheral driver <br>
for an new I/O operation
</td><td>

```c
uint8_t perDrv_IntPrepare(void *intv);
```
</td><td>

```c
uint8_t (*startTransaction)(void*);
```
</tr><tr><td>
Sending Data from the buffer to <br>
the slave IC, with the option of <br>
sending an address byte first
</td><td>

```c
uint8_t perDrv_IntSendBytes(void *intv,
                        uint8_t addr,
                        uint8_t *buf_ptr,
                        uint16_t buf_len);
```
</td><td>

```c
uint8_t (*sendBytes)(void*,
                    uint8_t,
                    uint8_t*,
                    uint16_t);
```
</tr><tr><td>
Both sending & receiving Data from<br>
the buffer to the slave IC, with the<br>
option of sending an address byte first
</td><td>

```c
uint8_t perDrv_IntTransceiveBytes(void *intv,
                        uint8_t addr,
                        uint8_t *buf_ptr,
                        uint16_t buf_len);
```
</td><td>

```c
uint8_t (*transceiveBytes)(void*,
                    uint8_t,
                    uint8_t*,
                    uint16_t);
```
</tr><tr><td>
Receiving Data from the buffer to <br>
the slave IC, with the option of <br>
sending an address byte first
</td><td>

```c
uint8_t perDrv_IntGetBytes(void *intv,
                        uint8_t addr,
                        uint8_t *buf_ptr,
                        uint16_t buf_len);
```
</td><td>

```c
uint8_t (*getBytes)(void*,
                    uint8_t,
                    uint8_t*,
                    uint16_t);
```
</tr><tr><td>
Closing the peripheral driver <br>
about the last I/O operation
</td><td>

```c
uint8_t perDrv_IntFinish(void *intv);
```
</td><td>

```c
uint8_t (*endTransaction)(void*);
```
</tr>
</table>

## Function explanation
___
### Interface Prepare
```c
uint8_t perDrv_IntPrepare(void *intv);  			// Generic function name / example
uint8_t usartSPI_InterfacePrepare(void *intUSPI);	// Example for USART-SPI
```
This functions warns the peripheral library about an starting I/O transaction and allows it to set thigns up. Especially with SPI, the chip-select can be controlled by the peripheral driver inside this function, taking any direct I/O access away from the IC driver and making it as generic / portable as possible.

Usually the data structure of the perhiperal driver is passed over, which might contain additional information like microcontroller-I/O for chip-select control of slave ICs.

`! Needs to be called BEFORE any of the three I/O functions !`
#### Parameters:
* `void *intv` : Pointer to the peripheral driver's data structure
#### Returns:
* `uint8_t` : Returns non-zero in case of an internal error
___
### Interface Send Bytes
```c
uint8_t perDrv_IntSendBytes(void *intv,             // Generic function name / example
                        uint8_t addr,
                        uint8_t *buf_ptr,
                        uint16_t buf_len);
uint8_t usartSPI_InterfaceSendBytes(void *intUSPI   // Example for USART-SPI
                        uint8_t addr,
                        uint8_t *buf_ptr,
                        uint16_t buf_len); 
```
Basic function to send bytes via the peripheral driver. This function should only be called after `perDrv_IntPrepare` in order to ensure that the peripheral driver is ready to handle the transaction. The function needs a pointer to the peripheral driver's data structure, an address, a pointer to the buffer containing all the data to be sent and the amount of bytes to send.

The address is optional, as not every interface uses addresses (I2C does, SPI doesn't, for example). If a interface doesn't support addresses but a non-zero value is given to this parameter, it will first send that value before sending the buffer's contents.
#### Parameters:
* `void *intv` : Pointer to the peripheral driver's data structure
* `uint8_t addr` : Optional address of the slave-IC, only sent when the value is not zero
* `uint8_t *buf_ptr` : Pointer to the buffer with the data to be sent
* `uint8_t buf_len` : Amount of botes in the buffer to send
#### Returns:
* `uint8_t` : Returns non-zero in case of an internal error
___
### Interface Transceive Bytes
```c
uint8_t perDrv_IntTransceiveBytes(void *intv,       		// Generic function name / example
                        uint8_t addr,
                        uint8_t *buf_ptr,
                        uint16_t buf_len);
uint8_t usartSPI_InterfaceTransceiveBytes(void *intUSPI,	// Example for USART-SPI
                        uint8_t addr,
                        uint8_t *buf_ptr,
                        uint16_t buf_len);
```
This function sends the bytes from the buffer and stores the incoming bytes right back into the buffer. This feature might be mostly used in SPI peripheral drivers. This function should only be called after `perDrv_IntPrepare` in order to ensure that the peripheral driver is ready to handle the operation.

The address is optional, as not every interface uses addresses (I2C does, SPI doesn't, for example). If a interface doesn't support addresses but a non-zero value is given to this parameter, it will first send that value before sending the buffer's contents.
#### Parameters:
* `void *intv` : Pointer to the peripheral driver's data structure
* `uint8_t addr` : Optional address of the slave-IC, only sent when the value is not zero
* `uint8_t *buf_ptr` : Pointer to the buffer with the data to be sent
* `uint8_t buf_len` : Amount of botes in the buffer to send
#### Returns:
* `uint8_t` : Returns non-zero in case of an internal error
___
### Interface Get Bytes
```c
uint8_t perDrv_IntGetBytes(void *intv,              // Generic function name / example
                        uint8_t addr,
                        uint8_t *buf_ptr,
                        uint16_t buf_len);
uint8_t usartSPI_InterfaceGetBytes(void *intUSPI,   // Example for USART-SPI
                        uint8_t addr,
                        uint8_t *buf_ptr,
                        uint16_t buf_len);
```
Receiving data from the peripheral driver and storing it in the buffer. This function should only be called after `perDrv_IntPrepare` in order to ensure that the peripheral driver is ready to handle the operation.

The address is optional, as not every interface uses addresses (I2C does, SPI doesn't, for example). If a interface doesn't support addresses but a non-zero value is given to this parameter, it will first send that value before sending the buffer's contents.
#### Parameters:
* `void *intv` : Pointer to the peripheral driver's data structure
* `uint8_t addr` : Optional address of the slave-IC, only sent when the value is not zero
* `uint8_t *buf_ptr` : Pointer to the buffer with the data to be sent
* `uint8_t buf_len` : Amount of botes in the buffer to send
#### Returns:
* `uint8_t` : Returns non-zero in case of an internal error
___
### Interface Finish
```c
uint8_t perDrv_IntFinish(void *intv);               // Generic function name / example
uint8_t usartSPI_InterfaceFinish(void *intUSPI);    // Example for USART-SPI
```
This functions informs the peripheral library that an I/O transaction has finished. Especially with SPI, the chip-select can be controlled by the peripheral driver inside this function, taking any direct I/O access away from the IC driver and making it as generic / portable as possible.

`! Needs to be called AFTER any of the three I/O functions !`
#### Parameters:
* `void *intv` : Pointer to the peripheral driver's data structure
#### Returns:
* `uint8_t` : Returns non-zero in case of an internal error