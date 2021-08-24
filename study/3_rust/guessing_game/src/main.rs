use std::io;
use rand::Rng;
use std::cmp::Ordering;

fn main() {
    println!("Guess the number!");

    let secret_number = rand::thread_rng().gen_range(1..101);
    println!("the secrete number is {}", secret_number);

    loop{
        println!("Please input your guess.");
        let mut guess = String::new();

        io::stdin()
            .read_line(&mut guess)
            .expect("Failed to read line");
        let guess: i32 = match guess.trim().parse(){
            Ok(num) => num,
            Err(_) => continue,
        };

        println!("You guess: {}", guess);

        match guess.cmp(&secret_number){
            Ordering::Less => println!("too small"),
            Ordering::Equal => {
                println!("win");
                break;
            },
            Ordering::Greater => println!("too big"),
            
        }
    }
}
