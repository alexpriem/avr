#ifndef SCPI_CMDS_H
#define SCPI_CMDS_H


#include <avr/pgmspace.h>


typedef struct {
    char prefix[4];
    uint8_t unittype;
    int8_t factor;
} str_units;
    
enum {
        UNIT_NONE ,
        UNIT_VOLT ,
        UNIT_AMP  ,
        UNIT_SEC  ,
        UNIT_END,
    };

extern uint8_t num_keywords;
extern PGM_P keywords[] PROGMEM;

extern str_units units[];

#endif  /* SCPI_CMDS_H */
