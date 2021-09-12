#![allow(unused)]
fn main()
{
    pub struct Guess{
        value: i32,
    }
    impl Guess{
        pub fn new(value: i32) -> Guess
        {
            if value < 1 || value > 100 {
                panic!("guess value must be btw 1 and 100, got {}. ", value);
            }
            Guess {value}
        }

        pub fn value(&self) -> i32
        {
            self.value
        }
    }
}