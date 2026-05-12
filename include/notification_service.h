#ifndef NOTIFICATION_SERVICE_H
#define NOTIFICATION_SERVICE_H

#include <string>
#include "database.h"

class NotificationService {
public:
    NotificationService();
    
    bool sendEmail(const std::string& to_email, 
                   const std::string& subject, 
                   const std::string& body);
    
    bool sendPriceAlert(const MonitorTask& task, double current_price);
    
private:
    std::string smtp_server_;
    int smtp_port_;
    std::string smtp_user_;
    std::string smtp_password_;
};

#endif