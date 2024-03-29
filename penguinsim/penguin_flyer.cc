#include <iostream>
#include <unistd.h>
#include <fstream>
#include <list>
#include <map>
#include <stdexcept>
#include <cassert>
#include <random>
#include <iterator>

class Tracker {
    private:
        std::string delimiter;
        std::ofstream tracker_file;
    public:
        Tracker(const std::string filename, const std::string delimiter = ",") {
            Tracker::delimiter = delimiter;
            tracker_file.open(filename);//, std::ios_base::app);
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
        Tracker & p_tracker;
    public:
        double stored_calories = 0;
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
            return stored_calories >= 300;
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

class Seal {
    public:
        double distance;
        Seal(const double initial_distance) : distance(initial_distance) {}
};

std::list<std::string> split_string (std::string input_string, std::string delimiter) {
    size_t pos = 0;
    std::string token;
    std::list<std::string> output_list;
    while (true) {
        pos = input_string.find(delimiter);
        if (pos != std::string::npos) {
            token = input_string.substr(0, pos);
            output_list.push_back(token);
            input_string.erase(0, pos + delimiter.length());
        }
        else {
            token = input_string.substr(0, pos);
            output_list.push_back(token);
            break;
        }
    }
    return output_list;
}

namespace RandomUtils {
    template<typename Iter, typename RandomGenerator>
    Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
        std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
        std::advance(start, dis(g));
        return start;
    }

    template<typename Iter>
    Iter select_randomly(Iter start, Iter end) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return select_randomly(start, end, gen);
    }
    double random_in_range (const double range_from = 0.0, const double range_to = 1.0) {
        std::random_device rand_dev;
        std::mt19937 generator(rand_dev());
        std::uniform_real_distribution<double> distr(range_from, range_to);
        return distr(generator);
    }
}

namespace Fishing {
    enum FishingRanges {short_range = 0, medium_range = 1, long_range = 2};

    FishingRanges str_to_fish_range(std::string input_string) {
        if (input_string == "short") {
            return short_range;
        }
        else if (input_string == "medium") {
            return medium_range;
        }
        else if (input_string == "long") {
            return long_range;
        }
        else {
            throw std::invalid_argument("invalid string");
        }
    }

    class Fish {
        public:
            std::string name;
            FishingRanges range;
            double calorific_value;

            Fish(
                const std::string name,
                const FishingRanges range,
                const double calorific_value
            ) : name(name), range(range), calorific_value(calorific_value) {}
    };

    class FishCatcher {
        private:
            std::string delimiter;
            std::ifstream fish_file_stream;
            std::list<Fish> sr_fish_catalogue;
            std::list<Fish> mr_fish_catalogue;
            std::list<Fish> lr_fish_catalogue;
            double sr_prob;
            double mr_prob;
            double lr_prob;
        public:
            FishCatcher(
                std::string fish_file,
                const double sr_prob = 0.8,
                const double mr_prob = 0.5,
                const double lr_prob = 0.1
            ) : sr_prob(sr_prob), mr_prob(mr_prob), lr_prob(lr_prob) {
                fish_file_stream.open(fish_file);
                std::string line;
                while ( std::getline(fish_file_stream, line) ) {
                    std::list<std::string> line_list;
                    FishingRanges associated_range;
                    line_list = split_string(line, ",");
                    std::string range;
                    std::string fish_name;
                    std::string fish_cals;
                    if (line_list.size() == 3) {
                        range = line_list.front();
                        line_list.pop_front();
                        fish_name = line_list.front();
                        line_list.pop_front();
                        fish_cals = line_list.front();
                        line_list.pop_front();
                    }
                    else {
                        throw std::invalid_argument("Size not 3"); 
                    }
                    assert(line_list.size() == 0);
                    associated_range = str_to_fish_range(range);
                    Fish new_fish(fish_name, associated_range, std::stod(fish_cals));
                    if (associated_range == short_range) {
                        sr_fish_catalogue.push_back(new_fish);
                    }
                    else if (associated_range == medium_range) {
                        mr_fish_catalogue.push_back(new_fish);
                    }
                    else {
                        lr_fish_catalogue.push_back(new_fish);
                    }
                    
                }
            }
            Fish catch_fish(const FishingRanges range) {
                double score = RandomUtils::random_in_range();
                std::cout << "Score: " << score << std::endl;
                if (range == short_range and score < sr_prob) {
                    return *RandomUtils::select_randomly(sr_fish_catalogue.begin(), sr_fish_catalogue.end());
                }
                else if (range == medium_range and score < mr_prob) {
                    return *RandomUtils::select_randomly(mr_fish_catalogue.begin(), mr_fish_catalogue.end());
                }
                else if (range == long_range and score < lr_prob) {
                    return *RandomUtils::select_randomly(lr_fish_catalogue.begin(), lr_fish_catalogue.end());
                }
                else {
                    Fish null_fish("Nothing!", range, 0);
                    return null_fish;
                }
            };

    };
}

