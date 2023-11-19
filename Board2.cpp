#include <iostream>
#include <sstream>

#include <netinet/in.h> 

#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <jsoncpp/json/json.h>

#define PORT 8888

char board_state[256];
char player_two = 'X';
char player_one = 'O';
bool is_correct_value = false;

int x_dim_1 {0};
int x_dim_2 {0};
int x_count {0};

char board[3][3] {{' ', ' ', ' '},
                  {' ', ' ', ' '},
                  {' ', ' ', ' '}
};

bool game_over = false;

bool is_game_over_pl_one () {
  bool already_counted = false;
  int k = 3 - 1;
  for (int i = 0; i < 3; ++i) {
    if(board[i][0] == player_one && board[i][1] == player_one && board[i][2] == player_one) {
      return true;
    }

    if(board[0][i] == player_one && board[1][i] == player_one && board[2][i] == player_one) {
      return true;
    }
   }
 if(board[0][0] == player_one && board[1][1] == player_one && board[2][2] == player_one) {
      return true;
    }
 if(board[0][2] == player_one && board[1][1] == player_one && board[2][0] == player_one) {
      return true;
    }

  return false;
}

bool is_game_over_pl_two () {
  bool already_counted = false;
  int k = 3 - 1;
  for (int i = 0; i < 3; ++i) {
    if(board[i][0] == player_two && board[i][1] == player_two && board[i][2] == player_two) {
      return true;
    }

    if(board[0][i] == player_two && board[1][i] == player_two && board[2][i] == player_two) {
      return true;
    }
   }
 if(board[0][0] == player_two && board[1][1] == player_two && board[2][2] == player_two) {
      return true;
    }
 if(board[0][2] == player_two && board[1][1] == player_two && board[2][0] == player_two) {
      return true;
    }

  return false;
}

void set_board_value(const std::string& user_in) {
  std::istringstream iss(user_in);
  int x_ordin{};
  iss >> x_ordin;
  iss.clear();
  int y_ordin {};
  iss >> y_ordin;
  iss.clear();
  char val {};
  iss >> val;

  if (x_ordin < 0 || x_ordin >= 3 || y_ordin < 0 || y_ordin >= 3) {
    std::cout << "wrong coordinates, try again" << std:: endl;
    return;
  } 

  if (board[x_ordin][y_ordin] != 32) {
    std::cout << "Already filled, try again" << std:: endl;
    return;
  }

  if (val == 'X') {
    board[x_ordin][y_ordin] = val;
    is_correct_value = true;
  } else {
    std::cout << "wrong value, enter 'X' or 'O'" << std::endl;
  }
}
void print_board() {
  int j = 0;
  std::cout << "+--+---+---+" << std::endl;
  for (int i = 0; i < 3; ++i) {
    for (; j < 3; ++j) {
      std::cout << " " << board[i][j] << " ";
      std::cout << "|";
    }
     std::cout << '\n';
      for (int t = 0; t < 3; ++t) {
        if (t == 0) {
          std::cout << "+--+";
        } else {
        std::cout << "---+";   
        }
      } 
   std::cout << '\n';
   j = 0;
  }
}

std::string serialize_message() {
  Json::Value root;

  for (int i = 0; i < 3; ++i) {
    for(int j = 0; j < 3; ++j) {
       root [i][j] = board[i][j];
    }
  }

  return root.toStyledString();
}

void deserialize_message(const std::string& js_str) {
  Json::Value root;
  Json::Reader reader;

  if(!reader.parse(js_str, root)) {
    return;
  }
  std::string val;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      board [i][j] = root[i][j].asInt();
    }
  }
}
int main() {
  int counter {0};
  std::string user_in{};
    
  int  client_socket;
  struct sockaddr_in server_addr;

  client_socket = socket(AF_INET, SOCK_STREAM, 0);
   
  // initlialize socket
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Connect to the server
  if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    std::cerr << "Error connecting to the server" << std::endl;
    return 1;
  }

  do {
    while (!is_correct_value) {
       std::cout << "Player one: enter coordinates and vaule i.e 0 0 X" << std::endl;
       std::getline(std::cin, user_in);
       set_board_value(user_in);
     }
    std::string js_str = serialize_message();
    send(client_socket, js_str.c_str(), js_str.size(), 0);
    
    is_correct_value = false;
    recv(client_socket, board_state, sizeof(board_state),0);
    std::string recv_str(board_state);
    deserialize_message(recv_str);

    print_board();

   if(is_game_over_pl_one()) {
     std::cout << "Player one wins! " << std::endl;
     close(client_socket);
     break;
   }

    if(is_game_over_pl_two()) {
     std::cout << "Player two wins! " << std::endl;
     close(client_socket);
     break;
   }

    ++counter;
   }
  while(counter != 9);
 close(client_socket);

  print_board();
    return 0;
}
