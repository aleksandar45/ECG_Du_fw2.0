This code is written with idea to accomplish requested demands of iCardiologist device specification. 
The code is intented to be used on RN4871_Nucleo_Test_Board with STM32Nucleo-L452RE board or on ECG_Du_v1 board with STM32L462CEU6 MCU.

To activate version of code that is adopted to RN4871_Nucleo_Test_Board uncomment preprocessor definition RN4871_Nucleo_Test in main.h file.
To activate version of code that is adopted to ECG_Du_v1 comment preprocessor definition RN4871_Nucleo_Test in main.h file. This version is not tested.

@User Description (RN4871_Nucleo_Test_Board)
When device is connected to power supply it initializes BLE module and starts advertising process. When Android App is connected 
to BLE module device is waiting START command that starts data transfer. Data is being generated inside processor (in timer interrupt routine) 
and sent to Andoid App until it receives Stop command. The process can be repeated multiple times until Android App disconnects device.
There are few time-out requirements that must be fulfilled in order to avoid device disconnection and STANDBY state. 
The Device waits for connection 35s (led diode is blinking with 1s period). When Android App connects to device device waits for connection parameters
update and enable Tx characteristic notification for 12s after it sends battery level information. The device then waits 10s to receive START command.
When device receives START command (led diode is blinking with 300ms period) the device waits 50s to transfer data and to receive STOP command. All further
action is the same because device again is waiting START command. 
If some of previously mentioned time-outs occurs the device enters low power advertising mode - processor is going in STANDBY mode and low power advertising is turned on.


@Notes


@New features


@Known Issues
	- Connection problem still exists when device is performing initialization of BLE module or device is in STANDBY mode. If Android App connects to device
	it will cause irregular state of device.
	- Connection parameters update problem exists when Andoid App performs not equal 3 connection parameters updates as it is assumed. 
	- BLE transmission power is low in some cases and transmission speed is decreased. In that case transmission is stopped.
	
@Resolved Issues
	- UART error problem is solved because parsing of messages is not yet in interrupt routine.


@Algorithm 
The device is operating according to this algorithm:
1. MCU Initializes BLE module RN4871, Initializes Low Energy Mode (fclk = 8MHz) and starts advertising process
2. MCU waits for BLE connection 
	- in the case of time-out, the device starts LP advertising of BLE module and goes in standby state 
3. After the device is connected to Android App, the device waits for START command from Android App
4. After START command is received, acquisition from ECG module is started. The device transmits data to Android App over BLE
	- High Power mode is enabled (fclk = 48MHz)
5. If BLE transmission is done correctly the device enters Low Energy Mode (fclk = 8MHz) and goes to step 3 where it is waiting again for START command.
- All the time device measures battery level

This code is version of project(v1.2).  