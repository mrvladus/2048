#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

// Field constants
#define FIELD_SIZE 4 // Must be devidedable by 2
#define SIZE 100	 // Size of the tile
#define GAP 10		 // Size of the gap between tiles
#define MOVE_UNIT (SIZE * GAP)
#define SCORE_HEIGHT (SIZE * 2 + GAP * 2)
#define BOARD_HEIGHT (GAP * (FIELD_SIZE + 1) + SIZE * FIELD_SIZE)
#define BOARD_WIDTH (GAP * (FIELD_SIZE + 1) + SIZE * FIELD_SIZE)
#define SCREEN_WIDTH (BOARD_WIDTH + GAP * 2)
#define SCREEN_HEIGHT (SCORE_HEIGHT + BOARD_HEIGHT + GAP)

// ----------------------------------------------
// Colors
// ----------------------------------------------

// Interface colors
#define COLOR_WHITE \
	(Color) { 249, 246, 242, 255 }
#define COLOR_GREY \
	(Color) { 119, 110, 102, 255 }
#define COLOR_BOARD \
	(Color) { 186, 173, 161, 255 }
#define COLOR_CELL \
	(Color) { 204, 193, 181, 245 }
#define COLOR_LOGO \
	(Color) { 237, 194, 46, 255 }

// Tile structure
typedef struct Tile
{
	Vector2 pos, new_pos, old_pos;
	int num, new_num;
	int size;
	// Animation states
	bool merging, merged;
	bool moving;
} Tile;

// Get color for the value
Color GetTileColor(int value)
{
	switch (value)
	{
	case 2:
		return (Color){238, 228, 218, 255};
	case 4:
		return (Color){237, 224, 200, 255};
	case 8:
		return (Color){242, 177, 121, 255};
	case 16:
		return (Color){245, 149, 99, 255};
	case 32:
		return (Color){246, 124, 95, 255};
	case 64:
		return (Color){246, 94, 59, 255};
	case 128:
		return (Color){237, 207, 114, 255};
	case 256:
		return (Color){237, 204, 97, 255};
	case 512:
		return (Color){237, 200, 80, 255};
	case 1024:
		return (Color){237, 197, 63, 255};
	case 2048:
		return (Color){237, 194, 46, 255};
	default:
		return BLACK;
	}
}

// Tiles matrix
Tile tiles[FIELD_SIZE][FIELD_SIZE];

// Initialize tiles matrix
void InitTiles()
{
	for (int i = 0; i < FIELD_SIZE; i++)
		for (int j = 0; j < FIELD_SIZE; j++)
		{
			tiles[i][j] = (Tile){
				.pos.x = j * (SIZE + GAP) + GAP * 2,
				.pos.y = i * (SIZE + GAP) + GAP + SCORE_HEIGHT,
				.num = 0,
				.size = SIZE,
				.moving = false,
			};
		}
}

// ----------------------------------------------
// Render functions
// ----------------------------------------------

void Render()
{
	BeginDrawing();

	// Draw background
	ClearBackground(COLOR_WHITE);

	// Draw logo
	DrawRectangle(GAP, GAP, SIZE * 2, SIZE * 2, COLOR_LOGO);
	DrawText("2048", GAP + SIZE - MeasureText("2048", 60) / 2, GAP + SIZE - 30, 60, COLOR_WHITE);

	// Draw board
	DrawRectangle(GAP, SCORE_HEIGHT, BOARD_WIDTH, BOARD_HEIGHT, COLOR_BOARD);
	int score = 0;
	for (int i = 0; i < FIELD_SIZE; i++)
		for (int j = 0; j < FIELD_SIZE; j++)
		{
			// Draw tile background
			DrawRectangle(j * (SIZE + GAP) + GAP * 2, i * (SIZE + GAP) + GAP + SCORE_HEIGHT, SIZE, SIZE, COLOR_CELL);
			score += tiles[i][j].num;

			// Draw tile
			if (tiles[i][j].num > 0)
			{
				// Draw tile rectangle
				DrawRectangle(tiles[i][j].pos.x, tiles[i][j].pos.y, tiles[i][j].size, tiles[i][j].size, GetTileColor(tiles[i][j].num));

				// Draw tile value
				DrawText(TextFormat("%d", tiles[i][j].num), tiles[i][j].pos.x + tiles[i][j].size / 2 - MeasureText(TextFormat("%d", tiles[i][j].num), 36) / 2, tiles[i][j].pos.y + tiles[i][j].size / 2 - 18, 36, tiles[i][j].num < 8 ? COLOR_GREY : COLOR_WHITE);
			}
		}

	// Draw score
	DrawRectangle(SIZE * 2 + GAP * 4, GAP, SIZE * 2, SIZE, COLOR_BOARD);
	DrawText("SCORE:", SIZE * 3 + GAP * 4 - MeasureText("SCORE:", 30) / 2, GAP * 2, 30, COLOR_WHITE);
	DrawText(TextFormat("%d", score), SIZE * 3 + GAP * 4 - MeasureText(TextFormat("%d", score), 30) / 2, GAP * 6, 30, COLOR_WHITE);

	// Draw restart button
	DrawRectangle(SIZE * 2 + GAP * 4, SIZE + GAP * 3, SIZE * 2, GAP * 5, COLOR_BOARD);
	DrawText("RESTART", SIZE * 3 + GAP * 2 - MeasureText("SCORE:", 30) / 2, SIZE + GAP * 4, 30, COLOR_WHITE);

	// Render to the screen
	EndDrawing();
}

