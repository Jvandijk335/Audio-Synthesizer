/******************************************************************************
* Copyright (C) 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <stdlib.h>

#include "platform.h"
#include "xil_printf.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include "xscutimer.h"
#include "xscugic.h"
#include "xparameters.h"
#include "xiicps.h"
#include "sleep.h"
#include "xgpio.h"

#include "audio.h"
#include "frequency_generator.h"
#include "audio_effects.h"

// Timer/Interrupt defines
#define TIMER_DEVICE_ID		XPAR_XSCUTIMER_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define TIMER_IRPT_INTR		XPAR_SCUTIMER_INTR
#define TIMER_LOAD_VALUE	0xFFFF

#define SAMPLE_RATE 		96000 //Hz

WaveNode *wave_list = NULL;
uint32_t input;

// Functies
static int Timer_Intr_Setup(XScuGic * IntcInstancePtr, XScuTimer *TimerInstancePtr, u16 TimerIntrId);
static int Timer_Config(XScuTimer *Scu_Timer, XScuGic *IntcInstance);

static void Timer_ISR(void * CallBackRef)
{
	XScuTimer *TimerInstancePtr = (XScuTimer *) CallBackRef;
	XScuTimer_ClearInterruptStatus(TimerInstancePtr);

	//-------------------------------------------------------//


	q31_t wave = mix_waves( wave_list, NULL );
	//wave = process_effect(wave);

	uint32_t wave24 = (uint32_t)(wave >> 8);

	Xil_Out32(I2S_DATA_TX_L_REG, wave24);
	Xil_Out32(I2S_DATA_TX_R_REG, wave24);
}

int main()
{
	int ret;
	uint8_t toggles = 0xFF;
	XGpio gpio_instance;
	int id_1 = -1;
	int id_2 = -1;
	int id_4 = -1;
	int id_8 = -1;

	XScuTimer Scu_Timer;
	XScuGic IntcInstance;

    init_platform();

    //configure buttons
    XGpio_Initialize( &gpio_instance, XPAR_AXI_GPIO_1_BASEADDR );
    XGpio_SetDataDirection( &gpio_instance, 1, 0x0F );

	//Configure the IIC data structure
	IicConfig(XPAR_XIICPS_0_DEVICE_ID);

	//Configure the Audio Codec's PLL
	AudioPllConfig();

	//Configure the Line in and Line out ports.
	//Call LineInLineOutConfig() for a configuration that
	//enables the HP jack too.
	AudioConfigureJacks();
	LineinLineoutConfig();

	print("SYNTHESIZER 92kHz\n\r");
	print("=================================================\n\r");

//	wave_list = add_wave(wave_list, 5, 3.5f, SAMPLE_RATE, WAVE_SINE);
//	wave_list = add_wave(wave_list, 440, 6.0f, SAMPLE_RATE, WAVE_SINE);
//	wave_list = add_wave(wave_list, 880, 3.0f, SAMPLE_RATE, WAVE_SINE);
//	wave_list = add_wave(wave_list, 1320, 2.0f, SAMPLE_RATE, WAVE_SINE);
//	wave_list = add_wave(wave_list, 1760, 1.5f, SAMPLE_RATE, WAVE_SINE);
//	wave_list = add_wave(wave_list, 440,3.5f, SAMPLE_RATE, WAVE_SQUARE);

//	audio_effects_init(SAMPLE_RATE);
//	enable_delay(500.0);
//	enable_echo(300.0f, 0.6f);
//	enable_predefined_fir_filter(PREDEFINED_FILTER_CUSTOM);

	ret = Timer_Config(&Scu_Timer, &IntcInstance);
	if(ret == -1){
		perror("Timer Config");
		return -1;
	}

	print("Starting Synthesizer...\r\n");


	for (;;) {
		uint32_t read_value = XGpio_DiscreteRead( &gpio_instance, 1 );

		if ( ( read_value & 0x1 ) == 0x1 )
		{
			usleep_A9( 5000 );
			while ( ( read_value & 0x1 ) == 0x1 )
			{
				read_value = XGpio_DiscreteRead( &gpio_instance, 1 );
				usleep_A9( 500 );
			}

			if ( ( toggles & 0x1 ) == 0x1 )
			{
				wave_list = add_wave(wave_list, 440, 6.0f, SAMPLE_RATE, WAVE_SINE);
				id_1 = wave_list->wave->id;
			}
			else
			{
				wave_list = remove_wave( wave_list, id_1 );
			}

			toggles ^= 0x1;
		}
		else if ( ( read_value & 0x2 ) == 0x2 )
		{
			usleep_A9( 5000 );
			while ( ( read_value & 0x2 ) == 0x2 )
			{
				read_value = XGpio_DiscreteRead( &gpio_instance, 1 );
				usleep_A9( 500 );
			}

			if ( ( toggles & 0x2 ) == 0x2 )
			{
				wave_list = add_wave(wave_list, 880, 3.0f, SAMPLE_RATE, WAVE_SINE);
				id_2 = wave_list->wave->id;
			}
			else
			{
				wave_list = remove_wave( wave_list, id_2 );
			}

			toggles ^= 0x2;
		}
		else if ( ( read_value & 0x4 ) == 0x4 )
		{
			usleep_A9( 5000 );
			while ( ( read_value & 0x4 ) == 0x4 )
			{
				read_value = XGpio_DiscreteRead( &gpio_instance, 1 );
				usleep_A9( 500 );
			}

			if ( ( toggles & 0x4 ) == 0x4 )
			{
				wave_list = add_wave(wave_list, 1320, 2.0f, SAMPLE_RATE, WAVE_SINE);
				id_4 = wave_list->wave->id;
			}
			else
			{
				wave_list = remove_wave( wave_list, id_4 );
			}

			toggles ^= 0x4;
		}
		else if ( ( read_value & 0x8 ) == 0x8 )
		{
			usleep_A9( 5000 );
			while ( ( read_value & 0x8 ) == 0x8 )
			{
				read_value = XGpio_DiscreteRead( &gpio_instance, 1 );
				usleep_A9( 500 );
			}

			if ( ( toggles & 0x8 ) == 0x8 )
			{

				wave_list = add_wave(wave_list, 1760, 1.5f, SAMPLE_RATE, WAVE_SINE);
				id_8 = wave_list->wave->id;
			}
			else
			{
				wave_list = remove_wave( wave_list, id_8 );
			}

			toggles ^= 0x8;
		}
		else
		{
			usleep_A9( 5000 );
		}
	}

	free_waves(wave_list);
	cleanup_platform();
	return 0;
}

static int Timer_Intr_Setup(XScuGic * IntcInstancePtr, XScuTimer *TimerInstancePtr, u16 TimerIntrId)
{
	int Status;
	XScuGic_Config *IntcConfig;
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig, IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) return Status;

	// Step 1: Interrupt Setup
	Xil_ExceptionInit();

	// Step 2: Interrupt Setup
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, IntcInstancePtr);
	// Step 3: Interrupt Setup
	Status = XScuGic_Connect(IntcInstancePtr, TimerIntrId, (Xil_ExceptionHandler)Timer_ISR, (void *)TimerInstancePtr);
	if (Status != XST_SUCCESS) return Status;

	// Step 4: Interrupt Setup
	XScuGic_Enable(IntcInstancePtr, TimerIntrId);
	// Step 5: Interrupt Setup
	XScuTimer_EnableInterrupt(TimerInstancePtr);
	// Step 6: Interrupt Setup
	Xil_ExceptionEnable();
	return XST_SUCCESS;
}

static int Timer_Config(XScuTimer *Scu_Timer, XScuGic *IntcInstance)
{
	int Status;

	XScuTimer_Config *Scu_ConfigPtr;

	Scu_ConfigPtr = XScuTimer_LookupConfig(XPAR_PS7_SCUTIMER_0_DEVICE_ID);
	Status = XScuTimer_CfgInitialize(Scu_Timer, Scu_ConfigPtr, Scu_ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS) {
		print("Timer Initialization Failed\r\n");
		cleanup_platform();
		return -1;
	}

	Status = Timer_Intr_Setup(IntcInstance, Scu_Timer, XPS_SCU_TMR_INT_ID);
	if (Status != XST_SUCCESS) {
		print("Interrupt Setup Failed\r\n");
		cleanup_platform();
		return -1;
	}

	// Configure timer to trigger at the sampling rate (8kHz)
	XScuTimer_LoadTimer(Scu_Timer, (XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2) / (unsigned int)SAMPLE_RATE);
	XScuTimer_EnableAutoReload(Scu_Timer);
	XScuTimer_Start(Scu_Timer);

	return 0;
}
