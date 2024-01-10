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

struct Eat {
  int x_pos_;
  int y_pos_;
  bool initialized_;
};

class Game {
 public:
  Game(int size_rows, bool isActive)
      : size_rows_(size_rows), game_is_active_(isActive), scope_(0) {
    snake_head_->x_pos_ = std::rand() % size_rows_;
    snake_head_->y_pos_ = std::rand() % size_rows_;
    snake_head_->next = nullptr;
    snake_head_->prev = nullptr;
  }

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

    std::thread listener(keyListener);
    while (game_is_active_) {
      Move();
      if (!eat_.initialized_) {
        GenerateEat();
      }
      Draw();
      sleep(1);
    }
    listener.join();
  }

  void Move() {
    char value = keyPressed.load();
    // int snake_prev_x = snake_head_->x_pos_;
    // int snake_prev_y = snake_head_->y_pos_;
    switch (value) {
      case 'w':
        snake_head_->x_pos_ <= 0 ? snake_head_->x_pos_ = size_rows_ - 1
                                 : --snake_head_->x_pos_;
        break;
      case 's':
        snake_head_->x_pos_ >= size_rows_ - 1 ? snake_head_->x_pos_ = 0
                                              : ++snake_head_->x_pos_;
        break;
      case 'a':
        snake_head_->y_pos_ <= 0 ? snake_head_->y_pos_ = size_rows_ - 1
                                 : --snake_head_->y_pos_;
        break;
      case 'd':
        snake_head_->y_pos_ >= size_rows_ - 1 ? snake_head_->y_pos_ = 0
                                              : ++snake_head_->y_pos_;
        break;
    }
    if (snake_head_->x_pos_ == eat_.x_pos_ &&
        snake_head_->y_pos_ == eat_.y_pos_) {
      // snake_head_->next = new SnakePart;
      // snake_head_->next->x_pos_ = eat_.x_pos_;
      // snake_head_->next->y_pos_ = eat_.y_pos_;
      eat_.initialized_ = false;
      ++scope_;
    }
    // if (length_ > 0) {
    //   while (snake_body_ != NULL) {
    //     snake_body_->next->x_pos_ = snake_prev_x;
    //     snake_body_->next->y_pos_ = snake_prev_y;
    //     snake_body_ = snake_body_->next;
    //   }
    // }
  }

  void Draw() {
    system("clear");
    std::cout << "_____scope_____: " << scope_ << "\n";
    for (int i = 0; i < size_rows_; i++) {
      for (int j = 0; j < size_rows_; j++) {
        ptr_polygon_[i][j] = '.';
      }
    }
    ptr_polygon_[eat_.x_pos_][eat_.y_pos_] = '#';
    ptr_polygon_[snake_head_->x_pos_][snake_head_->y_pos_] = '@';
    // while (snake_head_->next != NULL) {
    //   ptr_polygon_[snake_head_->next->x_pos_][snake_head_->next->y_pos_] =
    //   '*'; snake_head_ = snake_head_->next;
    // }

    for (int i = 0; i < size_rows_; i++) {
      for (int j = 0; j < size_rows_; j++) {
        std::cout << ptr_polygon_[i][j] << " ";
      }
      std::cout << "\n";
    }
  }

  void GenerateEat() {
    do {
      eat_.y_pos_ = std::rand() % size_rows_;
      eat_.x_pos_ = std::rand() % size_rows_;
    } while (snake_head_->x_pos_ == eat_.x_pos_ ||
             snake_head_->y_pos_ == eat_.y_pos_);
    eat_.initialized_ = true;
  }

 private:
  int size_rows_;
  char** ptr_polygon_;
  SnakePart* snake_head_;
  // TODO: подумать почему тут не работает инициализация через указатель!?!?
  Eat eat_;
  bool game_is_active_;
  int scope_;
};

int main() {
  Game game(20, true);
  game.Start();

  return 0;
}