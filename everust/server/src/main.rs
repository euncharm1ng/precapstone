#![allow(unused)]
mod board_operation;
use board_operation::*;

fn main() {
	let mut myboard = Board::new("");
	myboard.print_board();
}
