#include <unistd.h>

#include <atomic>
#include <cstdlib>
#include <iostream>
#include <thread>

std::atomic<char> keyPressed('_');

void keyListener() {
  while (true) {
    char key = 'w';
    std::cin >> key;
    keyPressed = static_cast<char>(key);
  }
}

struct Snake {
  int x_pos_;
  int y_pos_;
};

class Game {
 public:
  void Start() {
    // int size;
    // std::cout << "Set size of rows from 10 to 20" << "\n";
    // std::cin >> size;

    // эта строка для того что бы всегда были разные числа
    std::srand(std::time(nullptr));
    // size_rows_ = size;
    place_ = new int*[size_rows_];
    for (int i = 0; i < size_rows_; i++) {
      place_[i] = new int[size_rows_];
    }

    for (int i = 0; i < size_rows_; i++) {
      for (int j = 0; j < size_rows_; j++) {
        place_[i][j] = 0;
      }
    }

    snake_.x_pos_ = std::rand() % 10;
    snake_.y_pos_ = std::rand() % 10;

    std::thread listener(keyListener);
    while (game_is_active_) {
      Move();
      Draw();
    }
    listener.join();
  }

  void Move() {
    sleep(1);
    char value = keyPressed.load();
    switch (value) {
      case 'w':
        --snake_.x_pos_;
        break;
      case 's':
        ++snake_.x_pos_;
        break;
      case 'a':
        --snake_.y_pos_;
        break;
      case 'd':
        ++snake_.y_pos_;
        break;
    }

    if (snake_.x_pos_ == 10) {
      snake_.x_pos_ = 0;
      return;
    } else if (snake_.x_pos_ == -1) {
      snake_.x_pos_ = 9;
      return;
    } else if (snake_.y_pos_ == 10) {
      snake_.y_pos_ = 0;
      return;
    } else if (snake_.y_pos_ == -1) {
      snake_.y_pos_ = 9;
      return;
    }
  }

  void Draw() {
    system("clear");

    for (int i = 0; i < size_rows_; i++) {
      for (int j = 0; j < size_rows_; j++) {
        if (snake_.x_pos_ == i && snake_.y_pos_ == j) {
          std::cout << "@"
                    << " ";
        } else {
          std::cout << "."
                    << " ";
        }
      }
      std::cout << "\n";
    }
  }

 private:
  int size_rows_ = 10;
  int** place_;
  Snake snake_;
  bool game_is_active_ = true;
};

int main() {
  Game game;
  game.Start();

  return 0;
}