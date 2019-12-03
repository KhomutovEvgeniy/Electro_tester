#pragma once

#include <stdint.h>

void motor_revert_ABC_to_DQ( int32_t currentA, int32_t currentB, int32_t currentC, uint16_t tempPosition );
void motor_revert_DQ_to_ABC( int32_t curD, int32_t curQ, uint16_t tempPosition);
void motor_limit_ABC( int32_t currentA, int32_t currentB, int32_t currentC );

int32_t motor_current_getCurrent( uint8_t numberOfValue );
int32_t motor_current_getCurrentD( void );
int32_t motor_current_getCurrentQ( void );

int32_t motor_current_getCurrentA( void );
int32_t motor_current_getCurrentB( void );
int32_t motor_current_getCurrentC( void );

int32_t motor_current_getLimitCurrentA(void);
int32_t motor_current_getLimitCurrentB(void);
int32_t motor_current_getLimitCurrentC(void);


float fastInvSqrt(float x);
float fastSqrt(float x);
