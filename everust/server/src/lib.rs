#![allow(unused)]

pub mod board_operation	{

	const BOARDSIZE: usize = 19;
	pub const EMPTY:i8 = 0;
	pub const BLACK:i8 = 1;
	pub const WHITE:i8 = 2;
	pub const RED:i8 = 3;

	pub struct Board{
		board:Vec<Vec<i8>>,
	}

	impl Board{
		pub fn new() -> Board
		{
			Board{ board:vec![vec![EMPTY; BOARDSIZE]; BOARDSIZE], }
		}

		fn is_end(&self, color:i8, x:usize, y:usize) -> bool
		{
			let mut count = 0;
			//horizontal
			for j in (x..19){
				if self.board[y][j] == color { count += 1; }
				else { break; }
			}
			for j in (0..x).rev(){
				if self.board[y][j] == color { count += 1; }
				else { break; }
			}
			if count > 5 { return true; }
			else { count = 0; }

			//vertical
			for i in (y..19){
				if self.board[i][x] == color { count += 1; }
				else { break; }
			}
			for i in (0..y).rev(){
				if self.board[i][x] == color { count += 1; }
				else { break; }
			}
			if count > 5 { return true; }
			else { count = 0; }

			//right up
			for (i, j) in (y..19).zip((0..=x).rev()){
				if self.board[i][j] == color { count += 1; }
				else { break; }
			}
			for (i, j) in (0..y).rev().zip(x+1..19){
				if self.board[i][j] == color { count += 1; }
				else { break; }
			}
			if count > 5 { return true; }
			else { count = 0; }

			//left up
			for (i, j) in (0..y).rev().zip((0..x).rev()){
				if self.board[i][j] == color { count += 1; }
				else { break; }
			}
			for (i, j) in (y..19).zip(x..19){
				if self.board[i][j] == color { count += 1; }
				else { break; }
			}
			if count > 5 { return true; }
			else { return false; }
		}

		fn is_valid(&self, x1:usize, y1:usize, x2:usize, y2:usize) -> bool
		{
			if self.board[y1][x1] != 0
				{ return false; }
			else if y2 != 31 && y2 != 31 && self.board[y2][x2] != 0 // for k10
				{ return false; }
			else
				{ return true; }
		}

		fn place_stone(&mut self, color:i8, x:usize, y:usize)
		{ self.board[y][x] = color; }

		pub fn print_board(&self)
		{
			for row in &self.board{
				for stone in row { print!("{} ", stone); }
				println!("");
			}
		}

		pub fn check_and_forward(&mut self, msg:&str, color:i8) -> u8
		{
			if is_coor_msg(msg) == false { return 1; } // received error msg
			let ((x1, y1), (x2, y2)) = parse(msg);
			if self.is_valid(x1, y1, x2, y2) == false { return 2; } // invalid input

			self.place_stone(x1, y1);
			if self.is_end(x1, y1) { return 3; } // the game ended
			self.place_stone(x2, y2);
			if self.is_end(x2, y2) { return 3; } // the game ended

			4 // FIXME
		}

	}

	fn is_coor_msg(msg:&str) -> bool
	{
		if msg.len() > 7 { return false; }
		else{ return true; }
	}

	fn parse(msg:&str) -> ((usize, usize), (usize, usize))
	{
		let mut coors = msg.split(':');
		let mut myvec:Vec<usize> = vec![];
		for (cnt, a_coor) in coors.enumerate(){
			//println!("{} {}", cnt, a_coor);
			let mut alphabet = a_coor.chars().nth(0).unwrap() as usize - 65;
			if alphabet > 8 { alphabet += 1; }
			myvec.push(alphabet);
			myvec.push( match &a_coor.to_string()[1..3].parse::<usize>() {
				Ok(value) => { 19 - value },
				Err(e) => { panic!("parse error");} //FIXME
			});
		}
		((myvec[0], myvec[1]), (myvec[2], myvec[3]))
	}
}

