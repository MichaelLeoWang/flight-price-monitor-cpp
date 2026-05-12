#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <optional>

struct MonitorTask {
    int id;
    std::string origin;
    std::string destination;
    std::string departure_date;
    double price_threshold;
    std::string email;
    bool active;
    std::string created_at;
    std::string last_checked;
    double last_price;
};

struct PriceRecord {
    int id;
    int task_id;
    double price;
    std::string checked_at;
};

class Database {
public:
    Database(const std::string& db_path);
    
    bool init();
    bool createTask(const MonitorTask& task, int& out_id);
    bool getTask(int id, MonitorTask& out_task);
    bool getAllTasks(std::vector<MonitorTask>& out_tasks);
    bool updateTask(int id, const MonitorTask& task);
    bool deleteTask(int id);
    bool updateTaskPrice(int id, double price);
    bool addPriceRecord(int task_id, double price);
    
private:
    std::string db_path_;
    std::string tasks_file_;
    std::string records_file_;
    int next_id_;
    
    void loadNextId();
    void saveNextId();
    std::string getCurrentTime() const;
};

#endif