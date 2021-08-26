

fn first_word(s: &str) -> &str{
    let bytes = s.as_bytes();
    for(i, &item) in bytes.iter().enumerate(){
        if item == b' ' {
            return &s[..i];
        }
    }
    return &s[..];
}

fn main(){
    let mut s = String::from("hello world");
    let word = first_word(&s[..]);
    println!("{}", word);

    let my_string_literal = "hello world";
    
    let word = first_word(&my_string_literal[..]);
    println!("{}", word);

    let word = first_word(my_string_literal);
    println!("{}", word);
}

