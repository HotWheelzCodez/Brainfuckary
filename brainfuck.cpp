#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::pair<std::string, std::string> run_program(std::vector<uint8_t>& cells, const std::vector<std::string>& lines) noexcept
{
  std::pair<std::string, std::string> output;
  std::vector<std::pair<int, int>> loop_positions; 
  int current_cell = 0;
  bool skip = false;

  for (int i = 0; i < lines.size(); i++)
  {
    for (int j = 0; j < lines[i].length(); j++)
    {
      const char& command = lines[i][j];

      if (command == '#')
      {
        if (i == lines.size()-1)
          return output;

        break;
      }

      if (skip) 
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
        case '>':
          current_cell++;
          if (current_cell > cells.size()-1)
          {
            output.first = "POINTER OUT OF BOUNDS";
            return output;
          }
          break;
        case '<':
          current_cell--;
          if (current_cell < 0)
          {
            output.first = "POINTER OUT OF BOUNDS";
            return output;
          }
          break;
        case '+':
          if ((int)cells[current_cell]+1 > 255)
          {
            output.first = "INCORRECT VALUE";
            return output;
          }

          cells[current_cell]++;
          break;
        case '-':
          if ((int)cells[current_cell]-1 < 0)
          {
            output.first = "INCORRECT VALUE";
            return output;
          }

          cells[current_cell]--;
          break;
        case '.':
          if (lines[i][j-1] == '\\')
            output.first += std::to_string((int)cells[current_cell]);
          else
            output.second += (char)cells[current_cell];
          break;
        case ',':
        {
          std::string input;
          std::cin >> input;
  
          int value;
          try
          {
            value = std::stoi(input);

            if (value < 0)
            {
              output.first = "INCORRECT VALUE";
              return output;
            }
            if (value > 255)
            {
              output.first = "INCORRECT VALUE";
              return output;
            }

            cells[current_cell] = (uint8_t)value;
          }
          catch (const std::exception& error)
          {
            output.first = "INCORRECT VALUE";
            return output;
          }

          break;
        }
        case '[':
          if (!cells[current_cell])
            skip = true;

          if (loop_positions.empty())
            loop_positions.push_back({ i , j });
          else if (loop_positions[loop_positions.size()-1].first != i || loop_positions[loop_positions.size()-1].second != j)
            loop_positions.push_back({ i, j });
          break; 
        case ']':
          if (cells[current_cell])
          {
            if (loop_positions.empty())
            {
              output.first = "SYNTAX ERROR";
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
    output.first = "SYNTAX ERROR";

  return output;
}

int main(int argc, char** argv)
{
  if (argc == 1)
  {
    std::cerr << "No file provided! Provided a single '.bf' file!\n";
    return 1;
  }

  const std::string file_path(argv[1]);
  std::ifstream file(file_path);

  if (file.bad() || !file.is_open())
  {
    std::cerr << "File does not exist or could not be found/opened!\n";
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
    std::cout << "You have 10 bytes/cells, if you need more then use the --cells flag, example: --cells=100, if you don't want this message use --message-off\n";

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
