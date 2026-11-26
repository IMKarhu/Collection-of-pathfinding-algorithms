#include "algorithms.h"
#include <print>
#include <queue>
#include <stack>
#include <limits>

/*
 * Helper function to walk the finished path
 * so I don't have to write this in every algorithm.
 */
static void walkPath(const Grid& grid, SearchState& state)
{
    if (state.found) {
        for (int i = state.camefrom[grid.index(grid.goal())]; i != -1; i = state.camefrom[i]) {
            state.marks[i] = Mark::Path;
        }
    }
    state.done = true;
}

/*
 * Helper function to determin cost of each step.
 * Diagonal steps cost more.
 */
static int stepCost(Point from, Point to)
{
    const bool diagonal = from.x != to.x && from.y != to.y;
    return diagonal ? 14 : 10;
}

std::generator<Point> bfs(const Grid& grid, SearchState& state, bool diagonals)
{
    const int n = grid.width() * grid.height();
    state = {};
    state.marks.assign(n, Mark::None);
    state.camefrom.assign(n, -1);
    std::vector<bool> discovered(n, false);
    std::queue<Point> queue;
    queue.push(grid.start());
    discovered[grid.index(grid.start())] = true;

    while (!queue.empty()) 
    {
        auto curnode = queue.front();
        queue.pop();

        state.marks[grid.index(curnode)] = Mark::Visited;
        ++state.visitedcount;

        if (curnode == grid.goal()) {
            state.found = true;
            break;
        }

        for (const Point point : grid.neighbours(curnode, diagonals)) {
            const int p = grid.index(point);
            if (discovered[p]) {
                continue;
            }
            discovered[p] = true;
            state.camefrom[p] = grid.index(curnode);
            state.marks[p] = Mark::Frontier;
            queue.push(point);
        }
        co_yield curnode;
    }
    walkPath(grid, state);
}

std::generator<Point> dfs(const Grid& grid, SearchState& state, bool diagonals)
{
    const int n = grid.width() * grid.height();
    state = {};
    state.marks.assign(n, Mark::None);
    state.camefrom.assign(n, -1);
    struct Entry {
        Point node;
        int parent;
    };
    std::stack<Entry> stack;
    std::vector<bool> visited(n, false);

    stack.push({grid.start(), -1});

    while (!stack.empty())
    {
        const auto [curnode, parent] = stack.top();
        stack.pop();

        const int cellindex = grid.index(curnode);
        if (visited[cellindex]) {
            continue;
        }
        visited[cellindex] = true;
        state.camefrom[cellindex] = parent;
        state.marks[cellindex] = Mark::Visited;
        ++state.visitedcount;

        if (curnode == grid.goal()) {
            state.found = true;
            break;
        }

        for (const Point p : grid.neighbours(curnode, diagonals)) {
            const int neighbourindex = grid.index(p);
            if (visited[neighbourindex]) {
                continue;
            }
            state.marks[neighbourindex] = Mark::Frontier;
            stack.push({p, cellindex});
        }
        co_yield curnode;
    }
    walkPath(grid, state);
}

std::generator<Point> dijkstra(const Grid& grid, SearchState& state, bool diagonals)
{
    const int n = grid.width() * grid.height();
    constexpr int max = std::numeric_limits<int>::max();
    state = {};
    state.marks.assign(n, Mark::None);
    state.camefrom.assign(n, -1);
    struct Entry {
        int cost;
        int index;
        auto operator<=>(const Entry& other) const = default;
    };
    std::priority_queue<Entry, std::vector<Entry>, std::greater<>> open;
    std::vector<int> dist(n, max);
    const int startindex = grid.index(grid.start());
    dist[startindex] = 0;
    open.push({0, startindex});

    while (!open.empty())
    {
        const auto [cost, cellindex] = open.top();
        open.pop();
        const auto curpoint = grid.point(cellindex);

        if (cost > dist[cellindex]) {
            continue;
        }

        state.marks[cellindex] = Mark::Visited;
        ++state.visitedcount;

        if (curpoint == grid.goal()) {
            state.found = true;
            break;
        }

        for (const Point point : grid.neighbours(curpoint, diagonals)) {
            const int neighbourindex = grid.index(point);
            const int newcost = dist[cellindex] + stepCost(curpoint, point);
            if (newcost < dist[neighbourindex]) {
                dist[neighbourindex] = newcost;
                state.camefrom[neighbourindex] = cellindex;
                state.marks[neighbourindex] = Mark::Frontier;
                open.push({newcost, neighbourindex});
            }
        }
        co_yield curpoint;
    }
    walkPath(grid, state);
}

std::generator<Point> astar(const Grid& grid, SearchState& state, bool diagonals)
{
    const int n = grid.width() * grid.height();
    constexpr int max = std::numeric_limits<int>::max();
    state = {};
    state.marks.assign(n, Mark::None);
    state.camefrom.assign(n, -1);
    struct Entry {
        int f;
        int h;
        int index;
        auto operator<=>(const Entry& other) const = default;
    };
    std::priority_queue<Entry, std::vector<Entry>, std::greater<>> open;
    std::vector<int> dist(n, max);
    const int startindex = grid.index(grid.start());
    dist[startindex] = 0;
    const Point goal = grid.goal();
    auto heuristic = [&](Point p) {
        const int dx = std::abs(p.x - goal.x);
        const int dy = std::abs(p.y - goal.y);
        return diagonals ? 10 * std::max(dx, dy) + 4 * std::min(dx, dy)
                         : 10 * (dx + dy);
    };
    const int hstart = heuristic(grid.start());
    open.push({0, hstart, startindex});
    while (!open.empty())
    {
        const auto [f, hvalue, cellindex] = open.top();
        open.pop();
        const auto curpoint = grid.point(cellindex);

        if (f - hvalue > dist[cellindex]) {
            continue;
        }
        state.marks[cellindex] = Mark::Visited;
        ++state.visitedcount;

        if (curpoint == goal) {
            state.found = true;
            break;
        } 

        for (const Point point : grid.neighbours(curpoint, diagonals)) {
            const int neighbourindex = grid.index(point);
            const int newcost = dist[cellindex] + stepCost(curpoint, point);
            if (newcost < dist[neighbourindex]) {
                dist[neighbourindex] = newcost;
                state.camefrom[neighbourindex] = cellindex;
                state.marks[neighbourindex] = Mark::Frontier;
                const auto hval = heuristic(point);
                open.push({newcost+hval, hval, neighbourindex});
            }
        }
        co_yield curpoint;
    }
    walkPath(grid, state);
}
