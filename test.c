// tests
#include "NMEA.h"
#include "stdio.h"

void print(char *arg, int len) { printf(">%s | %i\n\r", arg, len); }

#ifdef __C51__
void main(void)
#else
int main(int argc, char *argv[])
#endif
{
NMEA_msg_t M, *m = &M;
char msg[256], arg[256], i, len;
	NMEA_start(m, "ABC");
	NMEA_add(m, "abc");
	NMEA_add(m, "def");
	len = NMEA_finish(m, msg);
	print(msg, len);
	if(len = NMEA_get(m, 0, arg))
		print(arg, len);
	if(len = NMEA_get(m, 1, arg))
		print(arg, len);
	if(len = NMEA_get(m, 2, arg))
		print(arg, len);
	if(len = NMEA_get(m, 3, arg))
		print(arg, len);
	printf("\r\n");
	if(len = NMEA_command(m, arg))
		print(arg, NMEA_argc(m));
	if(len = NMEA_get(m, 0, arg))
		print(arg, len);
	if(len = NMEA_get(m, 1, arg))
		print(arg, len);
	if(len = NMEA_get(m, 2, arg))
		print(arg, len);
	if(len = NMEA_get(m, -18, arg))
		print(arg, len);
	printf("\r\n");
	NMEA_parse_byte(m, '$');
	NMEA_parse_byte(m, 'A');
	NMEA_parse_byte(m, 'B');
	NMEA_parse_byte(m, 'C');
	NMEA_parse_byte(m, ',');
	NMEA_parse_byte(m, 'a');
	NMEA_parse_byte(m, 'b');
	NMEA_parse_byte(m, 'c');
	NMEA_parse_byte(m, ',');
	NMEA_parse_byte(m, 'd');
	NMEA_parse_byte(m, 'e');
	NMEA_parse_byte(m, 'f');
	NMEA_parse_byte(m, '*');
	NMEA_parse_byte(m, 'B');
	NMEA_parse_byte(m, '8');
//	NMEA_parse_byte(m, 0);
	if(len = NMEA_get(m, 2, arg))
		print(arg, len);
	if(len = NMEA_parse_string(m, msg))
		print(msg, len);
	printf("\r\n");
}

