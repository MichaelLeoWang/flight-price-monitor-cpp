#include "notification_service.h"
#include <sstream>
#include <cstdlib>
#include <iostream>

NotificationService::NotificationService() {
    smtp_server_ = std::getenv("SMTP_SERVER") ? std::getenv("SMTP_SERVER") : "smtp.gmail.com";
    smtp_port_ = std::getenv("SMTP_PORT") ? std::atoi(std::getenv("SMTP_PORT")) : 587;
    smtp_user_ = std::getenv("SMTP_USER") ? std::getenv("SMTP_USER") : "";
    smtp_password_ = std::getenv("SMTP_PASSWORD") ? std::getenv("SMTP_PASSWORD") : "";
}

bool NotificationService::sendEmail(const std::string& to_email,
                                    const std::string& subject,
                                    const std::string& body) {
    if (smtp_user_.empty() || smtp_password_.empty()) {
        std::cout << "Notification: " << subject << "\n" << body << std::endl;
        return true;
    }
    return true;
}

bool NotificationService::sendPriceAlert(const MonitorTask& task, double current_price) {
    std::stringstream subject;
    subject << "Flight Price Alert: " << task.origin << " -> " << task.destination;
    
    std::stringstream body;
    body << "Flight Price Monitor Alert:\n\n"
         << "Origin: " << task.origin << "\n"
         << "Destination: " << task.destination << "\n"
         << "Departure Date: " << task.departure_date << "\n\n"
         << "Current Price: ¥" << current_price << "\n"
         << "Threshold: ¥" << task.price_threshold << "\n\n"
         << "Price is below threshold! Please book now.\n\n"
         << "-- Flight Price Monitor System";
    
    return sendEmail(task.email, subject.str(), body.str());
}