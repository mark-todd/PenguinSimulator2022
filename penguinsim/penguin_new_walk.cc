#include <ncurses.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <random>
#include <cassert>
#include <tuple>

std::string moveStartToEnd(const std::string& str, int x_coord) {
    if (x_coord >= 0) {
        if (x_coord >= str.length()) {
            return str;  // No change needed if x_coord is greater than or equal to string length
        }

        std::string start = str.substr(0, x_coord);
        std::string end = str.substr(x_coord);

        return end + start;
    } else {
        int startIndex = str.length() + x_coord;
        if (startIndex < 0) {
            startIndex = 0;  // Set startIndex to 0 if it goes below 0
        }

        std::string start = str.substr(0, startIndex);
        std::string end = str.substr(startIndex);

        return end + start;
    }
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



class LoreLoader {
    private:
        std::list<std::string> lore;
    public:
        LoreLoader(const std::string lore_file) {
            std::ifstream f;
            f.open(lore_file);
            std::string line;
            for( std::string line; getline( f, line ); )
            {
                lore.push_back(line);
            }
            f.close();
        }
        std::string get_lore() {
            return *RandomUtils::select_randomly(lore.begin(), lore.end()) + "\n" + "\n";
        }
};

class AsciiPenguin {
    private:
        bool facing_left;
        std::string get_head(const bool& is_left) {
            return is_left ? "<· ) " : " ( ·>";
        }
        std::string get_feet(const int& i, const bool& is_left) {
            if (i == 0) {
                return " \" \" ";
            } else if (i == 1) {
                return is_left ? " \"  \"" : "\"  \" ";
            } else if (i == 2) {
                return is_left ? "\"  \" " : " \"  \"";
            } else {
                return is_left ? "  \"\" " : " \"\"  ";
            }
        }
        std::string wrap_line(const std::string& line) {
            return std::string(18, ' ') + line + std::string(18, ' ') + "\n";
        }

    public:
        AsciiPenguin(const bool starts_facing_left) {
            facing_left = starts_facing_left;
        }
        std::string get_penguin(const bool is_left, const int x_position) {
            std::string strout = AsciiPenguin::wrap_line(AsciiPenguin::get_head(is_left));
            strout += AsciiPenguin::wrap_line("/(V)\\");
            strout += AsciiPenguin::wrap_line(AsciiPenguin::get_feet(x_position % 4, is_left));
            return strout;
        }
};

class AsciiFlyingPenguin {
    private:
        bool facing_left;
        std::string get_head(const bool is_left) {
            return is_left ? "<· )___/\\" : "/\\___( ·>";
        }
        std::string get_mid(const bool is_left) {
            return is_left ? " // \\\\   " : "   // \\\\ ";
        }

        std::string wrap_line(const std::string& line) {
            return std::string(18, ' ') + line + std::string(18, ' ') + "\n";
        }

    public:
        AsciiFlyingPenguin(const bool starts_facing_left) {
            facing_left = starts_facing_left;
        }
        std::string get_penguin(const bool is_left, const int x_position) {
            std::string strout = AsciiFlyingPenguin::wrap_line(AsciiFlyingPenguin::get_head(is_left));
            strout += AsciiFlyingPenguin::wrap_line(AsciiFlyingPenguin::get_mid(is_left));
            return strout;
        }

};

class BoardMaker {
    private:
        std::vector<std::string> lines;
        AsciiPenguin penguin = AsciiPenguin(true);
        AsciiFlyingPenguin flying_penguin = AsciiFlyingPenguin(true);
        const int offset = 20;
        const int width = 40;
        std::string get_lake_line(const int line_no, const int rel_to_penguin)
        {
            const std::string str = BoardMaker::lines[line_no];
            const int lake_start = rel_to_penguin + offset;
            if (lake_start > width || lake_start < -width) {
                return "";
            } else if (lake_start > 0) {
                return std::string(lake_start, ' ') + str.substr(0, width - lake_start) + "\n";
            } else if (lake_start == 0) {
                return str + "\n";
            } else {
                return str.substr(-lake_start) + "\n";
            }
        }
        std::string get_seal_line(const int line_no, const int rel_to_penguin)
        {
            const std::string str = BoardMaker::lines[line_no];

            const int seal_start = rel_to_penguin + offset -7 ;
            if (seal_start > width || seal_start < -width) {
                return "\n";
            } else if (seal_start > 0) {
                return std::string(seal_start, ' ') + str.substr(0, width - seal_start) + "\n";
            } else if (seal_start == 0) {
                return str + "\n";
            } else {
                return str.substr(-seal_start) + "\n";
            }
        }
        std::string get_line(const int line_no, const int x_offset) {
            return moveStartToEnd(BoardMaker::lines[line_no], x_offset);
        }

        std::string get_trees(const int x_position, const int slowness) {
            std::string new_str = "";
            for (int line_no=0; line_no < 11; ++line_no) {
                new_str += BoardMaker::get_line(line_no, (x_position/slowness) %width) + "\n";
            }
            return new_str;
        }

        std::string get_grass(const int x_position, const int slowness) {
            return BoardMaker::get_line(12, (x_position/slowness) %width);
        }

    public:
        BoardMaker(const std::string filename) {
            std::ifstream background_file;
            background_file.open(filename);
            std::string line;
            for( std::string line; getline( background_file, line ); )
            {
                lines.push_back(line);
            }
            background_file.close();

        }

        std::string get_trees_and_grass(const int x_position) {
            std::string str = BoardMaker::get_trees(x_position, 10);
            str +=  BoardMaker::get_grass(x_position, 5) + "\n";
            str +=  BoardMaker::get_grass(x_position, 4) + "\n";
            str +=  BoardMaker::get_grass(x_position, 3) + "\n";
            str +=  BoardMaker::get_grass(x_position, 2) + "\n";
            return str;
        }

        std::string get_lake(const int x_lake) {
            std::string new_str = "";
            for (int line_no=16; line_no < 22; ++line_no) {
                new_str += BoardMaker::get_lake_line(line_no, x_lake);
            }
            return new_str;
        }

        std::string get_penguin(const bool is_left, const int x_position) {
            return penguin.get_penguin(is_left, x_position);
        }
        std::string get_flying_penguin(const bool is_left, const int x_position) {
            return flying_penguin.get_penguin(is_left, x_position);
        }
        std::string get_seal(const int x_seal) {
            std::string new_str = "";
            for (int line_no=23; line_no < 27; ++line_no) {
                new_str += BoardMaker::get_seal_line(line_no, x_seal/3);
            }
            return new_str;
        }
};

std::string display_intro(const std::string intro_file) {
    std::ifstream filestream;
    filestream.open(intro_file);
    std::string all_lines = "";
    std::string line;
    while ( std::getline(filestream, line) ) {
        all_lines += line;
        all_lines += "\n";
    }
    return all_lines;
}


std::string addLineBreaks(const std::string& inputString, int maxLineLength) {
    std::string outputString;
    std::string currentWord;
    std::string currentLine;
    for (char c : inputString) {
        if (c == ' ') {
            // Check if adding the current word would exceed the maximum line length
            if (currentLine.length() + currentWord.length() + 1 > maxLineLength) {
                // Add line break and current word to the output string
                outputString += ' ';
                outputString += currentWord;
                outputString += '\n';
                currentLine = "";
            } else {
                // Add current word to the output string
                outputString += ' ';
                outputString += currentWord;
                currentLine += currentWord;
            }

            // Reset current word
            currentWord = "";
        } else {
            // Append character to current word
            currentWord += c;
        }
    }

    // Add the last word to the output string
    if (currentLine.length() + currentWord.length() > maxLineLength) {
        outputString += '\n';
    }
    outputString += ' ';
    outputString += currentWord;

    return outputString;
};


std::vector<std::tuple<int, int>> get_lake_info(int n_lakes) {
    std::random_device rd;
    std::mt19937 gen(rd());

    double coord_mean = 100.0;
    std::poisson_distribution<int> coord_d(coord_mean);

    double fish_mean = 2.0;
    std::poisson_distribution<int> fish_d(fish_mean);

    std::vector<std::tuple<int, int>> lake_info;
    int prev_coord = coord_d(gen);
    lake_info.push_back(std::make_tuple(prev_coord, fish_d(gen)));
    for (int i = 0; i < n_lakes; i++) {
        int coord_sample = coord_d(gen);
        prev_coord = prev_coord + coord_sample + 60;
        lake_info.push_back(std::make_tuple(prev_coord, fish_d(gen)));
    }
    return lake_info;
}

std::vector<std::string> split_string(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end = str.find(delimiter);
    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }
    tokens.push_back(str.substr(start));
    return tokens;
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
                    std::vector<std::string> line_list;
                    FishingRanges associated_range;
                    line_list = split_string(line, ',');
                    const std::string range = line_list.at(0);
                    const std::string fish_name = line_list.at(1);
                    const std::string fish_cals = line_list.at(2);

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
                printw("Score: %f\n", score);
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

Fishing::Fish catch_fish (Fishing::FishCatcher & fish_catcher) {
    std::string range_choice;
    Fishing::Fish fish("Error", Fishing::short_range, 0);
    while (true) {
        // Create a buffer to store the user input
        char buffer[256];

        // Prompt the user for input
        printw("What range would you like to search \nfor fish at? (short/med/long): ");
        echo();
        // Read the string from the user
        getstr(buffer);
        std::string range_choice = buffer;
        noecho();
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
            printw("Invalid choice\n");
        }
    }
    assert (fish.name!="Error");
    return fish;
}


