#![allow(unused)]
#[derive(Clone)]
enum address {
    address(Box<myList>),
	Nil,
}
#[derive(Clone)]
struct myList {
    value: u32,
	next: address,
} 
impl myList {
	fn append(&mut self, elem: u32) {
		match self.next {
			address::address(ref mut next_address) => {
				next_address.append(elem);
			}
			address::Nil => {
				let node = myList {
					value: elem,
					next: address::Nil,
				};
				self.next = address::address(Box::new(node));
			}
		} 
	}
    fn delete(&mut self, elem: u32) {
		match &self.next {
			address::address(mut next_address) => {
				if next_address.value == elem {
					println!("Deleting value {}", next_address.value);
					self.next = next_address.next.clone();
					//self.next =
				} else {
					next_address.delete(elem);
				}
			} 
			address::Nil => {
				if self.value == elem {
					self.value = 0;
				} else {
					println!("Element {} does not exist in the linked list", elem);
				}
			}
		}
	}
}

fn main()
{
	let mut head = myList {
		value: 8,
		next:address::Nil,
	};
	head.append(10);

	let mut temp1 = myList{ value: 10, next: address::Nil, };
	let mut temp2 = myList{ value: 11, next: address::Nil, };
	let mut temp3 = myList{ value: 12, next: address::Nil, };
	let mut temp4 = myList{ value: 13, next: address::Nil, };
	temp2.next = address::address(Box::new(temp3));
	temp1.next = address::address(Box::new(temp2));
	temp4.next = temp1.next.clone();
	match temp1.next{
		address::address(ref mut next_add)=>{
			println!("ori: temp2: {} {:p}", next_add.value, &next_add.value);
			match next_add.next{
				address::address(ref mut next_add)=>{
					println!("ori: temp3: {} {:p}", next_add.value, &next_add.value);
				}
				_=> { println!("temp nil"); }
			}
		}
		_=> { println!("temp2 nil"); }
	}
	match temp4.next{
		address::address(ref mut next_add)=>{
			println!("temp4: temp2: {} {:p}", next_add.value, &next_add.value);
			match next_add.next{
				address::address(ref mut next_add)=>{
					println!("temp4: temp3: {} {:p}", next_add.value, &next_add.value);
				}
				_=> { println!("temp nil"); }
			}
		}
		_=> { println!("temp4 nil"); }
	}
}
