#include <iostream>
#include <signal.h>
#include <thread>
#include <atomic>
#include <chrono>
#include "database.h"
#include "flight_service.h"
#include "notification_service.h"

std::atomic<bool> running(true);

void handleSignal(int signal) {
    running = false;
}

void checkAllTasks(Database& db, FlightService& fs, NotificationService& ns) {
    std::vector<MonitorTask> tasks;
    if (db.getAllTasks(tasks)) {
        for (const auto& task : tasks) {
            auto price = fs.getLowestPrice(task.origin, task.destination, task.departure_date);
            if (!price) {
                std::cerr << "Failed to get price for task " << task.id << std::endl;
                continue;
            }
            
            db.updateTaskPrice(task.id, *price);
            db.addPriceRecord(task.id, *price);
            
            std::cout << "Task " << task.id << ": " << task.origin << " -> " 
                      << task.destination << " = ¥" << *price << std::endl;
            
            if (*price < task.price_threshold) {
                std::cout << "Price below threshold! Sending notification..." << std::endl;
                ns.sendPriceAlert(task, *price);
            }
        }
    }
}

void printMenu() {
    std::cout << "\nFlight Price Monitor - Main Menu\n";
    std::cout << "1. Add monitoring task\n";
    std::cout << "2. List all tasks\n";
    std::cout << "3. Check price manually\n";
    std::cout << "4. Exit\n";
    std::cout << "Enter your choice: ";
}

void addTask(Database& db) {
    MonitorTask task;
    task.id = 0;
    std::cout << "Enter origin (e.g., PEK): ";
    std::cin >> task.origin;
    std::cout << "Enter destination (e.g., SHA): ";
    std::cin >> task.destination;
    std::cout << "Enter departure date (YYYY-MM-DD): ";
    std::cin >> task.departure_date;
    std::cout << "Enter price threshold: ";
    std::cin >> task.price_threshold;
    std::cout << "Enter notification email: ";
    std::cin >> task.email;
    task.active = true;
    
    int id;
    if (db.createTask(task, id)) {
        std::cout << "Task created successfully with ID: " << id << std::endl;
    } else {
        std::cout << "Failed to create task" << std::endl;
    }
}

void listTasks(Database& db) {
    std::vector<MonitorTask> tasks;
    if (db.getAllTasks(tasks)) {
        std::cout << "\nMonitoring Tasks:\n";
        for (const auto& task : tasks) {
            std::cout << "ID: " << task.id 
                      << " | " << task.origin << " -> " << task.destination
                      << " | Date: " << task.departure_date
                      << " | Threshold: ¥" << task.price_threshold
                      << " | Last Price: ¥" << task.last_price << std::endl;
        }
    } else {
        std::cout << "Failed to get tasks" << std::endl;
    }
}

void checkPriceManual(FlightService& fs) {
    std::string origin, destination, date;
    std::cout << "Enter origin: ";
    std::cin >> origin;
    std::cout << "Enter destination: ";
    std::cin >> destination;
    std::cout << "Enter date (YYYY-MM-DD): ";
    std::cin >> date;
    
    auto price = fs.getLowestPrice(origin, destination, date);
    if (price) {
        std::cout << "Current price: ¥" << *price << std::endl;
    } else {
        std::cout << "Failed to get price" << std::endl;
    }
}

int main() {
    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

    Database db("./flight_monitor.db");
    if (!db.init()) {
        std::cerr << "Failed to initialize database" << std::endl;
        return 1;
    }
    std::cout << "Database initialized successfully" << std::endl;

    FlightService fs;
    NotificationService ns;

    std::thread monitorThread([&db, &fs, &ns]() {
        while (running) {
            checkAllTasks(db, fs, ns);
            std::this_thread::sleep_for(std::chrono::hours(1));
        }
    });
    monitorThread.detach();

    std::cout << "Price monitor started (checks every hour)" << std::endl;

    int choice;
    while (running) {
        printMenu();
        std::cin >> choice;
        
        switch (choice) {
            case 1:
                addTask(db);
                break;
            case 2:
                listTasks(db);
                break;
            case 3:
                checkPriceManual(fs);
                break;
            case 4:
                running = false;
                break;
            default:
                std::cout << "Invalid choice" << std::endl;
        }
    }

    std::cout << "\nExiting..." << std::endl;
    return 0;
}