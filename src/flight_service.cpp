#include "flight_service.h"
#include <random>
#include <cstdlib>

FlightService::FlightService() {
    api_key_ = std::getenv("API_KEY") ? std::getenv("API_KEY") : "";
    api_secret_ = std::getenv("API_SECRET") ? std::getenv("API_SECRET") : "";
}

double FlightService::mockPrice() const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(600.0, 1000.0);
    return dis(gen);
}

std::optional<double> FlightService::getLowestPrice(const std::string& origin,
                                                    const std::string& destination,
                                                    const std::string& departure_date) {
    return mockPrice();
}