# STM32-IntermittentSecurity
This repository contains a secure checkpoint utility for intermittent computing systems based on STM32U5, featuring ARM Trustzone for Cortex-M.

## Index
1. [Features](#features)
2. [Hardware](#hardware)
3. [Software](#software)
4. [Testing](#testing)
5. [Caveats](#caveats)

## Features
This utility allows to securely checkpoint the state of secure world (peripheral state excluded).

Selectable confidentiality for global variables located in the .data and .bss sections is available via compiler attributes.
The stack, instead, is only authenticated.

Checkpoint and restoration utilities are invoked via system calls.
After a successful restoration, the *restore* system call exists as if it was the *checkpoint* system call that generated the checkpoint.

There are some limitations:
- no peripheral state is checkpointed.
- non-secure world is not checkpointed.
- stack is authenticated but not encrypted.
- heap is not checkpointed.
- interrupts may interfere with the checkpoint creation and restoration routines.
- flash page clearance for checkpoint nonces is currently not implemented.
- secure boot functionalities are not implemented.
- code is not optimized (e.g. using DMA functionalities)

## Hardware
The following hardware is required to build and test the prototype:
- [B-U585I-IOT02A Discovery kit](https://www.st.com/en/evaluation-tools/b-u585i-iot02a.html)
- [CY15B102QN-50PZXI](https://www.infineon.com/cms/en/product/memories/f-ram-ferroelectric-ram/excelon-f-ram/cy15b102qn-50pzxi/) (3.3V and  8-pin DIP version)
- Breadboard and jumpers

Position the memory chip on the breadboard and connect it via jumper cables to the SPI1 interface of the development board.
The interface is accessible from the ARDUINO connectors on the back of the board.
These are described at page 32 of the board's [user manual](https://www.st.com/resource/en/user_manual/um2839-discovery-kit-for-iot-node-with-stm32u5-series-stmicroelectronics.pdf).
The ARDUINO connectors also provide GND and 3.3V power for the memory chip.

Tie the DNU and WP pins of the memory chip to VDD.
Tying WP to VDD disables write protection functionalities, which are not required for this project.

The USART3 interface is required for the test.
This is also accessible from the ARDUINO connectors.
This will be configured and used as a UART interface,to log the measured clock cycles.

## Software
The solution was developed using [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html), therefore instruction are provided for this development enviroment.
Copyrighted code by STMicroelectronics is not included in this repository, but this can be generated via either [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) (described in this guide) or [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html).


### Board configuration
Board options bytes must be configured to activate Trustzone and define the default secure/non-secure flash memory partition.
This can be done connecting to the board via [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html).

The default Trustzone configuration, which HAL drivers expects, is the following:
- *TZEN=1* (all memory is secure)
- *SECWM1_PSTRT=0x0* and *SECWM1_PEND=0x7F* (meaning all 128 pages of Bank1 set as secure)
- *SECWM2_PSTRT=0x7F* and *SECWM2_PEND=0x0* (meaning all 128 pages of Bank2 set as non-secure)

### Project configuration
Using [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html), create a new *STM32 project*.
In the section *board selector* search the name of the board (B-U585I-IOT02A) and select it.
When asked, enable Trustzone.
This will create two subprojects, one for the non-secure image and the other for the secure image.
When asked to set up peripherals to their default configuration, answer **NO**.

An *.ioc* file is generated inside the home of the project
It opens a graphical interface that can be used to automatically generate configuration code for the microcontroller.

### Flash configuration
Open the *.ioc* and enter the *Tools* page.
A picture of the address space should appear.

Click on the *FLASH (S)* area inside *Flash Bank1 Memory (S)*, and reduce its size to 760 KB.
Click on the empty section that appears and define a new memory area called *FLASH_CKP* of 256 KB.
Mark the new area as *secure*, set access permission to *RW by privileged only* and disable code execution.

### SPI interface
Open the *.ioc* file and enter the *Pinout & Configuration* page.
Find the SPI1 interface from the later menu, inside the *Connectivity* section.
Set the following main configuration.

- Runtime context -> *Cortex-M33 secure*
- Mode -> *Full-Duplex Master*
- Hardware NSS signal -> *disable*
- Hardware RDY signal -> *disable*

In the *Parameter settings* menu specify the following parameters.
The memory was limited to 20MHz, since it is the maximum stable frequency for our configuration.

- Frame format -> *Motorola*
- Data size -> *8 Bits*
- First bit -> *MSB First* 
- Prescaler -> *8* (to achieve a 20 Mbit/s baud rate)
- Configure SPI mode 0:
  - Clock Polarity (CPOL) -> *0* (idle low)
  - Clock Phase (CPHA) -> *0* (edge one of clock, raising or falling depends on polarity)
- NSSP Mode -> *disabled* (no chip select pulses in between data frames, [ref](https://www.st.com/content/ccc/resource/training/technical/product_training/group0/3e/ee/cd/b7/84/4b/45/ee/STM32F7_Peripheral_SPI/files/STM32F7_Peripheral_SPI.pdf/_jcr_content/translations/en.STM32F7_Peripheral_SPI.pdf))
- *Leave others as default*

Using the *Pinout view*, configure the MCU pins as follows:
- PE13 -> *SPI1_CLK*
- PE14 -> *SPI1_MISO* (master in, slave out)
- PE15 -> *SPI1_MOSI* (master out, slave in)

In the *GPIO setting* menu of the interface the pins should appear configured as follows:
- Pin Context Assignment -> *Cortex-M33 Secure*
- GPIO Mode -> *Alternate Function Push Pull*
- GPIO Pull-up/Pull-down -> *No pull-up and no pull-down*
- Maximum output speed -> *Low*

Using the *Pinout view* perform the following configuration (PE12 will act as a software-driven chip select):
- PE12 -> *GPIO output*

From *Pinout & Configuration* enter the *System core* section in the lateral menu and select *GPIO*.
Find PE12 and change its configuration to the following:
- Pin Context Assignment -> *Cortex-M33 Secure*
- GPIO output level -> High
- GPIO Mode -> *Output Push Pull*
- GPIO Pull-up/Pull-down -> *No pull-up and no pull-down*
- Maximum output speed -> *Low*
- User label -> *SPI1_CS*

### Crypto modules
From the *Pinout & Configuration* page, enter the *Security* section and configure the RNG, SAES and AES modules as follows:
- Runtime context -> *Cortex-M33 secure*
- Activated -> *True*

Leave al the other parameters as default, as they will be configured with custom code.

### UART interface
From the *Pinout & Configuration* page, enter the *Connectivity* section and configure USART3 as follows:

- Runtime context -> *Cortex-M33 secure*
- Mode -> *Asynchronous*
- Hardware Flow Control (RS232) -> *disable*
- Hardware RDY signal -> *disable*

In the Parameter settings menu specify the following parameters:
- *Baud rate* -> *9600* (depends on the used setup)
- *Leave others as default*

Using the Pinout view, attribute the MCU pins as follows:
- PD8 -> *USART3_TX*
- PD9 -> *USART3_RX*

Check this change in the *GPIO settings* section.

### Code generation
Save the *.ioc* file to perform code generation.
This will add code to the project that performs the configuration specified via the graphical interface.

### Checkpoint utility
Copy the content of the *Inc* folder inside the *Core/Inc* folder in the secure project.

Copy the content of the *Src* folder, **EXCEPT** the *changes* sub-folder, inside the *Core/Src* folder in the secure project.

The *changes* sub-folder contains files that are already in the project and need to be modified.
Perform the modifications as explained in the files.

The *FLASH.ld* file contains the modification to be performed to the linker script of the secure project.
Perform these modifications.

### Build and run
To build both images, right-click on the non-secure project and select *Build Project*.

Once the images are build, create a new debug configuration, selecting as the main image the secure image (this must be the first to run).
Then, in the startup section of the debug configuration menu, click on *add* and select the non-secure image.

## Testing
All testing code is run in secure world the jump to non-secure.

In the *ckp_test.h*, via defines, it is possible to select whether to run the overall test or the microbenchmark, which logs the time for the single operations.

By changing the experiment number, the code will increase the size of the of an array in the .bss section, simulating and increased memory size.
Some experiment make the array confidential.
Detail of the experiment are in the *ckp_test_perf.c* file.

If the code work, it ends up in a restore loop, continuously logging the performance metrics of the restoration utility.
Information on how interpret the logs is found in *ckp_dwt.h*.

## Caveats
- *SystemClock_Config()* must be performed at boot in secure world (this should be set automatically when the RNG is configured).
- External memory
	- In our setup the max FRAM memory frequency achievable was 20 MHz.
	- All GPIO output speed were set to low. Higher speed did not work.
	- We ofter encountered problem with corrupted reads from the memory when jumpers were not making a good connection.
	- A custom PCB should help solve these problems







