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
    default: return TileType::EMPTY;  // '.'、空格及未知字符
  }
}

// 内置默认关卡（兜底），保证无外部文件也能跑起来。宽 32 格。
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
      "........................PP....G.........................PP..G...........................PP......\n"
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
    if (!line.empty() && line.back() == '\r') line.pop_back();  // 兼容 CRLF
    if (line.empty()) continue;                                 // 跳过空行
    maxLen = std::max(maxLen, line.size());
    lines.push_back(std::move(line));
  }
  if (lines.empty() || maxLen == 0) return false;

  const int cols = static_cast<int>(maxLen);
  const int rows = static_cast<int>(lines.size());
  std::vector<TileType> tiles(static_cast<std::size_t>(cols) * rows, TileType::EMPTY);

  // 默认出生点：左侧近底部；若关卡含 'M' 则被覆盖。
  int spawnCol = 1;
  int spawnRow = rows - 2;
  std::vector<SpawnCell> enemySpawns;
  std::vector<SpawnCell> coinSpawns;

  for (int r = 0; r < rows; ++r) {
    const std::string& row = lines[static_cast<std::size_t>(r)];
    for (int c = 0; c < cols; ++c) {
      const char ch = (c < static_cast<int>(row.size())) ? row[static_cast<std::size_t>(c)] : '.';
      if (ch == 'M' || ch == 'm') {
        spawnCol = c;
        spawnRow = r;  // 出生格保持 EMPTY
      } else if (ch == 'G' || ch == 'g') {
        enemySpawns.push_back({c, r});  // 敌人出生格保持 EMPTY
      } else if (ch == 'C' || ch == 'c') {
        coinSpawns.push_back({c, r});   // 金币格保持 EMPTY
      } else {
        tiles[static_cast<std::size_t>(r) * cols + c] = charToTile(ch);
      }
    }
  }

  // 全部解析成功后再提交，保证加载失败不破坏现有地图。
  tiles_ = std::move(tiles);
  enemySpawns_ = std::move(enemySpawns);
  coinSpawns_ = std::move(coinSpawns);
  cols_ = cols;
  rows_ = rows;
  spawnCol_ = std::max(0, std::min(spawnCol, cols_ - 1));
  spawnRow_ = std::max(0, std::min(spawnRow, rows_ - 1));
  return true;
}
