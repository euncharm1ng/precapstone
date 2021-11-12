use server::board_operation::*;
fn main() {
	let mut myboard = Board::new("A01:B01");
	println!("check and forward {}", myboard.check_and_forward("K10", BLACK));
	println!("check and forward {}", myboard.check_and_forward("K11:K12", WHITE));
	myboard.print_board();
}