// ----------------------------------------------
// Functions to check states of the tiles
// ----------------------------------------------

// Check if there is merging tiles
bool MergingTiles()
{
	for (int i = 0; i < FIELD_SIZE; i++)
		for (int j = 0; j < FIELD_SIZE; j++)
			if (tiles[i][j].merging)
				return true;
	return false;
}

// Check if there is moving tiles
bool MovingTiles()
{
	for (int i = 0; i < FIELD_SIZE; i++)
		for (int j = 0; j < FIELD_SIZE; j++)
			if (tiles[i][j].moving)
				return true;
	return false;
}

// Check if there is empty tiles with 0 value
bool EmptyTiles()
{
	for (int i = 0; i < FIELD_SIZE; i++)
		for (int j = 0; j < FIELD_SIZE; j++)
			if (tiles[i][j].num == 0)
				return true;
	return false;
}

// ----------------------------------------------
//  Matrix operations
// ----------------------------------------------

// DEBUG: Print matrix to terminal
void PrintMatrix(char *msg)
{
	printf("-------\n");
	for (int i = 0; i < FIELD_SIZE; i++)
	{
		for (int j = 0; j < FIELD_SIZE; j++)
		{
			printf("%d ", tiles[i][j].num);
		}
		printf(" %s\n", msg);
	}
	printf("-------\n");
}

// Add 2 or 4 to a random tile
void AddTwoOrFour()
{
	if (!EmptyTiles())
		return;
	int i = GetRandomValue(0, FIELD_SIZE - 1);
	int j = GetRandomValue(0, FIELD_SIZE - 1);
	if (tiles[i][j].num == 0)
	{
		tiles[i][j].num = GetRandomValue(1, 100) >= 90 ? 4 : 2;
		tiles[i][j].merging = true;
	}
	else
	{
		AddTwoOrFour(); // Call itself until free tile is found
	}
}

// Flip rows
void Flip()
{
	for (int i = 0; i < FIELD_SIZE; i++)
		for (int j = 0; j < FIELD_SIZE / 2; j++)
		{
			Tile tmp = tiles[i][j];
			tiles[i][j] = tiles[i][FIELD_SIZE - j - 1];
			tiles[i][FIELD_SIZE - j - 1] = tmp;
		}
}

// Change is and columns places
void Transpose()
{
	for (int i = 1; i < FIELD_SIZE; i++)
		for (int j = 0; j < i; j++)
		{
			Tile tmp = tiles[i][j];
			tiles[i][j] = tiles[j][i];
			tiles[j][i] = tmp;
		}
}

void Compress()
{
	PrintMatrix("compress");
	for (int i = 0; i < FIELD_SIZE; i++)
	{
		int zeros = 0;
		for (int j = 0; j < FIELD_SIZE; j++)
			if (tiles[i][j].num != 0)
			{
				tiles[i][zeros].num = tiles[i][j].num;
				zeros++;
			}
		while (zeros < FIELD_SIZE)
		{
			tiles[i][zeros].num = 0;
			zeros++;
		}
	}
	PrintMatrix("compress");
}
// Merge and sum same values
void Merge()
{
	Compress();
	for (int i = 0; i < FIELD_SIZE; i++)
		for (int j = 0; j < FIELD_SIZE - 1; j++)
			if (tiles[i][j].num == tiles[i][j + 1].num != 0)
			{
				tiles[i][j].num *= 2;
				tiles[i][j + 1].num = 0;
				tiles[i][j].merging = true;
			}
	Compress();
}

