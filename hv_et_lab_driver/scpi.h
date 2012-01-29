#ifndef SCPI_H
#define SCPI_H

#define FALSE 0
#define TRUE 1
#define OK 0
#define ERROR -1

#define E_UNTERMINATED	1 
#define E_INTRANGE 		2
#define E_LONGRANGE 	3
#define E_CMD 			15

#define E_VOLTAGERANGE  16
#define E_CURRENTRANGE  17
#define E_OVPRANGE  	18
#define E_STANDBYRANGE  19


extern int scpi_parse_command (char *s, char **s2);
extern int scpi_parse_line (char *s);
extern int scpi_function (uint8_t keynr, char *s);


extern int scpi_check_buf_empty(char *s); 
extern int  scpi_get_boolean (char *s, uint8_t *val);


extern long  scpi_get_longd (char *s, int *err, char **s2);


extern uint8_t  scpi_get_digit (char *s, int *err, char **s2);
extern int  scpi_get_int (char *s, int *err, char **s2);
extern long  scpi_get_long (char *s, int *err, char **s2);
extern long  scpi_get_longfrac (char *s, int *err, char **s2);

extern char return_buf[80];

#endif /* SCPI_H */
