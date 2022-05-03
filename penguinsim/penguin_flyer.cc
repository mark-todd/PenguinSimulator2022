#include <iostream>
#include <unistd.h>
#include <fstream>

class Tracker {
    private:
        std::string delimiter;
        std::ofstream tracker_file;
    public:
        Tracker(const std::string filename, const std::string delimiter = ",") {
            Tracker::delimiter = delimiter;
            tracker_file.open("tracker_file.csv");//, std::ios_base::app);
        }

        void record_flight(const std::string name, const double burned_calories, const double flight_distance) {
            const std::string new_line = name + delimiter + std::to_string(burned_calories) + delimiter + std::to_string(flight_distance);
            tracker_file << new_line << std::endl;
            std::cout << new_line << std::endl;
        }

};


class Penguin {
    // Penguins can fly if they're fed enough!
    private:
        std::string name;
        double cals_per_meter;
        double stored_calories;
        Tracker & p_tracker;
    public:
        Penguin(
            const std::string name, 
            Tracker & p_tracker,
            const double cals_per_meter = 10
        ) : name(name), cals_per_meter(cals_per_meter), p_tracker(p_tracker) {}
        void give_food(const double calorific_value, const int quantity) {
            float total_calories = 0;
            for (int i=0; i< quantity; i++) {
                total_calories += calorific_value;
            }
            stored_calories += total_calories;
        }

        bool can_fly() {
            return stored_calories >= 5000;
        }
        double fly(const double flight_distance) {
            double possible_flight_distance;
            double actual_flight_distance;
            double delta_calories;
            possible_flight_distance = stored_calories / cals_per_meter;
            if (flight_distance <= possible_flight_distance) {
                actual_flight_distance = flight_distance;
            }
            else {
                actual_flight_distance = possible_flight_distance;
            }
            delta_calories = (cals_per_meter * actual_flight_distance);
            stored_calories -= (cals_per_meter * actual_flight_distance);
            p_tracker.record_flight(name, delta_calories, actual_flight_distance);
            return actual_flight_distance;
        }

};


int main()
{
    int food_items;
    double cal_value;
    Tracker penguin_tracker("output.csv");
    Penguin gentoo("Fred", penguin_tracker);
    while (true) {
        std::cout << "How many food items shall I feed you penguin?" << std::endl;
        std::cin >> food_items;
        std::cout << "What's the calorific value of each item?" << std::endl;
        std::cin >> cal_value;
        gentoo.give_food(cal_value, food_items);
        while (true) {
            std::cout << "Your penguin ";
            if (gentoo.can_fly()) {
                std::cout << "can fly" << std::endl;
                std::string flight_choice;
                while (true) {
                    std::cout << "Would you like your penguin to fly? (y/n)" << std::endl;
                    std::cin >> flight_choice;
                    if (flight_choice == "y") {
                        break;
                    }
                    else if (flight_choice == "n") {
                        break;
                    }
                    else {
                        std::cout << "invalid choice" << std::endl;
                        continue;
                    }
                }
                if (flight_choice == "y") {
                    double flight_distance;
                    double actual_flight_distance;
                    std::cout << "How far would you like your penguin to fly?" << std::endl;
                    std::cin >> flight_distance;
                    actual_flight_distance = gentoo.fly(flight_distance);
                    std::cout << "Your penguin flew: " << actual_flight_distance << " metres" << std::endl;
                }
                else {
                    break;
                }
            }
            else {
                std::cout << "cannot fly" << std::endl;
                break;
            }
        }
    }
    return 1;
}

