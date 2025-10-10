use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::Arc;
use std::fs;
use std::net::TcpStream;
use clap::{Parser};


use crate::{analyser::TextAnalysis, config::{Config, Cli}};
use inotify::{Events, Inotify, WatchMask};
use std::io::{Write, ErrorKind};

pub mod analyser;
pub mod config;

fn process_event(events: Events<'_>, pool: &mut rayon_core::ThreadPool, config: &Config) {
    for event in events {
        // Handle event
        let Some(filename) = event.name else {
            continue;
        };

        let full_path = config.folder_to_scan.to_string() + filename.to_str().unwrap();
        let full_path = fs::canonicalize(full_path).unwrap();

        pool.install(move || {
            let mut my_analyser = TextAnalysis::new(full_path.clone().to_str().unwrap()).unwrap();
            my_analyser.analyse_file().unwrap();
            
            if let Ok(mut stream) = TcpStream::connect((config.server_ip, config.server_port)) {
                stream.write_all(my_analyser.build_json().as_bytes())
                .unwrap_or_else(|err| {
                    eprintln!("Could not send parsing information for {} -> {}", full_path.to_string_lossy(), err);
                });
            } else {
                eprintln!("Could not send parsing information for {0}", full_path.to_string_lossy());
            }
        });
    }
}

fn run (config: &Config, stop: &Arc<AtomicBool>) {

    rayon::ThreadPoolBuilder::new().num_threads(config.max_thread).build_global().unwrap();
    let mut pool = rayon_core::ThreadPoolBuilder::default().build().unwrap();

     let mut inotify = Inotify::init()
        .expect("Error while initializing inotify instance");

    // Watch for modify and close events.
    inotify
        .watches()
        .add(
            config.folder_to_scan.clone(),
            WatchMask::CREATE,
        )
        .expect("Failed to add file watch");

    let mut buffer = [0; 1024];

    while !stop.load(Ordering::Relaxed) {

        let events=  {
            match inotify.read_events(&mut buffer) {
                Ok(events) => events,
                Err(error) if error.kind() == (ErrorKind::WouldBlock) => continue,
                _ => panic!("Error while reading events"),
            }
        };

        process_event(events, &mut pool, config);
    };

}
 
fn main() {

    let stop = Arc::new(AtomicBool::new(false));
    let stop_clone = Arc::clone(&stop);

    let _ = ctrlc::set_handler( move|| {
        
        println!("Received end of program");
        stop.store(true, Ordering::Relaxed);
    });

    let cli = Cli::parse();
    let config : Config = Config::build_config(cli);

    println!("{config:?}");

    run(&config, &stop_clone);
}
