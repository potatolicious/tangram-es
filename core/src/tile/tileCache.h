#pragma once

#include "tileID.h"
#include "tile.h"

#include <unordered_map>
#include <list>
#include <memory>

namespace Tangram {
// TileSet serial + TileID
using TileCacheKey = std::pair<int32_t, TileID>;
}

namespace std {
    template <>
    struct hash<Tangram::TileCacheKey> {
        size_t operator()(const Tangram::TileCacheKey& k) const {
            std::size_t seed = 0;
            hash_combine(seed, k.first);
            hash_combine(seed, k.second);
            return seed;
        }
    };
}

namespace Tangram {

class TileCache {
    struct CacheEntry {
        TileCacheKey key;
        std::shared_ptr<Tile> tile;
    };

    using CacheList = std::list<CacheEntry>;
    using CacheMap = std::unordered_map<TileCacheKey, typename CacheList::iterator>;

public:

    TileCache(size_t _cacheSizeMB) :
        m_cacheUsage(0),
        m_cacheMaxUsage(_cacheSizeMB) {}

    void put(int32_t _source, std::shared_ptr<Tile> _tile) {
        TileCacheKey k(_source, _tile->getID());

        m_cacheList.push_front({k, _tile});
        m_cacheMap[k] = m_cacheList.begin();
        m_cacheUsage += _tile->getMemoryUsage();

        limitCacheSize(m_cacheMaxUsage);
    }

    std::shared_ptr<Tile> get(int32_t _source, TileID _tileID) {
        std::shared_ptr<Tile> tile;
        TileCacheKey k(_source, _tileID);

        auto it = m_cacheMap.find(k);
        if (it != m_cacheMap.end()) {
            std::swap(tile, (*(it->second)).tile);
            m_cacheList.erase(it->second);
            m_cacheMap.erase(it);
            m_cacheUsage -= tile->getMemoryUsage();
        }
        return tile;
    }

    void limitCacheSize(size_t _cacheSizeBytes) {
        m_cacheMaxUsage = _cacheSizeBytes;

        while (m_cacheUsage > m_cacheMaxUsage) {
            if (m_cacheList.empty()) {
                logMsg("Error: invalid cache state!\n");
                m_cacheUsage = 0;
                break;
            }
            auto& tile = m_cacheList.back().tile;
            m_cacheUsage -= tile->getMemoryUsage();
            m_cacheMap.erase(m_cacheList.back().key);
            m_cacheList.pop_back();
        }
    }

    size_t getMemoryUsage() const {
        size_t sum = 0;
        for (auto& entry : m_cacheList) {
            sum += entry.tile->getMemoryUsage();
        }
        return sum;
    }

    void clear() {
        m_cacheMap.clear();
        m_cacheList.clear();
    }

private:
    CacheMap m_cacheMap;
    CacheList m_cacheList;

    int m_cacheUsage;
    int m_cacheMaxUsage;
};

}
