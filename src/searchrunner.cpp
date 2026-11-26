#include "searchrunner.h"

void SearchRunner::start(Algorithm algorithm, const Grid& grid, bool diagonals)
{
    reset();
    m_gen.emplace(algorithm(grid, m_state, diagonals));
    m_it.emplace(m_gen->begin());
}

void SearchRunner::reset()
{
    m_it.reset();
    m_gen.reset();
    m_state = {};
}

void SearchRunner::step(int count)
{
    for (int i = 0; i < count && active() && !finished(); ++i) {
        ++*m_it;
    }
}

bool SearchRunner::active() const
{
    return m_it.has_value();
}

bool SearchRunner::finished() const
{
    return active() && *m_it == m_gen->end();
}

const SearchState& SearchRunner::state() const
{
    return m_state;
}

