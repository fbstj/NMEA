DOLLAR = '$'
STAR = '*'
DELIMIT = ','

def checksum(string):
	"""Cacluate the NMEA checksum of a string"""
	c = 0xFF
	for b in string:
		c ^= ord(b)
	return c

def make(*args):
	"""Make an NMEA message from an array of strings"""
	x = DELIMIT.join(args)
	return DOLLAR + x + STAR + "{0:X}".format(checksum(x))

def parse(string):
	"""Parse a string for an NMEA message"""
	if string.count(DOLLAR) == 0:
		return None
	start = string.index(DOLLAR)
	msg = string[start:]
	if msg.count(STAR) == 0:
		return None
	end = msg.index(STAR)
	cs = start + end + 1
	cs = string[cs:cs + 2]
	msg = msg[1:end]
	if int(cs,16) != checksum(msg):
		return None
	return msg.split(',')

def parse_all(string):
	"""Parse a string for multiple messages"""
	o = parse(string)
	n = 0
	while n < len(string):
		yield o
		m = make(*o)
		n = string.find(m) + len(m) + 1
		o = parse(string[n:])

if __name__ == "__main__":
	import sys
	s = make(*sys.argv[1:])
	print(s)