void catch_fish (Penguin & penguin, Fishing::FishCatcher & fish_catcher) {
    std::string range_choice;
    Fishing::Fish fish("Error", Fishing::short_range, 0);
    while (true) {
        std::cout << "What range would you like to search for fish at? (short/med/long)" << std::endl;
        std::cin >> range_choice;
        if (range_choice == "short") {
            fish = fish_catcher.catch_fish(Fishing::short_range);
            break;
        }
        else if (range_choice == "med") {
            fish = fish_catcher.catch_fish(Fishing::medium_range);
            break;
        }
        else if (range_choice == "long") {
            fish = fish_catcher.catch_fish(Fishing::long_range);
            break;
        }
        else {
            std::cout << "Invalid choice" << std::endl;
        }
    }
    assert (fish.name!="Error");
    std::cout << "You caught: " << fish.name << std::endl;
    penguin.give_food(fish.calorific_value, 1);
}

void display_intro(const std::string intro_file) {
    std::ifstream filestream;
    filestream.open(intro_file);
    std::string line;

    while ( std::getline(filestream, line) ) {
        std::cout << line << std::endl;
    }
}

int main()
{
    display_intro("penguinsim/intro_text.txt");
    Tracker penguin_tracker("output.csv");
    Penguin gentoo("Fred", penguin_tracker);
    Seal leo(100);
    double finish_distance = 500;
    Fishing::FishCatcher catcher("fish.csv");

    while (true) {
        std::cout << std::endl;
        std::cout << "Seal Distance: " << leo.distance << std::endl;
        std::cout << "Finish Distance: " << finish_distance << std::endl;
        std::cout << "Stored Calories: " << gentoo.stored_calories << std::endl;

        std::cout << "Your penguin ";
        if (gentoo.can_fly()) {
            std::cout << "can fly" << std::endl;
            std::string flight_choice;
            while (true) {
                std::cout << "Would you like your penguin to fly, or catch fish? (fly/catch)" << std::endl;
                std::cin >> flight_choice;
                if (flight_choice == "fly") {
                    break;
                }
                else if (flight_choice == "catch") {
                    break;
                }
                else {
                    std::cout << "invalid choice" << std::endl;
                    continue;
                }
            }
            if (flight_choice == "fly") {
                double flight_distance;
                double actual_flight_distance;
                std::cout << "How far would you like your penguin to fly?" << std::endl;
                std::cin >> flight_distance;
                actual_flight_distance = gentoo.fly(flight_distance);
                std::cout << "Your penguin flew: " << actual_flight_distance << " metres" << std::endl;
                leo.distance += actual_flight_distance;
                finish_distance -= actual_flight_distance;
            }
            else {
                catch_fish(gentoo, catcher);
            }
        }
        else {
            std::cout << "cannot fly" << std::endl;
            catch_fish(gentoo, catcher);
        }
        leo.distance -= 5 + RandomUtils::random_in_range(0.0, 10.0);;
        if (finish_distance < 0) {
            std::cout << "You made it!" << std::endl;
            break;
        }
        else if (leo.distance < 0) {
            std::cout << "You got eaten!" << std::endl;
            break;
        }
    }
    return 0;
}
