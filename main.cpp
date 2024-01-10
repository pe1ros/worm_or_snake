// либа не линкуется призодится юзать доп флаг
// g++ -o main main.cpp -lncurses
// #include <curses.h>
#include <unistd.h>

#include <atomic>
#include <cstdlib>
#include <iostream>
#include <thread>

std::atomic<char> keyPressed('_');

void keyListener() {
  // initscr();
  // cbreak();
  // noecho();
  while (true) {
    char ch;
    // char ch = getch();
    std::cin >> ch;
    keyPressed = static_cast<char>(ch);
  }
  // endwin();
}

struct SnakePart {
  int x_pos_;
  int y_pos_;
  SnakePart* prev;
  SnakePart* next;
};

struct Apple {
  int x_pos_;
  int y_pos_;
  bool initialized_;
};

void GenerateEat(Apple& apple, SnakePart& snake, int size_rows) {
  if (!apple.initialized_) {
    apple.x_pos_ = std::rand() % size_rows;
    apple.y_pos_ = std::rand() % size_rows;
    do {
      apple.y_pos_ = std::rand() % size_rows;
      apple.x_pos_ = std::rand() % size_rows;
    } while (snake.x_pos_ == apple.x_pos_ || snake.y_pos_ == apple.y_pos_);
    apple.initialized_ = true;
  }
}

class Game {
 public:
  Game(int size_rows, bool isActive)
      : size_rows_(size_rows), game_is_active_(isActive) {}

  ~Game() {
    for (int i = 0; i < size_rows_; i++) {
      delete[] ptr_polygon_[i];
    }
    delete[] ptr_polygon_;
  }

  void Start() {
    // int size;
    // std::cout << "Set size of rows from 10 to 20" << "\n";
    // std::cin >> size;

    // эта строка для того что бы всегда были разные числа
    std::srand(std::time(nullptr));
    // size_rows_ = size;
    ptr_polygon_ = new char*[size_rows_];
    for (int i = 0; i < size_rows_; i++) {
      ptr_polygon_[i] = new char[size_rows_];
    }

    for (int i = 0; i < size_rows_; i++) {
      for (int j = 0; j < size_rows_; j++) {
        ptr_polygon_[i][j] = '.';
      }
    }

    snake_.x_pos_ = std::rand() % size_rows_;
    snake_.y_pos_ = std::rand() % size_rows_;
    snake_.next = nullptr;
    snake_.prev = nullptr;
    std::thread listener(keyListener);
    while (game_is_active_) {
      GenerateEat(apple_, snake_, size_rows_);
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

    if (snake_.x_pos_ == size_rows_) {
      snake_.x_pos_ = 0;
      return;
    } else if (snake_.x_pos_ < 0) {
      snake_.x_pos_ = size_rows_ - 1;
      return;
    } else if (snake_.y_pos_ == size_rows_) {
      snake_.y_pos_ = 0;
      return;
    } else if (snake_.y_pos_ < 0) {
      snake_.y_pos_ = size_rows_ - 1;
      return;
    }
  }

  void Draw() {
    system("clear");

    for (int i = 0; i < size_rows_; i++) {
      for (int j = 0; j < size_rows_; j++) {
        snake_.x_pos_ == i&& snake_.y_pos_ == j
            ? std::cout << "@"
                        << " "
            : std::cout << ptr_polygon_[i][j] << " ";
      }
      std::cout << "\n";
    }
    ptr_polygon_[apple_.x_pos_][apple_.y_pos_] = '#';
  }

 private:
  int size_rows_;
  char** ptr_polygon_;
  SnakePart snake_;
  Apple apple_;
  bool game_is_active_;
};

int main() {
  Game game(20, true);
  game.Start();

  return 0;
}