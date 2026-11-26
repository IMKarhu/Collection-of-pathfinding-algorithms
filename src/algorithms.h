#pragma once
#include <cstdint>
#include <vector>
#include <generator>

#include "grid.h"

enum class Mark : uint8_t
{
    None,
    Frontier,
    Visited,
    Path
};

struct SearchState
{
    std::vector<Mark> marks;
    std::vector<int> camefrom;
    int visitedcount = 0;
    bool done = false;
    bool found = false;
};

std::generator<Point> bfs(const Grid& grid, SearchState& state, bool diagonals);
std::generator<Point> dfs(const Grid& grid, SearchState& state, bool diagonals);
std::generator<Point> dijkstra(const Grid& grid, SearchState& state, bool diagonals);
std::generator<Point> astar(const Grid& grid, SearchState& state, bool diagonals);
