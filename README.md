# STM32-IntermittentSecurity
This repository constains a secure checkpoint utility for intermittent computing systems based on STM32U5, featuring ARM Trustzone for Cortex-M.


## Features
### Talk about features and limitations of the implementation

## Repository structure
### Tree view of the repository


## Hardware
The following hardware is required to build and the the prototype:
- [B-U585I-IOT02A Discovery kit](https://www.st.com/en/evaluation-tools/b-u585i-iot02a.html)
- [CY15B102QN-50PZXI](https://www.infineon.com/cms/en/product/memories/f-ram-ferroelectric-ram/excelon-f-ram/cy15b102qn-50pzxi/) (3.3V and  8-pin DIP version)
- Breadboard and jumpers

Position the memory chip on the breadboard and connect it via jumper cables to the SPI1 interface of the development board.
The interface is avaiable from the ARDUINO connectors on the back of the board.
These are described at page 32 of the board's [user manual](https://www.st.com/resource/en/user_manual/um2839-discovery-kit-for-iot-node-with-stm32u5-series-stmicroelectronics.pdf).
The ARDUINO connectors also provide GND and 3.3V power for the memory chip.

Tie the DNU and WP pins of the chip to VDD. Tieing WP to VDD disables write protection functionalities, which are required for this project.

### UART connection

## Software
The solution was developed using [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html), therefore the instruction are provided for this development enviroment.
Copyrighted code by STMicroelectronics is not included in this repository, but this can be generate via either [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) (described in this guide) or [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html).


### Board configuration
Enable Trustzone via option bytes

### Project configuration
Using [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html), create a new *STM32 project*.
In the section *board selector* search the name of the board (B-U585I-IOT02A) and select it.
When asked, enable Trustzone.
This will create two sub-projects, one for the non-secure image and the other for the secure image.
When asked to setup peripherals to their default configuration, answer **NO**.

An *.ioc* file is generated inside the home of the project
It opens a graphical interface that can be used to automatically generate configuration code for the microcontroller.

### Flash configuration

Open the *.ioc* and enter the *Tools* page.
A picture of the address space should appear.

Click on the *FLASH (S)* area inside *Flash Bank1 Memory (S)*, and reduce its size to 760 KB.
Click on the empty section that apperas and define a new memory area called *FLASH_CKP* of 256 KB.
Mark the new area as *secure*, set access permission to *RW by priviledged only* and disable code execution.

### SPI interface
Open the *.ioc* and enter the *Pinout & Configuration* page.
Find the SPI1 interface from the later menu, inside the *Connectivity* section.
Set the following main configuration.

- Runtime context -> *Cortex-M33 secure*
- Mode -> *Full-Duplex Master*
- Hardware NSS signal -> *disable*
- Hardware RDY signal -> *disable*

In the *Parameter settings* menu specify the following parameters. The memory is limited to 20MHz, since it is the maximum stable frequency in our configuration.

- Frame format -> *Motorola*
- Data size -> *8 Bits*
- First bit -> *MSB First* 
- Prescaler -> *8* (to achieve a 20 Mbit/s baud rate)
- Configure SPI mode 0:
  - Clock Polarity (CPOL) -> *0* (idle low)
  - Clock Phase (CPHA) -> *0* (edge one of clock, rasing or falling depends on polarity)
- NSSP Mode -> *disabled* (no chip select pulses in between data frames, [ref](https://www.st.com/content/ccc/resource/training/technical/product_training/group0/3e/ee/cd/b7/84/4b/45/ee/STM32F7_Peripheral_SPI/files/STM32F7_Peripheral_SPI.pdf/_jcr_content/translations/en.STM32F7_Peripheral_SPI.pdf))
- *Leave others as default*

Using the *Pinout view*, attribute the MCU pins as follows:
- PE13 -> *SPI1_CLK*
- PE14 -> *SPI1_MISO* (master in, slave out)
- PE15 -> *SPI1_MOSI* (master out, slave in)

In the *GPIO setting* menu of the interface the pins should appear configured as follows:
- Pin Context Assignment -> *Cortex-M33 Secure*
- GPIO Mode -> *Alternate Function Push Pull*
- GPIO Pull-up/Pull-down -> *No pull-up and no pull-down*
- Maximum output speed -> *Low*

Using the *Pinout view* perform the following configuration (PE12 will act as a software driver chip select):
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
From *Pinout & Configuration* enter the *Security* section and configure both the RNG, SAES and AES modules as follows:
- Runtime context -> *Cortex-M33 secure*
- Activated -> *True*

Leave al the other parameters as default, as they will be configured with custom code.

### Code generation
Save the *.ioc* file to peform code generation.
This will add code to the project that performs the configuration specified via the graphical interface.


### Checkpoint utility





### Build and run


## Test









## Other stuff


- Configure UART
	- ***ADD LINK**

- Build project
	- right click on 
- Configure RUN/DEBUG
	- build project NON SECURE
		- it builds both
	- confb
	- 


## Caveats and problems
- test after *SystemClock_Config()* (otherwise slow startup clock is used)
- frequency not higher than 20 MHz
- all gpio output speed to low (even if the max frequency should be lower than 20 MHz for this configuration, according to datasheet)







