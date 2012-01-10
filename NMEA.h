/*
	A library for building pseudo-NMEA packets
*/
// Magic extern definition
#ifdef __NMEA_H
	#define EXTERN /**/
#else
#ifdef __cplusplus
	#define EXTERN extern "C"
#else
	#define EXTERN extern
#endif	//	C++
#endif	//	__NMEA_H

/* 	--- constants and types --- */
#define g_NMEA_buffer_length	256

typedef struct NMEA_MESSAGE_T {
	unsigned char String[g_NMEA_buffer_length];
	unsigned char Length;		// nunber of characters in String
	unsigned char Arguments;	// number of arguments in String
} NMEA_Packet_t;

/* 	--- functions and state --- */
// initialise all state
EXTERN void NMEA_init(void);

// initialise the message with $ and the command string
EXTERN void NMEA_start(char *command);

// insert a comma seperated argument
EXTERN void NMEA_add_argument(char *arg);

// conclude the packet with an asterisk and the checksum followed by an \r. return the string
EXTERN char *NMEA_finish(void);

// calculates the checksum of the passed NMEA packet and compares with the packet's own
EXTERN char NMEA_validate(char *str);

// copy the index'th argument from str into buf, return the length of the argument
EXTERN unsigned char NMEA_get_argument(char *str, char index, char *buf);

// retreive the command string from str into buf
#define NMEA_get_command(str, buf)		NMEA_get_argument(str, 0, buf)

// a complete and valid message has been received by NMEA_receive_character(ch)
EXTERN char NMEA_received;
// the latest message received by NMEA_receive_character
EXTERN NMEA_Packet_t NMEA_receive;
// an NMEA packet reading state machine
EXTERN void NMEA_receive_character(char ch);

#undef EXTERN	// end of extern magic
