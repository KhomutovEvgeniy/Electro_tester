#pragma once
/*******************************************************************************************************
Описание: Основной файл работы с синхронным двигателем
(Вторая лабораторка по дисциплине Программирование микроконтроллеров в робототехнике)
Разработчик: Хомутов Евгений
Заметки:
*******************************************************************************************************/
/*******************************************************************************************************
ПАРАМЕТРЫ ПОДКЛЮЧЕНИЯ прибора
High Input of Driver -> TIM1 channel4 PA11 Pin32
Low  Input of Driver  -> TIM1 channel1 PA8 Pin29
ShutDown   of Driver  -> TIM1 channel3 PA10 Pin31

Датчик напряжения -> ADC1 input8 PB0 Pin18
******************************************************************************************************/

#include "project_config.h"
#include "motor_voltage.h"

#include "adc.h"


#define SYSTICK_PRESCALER 1000 
#define DELAY 1
#define MAX_TIM_COUNTER 1000

/***************************************************************************************************
Локальные типы данных
***************************************************************************************************/
 
/***************************************************************************************************
Прототипы локальных функций
***************************************************************************************************/

/***************************************************************************************************
Локальные переменные файла
***************************************************************************************************/
volatile uint32_t timeStampMs = 0;

/***************************************************************************************************
Прототипы локальных функций
***************************************************************************************************/

void systickStart( void );
void systickStop( void );
bool delayMs(uint32_t delay);

/***************************************************************************************************
Описание: Временная задержка по SysTick
Аргументы: delay - время задержки в мс
Возврат: true - прошло заданное время, false - нет
Замечания: 
***************************************************************************************************/
bool delayMs(uint32_t delay)
{                                                                                  
    
    if ( timeStampMs < delay )
    {
        return false;
    }
    else
    {
        timeStampMs = 0;
        return true;
    }
}

  /*************************************************************************************************
Описание: Остановка таймера SysTick
Аргументы: Нет
Возврат:   Нет
Замечания: 
***************************************************************************************************/
void systickStop( void )
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE;
}

/***************************************************************************************************
Описание: Запуск таймера SysTick
Аргументы: Нет
Возврат:   Нет
Замечания: 
***************************************************************************************************/
void systickStart( void )
{
    SysTick->CTRL |= SysTick_CTRL_ENABLE;
}


/***************************************************************************************************
Глобальные функции
***************************************************************************************************/
/***************************************************************************************************
Описание: Инициализация SysTick
Аргументы: Нет
Возврат:   Нет
Замечания: Смотреть описание SYSTICK_PRESCALER
***************************************************************************************************/
void systick_init ( void )
{     
    SysTick_Config( SystemCoreClock/SYSTICK_PRESCALER ); 
}

/***************************************************************************************************
Описание: Прерывания по Systick
Аргументы: Нет
Возврат:   Нет
Замечания: 
***************************************************************************************************/
void SysTick_Handler()
{
    timeStampMs++;
}

int main(void)
{   
    initPWM(); // 200 Hz timer init from cycle to cycle
    adc_init();
    adc_startConvertion();
    systick_init();
    
//    static uint16_t dutyCycle0 = 100;
//    static uint16_t dutyCycle1 = 100;
//    tim_setDutyCycle(0, dutyCycle0);  // Low Channel
//    while(delayMs(2) != true);
//    tim_setDutyCycle(1, dutyCycle1);  // High Channel

    static uint16_t timCounter1 = 0; // From 0 to 999 and again
    
//    uint16_t startLowIn = 0; // The number of timCounter when LowIn in is HIGH
    uint16_t dutyCycleLowIn = 300;
    uint16_t dutyCycleHighIn = 200;
    uint16_t lowInToHighIn = (MAX_TIM_COUNTER - dutyCycleLowIn - dutyCycleHighIn) / 2; 
    // It makes the same distance between borders signals
    
    uint16_t startHighIn = dutyCycleLowIn + lowInToHighIn; // The number of timCounter when HighIn in is HIGH
    uint16_t endHighIn = startHighIn + lowInToHighIn; // The number of timCounter when Highin in is LOW
    
    static uint16_t voltage = 0;
        
    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
    GPIO_ResetBits(GPIOA, GPIO_Pin_10);

    GPIO_ResetBits(GPIOA, GPIO_Pin_11);
    
    while ( 1 )
    {
        timCounter1 = TIM_GetCounter(TIM1);
        if(timCounter1 < dutyCycleLowIn) 
        {
            GPIO_SetBits(GPIOA, GPIO_Pin_8);
            GPIO_ResetBits(GPIOA, GPIO_Pin_11);
        }
        else if(timCounter1 > startHighIn && timCounter1 < endHighIn) 
        {
            GPIO_SetBits(GPIOA, GPIO_Pin_11);
            GPIO_ResetBits(GPIOA, GPIO_Pin_8);
        }
        else 
        {
            GPIO_ResetBits(GPIOA, GPIO_Pin_8);
            GPIO_ResetBits(GPIOA, GPIO_Pin_11);
        }
        voltage = adc_getValue(0);
    }
}

// классический ассерт для STM32
#ifdef USE_FULL_ASSERT
    void assert_failed(uint8_t * file, uint32_t line)
    { 
        /* User can add his own implementation to report the file name and line number,
         ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
         
        (void)file;
        (void)line;

        while(1){};
    }
#endif
