use server::board_operation::*;
fn main() {
	let mut myboard = Board::new();
	myboard.print_board();

	//let ((x1, y1), (x2, y2)) = parse("A01:B02");
	//println!("{} {}, {} {}", x1, y1, x2, y2);

	println!("check and forward {}", myboard.check_and_forward("A01:B02", BLACK));
}
