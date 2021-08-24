// use std::io;

fn another_function(x:i32, y:i32) -> i32 {
    let z = {
        let w = x;
        w + y
    };
    // return z;
    println!("another function {} {} {}", x, y, z);
    return z;
}

fn plus_one(x: i32) -> i32 {
    x + 1
}

fn main() {
    /*
    let mut x = 18_4;
    println!("the values of x is : {}", x);
    *(&mut x) = 629438; 
    println!("the value of x is : {}", x);

    let aaa = 2.0;
    let bbb:f32 = 3.0;

    println!("aaa == {}", aaa);
    println!("bbb == {}", bbb);

    // let i = 0 , j = 1;
    // i = j = 0;
    // println!("i == {}, j == {}", i, j);

    let c:char = '😻';
    println!("{}", c);

    let tup = (500, 6.4, 1);
    // let (a, b, c) = tup;
    println!("tup == ( {}, {}, {} )", tup.0, tup.1, tup.2);
    tup.0 = 300; tup.1 = 2.; tup.2 = 2;
    println!("tup == ( {}, {}, {} )", tup.0, tup.1, tup.2);
    */

    /*
    let a = [1,2,3,4,5];
    println!("enter an array index:");
    let mut index = String::new();
    io::stdin()
        .read_line(&mut index)
        .expect("error reading line");
    let index: usize = index.trim().parse().expect("input num");

    let element = a[index];


    println!("the value is {}", element);
    */

    // println!("{}", another_function(6, 23));
    println!("x is {}", plus_one(5));
}
