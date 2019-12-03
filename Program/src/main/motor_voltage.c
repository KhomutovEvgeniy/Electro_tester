/***************************************************************************************************
Описание: Инициализация ног для управления Н-драйвером
Разработчик: Хомутов Евгений
Заметки: High Input to PA11_32pin, Low Input to PA8_29pin, ShutDown to PA10_31pin
****************************************************************************************************/
#include "project_config.h"
#include "motor_voltage.h"

/***************************************************************************************************
Локальные дефайны
***************************************************************************************************/
// Частота генерации ШИМа = SystemCoreClock / (PRESCALER * PERIOD) = 72МГц /(72 * 5000) = 200 Гц
#define PERIOD 1000
#define PRESCALER (SystemCoreClock / 200000)   // = 36
#define MAX_PWM 1000

/***************************************************************************************************
Описание: Инициализация таймера под ШИМ
Аргументы: Нет
Возврат:   Нет
Замечания: Нет
***************************************************************************************************/
void initPWM( void ) 
{   
    GPIO_InitTypeDef port;
    TIM_TimeBaseInitTypeDef timer;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // Тактирование порта A
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);   // Тактирование TIM1
     
    //Настраиваем PA8 PA11 на генерацию ШИМ
    GPIO_StructInit(&port);
    port.GPIO_Mode = GPIO_Mode_Out_PP;
    port.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_11;
    port.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &port);        
    
    TIM_TimeBaseStructInit(&timer);  
    
    //Настройка TIM1 на частоту 200Гц и направлением счета на возрастание
    timer.TIM_Prescaler = PRESCALER - 1;
    timer.TIM_Period = PERIOD - 1;
    timer.TIM_ClockDivision = TIM_CKD_DIV1;
    timer.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &timer);  
    
    //Структура для канала TIM1
    TIM_OCInitTypeDef timerPWM;       
    
    TIM_OCStructInit(&timerPWM);
    
    timerPWM.TIM_OCMode = TIM_OCMode_PWM1;
    timerPWM.TIM_OutputState = TIM_OutputState_Disable;
    timerPWM.TIM_Pulse = 0;
    TIM_OC1Init(TIM1, &timerPWM);    // Настройка первого канала таймера
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

    TIM_OC4Init(TIM1, &timerPWM);    // Настройка четвертого канала таймера
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM1, ENABLE);
    
//    TIM_BDTRInitTypeDef bdtr;
//    TIM_BDTRStructInit(&bdtr);
//    bdtr.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
//    bdtr.TIM_DeadTime = 60000;
//    TIM_BDTRConfig(TIM1, &bdtr);
    
    TIM_CtrlPWMOutputs(TIM1, DISABLE);
    TIM_Cmd(TIM1, ENABLE);           //Запуск TIM1
}

/***************************************************************************************************
Описание: Преобразование значения из одного диапазона в другой  
Аргументы:
x - значение, которое преобразуем
in_min/in_max  -диапазон, из которого преобразуем
out_min/out_max  -диапазон, в который преобразуем
Возврат:   Нет
Замечания: для генерации ШИМа на двигателе в диапазоне от -1000 до 1000 
***************************************************************************************************/
int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/***************************************************************************************************
Описание: Выдача напряжений обоих полярностей на обмотки двигателя  
Аргументы: 
dutyCycle - скважность ШИМа, 0-65535; 
channel - номер канала, 1 - High Input of Driver, 0 - Low Input of Driver
Возврат:   Нет
Замечания: для генерации ШИМа на двигателе в диапазоне от 0 до 1000 
***************************************************************************************************/
void tim_setDutyCycle(uint16_t channel, uint16_t dutyCycle)
{    
    if (channel ==  1)
    {
        TIM_SetCompare4 ( TIM1, dutyCycle );
    }
    else 
    {
        TIM_SetCompare1 ( TIM1, dutyCycle);
    }
}