// Check if there is available moves
bool MoveIsAvailable(char *direction)
{
	if (direction == "L")
	{
		for (int i = 0; i < FIELD_SIZE; i++)
			for (int j = FIELD_SIZE - 1; j > 0; j--)
				if (tiles[i][j].num != 0 && (tiles[i][j - 1].num == tiles[i][j].num || tiles[i][j - 1].num == 0))
					return true;
	}
	else if (direction == "R")
	{
		for (int i = 0; i < FIELD_SIZE; i++)
			for (int j = 0; j < FIELD_SIZE - 1; j++)
				if (tiles[i][j].num != 0 && (tiles[i][j + 1].num == tiles[i][j].num || tiles[i][j + 1].num == 0))
					return true;
	}
	else if (direction == "D")
	{
		for (int i = 0; i < FIELD_SIZE - 1; i++)
			for (int j = 0; j < FIELD_SIZE; j++)
				if (tiles[i][j].num != 0 && (tiles[i + 1][j].num == tiles[i][j].num || tiles[i + 1][j].num == 0))
					return true;
	}
	else if (direction == "U")
	{
		for (int i = FIELD_SIZE - 1; i > 0; i--)
			for (int j = 0; j < FIELD_SIZE; j++)
				if (tiles[i][j].num != 0 && (tiles[i - 1][j].num == tiles[i][j].num || tiles[i - 1][j].num == 0))
					return true;
	}
	return false;
}

// ----------------------------------------------
// Animation
// ----------------------------------------------

void MoveAnimation()
{
	while (MovingTiles())
	{
		for (int i = 0; i < FIELD_SIZE; i++)
			for (int j = 0; j < FIELD_SIZE; j++)
			{
				if (tiles[i][j].moving)
					if (!Vector2Equals(tiles[i][j].pos, tiles[i][j].new_pos))
					{
						tiles[i][j].pos = Vector2MoveTowards(tiles[i][j].pos, tiles[i][j].new_pos, 10.0f);
					}
					else
					{
						tiles[i][j].moving = false;
						tiles[i][j].pos = tiles[i][j].old_pos;
						tiles[i][j].num = tiles[i][j].new_num;
					}
			}
		Render();
	}
}

// Merging and appear animation
void MergeAnimation()
{
	while (MergingTiles())
	{
		for (int i = 0; i < FIELD_SIZE; i++)
			for (int j = 0; j < FIELD_SIZE; j++)
			{
				// Begin animation
				if (tiles[i][j].merging && !tiles[i][j].merged && tiles[i][j].size != SIZE + GAP * 2)
				{
					tiles[i][j].pos = Vector2AddValue(tiles[i][j].pos, -2.0f);
					tiles[i][j].size += 5;
				}
				// End animation
				if (tiles[i][j].merging && !tiles[i][j].merged && tiles[i][j].size == SIZE + GAP * 2)
					tiles[i][j].merged = true;
				if (tiles[i][j].merged && tiles[i][j].size > SIZE)
				{
					tiles[i][j].pos = Vector2AddValue(tiles[i][j].pos, 2.0f);
					tiles[i][j].size -= 5;
				}
				// Reset tile state
				if (tiles[i][j].merged && tiles[i][j].size == SIZE)
					tiles[i][j].merged = tiles[i][j].merging = false;
			}
		Render();
	}
}

void Animate()
{
	// MoveAnimation();
	MergeAnimation();
}

// ----------------------------------------------
// Input handling
// ----------------------------------------------

void Input()
{
	if (IsKeyPressed(KEY_LEFT) && MoveIsAvailable("L"))
	{
		Merge();
		AddTwoOrFour();
	}
	if (IsKeyPressed(KEY_RIGHT) && MoveIsAvailable("R"))
	{
		Flip();
		Merge();
		Flip();
		AddTwoOrFour();
	}
	if (IsKeyPressed(KEY_DOWN) && MoveIsAvailable("D"))
	{
		Transpose();
		Flip();
		Merge();
		Flip();
		Transpose();
		AddTwoOrFour();
	}
	if (IsKeyPressed(KEY_UP) && MoveIsAvailable("U"))
	{
		Transpose();
		Merge();
		Transpose();
		AddTwoOrFour();
	}
}

int main()
{
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "2048");
	SetTargetFPS(60);
	InitTiles();
	AddTwoOrFour();
	AddTwoOrFour();
	while (!WindowShouldClose())
	{
		Input();
		Animate();
		Render();
	}

	return 0;
}
