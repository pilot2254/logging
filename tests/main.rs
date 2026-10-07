use logging::{info, success, debug, warning, error, fatal, Logger};

fn main() {
    info!("this is info");
    success!("this is success");
    debug!("this is a debug message");
    warning!("this is a warning");
    error!("this is an error");
    fatal!("this is fatal");

    println!("\n\n");

    println!("also comes with args support: ");
    info!("my name is {} and im {} years old", "mike", 17);

    println!("\n\n");
    println!("you can also hide locations");
    Logger::get().lock().unwrap().set_show_location(false);
    info!("location hidden");
}
