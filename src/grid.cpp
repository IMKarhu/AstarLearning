#include "grid.h"
#include <glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>

Grid::Grid(float tileSize)
    : m_tileSize(tileSize)
    , m_half(0.0f)
    , m_size(0)
{
    loadFromFile("assets/grid.txt");

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(vertex), m_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, color));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texCoord));
    glEnableVertexAttribArray(3);


    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

Grid::~Grid() {}

bool Grid::loadFromFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cout << "Failed to open grid file!\n";
        return false;
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line))
    {
        if (!line.empty())
        {
            lines.push_back(line);
        }
    }

    file.close();

    //check if vector is empty
    if (lines.empty())
    {
        std::cout << "lines vector seems to be empty!\n";
        return false;
    }
    
    int rows = lines.size();
    int cols = lines[0].size();

    m_size = cols;
    m_half = (cols * m_tileSize) / 2.0f;

    m_walls.assign(rows, std::vector<bool>(cols, false));

    for (int z = 0; z < rows; z++)
    {
        for (int x = 0; x < cols; x++)
        {
            m_walls[z][x] = (lines[z][x] == 'x');
        }
    }

    //should always be empty at this point but doesn't hurt to clear them.
    m_vertices.clear();
    m_indices.clear();
    generateGrid(m_vertices, m_indices, m_size);
    return true;
}

void Grid::generateGrid(std::vector<vertex>& vertices, std::vector<unsigned int>& indices, int size)
{
    int sizePerRow = size + 1;

    for (int z = 0; z <= size; ++z)
    {
        for (int x = 0; x <= size; ++x)
        {
            vertex v;
            v.position = glm::vec3(x * m_tileSize - m_half, 0.0f, z * m_tileSize - m_half);
            v.color = glm::vec3(1.0f, 0.5f, 1.0);
            v.texCoord = glm::vec2((float)x / size, (float)z / size);
            v.normal = glm::vec3(0.0f, 1.0f, 0.0f);

            vertices.push_back(v);
        }
    }

    for (int z = 0; z < size; ++z)
    {
        for (int x = 0; x < size; ++x)
        {
            int topLeft = z * sizePerRow + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * sizePerRow + x;
            int bottomRight = bottomLeft + 1;

            // two triangles per square
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
}

void Grid::create()
{
        glGenVertexArrays(1, &m_wvao);
        glGenBuffers(1, &m_wvbo);
        glGenBuffers(1, &m_webo);

        glBindVertexArray(m_wvao);
        glBindBuffer(GL_ARRAY_BUFFER, m_wvbo);
        glBufferData(GL_ARRAY_BUFFER, m_wallVertices.size() * sizeof(vertex), m_wallVertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_webo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_wallIndices.size() * sizeof(unsigned int), m_wallIndices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, color));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texCoord));
        glEnableVertexAttribArray(3);

}

glm::vec3 Grid::getTileWorldPos(int x, int z)
{
    float worldX = (x + 0.5f) * m_tileSize - m_half;
    float worldZ = (z + 0.5f) * m_tileSize - m_half;
    return glm::vec3(worldX, 0.0f, worldZ);
}

glm::vec2 Grid::getTileIndex(glm::vec3& wPos)
{
    int x = (int)floor((wPos.x + m_half) / m_tileSize);
    int z = (int)floor((wPos.z + m_half) / m_tileSize);

    x = glm::clamp(x, 0, m_size - 1);
    z = glm::clamp(z, 0, m_size - 1);
    return glm::vec2(x, z);
}

glm::vec2 Grid::getWalkableTile()
{
    std::vector<glm::vec2> wtiles;
    for (int y = 0; y < m_size; y++)
    {
        for (int x = 0; x < m_size; x++)
        {
            if (!wall(x, y))
            {
                wtiles.push_back(glm::vec2(x, y));
            }
        }
    }

    if (wtiles.empty())
    {
        return glm::vec2(0.0, 0.0);
    }

    int r = rand() % wtiles.size();
    return wtiles[r];
}

int Grid::getSize()
{
    return m_size;
}

void Grid::setState(GameState state)
{
    m_state = state;
}

std::vector<vertex>& Grid::getWallVerts()
{
    return m_wallVertices;
}

std::vector<unsigned int>& Grid::getWallIndices()
{
    return m_wallIndices;
}

bool Grid::wall(int x, int z)
{
    return m_walls[z][x];
}

void Grid::setWall(int x, int z, bool value)
{
    m_walls[z][x] = value;
}

void Grid::draw()
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
}

void Grid::drawWall()
{
    glBindVertexArray(m_wvao);
    //glDrawArrays(GL_TRIANGLES, 0, size);
    glDrawElements(GL_TRIANGLES, m_wallIndices.size(), GL_UNSIGNED_INT, 0);
}
