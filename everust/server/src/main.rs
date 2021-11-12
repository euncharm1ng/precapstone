use server::board_operation::*;
fn main() {
	let mut myboard = Board::new();
	//myboard.print_board();

	//let ((x1, y1), (x2, y2)) = parse("A01:B02");
	//println!("{} {}, {} {}", x1, y1, x2, y2);

	println!("check and forward {}", myboard.check_and_forward("K10", BLACK));
	myboard.print_board();
	/*
	myboard.place_stone(BLACK, 9, 9);
	if myboard.is_end(BLACK, 9, 9) { println!("won"); }
	myboard.place_stone(BLACK, 8, 10);
	if myboard.is_end(BLACK, 8, 10) { println!("won"); }
	myboard.place_stone(BLACK, 7, 11);
	if myboard.is_end(BLACK, 7, 11) { println!("won"); }
	myboard.place_stone(BLACK, 6, 12);
	if myboard.is_end(BLACK, 6, 12) { println!("won"); }
	myboard.place_stone(BLACK, 5, 13);
	if myboard.is_end(BLACK, 5, 13) { println!("won"); }
	myboard.place_stone(BLACK, 4, 14);
	if myboard.is_end(BLACK, 4, 14) { println!("won"); }
	myboard.print_board();
	*/
	//if myboard.is_k10("K11") { println!("k11!"); }
}
