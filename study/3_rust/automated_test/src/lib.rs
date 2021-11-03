#[cfg(test)]
mod tests{
	use super::*;
	#[test]
	fn greeting_contains_name(){
		let result = greeting("carol");
		assert!(result.contains("carol"));
	}
	#[test]
	fn larger_can_hold_smaller(){
		let larger = Rectangle {
			width: 8,
			height: 7,
		};
		let smaller = Rectangle {
			width: 5,
			height: 1,
		};
		assert!(larger.can_hold(&smaller));
	}
	#[test]
	fn exploration()
	{
		assert_eq!(2+2, 4);
	}
/*
	#[test]
	fn another()
	{
		panic!("make this test fail");
	}
	*/
}

#[derive(Debug)]
struct Rectangle {
	width: u32,
	height: u32,
}
impl Rectangle {
	fn can_hold(&self, other: &Rectangle) -> bool {
		self.width > other.width && self.height > other.height
	}
}
pub fn greeting(name:&str) -> String {
	format!("hello {}!", name)
}
