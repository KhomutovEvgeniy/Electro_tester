/*******************************************************************************************************
Описание: Файл для работы с ADC12_IN8
Разработчик: Хомутов Евгений
Заметки: Инициализация и настройка 8 канала ADC1
*******************************************************************************************************/
#include "adc.h"
#include "project_config.h"
#include <string.h>

/***************************************************************************************************
Локальные дефайны
***************************************************************************************************/

#define ADC_IRQ_HANDLER ADC1_2_IRQHandler

/***************************************************************************************************
Локальные типы данных
***************************************************************************************************/

typedef void ( *RccClockCmd )( uint32_t, FunctionalState );

/***************************************************************************************************
Локальные переменные файла
***************************************************************************************************/
const uint32_t adc_pin_rcc = RCC_APB2Periph_GPIOB;
GPIO_TypeDef * const adc_port = GPIOB;
const uint16_t adc_pin_0 = GPIO_Pin_0; // Первый пин, с которого считывает АЦП

const RccClockCmd adc_rcc_cmd = RCC_APB2PeriphClockCmd;
const uint32_t adc_rcc = RCC_APB2Periph_ADC1;
ADC_TypeDef * const adc = ADC1;

const uint8_t adc_channels = 1;
const uint8_t adc_channel_0 = ADC_Channel_0;

const uint8_t adc_sample_time = ADC_SampleTime_1Cycles5;

const IRQn_Type adc_irq_type = ADC1_2_IRQn;
const uint8_t adc_irq_prio = 2;

volatile uint16_t ADC_buffer[adc_channels];   // Массив (буфер) показаний, считывающихся с ADC

/***************************************************************************************************
Глобальные функции
***************************************************************************************************/

/**************************************************************************************************
Описание: Инициализация ADC 
Аргументы: Нет
Возврат:   Нет
Замечания: Для чтения значений с потенциометра
**************************************************************************************************/
void adc_init( void )
{
    dma_adc_init();
    
    // тактирование пинов АЦП
    adc_rcc_cmd( adc_pin_rcc | adc_rcc, ENABLE );          // Разрешение тактирования порта А (датчики тока)

    // инициализация пинов АЦП (0 и 1 каналы)
    GPIO_InitTypeDef gpioInitStructure;
    gpioInitStructure.GPIO_Pin = adc_pin_0;
    gpioInitStructure.GPIO_Mode = GPIO_Mode_AIN;
    gpioInitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init( adc_port, &gpioInitStructure );
        
    // Настройка АЦП
    ADC_InitTypeDef adcInitStruct;                                  // Структура для инициализации модуля ADC
    adcInitStruct.ADC_Mode = ADC_Mode_Independent;                  // Независимый режим (т.к. используем 1 АЦП)
    adcInitStruct.ADC_ScanConvMode = ENABLE;                        // Сканирующий режим (т.к. сигнал будем снимать не с одной ножки)
    adcInitStruct.ADC_ContinuousConvMode = ENABLE;                  // Циклическое сканирование
    adcInitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // Источник запуска АЦП - нет  
    adcInitStruct.ADC_DataAlign = ADC_DataAlign_Right;              // Выравнивание битов по правому краю
    adcInitStruct.ADC_NbrOfChannel = adc_channels;                  // Количество каналов сканирования
    ADC_Init (adc, &adcInitStruct); // Инициализация модуля ADC1
    
    // Определение регулярной группы сканирования АЦП 
    ADC_RegularChannelConfig( adc, adc_channel_0, 1, adc_sample_time );
    
    ADC_DMACmd(ADC1, ENABLE);       // Активация передачи в память запросу DMA с ADC1
    ADC_Cmd(adc, ENABLE);         // Запуск АЦП
}

/**************************************************************************************************
Описание: Инициализация модуля DMA на работу с ADC1
Аргументы: Нет
Возврат:   Нет
Замечания: 
**************************************************************************************************/
void dma_adc_init ()
{
    // Разрешение тактирвоания модуля DMA
    RCC_AHBPeriphClockCmd (RCC_AHBPeriph_DMA1, ENABLE); 
    
    // Настройка модуля DMA1 - режим работы: показание с ADC1 в буфер
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_BufferSize = adc_channels;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_buffer;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(ADC1->DR);
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);    //1 канал DMA1 соответствует взяимодействию с ADC1
    DMA_Cmd(DMA1_Channel1, ENABLE);                 //Включение прямого доступа к памяти
/*
    // Включение прерывания DMA по завершении цикла обработки
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
    NVIC_EnableIRQ(DMA1_Channel1_IRQn); 
*/    
}

/**************************************************************************************************
Описание: Запуск преобразования
Аргументы: Нет
Возврат:   Нет
Замечания: 
**************************************************************************************************/
void adc_startConvertion( void )
{    
    ADC_SoftwareStartConvCmd( adc, ENABLE );
}

/**************************************************************************************************
Описание: Чтение результата преобразования
Аргументы: numberOfValue - если 0 - то вернуть значение с нулевого канала, 1 - с первого
Возврат:   Данные с АЦП
Замечания: 
**************************************************************************************************/
uint16_t adc_getValue( uint8_t channel )
{
    return ADC_buffer[channel];
}
