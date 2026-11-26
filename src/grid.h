#pragma once
#include <cstdint>
#include <vector>
#include <optional>
#include <utility>

#include "raylib.h"

enum class Cell : uint8_t
{
    Empty,
    Wall
};

struct Point
{
    int x{};
    int y{};
    bool operator==(const Point&) const = default;
};

class Grid
{
public:
    Grid(int width, int height);

    int width() const;
    int height() const;
    bool inBounds(Point point) const;

    template<class Self>
    auto&& operator[](this Self&& self, int x, int y)
    {
        return std::forward<Self>(self).m_cells[self.index({x, y})];
    }

    int index(Point point) const;
    Point point(int index) const;

    Point start() const;
    void setStart(Point point);

    Point goal() const;
    void setGoal(Point point);

    void clearWalls();
    std::vector<Point> neighbours(Point point, bool diagonals) const;
private:
    int m_width;
    int m_height;
    std::vector<Cell> m_cells;
    Point m_start;
    Point m_goal;

};
/*
 * Forward declaring state struct
 * otherwise we would get circular include
 */
struct SearchState;

void drawGrid(const Grid& grid, Rectangle rect, const SearchState* state = nullptr);
std::optional<Point> screenToCell(const Grid& grid, Rectangle rect, Vector2 screenpos);
