/*
	A library for building pseudo-NMEA packets
*/
#define __NMEA_H
#include "NMEA.h"
#undef __NMEA_H
#include <ctype.h>		// isprint
#include <string.h>
#include "util.h"

#define g_CRC_init	0xFF

static xdata char s_NMEA_buf[g_NMEA_buffer_length], *s_NMEA_ptr;
static xdata unsigned char s_NMEA_crc;

void NMEA_init(void)
{
	NMEA_received = 0;
	memset(&NMEA_receive, 0, sizeof(NMEA_receive));
	memset(s_NMEA_buf, 0, sizeof(s_NMEA_buf));
	s_NMEA_ptr = s_NMEA_buf;
	s_NMEA_crc = g_CRC_init;
}

static void NMEA_add_char(char c)
{
	*s_NMEA_ptr++ = c;					// add character to string
	s_NMEA_crc ^= c;					// CRC logic
}

void NMEA_start(char *command)
{
	s_NMEA_ptr = s_NMEA_buf;
	s_NMEA_crc = g_CRC_init;
	*s_NMEA_ptr++ = '$';				// dont include $ in the CRC
	while(*command)
		NMEA_add_char(*command++);
	*s_NMEA_ptr = 0;
}

void NMEA_add_argument(char *arg)
{
	NMEA_add_char(',');
	while(*arg)
		NMEA_add_char(*arg++);
	*s_NMEA_ptr = 0;
}

char *NMEA_finish(void)
{
char *crc = HEX_write_byte(s_NMEA_crc);
	*s_NMEA_ptr++ = '*';
	*s_NMEA_ptr++ = *crc++;
	*s_NMEA_ptr++ = *crc;
	*s_NMEA_ptr = 0;
	return s_NMEA_buf;
}

char NMEA_validate(char *str)
{
unsigned char CRC_rec, CRC_calc;
char *ptr = str;
	CRC_calc = g_CRC_init;
	if(*ptr++ != '$')
		return 0;
	while(*ptr != '*')
	{
		if(!isprint(*ptr))
			return 0;
		else
			CRC_calc ^= *ptr++;
	}
	CRC_rec = HEX_read(ptr+1);
	return CRC_rec == CRC_calc;
}

static xdata char s_arg_buf[g_NMEA_buffer_length];
unsigned char NMEA_get_argument(char *const str, unsigned char index, char *const buf)
{
char *x;
	if(!NMEA_validate(str))
		return 0;
	strcpy(s_arg_buf, str + 1);
	x = strtok(s_arg_buf, "*,");
	while(index--)
		x = strtok(NULL, "*,");
	strncpy(buf, x, 16);
}

unsigned char NMEA_arguments(NMEA_Packet_t *packet)
{
unsigned char i, args = 0;
	for(i = 0; i < packet->Length; i++)
	{
		if(packet->String[i] == ',')
			args++;
	}
	return args;
}

void NMEA_receive_character(char ch)
{
static unsigned char state;
NMEA_Packet_t *buf = &NMEA_receive;
	if (ch == '$')
	{	// the start of a packet
		buf->Length = 0;
		buf->Arguments = 0;
		state = 0;
	}
	else if (ch == '*')
	{	// two more characters for the CRC
		state = 3;
	}
	else if (!isprint(ch))
	{	// invalid character, reset
		state = 0;
		buf->Length = 0;
	}
	buf->String[buf->Length++] = ch;
	state --;
	if (state == 0)
	{	// end of packet
		buf->String[buf->Length] = 0;
		NMEA_received = NMEA_validate(buf->String);
		buf->Arguments = NMEA_arguments(buf);
	}
}

//	internal testing procedures
void NMEA_test(void)
{
static char *_str, _buf[16], _tmp, _len;
	NMEA_start("ABCD");
	NMEA_add_argument("cde");
	NMEA_add_argument("efg");
	_str = NMEA_finish();
	_tmp = NMEA_validate(_str);
	_len = NMEA_get_argument(_str, 0, _buf);
	_len = NMEA_get_argument(_str, 1, _buf);
	_len = NMEA_get_argument(_str, 2, _buf);
	_len = NMEA_get_argument(_str, 3, _buf);
	_len = NMEA_get_argument(_str, 4, _buf);
}
