#ifndef SCPI_H
#define SCPI_H


extern int scpi_parse_command (char *s, char **s2);
extern int scpi_parse_line (char *s);
extern int scpi_parse_unit (char *s, uint8_t unittype, char **s2);
extern long scpi_apply_unit (long val, int unit);
extern int scpi_function (uint8_t keynr, char *s);
int  scpi_get_long_unit (char *s, long *val);

int  scpi_get_boolean (char *s, uint8_t *val);
int  scpi_get_long (char *s, long *val);
int  scpi_get_int (char *s, int *val);



char *ltostr (long i, char *s);


#define FALSE 0
#define TRUE 1
#define OK 0
#define ERROR -1

extern char return_buf[80];

#endif /* SCPI_H */
