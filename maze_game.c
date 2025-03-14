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
Texture texMaze;
Image GenImageMaze(int width, int height, int spacingRows, int spacingCols, float pointChance);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;
    int score = 0;
    bool playerHasWon = false;

    InitWindow(screenWidth, screenHeight, "Delivery04 - maze game");

    // Current application mode
    int currentMode = 1;    // 0-Game, 1-Editor

    // Random seed defines the random numbers generation,
    // always the same if using the same seed
    int seed = 37867;
    SetRandomSeed(seed);

    // Generate maze image using the grid-based generator
    // TODO: [1p] Implement GenImageMaze() function with required parameters
    Image imMaze = GenImageMaze(MAZE_WIDTH, MAZE_HEIGHT, 4, 4, 0.75f);

    // Load a texture to be drawn on screen from our image data
    // WARNING: If imMaze pixel data is modified, texMaze needs to be re-loaded
    texMaze = LoadTextureFromImage(imMaze);

    Color* pixels = LoadImageColors(imMaze);
    UpdateTexture(texMaze, pixels);
    UnloadImageColors(pixels);

    // Player start-position and end-position initialization
    Point startCell = { 1, 1 };
    Point endCell = { imMaze.width - 2, imMaze.height - 2 };

    // Maze drawing position (editor mode)
    Vector2 mazePosition;
    mazePosition.x = (GetScreenWidth() - texMaze.width * MAZE_SCALE) / 2;
    mazePosition.y = (GetScreenHeight() - texMaze.height * MAZE_SCALE) / 2;

    // Define player position and size
    Rectangle player = { mazePosition.x + 10 * MAZE_SCALE + 2, mazePosition.y + 10 * MAZE_SCALE + 2, 4, 4 };

    // Camera 2D for 2d gameplay mode
    // TODO: [2p] Initialize camera parameters as required
    Camera2D camera2d = { 0 };
    camera2d.target = (Vector2){ player.x, player.y };
    camera2d.offset = (Vector2){ screenWidth / 2, screenHeight / 2 };
    camera2d.rotation = 0.0f;
    camera2d.zoom = 10.0f;

    // Mouse selected cell for maze editing
    Point selectedCell = { 0 };

    // Maze items position and state
    Point mazeItems[MAX_MAZE_ITEMS] = { 0 };
    bool mazeItemPicked[MAX_MAZE_ITEMS] = { 0 };

    // Define textures to be used as our "biomes"
    // TODO: Load additional textures for different biomes
    Texture2D texBiomes[4] = 
    {
        LoadTexture("Resources/MazeAtlas01.png"),
        LoadTexture("Resources/MazeAtlas02.png"),
        LoadTexture("Resources/MazeAtlas03.png"),
        LoadTexture("Resources/MazeAtlas04.png"),
    };
    int currentBiome = 0;

    // TODO: Define all variables required for game UI elements (sprites, fonts...)

    SetTargetFPS(60);       // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // Select current mode as desired
        if (IsKeyPressed(KEY_SPACE)) currentMode = !currentMode; // Toggle mode: 0-Game, 1-Editor
        if (IsKeyPressed(KEY_R))
        {
            // Set a new seed and re-generate maze
            seed += 11;
            UnloadImage(imMaze);
            UnloadTexture(texMaze);
            imMaze = GenImageMaze(MAZE_WIDTH, MAZE_HEIGHT, 4, 4, 0.5f);
            texMaze = LoadTextureFromImage(imMaze);
        }
        if (currentMode == 0) // Game mode
        {
            // TODO: [2p] Player 2D movement from predefined Start-point to End-point
            // Implement maze 2D player movement logic (cursors || WASD)
            // Use imMaze pixel information to check collisions
            // Detect if current playerCell == endCell to finish game

            int playerCellX = (int)((player.x - mazePosition.x) / MAZE_SCALE);
            int playerCellY = (int)((player.y - mazePosition.y) / MAZE_SCALE);
            float speed = 2.0f;
            Vector2 move = { 0 };
            if (playerCellX >= 0 && playerCellX < imMaze.width && playerCellY >= 0 && playerCellY < imMaze.height)
            {

                Color* pixels = LoadImageColors(imMaze);
                if (!playerHasWon) {
                    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
                        int newCellY = (int)((player.y - speed - mazePosition.y) / MAZE_SCALE);
                        if (!(pixels[newCellY * imMaze.width + playerCellX].r == 255)) move.y = -speed;
                    }

                    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
                        int newCellY = (int)((player.y + speed - mazePosition.y) / MAZE_SCALE);
                        if (!(pixels[newCellY * imMaze.width + playerCellX].r == 255)) move.y = speed;
                    }

                    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
                        int newCellX = (int)((player.x - speed - mazePosition.x) / MAZE_SCALE);
                        if (!(pixels[playerCellY * imMaze.width + newCellX].r == 255)) move.x = -speed;
                    }

                    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
                        int newCellX = (int)((player.x + speed - mazePosition.x) / MAZE_SCALE);
                        if (!(pixels[playerCellY * imMaze.width + newCellX].r == 255)) move.x = speed;
                    }
                }

                player.x += move.x;
                player.y += move.y;

                playerCellX = (int)((player.x - mazePosition.x) / MAZE_SCALE);
                playerCellY = (int)((player.y - mazePosition.y) / MAZE_SCALE);
                if (playerCellX >= 0 && playerCellX < imMaze.width && playerCellY >= 0 && playerCellY < imMaze.height)
                {
                    Color pixelColor = pixels[playerCellY * imMaze.width + playerCellX];
                    if (ColorToInt(pixelColor) == ColorToInt(GREEN)) // Player Wins
                        playerHasWon = true;
                    // TODO: [1p] Camera 2D system following player movement around the map
                    // Update Camera2D parameters as required to follow player and zoom control
                    // TODO: [2p] Maze items pickup logic
                    if (ColorToInt(pixelColor) == ColorToInt(RED)) //Player Picks Item
                    {
                        ImageDrawPixel(&imMaze, playerCellX, playerCellY, BLACK);
                        UpdateTexture(texMaze, imMaze.data);
                        score += 10;
                    }
                }
                UnloadImageColors(pixels);
            }
            camera2d.target = (Vector2){ player.x, player.y };
        }
        else if (currentMode == 1) // Editor mode
        {
            // TODO: [2p] Maze editor mode, edit image pixels with mouse.
            // Implement logic to selecte image cell from mouse position -> TIP: GetMousePosition()
            // NOTE: Mouse position is returned in screen coordinates and it has to 
            // transformed into image coordinates
            // Once the cell is selected, if mouse button pressed add/remove image pixels

            // WARNING: Remember that when imMaze changes, texMaze must be also updated!

            // TODO: [2p] Collectible map items: player score
            // Using same mechanism than maze editor, implement an items editor, registering
            // points in the map where items should be added for player pickup -> TIP: Use mazeItems[]

            Vector2 mousePos = GetMousePosition();
            int cellX = (mousePos.x - mazePosition.x) / MAZE_SCALE;
            int cellY = (mousePos.y - mazePosition.y) / MAZE_SCALE;
            Color* pixels = LoadImageColors(imMaze);

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                ImageDrawPixel(&imMaze, cellX, cellY, BLACK); // Cambia un pixel a negro (camino)
            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
                ImageDrawPixel(&imMaze, cellX, cellY, WHITE); // Cambia un pixel a blanco (pared)
            if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
                ImageDrawPixel(&imMaze, cellX, cellY, RED);   // Añade un ítem
            if (IsKeyDown(KEY_LEFT_CONTROL) && IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
                ImageDrawPixel(&imMaze, cellX, cellY, GREEN); // Marca el final del laberinto
            UpdateTexture(texMaze, imMaze.data);
            UnloadImageColors(pixels);
        }
        
        // TODO: [1p] Multiple maze biomes supported
        // Implement changing between the different textures to be used as biomes
        // NOTE: For the 3d model, the current selected texture must be applied to the model material  
        if(IsKeyPressed(KEY_ONE))
        {
            currentBiome = 0;
        } 
        if(IsKeyPressed(KEY_TWO))
        {
            currentBiome = 1;
        } 
        if(IsKeyPressed(KEY_THREE))
        {
            currentBiome = 2;
        } 
        if(IsKeyPressed(KEY_FOUR))
        {
            currentBiome = 3;
        } 
        
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(DARKGRAY);

        if (currentMode == 0) // Game mode
        {
            // Draw maze using camera2d (for automatic positioning and scale)
            BeginMode2D(camera2d);

            // TODO: Draw maze walls and floor using current texture biome 
            for (int y = 0; y < imMaze.height; y++)
            {
                for (int x = 0; x < imMaze.width; x++)
                {
                    if (ColorIsEqual(GetImageColor(imMaze, x, y), WHITE))
                    {
                        DrawTexturePro(texBiomes[currentBiome], (Rectangle){ 0, texBiomes[currentBiome].height/2, texBiomes[currentBiome].width/2, texBiomes[currentBiome].height/2 },
                            (Rectangle){ mazePosition.x + x*MAZE_SCALE, mazePosition.y + y*MAZE_SCALE, MAZE_SCALE, MAZE_SCALE }, 
                            (Vector2){ 0 }, 0.0f, WHITE);
                    }
                    else
                    {
                        DrawTexturePro(texBiomes[currentBiome], (Rectangle){ texBiomes[currentBiome].width/2, texBiomes[currentBiome].height/2, texBiomes[currentBiome].width/2, texBiomes[currentBiome].height/2 },
                            (Rectangle){ mazePosition.x + x*MAZE_SCALE, mazePosition.y + y*MAZE_SCALE, MAZE_SCALE, MAZE_SCALE }, 
                            (Vector2){ 0 }, 0.0f, WHITE);
                    }
                }
            }
            // TODO: Draw player rectangle or sprite at player position
            DrawRectangleRec(player, BLUE);
            // TODO: Draw maze items 2d (using sprite texture?)
            for (int y = 0; y < imMaze.height; y++)
            {
                for (int x = 0; x < imMaze.width; x++)
                {
                    if (ColorIsEqual(GetImageColor(imMaze, x, y), RED))
                    {
                        Vector2 itemPosition = {mazePosition.x + x * MAZE_SCALE + MAZE_SCALE / 2, mazePosition.y + y * MAZE_SCALE + MAZE_SCALE / 2};
                        DrawCircleV(itemPosition, MAZE_SCALE / 4, RED);
                    }
                    //Draw the End
                    if (ColorIsEqual(GetImageColor(imMaze, x, y), GREEN))
                    {
                        Vector2 endPosition = {mazePosition.x + x * MAZE_SCALE + MAZE_SCALE / 2, mazePosition.y + y * MAZE_SCALE + MAZE_SCALE / 2};
                        DrawRectangleV(endPosition, (Vector2){ MAZE_SCALE / 2, MAZE_SCALE / 2 }, GREEN);
                    }
                }
            }
            
            EndMode2D();
            DrawText(TextFormat("Score: %d", score), screenWidth - 190, 20, 30, BLACK);
            if (playerHasWon)
            {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(WHITE, 0.6f));
                DrawText("¡You Won!", screenWidth / 2 - 100, screenHeight / 2 - 30, 50, GREEN);
            }
            // TODO: Draw game UI (score, time...) using custom sprites/fonts
            // NOTE: Game UI does not receive the camera2d transformations,
            // it is drawn in screen space coordinates directly
        }
        else if (currentMode == 1) // Editor mode
        {
            // Draw generated maze texture, scaled and centered on screen 
            DrawTextureEx(texMaze, mazePosition, 0.0f, MAZE_SCALE, WHITE);

            // Draw lines rectangle over texture, scaled and centered on screen 
            DrawRectangleLines(mazePosition.x, mazePosition.y, MAZE_WIDTH * MAZE_SCALE, MAZE_HEIGHT * MAZE_SCALE, RED);

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
    UnloadTexture(texMaze);     // Unload maze texture from VRAM (GPU)
    UnloadImage(imMaze);        // Unload maze image from RAM (CPU)

    // TODO: Unload all loaded resources

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

// Generate procedural maze image, using grid-based algorithm
// NOTE: Color scheme used: WHITE = Wall, BLACK = Walkable, RED = Item
Image GenImageMaze(int width, int height, int spacingRows, int spacingCols, float pointChance)
{
    Image imMaze = { 0 };

    // STEP 1: Generate empty image with borders
    //---------------------------------------------------------------------------------
    // STEP 1.1: Generate image of plain color
    imMaze = GenImageColor(width, height, BLACK);

    // STEP 1.2: Draw image border
    for (int y = 0; y < imMaze.height; y++)
    {
        for (int x = 0; x < imMaze.width; x++)
        {
            if ((x == 0) || (x == (imMaze.width - 1)) ||
                (y == 0) || (y == (imMaze.height - 1)))
            {
                ImageDrawPixel(&imMaze, x, y, WHITE);
            }
        }
    }
    //---------------------------------------------------------------------------------

    // STEP 2: Set some random point in image at specific row-column distances
    //---------------------------------------------------------------------------------
    // STEP 2.1: Define an array of point used for maze generation
    // NOTE A: Static array allocation, memory allocated in STACK (MAX: 1MB)
    // NOTE B: Dynamic array allocation, memory allocated in HEAP (MAX: Available RAM)
    //Point mazePoints[64] = { 0 }; // Static array
    Point* mazePoints = malloc(256 * sizeof(Point)); // Dynamic array (MAX: 256 points)
    int mazePointCounter = 0;

    // STEP 2.2: Store specific points, at specific row-column distances
    for (int y = 0; y < imMaze.height; y++)
    {
        for (int x = 0; x < imMaze.width; x++)
        {
            if ((x != 0) && (x != (imMaze.width - 1)) &&
                (y != 0) && (y != (imMaze.height - 1)))
            {
                if ((x % spacingCols == 0) && (y % spacingRows == 0))
                {
                    if (GetRandomValue(0, 100) <= (int)(pointChance * 100))
                    {
                        mazePoints[mazePointCounter] = (Point){ x, y };
                        mazePointCounter++;
                    }
                }
            }
        }
    }

    // STEP 2.3: Draw our points in image
    for (int i = 0; i < mazePointCounter; i++)
    {
        ImageDrawPixel(&imMaze, mazePoints[i].x, mazePoints[i].y, WHITE);
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
    int* pointOrder = LoadRandomSequence(mazePointCounter, 0, mazePointCounter - 1);

    // STEP 3.3: Process every random maze point, drawing pixels in one random direction,
    // until we collision with another wall (WHITE pixel)
    for (int i = 0; i < mazePointCounter; i++)
    {
        Point currentPoint = mazePoints[pointOrder[i]];
        Point currentDir = directions[GetRandomValue(0, 3)];
        Point nextPoint = { currentPoint.x + currentDir.x, currentPoint.y + currentDir.y };

        while (ColorIsEqual(GetImageColor(imMaze, nextPoint.x, nextPoint.y), BLACK))
        {
            ImageDrawPixel(&imMaze, nextPoint.x, nextPoint.y, WHITE);

            nextPoint.x += currentDir.x;
            nextPoint.y += currentDir.y;
        }
    }

    // If using dynamic memory allocation for mazePoints,
    // they allocated memory must be manually freed 
    free(mazePoints);

    // Free pointOrder array, allocated internally by LoadRandomSequence()
    UnloadRandomSequence(pointOrder);
    //-----------------------------------------------------------------------------------

    return imMaze;
}