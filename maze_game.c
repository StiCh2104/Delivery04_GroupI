/*******************************************************************************************
*
*   raylib maze game
*
*   Procedural maze generator using Maze Grid Algorithm
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2024-2025 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#include <stdlib.h>     // Required for: malloc(), free()

#define MAZE_WIDTH          64
#define MAZE_HEIGHT         64
#define MAZE_SCALE          10.0f

#define MAX_MAZE_ITEMS      16

// Declare new data type: Point
typedef struct Point {
    int x;
    int y;
} Point;

// Generate procedural maze image, using grid-based algorithm
// NOTE: Functions defined as static are internal to the module
Texture tex_maze;
Image GenImageMaze(int width, int height, int spacing_rows, int spacing_cols, float point_chance);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    const int screen_width = 1280;
    const int screen_height = 720;
    int score = 0;
    bool player_won = false;

    InitWindow(screen_width, screen_height, "Delivery04 - maze game");

    // Current application mode
    int current_mode = 1;    // 0-Game, 1-Editor

    // Random seed defines the random numbers generation,
    // always the same if using the same seed
    int seed = 37867;
    SetRandomSeed(seed);

    // Generate maze image using the grid-based generator
    // TODO: [1p] Implement GenImageMaze() function with required parameters
    Image im_maze = GenImageMaze(MAZE_WIDTH, MAZE_HEIGHT, 4, 4, 0.75f);

    // Load a texture to be drawn on screen from our image data
    // WARNING: If im_maze pixel data is modified, tex_maze needs to be re-loaded
    tex_maze = LoadTextureFromImage(im_maze);

    Color* pixels = LoadImageColors(im_maze);
    UpdateTexture(tex_maze, pixels);
    UnloadImageColors(pixels);

    // Player start-position and end-position initialization
    Point start_cell = { 1, 1 };
    Point end_cell = { im_maze.width - 2, im_maze.height - 2 };

    // Maze drawing position (editor mode)
    Vector2 maze_position;
    maze_position.x = (GetScreenWidth() - tex_maze.width * MAZE_SCALE) / 2;
    maze_position.y = (GetScreenHeight() - tex_maze.height * MAZE_SCALE) / 2;

    // Define player position and size
    Rectangle player = { maze_position.x + 10 * MAZE_SCALE + 2, maze_position.y + 10 * MAZE_SCALE + 2, 4, 4 };

    // Camera 2D for 2d gameplay mode
    // TODO: [2p] Initialize camera parameters as required
    Camera2D camera2d = { 0 };
    camera2d.target = (Vector2){ player.x, player.y };
    camera2d.offset = (Vector2){ screen_width / 2, screen_height / 2 };
    camera2d.rotation = 0.0f;
    camera2d.zoom = 10.0f;

    // Mouse selected cell for maze editing
    Point selected_cell = { 0 };

    // Maze items position and state
    Point maze_items[MAX_MAZE_ITEMS] = { 0 };
    bool maze_item_picked[MAX_MAZE_ITEMS] = { 0 };

    // Define textures to be used as our "biomes"
    // TODO: Load additional textures for different biomes
    Texture2D tex_biomes[4] = 
    {
        LoadTexture("resources/maze_atlas01.png"),
        LoadTexture("resources/maze_atlas02.png"),
        LoadTexture("resources/maze_atlas03.png"),
        LoadTexture("resources/maze_atlas04.png"),
    };
    int current_biome = 0;

    // TODO: Define all variables required for game UI elements (sprites, fonts...)

    SetTargetFPS(60);       // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // Select current mode as desired
        if (IsKeyPressed(KEY_SPACE)) current_mode = !current_mode; // Toggle mode: 0-Game, 1-Editor
        if (IsKeyPressed(KEY_R))
        {
            // Set a new seed and re-generate maze
            seed += 11;
            UnloadImage(im_maze);
            UnloadTexture(tex_maze);
            im_maze = GenImageMaze(MAZE_WIDTH, MAZE_HEIGHT, 4, 4, 0.5f);
            tex_maze = LoadTextureFromImage(im_maze);
        }
        if (current_mode == 0) // Game mode
        {
            // TODO: [2p] Player 2D movement from predefined Start-point to End-point
            // Implement maze 2D player movement logic (cursors || WASD)
            // Use im_maze pixel information to check collisions
            // Detect if current playerCell == end_cell to finish game

            int player_cell_x = (int)((player.x - maze_position.x) / MAZE_SCALE);
            int player_cell_y = (int)((player.y - maze_position.y) / MAZE_SCALE);
            float speed = 2.0f;
            Vector2 move = { 0 };
            if (player_cell_x >= 0 && player_cell_x < im_maze.width && player_cell_y >= 0 && player_cell_y < im_maze.height)
            {

                Color* pixels = LoadImageColors(im_maze);
                if (!player_won) {
                    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
                        int new_cell_y = (int)((player.y - speed - maze_position.y) / MAZE_SCALE);
                        if (!(pixels[new_cell_y * im_maze.width + player_cell_x].r == 255)) move.y = -speed;
                    }

                    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
                        int new_cell_y = (int)((player.y + speed - maze_position.y) / MAZE_SCALE);
                        if (!(pixels[new_cell_y * im_maze.width + player_cell_x].r == 255)) move.y = speed;
                    }

                    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
                        int new_cell_x = (int)((player.x - speed - maze_position.x) / MAZE_SCALE);
                        if (!(pixels[player_cell_y * im_maze.width + new_cell_x].r == 255)) move.x = -speed;
                    }

                    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
                        int new_cell_x = (int)((player.x + speed - maze_position.x) / MAZE_SCALE);
                        if (!(pixels[player_cell_y * im_maze.width + new_cell_x].r == 255)) move.x = speed;
                    }
                }

                player.x += move.x;
                player.y += move.y;

                player_cell_x = (int)((player.x - maze_position.x) / MAZE_SCALE);
                player_cell_y = (int)((player.y - maze_position.y) / MAZE_SCALE);
                if (player_cell_x >= 0 && player_cell_x < im_maze.width && player_cell_y >= 0 && player_cell_y < im_maze.height)
                {
                    Color pixel_color = pixels[player_cell_y * im_maze.width + player_cell_x];
                    if (ColorToInt(pixel_color) == ColorToInt(GREEN)) // Player Wins
                        player_won = true;
                    // TODO: [1p] Camera 2D system following player movement around the map
                    // Update Camera2D parameters as required to follow player and zoom control
                    // TODO: [2p] Maze items pickup logic
                    if (ColorToInt(pixel_color) == ColorToInt(RED)) //Player Picks Item
                    {
                        ImageDrawPixel(&im_maze, player_cell_x, player_cell_y, BLACK);
                        UpdateTexture(tex_maze, im_maze.data);
                        score += 10;
                    }
                }
                UnloadImageColors(pixels);
            }
            camera2d.target = (Vector2){ player.x, player.y };
        }
        else if (current_mode == 1) // Editor mode
        {
            // TODO: [2p] Maze editor mode, edit image pixels with mouse.
            // Implement logic to selecte image cell from mouse position -> TIP: GetMousePosition()
            // NOTE: Mouse position is returned in screen coordinates and it has to 
            // transformed into image coordinates
            // Once the cell is selected, if mouse button pressed add/remove image pixels

            // WARNING: Remember that when im_maze changes, tex_maze must be also updated!

            // TODO: [2p] Collectible map items: player score
            // Using same mechanism than maze editor, implement an items editor, registering
            // points in the map where items should be added for player pickup -> TIP: Use maze_items[]

            Vector2 mouse_pos = GetMousePosition();
            int cell_x = (mouse_pos.x - maze_position.x) / MAZE_SCALE;
            int cell_y = (mouse_pos.y - maze_position.y) / MAZE_SCALE;
            Color* pixels = LoadImageColors(im_maze);

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                ImageDrawPixel(&im_maze, cell_x, cell_y, BLACK); // Cambia un pixel a negro (camino)
            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
                ImageDrawPixel(&im_maze, cell_x, cell_y, WHITE); // Cambia un pixel a blanco (pared)
            if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
                ImageDrawPixel(&im_maze, cell_x, cell_y, RED);   // Añade un ítem
            if (IsKeyDown(KEY_LEFT_CONTROL) && IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
                ImageDrawPixel(&im_maze, cell_x, cell_y, GREEN); // Marca el final del laberinto
            UpdateTexture(tex_maze, im_maze.data);
            UnloadImageColors(pixels);
        }
        
        // TODO: [1p] Multiple maze biomes supported
        // Implement changing between the different textures to be used as biomes
        // NOTE: For the 3d model, the current selected texture must be applied to the model material  
        if(IsKeyPressed(KEY_ONE))
        {
            current_biome = 0;
        } 
        if(IsKeyPressed(KEY_TWO))
        {
            current_biome = 1;
        } 
        if(IsKeyPressed(KEY_THREE))
        {
            current_biome = 2;
        } 
        if(IsKeyPressed(KEY_FOUR))
        {
            current_biome = 3;
        } 
        
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(DARKGRAY);

        if (current_mode == 0) // Game mode
        {
            // Draw maze using camera2d (for automatic positioning and scale)
            BeginMode2D(camera2d);

            // TODO: Draw maze walls and floor using current texture biome 
            for (int y = 0; y < im_maze.height; y++)
            {
                for (int x = 0; x < im_maze.width; x++)
                {
                    if (ColorIsEqual(GetImageColor(im_maze, x, y), WHITE))
                    {
                        DrawTexturePro(tex_biomes[current_biome], (Rectangle){ 0, tex_biomes[current_biome].height/2, tex_biomes[current_biome].width/2, tex_biomes[current_biome].height/2 },
                            (Rectangle){ maze_position.x + x*MAZE_SCALE, maze_position.y + y*MAZE_SCALE, MAZE_SCALE, MAZE_SCALE }, 
                            (Vector2){ 0 }, 0.0f, WHITE);
                    }
                    else
                    {
                        DrawTexturePro(tex_biomes[current_biome], (Rectangle){ tex_biomes[current_biome].width/2, tex_biomes[current_biome].height/2, tex_biomes[current_biome].width/2, tex_biomes[current_biome].height/2 },
                            (Rectangle){ maze_position.x + x*MAZE_SCALE, maze_position.y + y*MAZE_SCALE, MAZE_SCALE, MAZE_SCALE }, 
                            (Vector2){ 0 }, 0.0f, WHITE);
                    }
                }
            }
            // TODO: Draw player rectangle or sprite at player position
            DrawRectangleRec(player, BLUE);
            // TODO: Draw maze items 2d (using sprite texture?)
            for (int y = 0; y < im_maze.height; y++)
            {
                for (int x = 0; x < im_maze.width; x++)
                {
                    if (ColorIsEqual(GetImageColor(im_maze, x, y), RED))
                    {
                        Vector2 item_position = {maze_position.x + x * MAZE_SCALE + MAZE_SCALE / 2, maze_position.y + y * MAZE_SCALE + MAZE_SCALE / 2};
                        DrawCircleV(item_position, MAZE_SCALE / 4, RED);
                    }
                    //Draw the End
                    if (ColorIsEqual(GetImageColor(im_maze, x, y), GREEN))
                    {
                        Vector2 end_position = {maze_position.x + x * MAZE_SCALE + MAZE_SCALE / 2, maze_position.y + y * MAZE_SCALE + MAZE_SCALE / 2};
                        DrawRectangleV(end_position, (Vector2){ MAZE_SCALE / 2, MAZE_SCALE / 2 }, GREEN);
                    }
                }
            }
            
            EndMode2D();
            DrawText(TextFormat("Score: %d", score), screen_width - 190, 20, 30, BLACK);
            if (player_won)
            {
                DrawRectangle(0, 0, screen_width, screen_height, Fade(WHITE, 0.6f));
                DrawText("¡You Won!", screen_width / 2 - 100, screen_height / 2 - 30, 50, GREEN);
            }
            // TODO: Draw game UI (score, time...) using custom sprites/fonts
            // NOTE: Game UI does not receive the camera2d transformations,
            // it is drawn in screen space coordinates directly
        }
        else if (current_mode == 1) // Editor mode
        {
            // Draw generated maze texture, scaled and centered on screen 
            DrawTextureEx(tex_maze, maze_position, 0.0f, MAZE_SCALE, WHITE);

            // Draw lines rectangle over texture, scaled and centered on screen 
            DrawRectangleLines(maze_position.x, maze_position.y, MAZE_WIDTH * MAZE_SCALE, MAZE_HEIGHT * MAZE_SCALE, RED);

            // TODO: Draw player using a rectangle, consider maze screen coordinates!

            // TODO: Draw editor UI required elements
        }
        // Draw required UI info
        DrawText("[R] GENERATE NEW RANDOM SEQUENCE", 10, 36, 10, LIGHTGRAY);
        DrawText("[ESC] QUIT GAME", 10, 56, 10, LIGHTGRAY);
        DrawText(TextFormat("SEED: %i", seed), 10, 76, 10, YELLOW);
        
        //CONTROLS
        DrawText("[AWDS/ARROW KEYS] PLAYER MOVEMENT", 10, GetScreenHeight() - 70, 10, WHITE);
        DrawText("[SPACE] TOGGLE MODE: EDITOR/GAME", 10, GetScreenHeight() - 60, 10, WHITE);
        DrawText("[LEFT CLICK] CREATE PATH ", 10, GetScreenHeight() - 50, 10, WHITE);
        DrawText("[RIGHT CLICK] CREATE WALL ", 10, GetScreenHeight() - 40, 10, WHITE);
        DrawText("[MIDDLE CLICK] ADD ITEM ", 10, GetScreenHeight() - 30, 10, WHITE);
        DrawText("[CTRL + RIGHT CLICK] SET END ", 10, GetScreenHeight() - 20, 10, WHITE);

        DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(tex_maze);     // Unload maze texture from VRAM (GPU)
    UnloadImage(im_maze);        // Unload maze image from RAM (CPU)

    // TODO: Unload all loaded resources

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

// Generate procedural maze image, using grid-based algorithm
// NOTE: Color scheme used: WHITE = Wall, BLACK = Walkable, RED = Item
Image GenImageMaze(int width, int height, int spacing_rows, int spacing_cols, float point_chance)
{
    Image im_maze = { 0 };

    // STEP 1: Generate empty image with borders
    //---------------------------------------------------------------------------------
    // STEP 1.1: Generate image of plain color
    im_maze = GenImageColor(width, height, BLACK);

    // STEP 1.2: Draw image border
    for (int y = 0; y < im_maze.height; y++)
    {
        for (int x = 0; x < im_maze.width; x++)
        {
            if ((x == 0) || (x == (im_maze.width - 1)) ||
                (y == 0) || (y == (im_maze.height - 1)))
            {
                ImageDrawPixel(&im_maze, x, y, WHITE);
            }
        }
    }
    //---------------------------------------------------------------------------------

    // STEP 2: Set some random point in image at specific row-column distances
    //---------------------------------------------------------------------------------
    // STEP 2.1: Define an array of point used for maze generation
    // NOTE A: Static array allocation, memory allocated in STACK (MAX: 1MB)
    // NOTE B: Dynamic array allocation, memory allocated in HEAP (MAX: Available RAM)
    //Point maze_points[64] = { 0 }; // Static array
    Point* maze_points = malloc(256 * sizeof(Point)); // Dynamic array (MAX: 256 points)
    int maze_point_counter = 0;

    // STEP 2.2: Store specific points, at specific row-column distances
    for (int y = 0; y < im_maze.height; y++)
    {
        for (int x = 0; x < im_maze.width; x++)
        {
            if ((x != 0) && (x != (im_maze.width - 1)) &&
                (y != 0) && (y != (im_maze.height - 1)))
            {
                if ((x % spacing_cols == 0) && (y % spacing_rows == 0))
                {
                    if (GetRandomValue(0, 100) <= (int)(point_chance * 100))
                    {
                        maze_points[maze_point_counter] = (Point){ x, y };
                        maze_point_counter++;
                    }
                }
            }
        }
    }

    // STEP 2.3: Draw our points in image
    for (int i = 0; i < maze_point_counter; i++)
    {
        ImageDrawPixel(&im_maze, maze_points[i].x, maze_points[i].y, WHITE);
    }
    //---------------------------------------------------------------------------------

    // STEP 3: Draw lines from every point in a random direction
    //---------------------------------------------------------------------------------
    // STEP 3.1: Define an array of 4 directions for convenience
    Point directions[4] = {
        { 1, 0 },   // East
        { -1, 0 },  // West
        { 0, 1 },   // South
        { 0, -1 },  // North
    };

    // STEP 3.2: Load a random sequence of points, to be used as indices, so,
    // we can access maze-points randomly indexed, instead of following the order we stored them
    int* point_order = LoadRandomSequence(maze_point_counter, 0, maze_point_counter - 1);

    // STEP 3.3: Process every random maze point, drawing pixels in one random direction,
    // until we collision with another wall (WHITE pixel)
    for (int i = 0; i < maze_point_counter; i++)
    {
        Point current_point = maze_points[point_order[i]];
        Point current_dir = directions[GetRandomValue(0, 3)];
        Point next_point = { current_point.x + current_dir.x, current_point.y + current_dir.y };

        while (ColorIsEqual(GetImageColor(im_maze, next_point.x, next_point.y), BLACK))
        {
            ImageDrawPixel(&im_maze, next_point.x, next_point.y, WHITE);

            next_point.x += current_dir.x;
            next_point.y += current_dir.y;
        }
    }

    // If using dynamic memory allocation for maze_points,
    // they allocated memory must be manually freed 
    free(maze_points);

    // Free point_order array, allocated internally by LoadRandomSequence()
    UnloadRandomSequence(point_order);
    //-----------------------------------------------------------------------------------

    return im_maze;
}