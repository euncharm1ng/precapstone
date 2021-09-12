#![allow(unused)]

//TODO: diff btw String, &String, str, &str
/*
* String is a growable heap-allocated data structure,
* str is an IMMUTABLE fixed-length string
* &String is a reference of String, can be coereced to &str
* &str (aka string slice) a borrowed type, preffered way to pass string around
*/    



fn main()
{
    use std::collections::HashMap;
    // let mut my_map = HashMap::new();
    let mut map = HashMap::new();
    let s1 = String::from("Add Sally to Engineering");
    let s2 = String::from("Add Amir to Sales");
    let mut split = s1.split_whitespace();
    let name = split.nth(1);
    let dept = split.nth(1);
    println!("{:?} {:?}", name, dept);
    let temp = map.entry(dept).or_insert(name);
    println!("{:?}", temp);
}

/*
fn pig_latin(mut a_string: String) -> String
{
    match &a_string[0..1] {
        "a" | "e" | "i" | "o" | "u" => 
            a_string.push_str("-hay"), 
        _ => 
            a_string = a_string[1..a_string.len()].to_string() + &"-" + &a_string[0..1] + &"ay",
    }
    return a_string;
}
fn main()
{
    let mut word = String::from("apple");
    word = pig_latin(word);
    println!("{}", word);
}
*/
/*
fn main()
{
    use std::collections::HashMap;
    let mut my_vec = vec![1,3,9,2, 2]; 
    let mut total = 0i32;
    let mut map = HashMap::new();
    let mut mode = 0;
    let mut mode_cnt = 0;
    my_vec.sort();
    for (i) in &my_vec {
        println!("{}", i);
        total += i;
       let count = map.entry(i).or_insert(0);
       *count += 1; 
    }
    println!("mean: {}", total as f32 / my_vec.len() as f32);
    println!("median: {}", my_vec[my_vec.len() / 2]);

    for (field, value) in &map{
        if *value > mode_cnt {
            mode_cnt = *value;
            mode = **field;
        }
    }
    println!("mode: {}", mode);
}
*/


/*
fn main()
{
    use std::collections::HashMap;
    let text = "hello world wonderful world";

    let mut map = HashMap::new();

    for word in text.split_whitespace(){
        let count = map.entry(word).or_insert(0);
        *count += 1;
    }
    println!("{:?}", map);
}
*/
/*
fn main()
{
    use std::collections::HashMap;
    let mut scores = HashMap::new();
    scores.insert(String::from("Blue"), 10);

    scores.entry(String::from("Yellow")).or_insert(50);
    scores.entry(String::from("Blue")).or_insert(100);
    println!("{:?}", scores);
    println!("{:?}", scores.entry(String::from("Blue")));
}
*/
/*
fn main()
{
    use std::collections::HashMap;
    let teams = vec![String::from("Blue"), String::from("Yellow")];
    let initial_scores = vec![10, 50];
   
    let mut scores: HashMap<String, i32> = 
        teams.into_iter().zip(initial_scores.into_iter()).collect();
    for i in initial_scores{
        println!("{}", i);
    }
    let field_name = String::from("Favorite color");
    let field_value = 10;

    let mut map = HashMap::new();
    map.insert(field_name, field_value);
    println!("{}", field_value);
}
*/
/*
fn main()
{
    let s1 = String::from("tic");
    let s2 = String::from("tac");
    let s3 = String::from("toe");
    // let s = s1 + "-" + &s2 + "-" + &s3;
    // println!("{}", s);

    let s = format!("{}-{}-{}", s1, s2, s3);
    println!("{}", s);
    println!("{}", s1);
    println!("{}", s2);
    println!("{}", s3);
}
*/
/*
fn main()
{
    let hello = "नमस्ते";
    // let s = &hello[0..3];
    // println!("{}", s);
    for c in hello.bytes(){
        println!("{}", c);
    }
}
*/
/*
enum SpreadsheetCell{
    Int(i32),
    Float(f64),
    Text(String),
}
fn main(){
    let row = vec![
        SpreadsheetCell::Int(3),
        SpreadsheetCell::Text(String::from("blue")),
        SpreadsheetCell::Float(10.12),
    ];
    for i in & row{
        match i {
            SpreadsheetCell::Int(value) => println!("{}", value),
            SpreadsheetCell::Float(value) => println!("{}", value),
            SpreadsheetCell::Text(value) => println!("{}", value),
        }
    }
    let i:Option<i32> = None;
}
*/
/*
fn copy(something:&str){
    println!("{}", something);
}
fn main(){
    let mut mystring = "string";
    copy(mystring);
    println!("{}", mystring);
}

*/
