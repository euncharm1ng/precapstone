import numpy
import socket

_lcs_board = numpy.zeros((19,19))
_socket_to_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
_socket_to_server.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
_home = -1
_away = -1

def connect(ip, port, color):
	global _socket_to_server 	
	global _home = color
	if color == 1: 
		global _away = 2
	else: 
		global _away = 1
	print("trying to connect to server with ip " + ip)
	_socket_to_server.connect((ip, port))

	print("connected to server, waiting for redstones...")
	size = int.from_bytes(_socket_to_server.recv(4), "little")
	data = _socket_to_server.recv(size).decode("utf-8").replace(" ", "")

	print("this is red stones from server: " + data) 
	looper = data.count(':') + 1
	for i in range(looper):
		chr_pos = 4 * i
		num_pos = 4 * i + 1
		x = ord(data[chr_pos]) - 65
		if x > 8:
			x = x - 1
		y = int(data[num_pos:num_pos+2]) - 1 
		print("red stone: x: " + str(x) + ", y: " + str(y))
		_lcs_board[19 - y][19 - x] = 3
	return data


def draw_and_wait(home):
	print("draw and wait received: " + str(home))
	global _socket_to_server
	global color
	home = home.replace(" ", "").upper()
	if home == "K10" or home == "":
		pass
	elif home[0] == 'I' or home[4] == 'I':
		print("ERROR: \'I\' included in input " + home)
		exit(1)
	else: 
		[x1, y1, x2, y2] = _coor_to_num(home)
		if _lcs_board[y1][x1] != 0 or _lcs_board[y2][x2] != 0:
			print(str(home) + " is not empty!")
			exit(1)
		else:
			_lcs_board[y1][x1] = color
			_lce_board[y2][x2] = color
		
	if len(home):
		_socket_to_server.sendall((len(home)).to_bytes(4, byteorder='little') + str.encode(home))
	print(home + " is sent, waiting for server")
	size = int.from_bytes(_socket_to_server.recv(4), "little")
	away = _socket_to_server.recv(size).decode("utf-8").replace(" ", "")
	print("this is away from server: " + str(away))
	[x1, y1, x2, y2] = _coor_to_num(away)
	_lcs_board[y1][x1] = _away
	_lcs_board[y2][x2] = _away
	return away


def get_lcs_board(ask):
	pass


def _coor_to_num(coor):
	x1 = ord(coor[0]) - 65
	y1 = int(coor[1:3]) - 1
	x2 = ord(coor[4]) - 65
	y2 = int (coor[5:7]) - 1

	if x1 > 8:
		x1 = x1 - 1
	if x2 > 8:
		x2 = x2 - 1

	if x2 < 0 or x2 > 18 or y2 < 0 or y2 > 18 or x1 < 0 or x1 > 18 or y1 < 0 or y1 > 18:
		print("ERROR: input out of bound in " + home)
		exit(1)

	return [19 - x1, 19 - y1, 19 - x2, 19 - y2]


def _num_to_coor(x, y):
	if x > 8: 
		x = x + 1
	x = chr(x + 64)
	y = str(y)
	if len(y) == 1:
		y = "0" + y
	return "" + x + y 
	

def main():
	red_stones = connect('192.168.137.51', 8080, 1)
	oppo_move = draw_and_wait("A09:S16")
	print(oppo_move)


if __name__ == "__main__":
	main()


