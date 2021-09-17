#![allow(unused)]

/*
pub trait Summary 
{
    fn summarize(&self) -> String ;
}

pub struct NewsArticle{
    pub headline: String,
    pub location: String,
    pub author: String,
    pub content: String,
}
impl Summary for NewsArticle {
    fn summarize(&self) -> String{
        format!("{}, by {} ({})", self.headline, self.author, self.location)
    }
}

pub struct Tweet {
    pub username: String,
    pub content: String,
    pub reply: bool,
    pub retweet: bool,
}
impl Summary for Tweet{
    fn summarize(&self) -> String{
        format!("{}: {}", self.username, self.content)
    }
}

pub fn notify <T: Summary>(item: &T){
    println!("Breaking news! {}", item.summarize());
}

fn returns_summarizable() -> impl Summary
{
    Tweet{
        username: String::from("horse_ebooks"),
        content: String::from("of course"),
        reply: false,
        retweet: false,
    }
}

fn main()
{
    let tweet = Tweet{
        username: String::from("horse_ebooks"),
        content: String::from("of course, as you probably already know, people",),
        reply: false,
        retweet: false,
    };

    println!("1 new tweet: {}", tweet.summarize());
    notify(&tweet);

}
*/
/*
struct Point<T,U>{
    x: T,
    y: U,
}

impl <T, U> Point<T, U> {
    fn mixup<V, W>(self, other:Point<V, W>) -> Point<T, W> 
    {
        Point {
            x: self.x,
            y: other.y,
        }
    }
}
fn main()
{
    // let integer = Point { x: 5, y: 10};
    // let float = Point { x: 1.0, y: 4.0};
    // println!("{}", integer.x());
    // println!("{}", float.distance_from_origin()); 
    
    let p1 = Point { x: 5, y: 10.4};
    let p2 = Point { x: "hello", y: 'c'};

    let p3 = p1.mixup(p2);
    println!("{}, {}", p3.x, p3.y);
}
*/
/*
fn largest<T: PartialOrd + Copy>(list: &[T]) -> T
{
    let mut largest = list[0];

    for &item in list{
        if item > largest {
            largest = item;
        }
    }
    largest
}
fn main() 
{
    let number_list = vec![34, 50, 25, 100, 65];
    let result = largest(&number_list);
    println!("the largest is {}", result);

    let char_list = vec!['y', 'n', 'a', 'q'];
    let result = largest(&char_list);
    println!("the largest is {}", result);
}
*/
/*
use std::fmt:Display;
struct Pair<T> {
    x: T,
    y: T,
}

impl <T> Pair<T> {
    fn new(x:T, y:T)->Self {
        Self{x,y}
    }
}

impl<T: Display + PartialOrd> Pair<T> {
    fn cmp_display(&self){
        if self.x >= self.y {
            println!("the largest mem is x = {}", self.x);
        }
        else {
            println!("the largest mem is y = {}", self.y);
        }
    }
}
*/

fn main()
{
    let x;
    {
        let r = 5;
        x = &r;
    }
    println!("{}", x);
}