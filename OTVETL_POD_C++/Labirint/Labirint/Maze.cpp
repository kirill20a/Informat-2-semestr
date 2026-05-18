#include "Maze.h"
#include <fstream>
#include <queue>
#include <algorithm>
#include <cstring>

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
};

struct BMPInfoHeader {
    uint32_t size;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t imageSize;
    int32_t  xPixelsPerMeter;
    int32_t  yPixelsPerMeter;
    uint32_t colorsUsed;
    uint32_t colorsImportant;
};
#pragma pack(pop)

// ---------- Point ----------
bool Point::operator==(const Point& other) const {
    return x == other.x && y == other.y;
}

bool Point::operator!=(const Point& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& out, const Point& p) {
    out << "(" << p.x << ", " << p.y << ")";
    return out;
}

// ---------- Maze ----------
Maze::Maze() : width(0), height(0), rowSize(0) {}

bool Maze::readBMP(const std::string& filename) {
    std::ifstream fin(filename, std::ios::binary);
    if (!fin) return false;

    BMPHeader header;
    BMPInfoHeader info;
    fin.read((char*)&header, sizeof(header));
    fin.read((char*)&info, sizeof(info));

    if (header.type != 0x4D42 || info.bitCount != 24) return false;

    width = info.width;
    height = abs(info.height);
    rowSize = (width * 3 + 3) & ~3;
    pixels.resize(width * height * 3);

    fin.seekg(header.offset, std::ios::beg);
    std::vector<uint8_t> row(rowSize);
    for (int y = 0; y < height; ++y) {
        fin.read((char*)row.data(), rowSize);
        std::memcpy(&pixels[y * width * 3], row.data(), width * 3);
    }
    return true;
}

bool Maze::writeBMP(const std::string& filename) const {
    if (pixels.empty()) return false;

    std::ifstream fin("ДНК_Лабиринт.bmp", std::ios::binary);
    if (!fin) return false;

    BMPHeader header;
    BMPInfoHeader info;
    fin.read((char*)&header, sizeof(header));
    fin.read((char*)&info, sizeof(info));
    fin.close();

    std::ofstream fout(filename, std::ios::binary);
    if (!fout) return false;

    fout.write((char*)&header, sizeof(header));
    fout.write((char*)&info, sizeof(info));
    fout.seekp(header.offset, std::ios::beg);

    std::vector<uint8_t> row(rowSize, 0);
    for (int y = 0; y < height; ++y) {
        std::memcpy(row.data(), &pixels[y * width * 3], width * 3);
        fout.write((char*)row.data(), rowSize);
    }
    return true;
}
void Maze::findStartEnd() {
    std::vector<Point> colored;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = (y * width + x) * 3;
            int r = pixels[idx + 2];
            int g = pixels[idx + 1];
            int b = pixels[idx + 0];
            int brightness = (r + g + b) / 3;

            if (brightness >= 50 && brightness <= 200) {
                colored.push_back(Point(x, y));
            }
        }
    }

    if (colored.size() < 2) return;

    // ищем две самые удалённые точки
    double maxDist = -1;
    for (size_t i = 0; i < colored.size(); ++i) {
        for (size_t j = i + 1; j < colored.size(); ++j) {
            int dx = colored[i].x - colored[j].x;
            int dy = colored[i].y - colored[j].y;
            double dist = dx * dx + dy * dy;
            if (dist > maxDist) {
                maxDist = dist;
                start = colored[i];
                end = colored[j];
            }
        }
    }
}

void Maze::buildGrid(std::vector<std::vector<int>>& grid, int brightnessThreshold) const {
    grid.assign(height, std::vector<int>(width, 0));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = (y * width + x) * 3;
            int r = pixels[idx + 2];
            int g = pixels[idx + 1];
            int b = pixels[idx + 0];
            int brightness = (r + g + b) / 3;
            grid[y][x] = (brightness > brightnessThreshold) ? 1 : 0;
        }
    }
    grid[start.y][start.x] = 1;
    grid[end.y][end.x] = 1;
}

bool Maze::waveAlgo(int brightnessThreshold) {
    std::vector<std::vector<int>> grid;
    buildGrid(grid, brightnessThreshold);

    const int dx[] = { 1, -1, 0, 0 };
    const int dy[] = { 0, 0, 1, -1 };

    std::vector<std::vector<int>> dist(height, std::vector<int>(width, -1));
    std::queue<Point> q;

    dist[start.y][start.x] = 0;
    q.push(start);

    while (!q.empty()) {
        Point cur = q.front(); q.pop();
        if (cur == end) break;

        for (int i = 0; i < 4; ++i) {
            int nx = cur.x + dx[i];
            int ny = cur.y + dy[i];
            if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
                grid[ny][nx] == 1 && dist[ny][nx] == -1) {
                dist[ny][nx] = dist[cur.y][cur.x] + 1;
                q.push(Point(nx, ny));
            }
        }
    }

    if (dist[end.y][end.x] == -1) return false;

    path.clear();
    Point cur = end;
    path.push_back(cur);
    while (cur != start) {
        for (int i = 0; i < 4; ++i) {
            int nx = cur.x + dx[i];
            int ny = cur.y + dy[i];
            if (nx >= 0 && nx < width && ny >= 0 && ny < height &&
                dist[ny][nx] == dist[cur.y][cur.x] - 1) {
                cur = Point(nx, ny);
                path.push_back(cur);
                break;
            }
        }
    }
    std::reverse(path.begin(), path.end());
    return true;
}

void Maze::drawPath(int thickness) {
    for (const auto& p : path) {
        for (int dy = -thickness; dy <= thickness; ++dy) {
            for (int dx = -thickness; dx <= thickness; ++dx) {
                int nx = p.x + dx;
                int ny = p.y + dy;
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    int idx = (ny * width + nx) * 3;
                    pixels[idx + 2] = 0;
                    pixels[idx + 1] = 0;
                    pixels[idx + 0] = 255;
                }
            }
        }
    }
}

bool Maze::load(const std::string& filename) {
    if (!readBMP(filename)) return false;
    findStartEnd();
    return true;
}

bool Maze::solve(int brightnessThreshold) {
    bool ok = waveAlgo(brightnessThreshold);
    if (ok) drawPath(2);
    return ok;
}

bool Maze::save(const std::string& filename) const {
    return writeBMP(filename);
}

void Maze::printInfo() const {
    std::cout << "Старт: " << start << std::endl;
    std::cout << "Финиш: " << end << std::endl;
    std::cout << "Длина пути: " << path.size() << std::endl;
}

bool Maze::operator!() const {
    return path.empty();
}

Maze::operator bool() const {
    return !path.empty();
}