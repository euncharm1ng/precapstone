import socket

_lcs_board = [[0 for i in range(19)] for j in range(19)]
_socket_to_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
_socket_to_server.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
_home = -1
_away = -1

def connect(ip:str, port:int, color:int) -> str:
	global _home, _away, _lcs_board
	_home = color
	if _home == 1: 
		_away = 2
	elif _home == 2: 
		_away = 1
	else:
		print("ERROR: input 1 or 2 for color!")
		exit(1)
	print("Trying to connect to server with ip " + ip + " and port being " + str(port))
	_socket_to_server.connect((ip, port))
	print("Connected to server! Now waiting for redstones...")
	size = int.from_bytes(_socket_to_server.recv(4), "little")
	data = _socket_to_server.recv(size).decode("utf-8")
	print("Received red stones from server: " + str(size) + " " + data) 
	looper = data.count(':') + 1
	for i in range(looper):
		chr_pos = 4 * i
		num_pos = 4 * i + 1
		x = ord(data[chr_pos]) - 65
		if x > 8:
			x = x - 1
		y = 19 - int(data[num_pos:num_pos+2])
		_lcs_board[y][x] = 3
	_print_board()
	return data


def draw_and_wait(user_move:str) -> str:
	global _lcs_board
	print("Function draw_and_wait() received: " + user_move)
	msg = user_move.replace(" ", "").upper()
	if msg == "K10":
		_lcs_board[9][9] = _home
	elif msg == "":
		pass
	else:
		stones = msg.split(':')
		if len(stones) != 2:
			msg = "BADINPUT$" + user_move
		else:
			for coors in stones:
				print(coors)
				parsed_num = _a_coor_to_num(coors)
				if parsed_num == "BADINPUT":
					msg = parsed_num + "$" + user_move
				else:
					(x,y) = parsed_num
					if x > 18 or x < 0 or y > 18 or y < 0:
						msg = "BADCOORD$" + user_move
					elif _lcs_board[y][x] != 0:
						msg = "NOTEMPYT$" + user_move
					else:
						_lcs_board[y][x] = _home
						_print_board()

	if len(msg):
		_socket_to_server.sendall((len(msg)).to_bytes(4, byteorder='little') + str.encode(msg))
		print(msg + " is sent. ", end='')

	print("Now waiting for server...")
	size = int.from_bytes(_socket_to_server.recv(4), "little")
	away_move = _socket_to_server.recv(size).decode("utf-8")
	print("This is away from server: " + away_move)
	away_move = away_move.replace(" ", "").upper()
	if away_move == "K10":
		_lcs_board[9][9] = _away
	else: 
		away_coor = away_move.split(':')
		for coors in away_coor:
			(x, y) = _a_coor_to_num(coors)
			_lcs_board[y][x] = _away
	_print_board()
	return away_move


def get_lcs_board(ask:str) -> chr:
	result = _a_coor_to_num(ask)
	if result == "BADINPUT": 
		print("WRONG INPUT: " + ask)
	(x,y) = result
	if x > 18 or x < 0 or y > 18 or y < 0:
		print("OUT OF BOUND: " + ask)
	if _lcs_board[y][x] == 0:
		return 'E'
	elif _lcs_board[y][x] == 1:
		return 'B'
	elif _lcs_board[y][x] == 2: 
		return 'W'
	elif _lcs_board[y][x] == 3:
		return 'R'
	print("ERROR in api, plz contact maintainance team")
	exit(1)


def _a_coor_to_num(coor): 
	if not coor[0].isalpha() or not coor[1:].isnumeric() or coor[0] == 'I':
		return "BADINPUT" 
	x = ord(coor[0]) - 65 
	y = 19 - int(coor[1:])
	if x > 8:
		x = x - 1
	return (x, y)


def _print_board():
	for row in _lcs_board:
		print(row)


def main():
	_print_board()
	x = input("input: ")
	draw_and_wait(x)


if __name__ == "__main__":
	main()
