#include <ncurses.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <string>

std::string moveStartToEnd(const std::string& str, int numCharacters)
{
    if (numCharacters >= str.length())
        return str;  // No change needed if numCharacters is greater than or equal to string length

    std::string start = str.substr(0, numCharacters);
    std::string end = str.substr(numCharacters);

    return end + start;
}

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
            std::string n_line = std::string(20, ' ');
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

    public:
        BoardMaker(const std::string filename) {
            std::ifstream background_file;
            background_file.open(filename);//, std::ios_base::app);
            std::string line;
            for( std::string line; getline( background_file, line ); )
            {
                lines.push_back(line);
            }
            background_file.close();
        }
        std::string get_line(const int line_no, const int x_offset) {
            return moveStartToEnd(BoardMaker::lines[line_no], x_offset);
        }
        std::string get_trees(const int x_position, const int slowness) {
            std::string new_str = "";
            for (int line_no=0; line_no < 11; ++line_no) {
                new_str += BoardMaker::get_line(line_no, (x_position/slowness) %40);
                new_str += "\n";
            }
            return new_str;
        }
        std::string get_grass(const int x_position, const int slowness) {
            return BoardMaker::get_line(12, (x_position/slowness) %40);
        }
        std::string get_trees_and_grass(const int x_position) {
            std::string str = BoardMaker::get_trees(x_position, 10);
            str +=  BoardMaker::get_grass(x_position, 5) + "\n";
            str +=  BoardMaker::get_grass(x_position, 4) + "\n";
            str +=  BoardMaker::get_grass(x_position, 3) + "\n";
            str +=  BoardMaker::get_grass(x_position, 2) + "\n";
            return str;
        }

};




int main()
{
    // Initialize ncurses
    initscr();
    cbreak();              // Line buffering disabled
    noecho();              // Don't display keypresses
    keypad(stdscr, TRUE);  // Enable function keys

    // Print instructions
    printw("Press any key to begin. Press 'q' to quit.\n");
    refresh();
    BoardMaker board = BoardMaker("background.txt");
    AsciiPenguin penguin = AsciiPenguin(true);
    printw(board.get_line(3, 1).c_str());
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
        printw("penguin x '%d'", penguin_x);
        printw("offset'%d'", (penguin_x/3) %47);
        if (is_left) {
            printw("here");
        } else {
            printw("there");
        }
        std::string new_str = board.get_trees_and_grass(penguin_x);
        new_str += "\n";
        new_str += penguin.get_penguin(is_left, penguin_x);
        printw(new_str.c_str());
        refresh();
    }

    // Clean up ncurses
    endwin();

    return 0;
}
