#include "grid.h"
#include <cassert>
#include <algorithm>
#include <array>
#include <ranges>
#include <cmath>
#include <utility>

#include "algorithms.h"

/*
 * helper gridlayout and function to compute it
 */
struct GridLayout
{
    float cellsize;
    Vector2 origin;
};

static GridLayout computeLayout(const Grid& grid, Rectangle rect)
{
    const float cell = std::min(rect.width / grid.width(), rect.height / grid.height());
    return {
        cell,
        {
            rect.x + (rect.width - cell * grid.width()) / 2.0f,
            rect.y + (rect.height - cell * grid.height()) / 2.0f
        }
    };
}

Grid::Grid(int width, int height)
    : m_width{width}
    , m_height{height}
    , m_cells(static_cast<std::size_t>(width) * height, Cell::Empty)
    , m_start{width / 4, height / 2}
    , m_goal{3 * width / 4, height / 2}
{
    assert(width > 0 && height > 0);
}

int Grid::width() const
{
    return m_width;
}

int Grid::height() const
{
    return m_height;
}

bool Grid::inBounds(Point point) const
{
    return point.x >= 0 && point.x < m_width && point.y >= 0 && point.y < m_height;
}

int Grid::index(Point point) const
{
    assert(inBounds(point));
    return point.y * m_width + point.x;
}

Point Grid::point(int index) const
{
    return { index % m_width, index / m_width };
}

Point Grid::start() const
{
    return m_start;
}

void Grid::setStart(Point point)
{
    if (!inBounds(point) || point == m_goal) {
        return;
    }
    m_cells[index(point)] = Cell::Empty;
    m_start = point;
}

Point Grid::goal() const
{
    return m_goal;
}

void Grid::setGoal(Point point)
{
    if (!inBounds(point) || point == m_start) {
        return;
    }
    m_cells[index(point)] = Cell::Empty;
    m_goal = point;
}

void Grid::clearWalls()
{
    std::ranges::fill(m_cells, Cell::Empty);
}

std::vector<Point> Grid::neighbours(Point point, bool diagonals) const
{
    //reordering these neighbor offsets will make dfs path differently
    //TODO: add button to reverse these
    static constexpr std::array<Point, 8> offsets {{
        {1, 0}, {-1,  0}, { 0, 1}, { 0, -1},
        {1, 1}, { 1, -1}, {-1, 1}, {-1, -1}
    }};

    auto walkable = [&](Point p) {
        return inBounds(p) && m_cells[index(p)] != Cell::Wall;
    };

    std::vector<Point> result;
    const int count = diagonals ? 8 : 4;
    for (const Point offsetpoint : offsets | std::views::take(count)) {
        const Point p{point.x + offsetpoint.x, point.y + offsetpoint.y};
        if (!walkable(p)) {
            continue;
        }

        if (offsetpoint.x != 0 && offsetpoint.y != 0 &&
                (!walkable({point.x + offsetpoint.x, point.y}) ||
                 !walkable({point.x, point.y + offsetpoint.y})))
        {
            continue;
        }
        result.push_back(p);
    }
    return result;
}

void drawGrid(const Grid& grid, Rectangle rect, const SearchState* state)
{
    const auto [cell, origin] = computeLayout(grid, rect);
    static constexpr std::array<Color, 2> colors { RAYWHITE, BLACK };
    static constexpr std::array<Color, 4> markcolors { BLANK, SKYBLUE, BLUE, GOLD };

    const bool hassearch = state && !state->marks.empty();
    
    auto cellrect = [&](Point point) {
        return Rectangle{ origin.x + point.x * cell, origin.y + point.y * cell, cell - 1, cell - 1};
    };

    for (auto [x, y] : std::views::cartesian_product(
                       std::views::iota(0, grid.width()),
                       std::views::iota(0, grid.height()))) {
        Color color = colors[std::to_underlying(grid[x, y])];

        if (hassearch) {
            const Mark mark = state->marks[grid.index({x, y})];
            if (mark != Mark::None) {
                color = markcolors[std::to_underlying(mark)];
            }
        }

        DrawRectangleRec(cellrect({x, y}), color);
    }
    DrawRectangleRec(cellrect(grid.start()), GREEN);
    DrawRectangleRec(cellrect(grid.goal()), RED);
}

std::optional<Point> screenToCell(const Grid& grid, Rectangle rect, Vector2 screenpos)
{
    const auto [cell, origin] = computeLayout(grid, rect);
    const Point point {static_cast<int>(std::floor((screenpos.x - origin.x) / cell)),
                       static_cast<int>(std::floor((screenpos.y - origin.y) / cell))};

    if (!grid.inBounds(point)) {
        return std::nullopt;
    }
    return point;
}
