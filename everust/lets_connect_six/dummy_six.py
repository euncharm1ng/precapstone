from lets_connect_six import *
import numpy
import random


dummy_board = numpy.zeros((19,19))

def create_payload(x1, y1, x2, y2):
	if x1 > 8: 
		x1 = x1 + 1
	if x2 > 8: 
		x2 = x2 + 1
	x1 = chr(x1 + 64)
	x2 = chr(x2 + 64)
	y1 = str(y1)
	y2 = str(y2)
	if len(y1) == 1:
		y1 = "0" + y1
	if len(y2) == 1:
		y2 = "0" + y2
	return "" + x1 + y1 + ":" + x2 + y2
	

def make_move():
	x1 = random.randint(0, 18)
	y1 = random.randint(0, 18)
	while dummy_board[x1][y1] != 0:
		x1 = random.randint(0, 18)
		y1 = random.randint(0, 18)
	dummy_board[x1][y1] = 9
		
	x2 = random.randint(0, 18)
	y2 = random.randint(0, 18)
	while dummy_board[x2][y2] != 0:
		x2 = random.randint(0, 18)
		y2 = random.randint(0, 18)
	dummy_board[x2][y2] = 9
	
	return create_payload(19 - x1, 19 - y1, 19 - x2, 19 - y2)


def dummy_coor_to_num(coor):
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

	return [x1, y1, x2, y2]


def main():
	ip = input("input ip: ")
	port = input("input port number: ")
	color = input("input 1 for black, 2 for white: ")
	red_stones = connect('192.168.137.51', int(port), color)
	
	looper = red_stones.count(':') + 1
	for i in range(looper):
		chr_pos = 4 * i
		num_pos = 4 * i + 1
		x = ord(red_stones[chr_pos]) - 65
		if x > 8:
			x = x - 1
		y = int(red_stones[num_pos:num_pos+2]) - 1 
		print("red stone: x: " + str(x) + ", y: " + str(y))
		dummy_board[19 - y][19 - x] = 7
	print(dummy_board)

	if color == 1:
		dummy_board[9][9] = color
		draw_and_wait("K10")
	while 1:
		away = draw_and_wait(make_move())
		[x1, y1, x2, y2] = dummy_coor_to_num(away)
		dummy_board[19 - y1][19 - x1] = 5
		dummy_board[19 - y2][19 - x2] = 5
		print(dummy_board)
	
if __name__ == "__main__":
	main()
