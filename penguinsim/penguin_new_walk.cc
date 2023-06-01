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
    printw(board.get_line(3, 1).c_str());
    // Process keypresses
    int character;
    int penguin_x = 0;
    while ((character = getch()) != 'q')
    {
        clear();
        int asciiCode = static_cast<int>(character);
        if (asciiCode == 260) {
            // Left
            penguin_x = penguin_x - 1;
        } else if (asciiCode == 261) {
            // Right
            penguin_x = penguin_x + 1;
        }
        printw("penguin x '%d'", penguin_x);
        printw("offset'%d'", (penguin_x/3) %47);
        std::string new_str = "";
        for (int line_no=0; line_no < 12; ++line_no) {
            new_str += board.get_line(line_no, (penguin_x/3) %40);
            new_str += "\n";
        }
        printw(new_str.c_str());
        refresh();
    }

    // Clean up ncurses
    endwin();

    return 0;
}
