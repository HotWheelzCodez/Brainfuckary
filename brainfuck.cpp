#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::pair<std::string, std::string> run_program(std::vector<uint8_t>& cells, const std::vector<std::string>& lines) noexcept
{
  std::pair<std::string, std::string> output; // Used to store the error (if there is one) and the output of the program
  std::vector<std::pair<int, int>> loop_positions; // Stores the position of the '[' in the file (i.e row, col)
  int current_cell = 0;
  bool skip = false;

  for (int i = 0; i < lines.size(); i++)
  {
    for (int j = 0; j < lines[i].length(); j++)
    {
      const char& command = lines[i][j];

      if (command == '#') // Check for comment
      {
        if (i == lines.size()-1)
          return output;

        break;
      }

      if (skip) // If we have broke the inner most for loop
      {
        if (command == ']')
        {
          skip = false;
          loop_positions.pop_back();
        }
        continue;
      }

      switch (command)
      {
        case '>': // Incrementing cell pointer
          current_cell++;
          if (current_cell > cells.size()-1)
          {
            output.first = "BRAINFUCK-ERROR: Cell pointer out of bounds, too high! (Overflow)";
            return output;
          }
          break;
        case '<': // Decrementing cell pointer
          current_cell--;
          if (current_cell < 0)
          {
            output.first = "BRAINFUCK-ERROR: Cell pointer out of bounds, too low! (Underflow)";
            return output;
          }
          break;
        case '+': // Incrementing cell value
          if ((int)cells[current_cell]+1 > 255)
          {
            output.first = "BRAINFUCK-ERROR: Cell value is too large! (Overflow)";
            return output;
          }

          cells[current_cell]++;
          break;
        case '-': // Decrementing cell value
          if ((int)cells[current_cell]-1 < 0)
          {
            output.first = "BRAINFUCK-ERROR: Cell value is too low! (Underflow)";
            return output;
          }

          cells[current_cell]--;
          break;
        case '.': // Printing cell value
          if (lines[i][j-1] == '\\') // Checking format options
            output.first += std::to_string((int)cells[current_cell]);
          else
            output.second += (char)cells[current_cell];
          break;
        case ',': // Getting input
        {
          std::string input;
          std::cin >> input;
  
          int value;
          // Ensure it is a valid unsigned 8 bit integer
          try
          {
            value = std::stoi(input);

            if (value < 0)
            {
              output.first = "BRAINFUCK-ERROR: Input can not be lower than 0! (Underflow)";
              return output;
            }
            if (value > 255)
            {
              output.first = "BRAINFUCK-ERROR: Input can not be greater than 255! (Overflow)";
              return output;
            }

            cells[current_cell] = (uint8_t)value;
          }
          catch (const std::exception& error)
          {
            output.first = "BRAINFUCK-ERROR: Input must be a valid unsigned 8 bit integer (0-255)! (Unknown value)";
            return output;
          }

          break;
        }
        case '[': // Begining for loop
          if (!cells[current_cell])
            skip = true;

          if (loop_positions.empty())
            loop_positions.push_back({ i , j });
          else if (loop_positions[loop_positions.size()-1].first != i || loop_positions[loop_positions.size()-1].second != j)
            loop_positions.push_back({ i, j });
          break; 
        case ']': // Ending for loop
          if (cells[current_cell])
          {
            if (loop_positions.empty())
            {
              output.first = "BRAINFUCK-ERROR: No matching brackets found! (Syntax error)";
              return output;
            }
            
            i = loop_positions[loop_positions.size()-1].first;
            j = loop_positions[loop_positions.size()-1].second;
            continue;
          }

          loop_positions.pop_back();
          break;
      }
    }
  }

  if (skip)
    output.first = "BRAINFUCK-ERROR: No matching brackets found! (Syntax error)";

  return output;
}

int main(int argc, char** argv)
{
  if (argc == 1)
  {
    std::cerr << "BRAINFUCK-ERROR: No file provided! Provided a single '.bf' file!\n";
    return 1;
  }

  const std::string file_path(argv[1]);
  std::ifstream file(file_path);

  if (file.bad() || !file.is_open())
  {
    std::cerr << "BRAINFUCK-ERROR: File does not exist or could not be found/opened!\n";
    return 1;
  }

  size_t num_of_cells = 10;
  bool display_message = true;

  if (argc >= 3)
  {
    std::string param(argv[2]);
    if (param == "--message-off")
      display_message = false;
    else
      num_of_cells = std::stoll(param.substr(param.find("=")+1, param.length()+1));
  }
  if (argc >= 4)
  {
    std::string param(argv[3]);
    if (param == "--message-off")
      display_message = false;
    else
      num_of_cells = std::stoll(param.substr(param.find("=")+1, param.length()+1));
  }

  if (display_message)
    std::cout << "BRAINFUCK-INFO: You have 10 bytes/cells, if you need more then use the --cells flag, example: --cells=100, if you don't want this message use --message-off\n";

  std::vector<std::string> lines;
  std::string line;
  while (std::getline(file, line)) { lines.push_back(line); }

  std::vector<uint8_t> cells(num_of_cells);
  const std::pair<std::string, std::string> output = run_program(cells, lines);

  if (output.first == "")
    std::cout << output.second;
  else
    std::cout << output.first;
}
