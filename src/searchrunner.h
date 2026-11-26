#pragma once
#include <generator>
#include <optional>
#include <ranges>

#include "algorithms.h"

using Algorithm = std::generator<Point> (*)(const Grid& grid, SearchState& state, bool diagonals);

class SearchRunner
{
public:
    SearchRunner() = default;
    SearchRunner(const SearchRunner& other) = delete;
    SearchRunner& operator=(const SearchRunner& other) = delete;

    void start(Algorithm algorithm, const Grid& grid, bool diagonals);
    void reset();
    void step(int count = 1);

    bool active() const;
    bool finished() const;
    const SearchState& state() const;
private:
    SearchState m_state;
    std::optional<std::generator<Point>> m_gen;
    std::optional<std::ranges::iterator_t<std::generator<Point>>> m_it;
};