int main()
{
    Fishing::FishCatcher catcher("fish.csv");
    // Initialize ncurses
    initscr();
    cbreak();              // Line buffering disabled
    noecho();              // Don't display keypresses
    keypad(stdscr, TRUE);  // Enable function keys
    std::vector<std::tuple<int, int>> lake_info = get_lake_info(10);
    // Print instructions
    printw(display_intro("new_intro_text.txt").c_str());
    LoreLoader lore = LoreLoader("lore.txt");
    
    printw(addLineBreaks(lore.get_lore(), 55).c_str());
    printw("Press any key to begin. Press 'q' to quit.\n");
    refresh();
    BoardMaker board = BoardMaker("background.txt");

    // Process keypresses
    int character;
    int penguin_x = 0;
    double flying_calories = 0;
    bool fly_mode = false;
    int seal_x = -150;
    bool game_over = false;
    while ((character = getch()) != 'q')
    {
        clear();
        if (seal_x > penguin_x) {
            printw("GAME OVER\n");
            refresh();
            continue;
        }

        int asciiCode = static_cast<int>(character);
        bool is_left;
        if (asciiCode == 260) {
            // Left key
            if (fly_mode) {
                penguin_x = penguin_x - 3;
                flying_calories -= 4;
            } else {
                penguin_x = penguin_x - 1;
            }
            
            is_left = true;
        } else if (asciiCode == 261) {
            // Right key
            if (fly_mode) {
                penguin_x = penguin_x + 3;
                flying_calories -= 4;
            } else {
                penguin_x = penguin_x + 1;
            }
            is_left = false;
        }
        if (flying_calories <=0) {
            fly_mode = false;
        }
        seal_x += 2;
        printw("Distance from start: %d\n", penguin_x);
        printw("Flying calories: %f\n", flying_calories);
        printw("Seal Distance: %d\n", penguin_x - seal_x);
        std::string new_str = board.get_trees_and_grass(penguin_x);
        new_str += board.get_seal(seal_x-penguin_x);
        new_str += "\n";
        if (fly_mode) {
            new_str += board.get_flying_penguin(is_left, penguin_x);
        } else {
            new_str += board.get_penguin(is_left, penguin_x);
        }
        

        bool lake_aligned = false;
        int index = 0;
        int lake_i;
        for (const std::tuple<int, int>& element : lake_info) {
            int coord = std::get<0>(element);
            if ( penguin_x < (coord + 10) || penguin_x > (coord -10)) {
                if (penguin_x >= coord && penguin_x <= coord +7) {
                    lake_aligned = true;
                    lake_i = index;
                }
                new_str += board.get_lake(coord-penguin_x);
            }
            index++;
        }
        printw(new_str.c_str());
        if (lake_aligned) {
            int& n_fish = std::get<1>(lake_info.at(lake_i));
            
            if (n_fish > 0) {
                printw("Press space to fish!\n");
                if (asciiCode == 32) {
                    Fishing::Fish fish = catch_fish(catcher);
                    printw("You caught: %s\n", fish.name.c_str());
                    flying_calories += fish.calorific_value;
                    if (fish.calorific_value > 0) {
                        n_fish -= 1;
                    }
                }
            }
            printw("Fish remaining: %d\n", n_fish);
        }
        if (flying_calories > 0) {
            if (fly_mode) {
                printw("Press f to stop flying!\n");
                
                if (asciiCode == 102) {
                    fly_mode = false;
                    printw("Fly mode disengaged!\n");
                }
            } else {
                printw("Press f to fly!\n");
                
                if (asciiCode == 102) {
                    fly_mode = true;
                    printw("Fly mode engaged!\n");
                }
            }

        }
        refresh();
    }

    // Clean up ncurses
    endwin();

    return 0;
}
