#![allow(unused)]
use std::rc::Rc;
use std::cell::RefCell;

struct Node{
	val:i32,
	next:Addr,
}

enum Addr{
	Nil,
	Address(Rc<RefCell<Node>>),
}

impl Addr{
	fn get_next(&self) -> Addr
	{
		match &self{
			Addr::Address(next_addr) =>{
				Addr::Address(Rc::clone(&next_addr))
			}
			Addr::Nil=>{
				Addr::Nil
			}
		}
	}

	fn print_count(&self)
	{
		if let Addr::Address(next_addr) = &self{
			println!("count: {}", Rc::strong_count(&next_addr));
		}
	}
}

impl Node{
	fn append(&mut self, elem:i32){
		match &self.next{
			Addr::Nil => {
				let node = Node{
					val:elem,
					next:Addr::Nil,
				};
				self.next = Addr::Address(Rc::new(RefCell::new(node)))
			}
			Addr::Address(next_addr) => {
				next_addr.borrow_mut().append(elem);
			}
		}
	}

	fn print(&mut self){
		println!("{} - ", self.val);
		if let Addr::Address(next_addr) = &self.next{
			next_addr.borrow_mut().print();
		}
	}

	fn delete(&mut self, elem:i32){
		match &self.next {
			Addr::Nil => {
				println!("element{} does not exist", elem);
			}
			Addr::Address(next_addr) => {
				if next_addr.borrow().val == elem {
					let temp = next_addr.borrow_mut().next.get_next();
					temp.print_count();
					self.next = temp;
					self.next.print_count();
				}
				else{
					next_addr.borrow_mut().delete(elem);
				}
			}
		}
	}
}

fn main() {
    println!("Hello, world!");
	let mut head = Node{val:0, next:Addr::Nil};
	head.append(1);
	head.append(2);
	head.append(3);
	head.append(4);
	head.append(5);
	head.print();
	head.delete(2);
	head.print();
}
