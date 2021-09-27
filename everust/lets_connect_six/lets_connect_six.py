import socket

_lcs_board = [[0 for i in range(19)] for j in range(19)]
_socket_to_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
_socket_to_server.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
_home = -1
_away = -1

def connect(ip:str, port:int, color:int):
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
	print("Received red stones from server: " + data) 
	looper = data.count(':') + 1
	for i in range(looper):
		chr_pos = 4 * i
		num_pos = 4 * i + 1
		x = ord(data[chr_pos]) - 65
		if x > 8:
			x = x - 1
		y = 19 - int(data[num_pos:num_pos+2])
		_lcs_board[y][x] = 3
	print(_lcs_board)
	return data


def draw_and_wait(user_move:str):
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
			print(msg)
		else:
			for coors in stones:
				print(coors)
				parsed_num = _a_coor_to_num(coors)
				if parsed_num == "BADINPUT":
					msg = parsed_num + "$" + user_move
					print("here" + msg)
				else:
					(x,y) = parsed_num
					if x > 18 or x < 0 or y > 18 or y < 0:
						msg = "BADCOORD$" + user_move
					elif _lcs_board[y][x] != 0:
						msg = "NOTEMPYT$" + user_move
					else:
						print(str(x) + " " + str(y))
						_lcs_board[y][x] = _home
						_print_board()

	if len(msg):
		_socket_to_server.sendall((len(msg)).to_bytes(4, byteorder='little') + str.encode(msg))
		print(msg + " is sent, ", end='')
	print("Waiting for server...")
	size = int.from_bytes(_socket_to_server.recv(4), "little")
	away_move = _socket_to_server.recv(size).decode("utf-8")
	print("This is away from server: " + away_move)
	if away_move == "K10":
		_lcs_board[9][9] = _away
	else: 
		[x1, y1, x2, y2] = _coor_to_num(away_move)
		_lcs_board[y1][x1] = _away
		_lcs_board[y2][x2] = _away
	print(_lcs_board)
	return away_move


def get_lcs_board(ask:str):
	pass


def _a_coor_to_num(coor):
	if not coor[0].isalpha() or not coor[1:].isnumeric() or coor[0] == 'I':
		return "BADINPUT"
	x = ord(coor[0]) - 65
	y = 19 - int(coor[1:])
	if x > 8:
		x = x - 1
	return (x, y)


def _coor_to_num(coor):
	x1 = ord(coor[0]) - 65
	y1 = 19 - int(coor[1:3])
	x2 = ord(coor[4]) - 65
	y2 = 19 - int (coor[5:7])
	if x1 > 8:
		x1 = x1 - 1
	if x2 > 8:
		x2 = x2 - 1
	if x2 < 0 or x2 > 18 or y2 < 0 or y2 > 18 or x1 < 0 or x1 > 18 or y1 < 0 or y1 > 18:
		print("ERROR: input out of bound " + coor)
		exit(1)
	return [x1, y1, x2, y2]

def _print_board():
	for row in _lcs_board:
		print(row)

def main():
	_print_board()
	x = input("input: ")
	draw_and_wait(x)


if __name__ == "__main__":
	main()


