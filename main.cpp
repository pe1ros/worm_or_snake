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

class Game {
 public:
  Game(int size_rows_rows) : size_rows_(size_rows_rows) {
    score_ = 0;
    InitializeGame();
  }

  ~Game() {
    while (head_ != nullptr) {
      SnakePart* temp = head_;
      head_ = head_->next;
      delete temp;
    }
  }

  void Run() {
    while (true) {
      DrawGame();
      MoveSnake();

      if (CheckCollision()) GameOver();

      std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
  }

 private:
  SnakePart* head_;
  SnakePart* tail_;
  SnakePart* food_;
  int score_;
  int size_rows_;

  void InitializeGame() {
    head_ = new SnakePart;
    head_->x_pos_ = size_rows_ / 2;
    head_->y_pos_ = size_rows_ / 2;
    head_->next = nullptr;
    head_->prev = nullptr;
    tail_ = head_;
    GenerateFood();
  }

  void DrawGame() {
    system("clear");
    for (int i = 0; i < size_rows_; i++) {
      for (int j = 0; j < size_rows_; j++) {
        if (i == head_->y_pos_ && j == head_->x_pos_)
          std::cout << "@";
        else if (i == food_->y_pos_ && j == food_->x_pos_)
          std::cout << "*";
        else {
          SnakePart* current = head_->next;
          bool isBodyPart = false;
          while (current != nullptr) {
            if (i == current->y_pos_ && j == current->x_pos_) {
              std::cout << "o";
              isBodyPart = true;
            }
            current = current->next;
          }

          if (!isBodyPart) std::cout << "_";
        }
      }
      std::cout << std::endl;
    }
    std::cout << "Score: " << score_ << std::endl;
  }

  void MoveSnake() {
    int new_x = head_->x_pos_;
    int new_y = head_->y_pos_;

    char key = keyPressed.load();
    switch (key) {
      case 'w':
        new_y <= 0 ? new_y = size_rows_ - 1 : --new_y;
        break;
      case 's':
        new_y >= size_rows_ - 1 ? new_y = 0 : ++new_y;
        break;
      case 'a':
        new_x <= 0 ? new_x = size_rows_ - 1 : --new_x;
        break;
      case 'd':
        new_x >= size_rows_ - 1 ? new_x = 0 : ++new_x;
        break;
    }

    SnakePart* new_head = new SnakePart;
    new_head->x_pos_ = new_x;
    new_head->y_pos_ = new_y;
    new_head->next = head_;
    head_->prev = new_head;
    head_ = new_head;

    if (new_x == food_->x_pos_ && new_y == food_->y_pos_) {
      score_++;
      GenerateFood();
    } else {
      SnakePart* temp = tail_;
      tail_ = tail_->prev;
      delete temp;
      tail_->next = nullptr;
    }
  }

  void GenerateFood() {
    int food_x, food_y;
    do {
      food_x = rand() % size_rows_;
      food_y = rand() % size_rows_;
    } while (head_->x_pos_ == food_x && head_->y_pos_ == food_y);

    food_ = new SnakePart;
    food_->x_pos_ = food_x;
    food_->y_pos_ = food_y;
  }

  bool CheckCollision() {
    int head_x = head_->x_pos_;
    int head_y = head_->y_pos_;

    SnakePart* current = head_->next;
    while (current != nullptr) {
      if (head_x == current->x_pos_ && head_y == current->y_pos_) return true;
      current = current->next;
    }

    return false;
  }

  void GameOver() {
    system("clear");
    std::cout << "Game Over!" << std::endl;
    std::cout << "Your score: " << score_ << std::endl;
    exit(0);
  }
};

int main() {
  srand(static_cast<unsigned>(time(nullptr)));
  std::thread listener(keyListener);

  Game game(10);
  game.Run();

  return 0;
}