#include "grid.h"
#include "searchrunner.h"
#include "raygui.h"
#include <array>
#include <string>
#include <string_view>
#include <ranges>

enum class Brush
{
    None,
    Wall,
    Erase
};

struct AlgorithmInfo
{
    const char* name;
    Algorithm func;
};

struct AlgorithmSettings
{
    int algorithm = 0;
    int stepsperframe = 1;
    bool diagonals = false;
    bool running = false;
};

constexpr std::array<AlgorithmInfo, 4> algorithms {{
    { "DFS", dfs },
    { "BFS", bfs },
    { "Dijkstra", dijkstra },
    { "Astar", astar}
}};

int main()
{
    constexpr int initwidth = 800;
    constexpr int initheight = 600;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(initwidth, initheight, "Collection of pathfinding algorithms");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 18);
    SetTargetFPS(60);

    Grid grid{40, 30};
    SearchRunner runner;
    AlgorithmSettings settings;
    Brush brush = Brush::None;
    constexpr float panelwidth = 220.0f;
    bool showpanel = true;

    const std::string algorithmlabels = algorithms
        | std::views::transform([](const AlgorithmInfo& a) {return std::string_view{a.name}; })
        | std::views::join_with('\n')
        | std::ranges::to<std::string>();

    AlgorithmSettings previous = settings;
    while (!WindowShouldClose()) 
    {
        if (settings.algorithm != previous.algorithm || settings.diagonals != previous.diagonals) {
            runner.reset();
            settings.running = false;
        }
        previous = settings;
        if (runner.finished()) {
            settings.running = false;
        }

        const float screenwidth = (float)GetScreenWidth();
        const float screenheight = (float)GetScreenHeight();
        const float controlpanelwidth = showpanel ? panelwidth : 0.0f;
        const Rectangle controlpanel {
            .x = 0,
            .y = 0,
            .width = controlpanelwidth,
            .height = screenheight
        };
        const Rectangle gridrect {
            .x = controlpanelwidth,
            .y = 0,
            .width = screenwidth - controlpanelwidth,
            .height = screenheight
        };
        float y = controlpanel.y + 34;
        auto rect = [&](float h, float w = 0, float nritems = 1) {
            if (w == 0) {
                w = controlpanel.width;
            }
            const Rectangle r{ controlpanel.x + 10, y, w - 20, h};
            y += nritems * (h + 8);
            return r;
        };

        const Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, gridrect)) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                brush = Brush::Wall;
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                brush = Brush::Erase;
            }
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) || IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
            brush = Brush::None;
        }
        if (brush != Brush::None) {
            if(auto point = screenToCell(grid, gridrect, mouse);
               point && *point != grid.start() && *point != grid.goal()) {
                switch(brush)
                {
                case Brush::Erase:
                    if (grid[point->x, point->y] != Cell::Empty) {
                        grid[point->x, point->y] = Cell::Empty;
                        runner.reset();
                    }
                    break;
                case Brush::Wall:
                    if (grid[point->x, point->y] != Cell::Wall) {
                        grid[point->x, point->y] = Cell::Wall;
                        runner.reset();
                    }
                    break;
                case Brush::None:
                default:
                    break;
                }
            }
        }
        if (IsKeyPressed(KEY_TAB)) {
            showpanel = !showpanel;
        }
        if (IsKeyPressed(KEY_C)) {
            grid.clearWalls();
            runner.reset();
            settings.running = false;
        }
        if (IsKeyPressed(KEY_SPACE)) {
            settings.running = !settings.running;
        }
        if (IsKeyPressed(KEY_N)) {
            runner.step();
        }

        if (settings.running) {
            runner.step(settings.stepsperframe);
        }
        BeginDrawing();
        ClearBackground(DARKGRAY);
        drawGrid(grid, gridrect, &runner.state());
        if (showpanel) {
            GuiPanel(controlpanel, "Settings");
            if (GuiButton(rect(30), settings.running ? "Pause" : "Run")) {
                if (!settings.running) {
                    if (!runner.active() || runner.finished()) {
                        runner.start(algorithms[settings.algorithm].func, grid, settings.diagonals);
                    }
                    settings.running = true;
                } else {
                    settings.running = !settings.running;
                }
            }
            GuiCheckBox(rect(10, 30), "Diagonal movement", &settings.diagonals);
            GuiToggleGroup(rect(30, 100, (float)algorithms.size()), algorithmlabels.c_str(), &settings.algorithm);
            GuiLabel(rect(20), "Steps per Frame");
            float speed = (float)settings.stepsperframe;
            GuiSlider(rect(20, 100), nullptr, TextFormat("%d", settings.stepsperframe), &speed, 1, 100);
            settings.stepsperframe = (int)speed;
        }
        EndDrawing();
    }
    CloseWindow();
}
