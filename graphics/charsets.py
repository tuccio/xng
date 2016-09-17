# Script used to generate charset strings

import string

def ascii_charset():
	return "".join([chr(i) for i in range(0, 256) if chr(i) in string.printable])
	
if __name__ == '__main__':
	print(ascii_charset())