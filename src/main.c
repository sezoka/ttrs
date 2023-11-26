#include <stdio.h>

#include <raylib.h>

#define DEFAULT_SCR_W 800
#define DEFAULT_SCR_H 600

#define TETRIS_FIELD_W 10
#define TETRIS_FIELD_H 40

#define TETRIS_FIELD_LEN 400

#define SHAPE_W 4
#define SHAPE_H 4
#define SHAPE_LEN 16

typedef enum {
  Clr_Black,
  Clr_Red,
  Clr_Green,
  Clr_Blue,
  Clr_Orange,
  Clr_LightBlue,
  Clr_Yellow,
  Clr_Violet,
} Clr;

typedef enum {
  Shape_I,
  Shape_J,
  Shape_L,
  Shape_O,
  Shape_S,
  Shape_T,
  Shape_Z,
} Shape;

Clr get_clr_of_shape(Shape sh) {
  switch (sh) {
  case Shape_I:
    return Clr_LightBlue;
  case Shape_J:
    return Clr_Blue;
  case Shape_L:
    return Clr_Orange;
  case Shape_O:
    return Clr_Yellow;
  case Shape_S:
    return Clr_Green;
  case Shape_T:
    return Clr_Violet;
  case Shape_Z:
    return Clr_Red;
  }
}

Color clr_to_raylib_color(Clr clr) {
  switch (clr) {
  case Clr_Black:
    return (Color){0, 0, 0, 255};
  case Clr_Red:
    return (Color){200, 0, 0, 255};
  case Clr_Green:
    return (Color){0, 200, 0, 255};
  case Clr_Blue:
    return (Color){0, 0, 200, 255};
  case Clr_Orange:
    return (Color){200, 100, 0, 255};
  case Clr_LightBlue:
    return (Color){50, 50, 200, 255};
  case Clr_Yellow:
    return (Color){200, 200, 0, 255};
  case Clr_Violet:
    return (Color){100, 0, 200, 255};
  }
}

typedef struct {
  Clr field[TETRIS_FIELD_LEN];
  Clr shape[SHAPE_LEN];
  int shape_x;
  int shape_y;

} State;

void draw_field(State *s) {
  int block_size = DEFAULT_SCR_W / TETRIS_FIELD_W;
  for (int y = 0; y < TETRIS_FIELD_H; y += 1) {
    for (int x = 0; x < TETRIS_FIELD_W; x += 1) {
      Color color = clr_to_raylib_color(s->field[y * TETRIS_FIELD_W + x]);
      DrawRectangle(x * block_size, y * block_size, block_size, block_size,
                    color);
      DrawRectangleLines(x * block_size, y * block_size, block_size, block_size,
                         GRAY);
    }
  }

  for (int y = 0; y < SHAPE_H; y += 1) {
    for (int x = 0; x < SHAPE_W; x += 1) {
      Color color = clr_to_raylib_color(s->field[y * SHAPE_W + x]);
      DrawRectangle(x * block_size, y * block_size, block_size, block_size,
                    color);
      DrawRectangleLines(x * block_size, y * block_size, block_size, block_size,
                         GRAY);
    }
  }
}

void init_state(State *s) {
  for (int i = 0; i < TETRIS_FIELD_LEN; i += 1)
    s->field[i] = Clr_Black;
  for (int i = 0; i < SHAPE_LEN; i += 1)
    s->shape[i] = Clr_Black;
  s->shape_x = 0;
  s->shape_y = 0;
}

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);

  InitWindow(DEFAULT_SCR_W, DEFAULT_SCR_H, "Tetris");
  SetTargetFPS(60);

  State state;
  init_state(&state);

  while (!WindowShouldClose()) {
    BeginDrawing();
    draw_field(&state);
    ClearBackground(RAYWHITE);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
