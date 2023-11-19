#include <iostream>
#include <sstream>
#include <string.h>
#include <netinet/in.h> 

#include <jsoncpp/json/json.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <unistd.h>
#define PORT 8888


bool is_correct_value = false;
char player_two = 'O';
char player_one = 'X';

bool player_two_wins = false;
int o_dim_1 {0};
int o_dim_2 {0};
int o_count {0};
char board_state[256];

char board[3][3] {{' ', ' ', ' '},
                  {' ', ' ', ' '},
                  {' ', ' ', ' '}
};

bool game_over = false;

bool is_game_over_pl_two() {
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

bool is_game_over_pl_one() {
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

   if (board[x_ordin][y_ordin] != 32 ) {
    std::cout << "Already exist, try again" << std:: endl;
    return;
  } 

  if (val == 'O') {
    board[x_ordin][y_ordin] = val;
    is_correct_value = true;
  } else {
    std::cout << "wrong value, enter 'O'" << std::endl;
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
       root[i][j] = board[i][j];
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

  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      board[i][j] = root[i][j].asInt();
    }
  }
}

int main() {
    int counter {0};
    std::string user_in{};
    
    int server_socket, client_socket;
    struct sockaddr_in  server_addr, client_addr;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    // initlialize socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // bind socket 
    if (bind(server_socket,(struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
       std::cerr << "Error binding server socket" << std::endl;
       close(server_socket); // Close the socket
       return 1;
    }

    // listen for incomming connection
    if (listen(server_socket,1) == -1) {
      std::cerr << "Error listening for connection" << std::endl;
      return 1;
    }

    // Accept client connection 
    socklen_t client_len = sizeof(client_addr);
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);

    if(client_socket == -1) {
      std::cerr << "Error accepting connection" << std::endl;
      return 1;
    }

  do {
    recv(client_socket, board_state, sizeof(board_state), 0);
    std::string recv_str(board_state);
    std::cout << recv_str << std::endl;
    deserialize_message(recv_str);
    print_board();

    while (!is_correct_value) {
      std::cout << "Playe two: enter coordinates and vaule i.e 0 0 O" << std::endl;
      std::getline(std::cin, user_in);
      set_board_value(user_in);
     }

     is_correct_value = false;

     std::string ser_message =  serialize_message();
     
     send(client_socket, ser_message.c_str(), ser_message.size(), 0);
    
    if(is_game_over_pl_two()) {
      std::cout << "Players two wins! " << std::endl;
      close(client_socket);
      break;
    }

    if(is_game_over_pl_one()) {
      std::cout << "Players one wins! " << std::endl;
      close(client_socket);
      break;
    }

      ++counter;
   } while(counter != 9);

  close(server_socket);
  close(client_socket);

  print_board();
    return 0;
}
