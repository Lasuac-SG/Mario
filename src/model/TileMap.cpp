#include "model/TileMap.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <utility>

namespace {

TileType charToTile(char ch) {
  switch (ch) {
    case '#': return TileType::GROUND;
    case 'P': return TileType::PIPE;
    case '=': return TileType::PLATFORM;
    case '?': return TileType::QUESTION;
    case 'B': return TileType::BRICK;
    default: return TileType::EMPTY;
  }
}

const std::string& defaultLevel() {
  static const std::string kLevel =
      "................................................................................................\n"
      "................................................................................................\n"
      "................................................................................................\n"
      "................................................................................................\n"
      "..........=====.................................................................................\n"
      "................................................................................................\n"
      "......M.........................................................................................\n"
      "..............====......PP....................====......PP....................====......PP......\n"
      "........................PP....G.........................PP..G...........................F.PP......\n"
      "####################.###########################################################################\n"
      "################################################################################################\n";
  return kLevel;
}

}  // namespace

TileMap::TileMap() { loadFromString(defaultLevel()); }

bool TileMap::loadFromString(const std::string& text) {
  std::istringstream iss(text);
  return loadFromStream(iss);
}

bool TileMap::loadFromFile(const std::string& path) {
  std::ifstream ifs(path);
  if (!ifs) return false;
  return loadFromStream(ifs);
}

bool TileMap::loadFromStream(std::istream& in) {
  std::vector<std::string> lines;
  std::string line;
  std::size_t maxLen = 0;
  while (std::getline(in, line)) {
    if (!line.empty() && line.back() == '\r') line.pop_back();
    if (line.empty()) continue;
    maxLen = std::max(maxLen, line.size());
    lines.push_back(std::move(line));
  }
  if (lines.empty() || maxLen == 0) return false;

  const int cols = static_cast<int>(maxLen);
  const int rows = static_cast<int>(lines.size());
  std::vector<TileType> tiles(static_cast<std::size_t>(cols) * rows, TileType::EMPTY);

  int spawnCol = 1;
  int spawnRow = rows - 2;
  std::vector<SpawnCell> enemySpawns;
  std::vector<SpawnCell> coinSpawns;
  bool hasGoal = false;
  int goalCol = 0;
  int goalRow = 0;

  for (int r = 0; r < rows; ++r) {
    const std::string& row = lines[static_cast<std::size_t>(r)];
    for (int c = 0; c < cols; ++c) {
      const char ch = (c < static_cast<int>(row.size())) ? row[static_cast<std::size_t>(c)] : '.';
      if (ch == 'M' || ch == 'm') {
        spawnCol = c;
        spawnRow = r;
      } else if (ch == 'G' || ch == 'g') {
        enemySpawns.push_back({c, r});
      } else if (ch == 'C' || ch == 'c') {
        coinSpawns.push_back({c, r});
      } else if (ch == 'F' || ch == 'f') {
        hasGoal = true;
        goalCol = c;
        goalRow = r;
      } else {
        tiles[static_cast<std::size_t>(r) * cols + c] = charToTile(ch);
      }
    }
  }

  tiles_ = std::move(tiles);
  enemySpawns_ = std::move(enemySpawns);
  coinSpawns_ = std::move(coinSpawns);
  cols_ = cols;
  rows_ = rows;
  spawnCol_ = std::max(0, std::min(spawnCol, cols_ - 1));
  spawnRow_ = std::max(0, std::min(spawnRow, rows_ - 1));
  hasGoal_ = hasGoal;
  goalCol_ = std::max(0, std::min(goalCol, cols_ - 1));
  goalRow_ = std::max(0, std::min(goalRow, rows_ - 1));
  return true;
}
