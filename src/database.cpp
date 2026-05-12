#include "database.h"
#include <fstream>
#include <sstream>
#include <ctime>

Database::Database(const std::string& db_path) : db_path_(db_path) {
    tasks_file_ = db_path_ + ".tasks";
    records_file_ = db_path_ + ".records";
    next_id_ = 1;
}

bool Database::init() {
    std::ifstream id_file(db_path_ + ".id");
    if (id_file.is_open()) {
        id_file >> next_id_;
        id_file.close();
    }
    
    std::ofstream tasks_out(tasks_file_, std::ios::app);
    std::ofstream records_out(records_file_, std::ios::app);
    return tasks_out.is_open() && records_out.is_open();
}

std::string Database::getCurrentTime() const {
    std::time_t now = std::time(nullptr);
    char buf[80];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

void Database::loadNextId() {
    std::ifstream id_file(db_path_ + ".id");
    if (id_file.is_open()) {
        id_file >> next_id_;
        id_file.close();
    }
}

void Database::saveNextId() {
    std::ofstream id_file(db_path_ + ".id");
    if (id_file.is_open()) {
        id_file << next_id_;
        id_file.close();
    }
}

bool Database::createTask(const MonitorTask& task, int& out_id) {
    out_id = next_id_++;
    saveNextId();
    
    std::ofstream file(tasks_file_, std::ios::app);
    if (!file.is_open()) return false;
    
    file << out_id << "|"
         << task.origin << "|"
         << task.destination << "|"
         << task.departure_date << "|"
         << task.price_threshold << "|"
         << task.email << "|"
         << (task.active ? "1" : "0") << "|"
         << getCurrentTime() << "||0\n";
    
    file.close();
    return true;
}

bool Database::getAllTasks(std::vector<MonitorTask>& out_tasks) {
    out_tasks.clear();
    std::ifstream file(tasks_file_);
    if (!file.is_open()) return false;
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        MonitorTask task;
        
        std::getline(iss, token, '|'); task.id = std::stoi(token);
        std::getline(iss, task.origin, '|');
        std::getline(iss, task.destination, '|');
        std::getline(iss, task.departure_date, '|');
        std::getline(iss, token, '|'); task.price_threshold = std::stod(token);
        std::getline(iss, task.email, '|');
        std::getline(iss, token, '|'); task.active = (token == "1");
        std::getline(iss, task.created_at, '|');
        std::getline(iss, task.last_checked, '|');
        std::getline(iss, token); task.last_price = std::stod(token);
        
        if (task.active) {
            out_tasks.push_back(task);
        }
    }
    
    file.close();
    return true;
}

bool Database::getTask(int id, MonitorTask& out_task) {
    std::ifstream file(tasks_file_);
    if (!file.is_open()) return false;
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        int task_id;
        
        std::getline(iss, token, '|'); task_id = std::stoi(token);
        if (task_id != id) continue;
        
        MonitorTask task;
        task.id = task_id;
        std::getline(iss, task.origin, '|');
        std::getline(iss, task.destination, '|');
        std::getline(iss, task.departure_date, '|');
        std::getline(iss, token, '|'); task.price_threshold = std::stod(token);
        std::getline(iss, task.email, '|');
        std::getline(iss, token, '|'); task.active = (token == "1");
        std::getline(iss, task.created_at, '|');
        std::getline(iss, task.last_checked, '|');
        std::getline(iss, token); task.last_price = std::stod(token);
        
        out_task = task;
        file.close();
        return true;
    }
    
    file.close();
    return false;
}

bool Database::updateTask(int id, const MonitorTask& task) {
    std::ifstream in_file(tasks_file_);
    if (!in_file.is_open()) return false;
    
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in_file, line)) {
        std::istringstream iss(line);
        std::string token;
        std::getline(iss, token, '|');
        int task_id = std::stoi(token);
        
        if (task_id == id) {
            std::istringstream original_iss(line);
            std::string orig_token;
            std::getline(original_iss, orig_token, '|');
            std::getline(original_iss, orig_token, '|'); std::string origin = orig_token;
            std::getline(original_iss, orig_token, '|'); std::string dest = orig_token;
            std::getline(original_iss, orig_token, '|'); std::string date = orig_token;
            std::getline(original_iss, orig_token, '|');
            std::getline(original_iss, orig_token, '|');
            std::getline(original_iss, orig_token, '|');
            std::getline(original_iss, orig_token, '|'); std::string created = orig_token;
            std::getline(original_iss, orig_token, '|'); std::string checked = orig_token;
            std::getline(original_iss, orig_token); std::string last_price = orig_token;
            
            std::ostringstream new_line;
            new_line << id << "|" << origin << "|" << dest << "|" << date << "|"
                     << task.price_threshold << "|" << task.email << "|"
                     << (task.active ? "1" : "0") << "|" << created << "|" 
                     << checked << "|" << last_price;
            lines.push_back(new_line.str());
        } else {
            lines.push_back(line);
        }
    }
    in_file.close();
    
    std::ofstream out_file(tasks_file_);
    if (!out_file.is_open()) return false;
    
    for (const auto& l : lines) {
        out_file << l << "\n";
    }
    out_file.close();
    return true;
}

bool Database::deleteTask(int id) {
    std::ifstream in_file(tasks_file_);
    if (!in_file.is_open()) return false;
    
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in_file, line)) {
        std::istringstream iss(line);
        std::string token;
        std::getline(iss, token, '|');
        int task_id = std::stoi(token);
        if (task_id != id) {
            lines.push_back(line);
        }
    }
    in_file.close();
    
    std::ofstream out_file(tasks_file_);
    if (!out_file.is_open()) return false;
    
    for (const auto& l : lines) {
        out_file << l << "\n";
    }
    out_file.close();
    return true;
}

bool Database::updateTaskPrice(int id, double price) {
    std::ifstream in_file(tasks_file_);
    if (!in_file.is_open()) return false;
    
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in_file, line)) {
        std::istringstream iss(line);
        std::string token;
        std::getline(iss, token, '|');
        int task_id = std::stoi(token);
        
        if (task_id == id) {
            std::istringstream original_iss(line);
            std::string orig_token;
            std::getline(original_iss, orig_token, '|');
            std::getline(original_iss, orig_token, '|'); std::string origin = orig_token;
            std::getline(original_iss, orig_token, '|'); std::string dest = orig_token;
            std::getline(original_iss, orig_token, '|'); std::string date = orig_token;
            std::getline(original_iss, orig_token, '|'); std::string threshold = orig_token;
            std::getline(original_iss, orig_token, '|'); std::string email = orig_token;
            std::getline(original_iss, orig_token, '|'); std::string active = orig_token;
            std::getline(original_iss, orig_token, '|'); std::string created = orig_token;
            
            std::ostringstream new_line;
            new_line << id << "|" << origin << "|" << dest << "|" << date << "|"
                     << threshold << "|" << email << "|" << active << "|" 
                     << created << "|" << getCurrentTime() << "|" << price;
            lines.push_back(new_line.str());
        } else {
            lines.push_back(line);
        }
    }
    in_file.close();
    
    std::ofstream out_file(tasks_file_);
    if (!out_file.is_open()) return false;
    
    for (const auto& l : lines) {
        out_file << l << "\n";
    }
    out_file.close();
    return true;
}

bool Database::addPriceRecord(int task_id, double price) {
    std::ofstream file(records_file_, std::ios::app);
    if (!file.is_open()) return false;
    
    static int record_id = 1;
    file << record_id++ << "|" << task_id << "|" << price << "|" << getCurrentTime() << "\n";
    file.close();
    return true;
}