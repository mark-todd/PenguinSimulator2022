#include <ncurses.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <string>
#include <list>
#include <random>

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
        std::string get_head(const bool is_left) {
            if (is_left) {
                return "<· ) ";
            } else {
                return " ( ·>";
            }
        }
        std::string get_feet(const int i, const bool is_left) {
            if (is_left) {
                if (i == 0) {
                    return(" \" \" ");
                } else if (i==1) {
                    return(" \"  \"");
                } else if (i==2) {
                    return("\"  \" ");
                } else {
                    return("  \"\" ");
                }
            } else {
                if (i == 0) {
                    return(" \" \" ");
                } else if (i==1) {
                    return("\"  \" ");
                } else if (i==2) {
                    return(" \"  \"");
                } else {
                    return(" \"\"  ");
                }
            }
        }

        std::string wrap_line(const std::string line) {
            std::string n_line = std::string(18, ' ');
            n_line += line;
            n_line += std::string(18, ' ');
            n_line += "\n";
            return n_line;
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

class BoardMaker {
    private:
        std::vector<std::string> lines;
        AsciiPenguin penguin = AsciiPenguin(true);
        const int offset = 20;
        const int width = 40;
        std::string get_lake_line(const int line_no, const int rel_to_penguin)
        {
            const std::string str = BoardMaker::lines[line_no];
            const int lake_start = rel_to_penguin + offset;
            if (lake_start > width || lake_start < -width) {
                return "";
            } else if (lake_start > 0) {
                return std::string(lake_start, ' ') + str.substr(0, width - lake_start) + '\n';
            } else if (lake_start == 0) {
                return str + '\n';
            } else {
                return str.substr(-lake_start) + '\n';
            }
        }

        std::string get_line(const int line_no, const int x_offset) {
            return moveStartToEnd(BoardMaker::lines[line_no], x_offset);
        }

        std::string get_trees(const int x_position, const int slowness) {
            std::string new_str = "";
            for (int line_no=0; line_no < 11; ++line_no) {
                new_str += BoardMaker::get_line(line_no, (x_position/slowness) %width);
                new_str += "\n";
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


std::vector<int> get_lake_coords(int n_lakes) {
    // Initialize random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Set the mean of the Poisson distribution
    double mean = 100.0;

    // Create the Poisson distribution object
    std::poisson_distribution<int> distribution(mean);
    std::vector<int> coords;
    int prev_coord = distribution(gen);
    coords.push_back(prev_coord);
    // Generate and print some samples
    for (int i = 0; i < n_lakes; i++) {
        int sample = distribution(gen);
        prev_coord = prev_coord + sample + 60;
        coords.push_back(prev_coord);
    }
    return coords;
}

int main()
{
    // Initialize ncurses
    initscr();
    cbreak();              // Line buffering disabled
    noecho();              // Don't display keypresses
    keypad(stdscr, TRUE);  // Enable function keys
    std::vector<int> lake_coords = get_lake_coords(10);
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
    while ((character = getch()) != 'q')
    {
        clear();
        int asciiCode = static_cast<int>(character);
        bool is_left;
        if (asciiCode == 260) {
            // Left key
            penguin_x = penguin_x - 1;
            is_left = true;
        } else if (asciiCode == 261) {
            // Right key
            penguin_x = penguin_x + 1;
            is_left = false;
        }
        printw("Distance from start: %d", penguin_x);
        std::string new_str = board.get_trees_and_grass(penguin_x);
        new_str += "\n";
        new_str += board.get_penguin(is_left, penguin_x);

        bool lake_aligned = false;
        for (const int& element : lake_coords) {
            if ( penguin_x < (element + 10) || penguin_x > (element -10)) {
                if (penguin_x >= element && penguin_x <= element +7) {
                    lake_aligned = true;
                }
                new_str += board.get_lake(element-penguin_x);
            }
        }
        printw(new_str.c_str());
        if (lake_aligned) {
            printw("Press space to fish!\n");
            if (asciiCode == 32) {
                printw("You fished!");
            }
        }
        refresh();
    }

    // Clean up ncurses
    endwin();

    return 0;
}
