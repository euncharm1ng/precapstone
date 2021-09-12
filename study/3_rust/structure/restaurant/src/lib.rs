// mod front_of_house {
//     pub mod hosting {
//         pub fn add_to_waitlist() {}
//     }
// }

// pub fn eat_at_restaurant() {
//     // Absolute path
//     crate::front_of_house::hosting::add_to_waitlist();

//     // Relative path
//     front_of_house::hosting::add_to_waitlist();
// }
mod front_of_house {
    pub mod hosting {
        pub fn add_to_waitlist(){}
    }

    mod serving {
        fn take_order() {}

        fn server_order() {}
        
        fn take_payment() {}
    }
}

pub fn eat_at_restaurant(){
    front_of_house::hosting::add_to_waitlist();
}