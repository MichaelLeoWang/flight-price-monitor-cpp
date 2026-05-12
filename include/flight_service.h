#ifndef FLIGHT_SERVICE_H
#define FLIGHT_SERVICE_H

#include <string>
#include <optional>

class FlightService {
public:
    FlightService();
    
    std::optional<double> getLowestPrice(const std::string& origin, 
                                          const std::string& destination, 
                                          const std::string& departure_date);
    
private:
    std::string api_key_;
    std::string api_secret_;
    
    double mockPrice() const;
};

#endif