#include <stdbool.h>
#include <stdio.h>

#include <raylib.h>
#include <stdlib.h>

#define TETRIS_FIELD_W 10
#define TETRIS_FIELD_H 20

#define DEFAULT_SCR_W (TETRIS_FIELD_W * 48)
#define DEFAULT_SCR_H (TETRIS_FIELD_H * 48)

#define TETRIS_FIELD_LEN 400

#define SHAPE_FIELD_W 4
#define SHAPE_FIELD_H 4
#define SHAPE_FIELD_LEN 16

#define SHAPE_FALL_DELAY 0.3

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
  Shape_None,
  Shape_I,
  Shape_J,
  Shape_L,
  Shape_O,
  Shape_S,
  Shape_T,
  Shape_Z,
  Shape_Size,
} ShapeKind;

typedef struct {
  Clr field[TETRIS_FIELD_LEN];
  Clr shape[SHAPE_FIELD_LEN];
  int shape_x;
  int shape_y;
  int shape_w;
  float fall_delay;
  float curr_fall_delay;
} State;

typedef enum {
  ShCol_None,
  ShCol_Block,
  ShCol_Border,
} ShapeCollision;

typedef struct {
  int w;
  int h;
} Size;

Clr get_clr_of_shape(ShapeKind kind) {
  switch (kind) {
  case Shape_None:
    return Clr_Black;
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
  case Shape_Size:
    return Clr_Black;
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

void draw_field(State *s) {
  int block_size = DEFAULT_SCR_W / TETRIS_FIELD_W;
  for (int y = 0; y < TETRIS_FIELD_H; y += 1) {
    for (int x = 0; x < TETRIS_FIELD_W; x += 1) {
      Color color = clr_to_raylib_color(s->field[y * TETRIS_FIELD_W + x]);
      DrawRectangle(x * block_size, y * block_size, block_size, block_size,
                    color);
      DrawRectangleLines(x * block_size, y * block_size, block_size, block_size,
                         DARKBROWN);
    }
  }

  for (int y = 0; y < SHAPE_FIELD_H; y += 1) {
    for (int x = 0; x < SHAPE_FIELD_W; x += 1) {
      Clr clr = s->shape[y * SHAPE_FIELD_W + x];
      if (clr != Clr_Black) {
        Color color = clr_to_raylib_color(clr);
        DrawRectangle(((s->shape_x + x) * block_size),
                      ((s->shape_y + y) * block_size), block_size, block_size,
                      color);
      }
    }
  }
}

void clear_shape(State *s) {
  for (int i = 0; i < SHAPE_FIELD_LEN; i += 1) {
    s->shape[i] = Clr_Black;
  }
}

int get_shape_width(ShapeKind kind) {
  switch (kind) {
  case Shape_I:
    return 4;
  case Shape_O:
    return 2;
  case Shape_L:
  case Shape_J:
  case Shape_S:
  case Shape_T:
  case Shape_Z:
    return 3;
  case Shape_Size:
  case Shape_None:
    return 0;
  }
}

void spawn_random_shape(State *s) {
  ShapeKind kind = (ShapeKind)GetRandomValue(1, Shape_Size - 1);

  Clr clr = get_clr_of_shape(kind);

  clear_shape(s);

  switch (kind) {
  case Shape_I:
    s->shape[SHAPE_FIELD_W * 1 + 0] = clr;
    s->shape[SHAPE_FIELD_W * 1 + 1] = clr;
    s->shape[SHAPE_FIELD_W * 1 + 2] = clr;
    s->shape[SHAPE_FIELD_W * 1 + 3] = clr;
    break;
  case Shape_J:
    s->shape[SHAPE_FIELD_W * 1 + 0] = clr;
    s->shape[SHAPE_FIELD_W * 2 + 0] = clr;
    s->shape[SHAPE_FIELD_W * 2 + 1] = clr;
    s->shape[SHAPE_FIELD_W * 2 + 2] = clr;
    break;
  case Shape_L:
    s->shape[SHAPE_FIELD_W * 2 + 0] = clr;
    s->shape[SHAPE_FIELD_W * 2 + 1] = clr;
    s->shape[SHAPE_FIELD_W * 2 + 2] = clr;
    s->shape[SHAPE_FIELD_W * 1 + 2] = clr;
    break;
  case Shape_O:
    s->shape[SHAPE_FIELD_W * 0 + 0] = clr;
    s->shape[SHAPE_FIELD_W * 0 + 1] = clr;
    s->shape[SHAPE_FIELD_W * 1 + 0] = clr;
    s->shape[SHAPE_FIELD_W * 1 + 1] = clr;
    break;
  case Shape_S:
    s->shape[SHAPE_FIELD_W * 0 + 1] = clr;
    s->shape[SHAPE_FIELD_W * 0 + 2] = clr;
    s->shape[SHAPE_FIELD_W * 1 + 0] = clr;
    s->shape[SHAPE_FIELD_W * 1 + 1] = clr;
    break;
  case Shape_T:
    s->shape[SHAPE_FIELD_W * 1 + 1] = clr;
    s->shape[SHAPE_FIELD_W * 2 + 0] = clr;
    s->shape[SHAPE_FIELD_W * 2 + 1] = clr;
    s->shape[SHAPE_FIELD_W * 2 + 2] = clr;
    break;
  case Shape_Z:
    s->shape[SHAPE_FIELD_W * 0 + 0] = clr;
    s->shape[SHAPE_FIELD_W * 0 + 1] = clr;
    s->shape[SHAPE_FIELD_W * 1 + 1] = clr;
    s->shape[SHAPE_FIELD_W * 1 + 2] = clr;
    break;
  case Shape_Size:
  case Shape_None:
    printf("spawn_random_shape switch\n");
  }

  int shape_w = get_shape_width(kind);
  s->shape_x = shape_w / 2;
  s->shape_y = 0;
  s->shape_w = shape_w;
}

void init_state(State *s) {
  for (int i = 0; i < TETRIS_FIELD_LEN; i += 1)
    s->field[i] = Clr_Black;
  clear_shape(s);
  s->shape_x = 0;
  s->shape_y = 0;
  s->shape_w = 0;
  s->fall_delay = SHAPE_FALL_DELAY;
  s->curr_fall_delay = 0.0;
}

ShapeCollision check_shape_collision(State *s) {
  for (int y = 0; y < SHAPE_FIELD_H; y += 1) {
    for (int x = 0; x < SHAPE_FIELD_W; x += 1) {
      if (s->shape[y * SHAPE_FIELD_W + x] != Clr_Black) {
        int field_x = x + s->shape_x;
        int field_y = y + s->shape_y;

        if ((field_x < 0 || TETRIS_FIELD_W <= field_x) ||
            (field_y < 0 || TETRIS_FIELD_H <= field_y)) {
          return ShCol_Border;
        }

        if (s->field[field_y * TETRIS_FIELD_W + field_x] != Clr_Black) {
          return ShCol_Block;
        }
      }
    }
  }

  return ShCol_None;
}

// 1 2 3 4
// 5 6 7 8
// 9 A B C
// D E F J

void rotate_shape(State *s) {
  Clr tmp_shape[SHAPE_FIELD_LEN];

  for (int y = 0; y < s->shape_w; y += 1) {
    for (int x = 0; x < s->shape_w; x += 1) {
      Clr shape_clr = s->shape[y * SHAPE_FIELD_W + x];

      int x1 = SHAPE_FIELD_W - 1 - y;
      int y1 = x;

      tmp_shape[y1 * SHAPE_FIELD_W + x1] = shape_clr;
    }
  }

  for (int y = 0; y < s->shape_w; y += 1) {
    for (int x = 0; x < s->shape_w; x += 1) {
      int offset = y * SHAPE_FIELD_W + x;
      Clr shape_clr = tmp_shape[offset];
      s->shape[offset] = shape_clr;
    }
  }
}

void handle_input(State *s) {
  int prev_x = s->shape_x;
  int prev_y = s->shape_y;

  if (IsKeyDown(KEY_A)) {
    s->shape_x -= 1;
  }
  if (IsKeyDown(KEY_D)) {
    s->shape_x += 1;
  }
  if (IsKeyDown(KEY_S)) {
    s->shape_y += 1;
  }
  if (IsKeyDown(KEY_W)) {
    rotate_shape(s);
  }

  if (check_shape_collision(s) != ShCol_None) {
    s->shape_x = prev_x;
    s->shape_y = prev_y;
  }
}

void bake_shape(State *s) {
  for (int y = 0; y < SHAPE_FIELD_H; y += 1) {
    for (int x = 0; x < SHAPE_FIELD_W; x += 1) {
      Clr shape_clr = s->shape[y * SHAPE_FIELD_W + x];
      if (shape_clr != Clr_Black) {
        int field_x = x + s->shape_x;
        int field_y = y + s->shape_y;
        s->field[field_y * TETRIS_FIELD_W + field_x] = shape_clr;
      }
    }
  }
}

void update(State *s) {
  float delta = GetFrameTime();

  s->curr_fall_delay += delta;

  if (s->fall_delay <= s->curr_fall_delay) {
    s->curr_fall_delay = 0;
    s->shape_y += 1;
    if (check_shape_collision(s) != ShCol_None) {
      s->shape_y -= 1;
      bake_shape(s);
      spawn_random_shape(s);
    }
  }
}

int main() {
  // SetConfigFlags(FLAG_WINDOW_RESIZABLE);

  InitWindow(DEFAULT_SCR_W, DEFAULT_SCR_H, "Tetris");
  SetTargetFPS(60);

  State state;
  init_state(&state);

  spawn_random_shape(&state);

  while (!WindowShouldClose()) {
    handle_input(&state);
    update(&state);

    BeginDrawing();
    ClearBackground(BLACK);
    draw_field(&state);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
