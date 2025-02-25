#ifndef CUTIL_H
#define CUTIL_H

#include <QMutex>
#include <QString>

#include <random>

#include "cubiomes/quadbase.h"
#include "cubiomes/util.h"


inline const char* struct2str(int stype)
{
    switch (stype)
    {
    case Desert_Pyramid:    return "沙漠神殿";
    case Jungle_Temple:     return "丛林神庙";
    case Swamp_Hut:         return "沼泽小屋";
    case Igloo:             return "冰屋";
    case Village:           return "村庄";
    case Ocean_Ruin:        return "海底遗迹";
    case Shipwreck:         return "沉船";
    case Monument:          return "海底神殿";
    case Mansion:           return "林地府邸";
    case Outpost:           return "前哨站";
    case Ruined_Portal:     return "废弃传送门";
    case Ruined_Portal_N:   return "废弃传送门(下界)";
    case Treasure:          return "宝藏";
    case Mineshaft:         return "废弃矿井";
    case Fortress:          return "下界要塞";
    case Bastion:           return "堡垒遗迹";
    case End_City:          return "末地城";
    case End_Gateway:       return "末地折越门";
    case Ancient_City:      return "远古城市";
    }
    return "?";
}

struct StartPiece
{
    int stype;
    int biome;
    int start;
    int giant;
    const char *name;
    int row, col; // UI layout
};

static const StartPiece g_start_pieces[] =
{   // the index is used to encode the start pieces in the condition
    // (and should therefore ideally remain constant across upgrades)
    {Village, plains, 0, -1, "平原-喷泉", 0, 0},
    {Village, plains, 1, -1, "平原-会合点1", 1, 0},
    {Village, plains, 2, -1, "平原-会合点2", 2, 0},
    {Village, plains, 3, -1, "平原-会合点3", 3, 0},
    {Village, desert, 1, -1, "沙漠-会合点1", 4, 1},
    {Village, desert, 2, -1, "沙漠-会合点2", 5, 1},
    {Village, desert, 3, -1, "沙漠-会合点3", 6, 1},
    {Village, savanna, 1, -1, "稀树草原-会合点1", 0, 1},
    {Village, savanna, 2, -1, "稀树草原-会合点2", 1, 1},
    {Village, savanna, 3, -1, "稀树草原-会合点3", 2, 1},
    {Village, savanna, 4, -1, "稀树草原-会合点4", 3, 1},
    {Village, taiga, 1, -1, "针叶林-会合点1", 7, 0},
    {Village, taiga, 2, -1, "针叶林-会合点2", 8, 0},
    {Village, snowy_tundra, 1, -1, "雪原-会合点1", 4, 0},
    {Village, snowy_tundra, 2, -1, "雪原-会合点2", 5, 0},
    {Village, snowy_tundra, 3, -1, "雪原-会合点3", 6, 0},
    {Bastion, -1, 0, -1, "居住区", 0, 0},
    {Bastion, -1, 1, -1, "疣猪兽棚", 1, 0},
    {Bastion, -1, 2, -1, "藏宝室", 2, 0},
    {Bastion, -1, 3, -1, "桥", 3, 0},
    {Ruined_Portal, -1, 1, 1, "巨型废弃传送门1", 0, 1},
    {Ruined_Portal, -1, 2, 1, "巨型废弃传送门2", 1, 1},
    {Ruined_Portal, -1, 3, 1, "巨型废弃传送门3", 2, 1},
    {Ruined_Portal, -1, 1, 0, "废弃传送门1", 0, 0},
    {Ruined_Portal, -1, 2, 0, "废弃传送门2", 1, 0},
    {Ruined_Portal, -1, 3, 0, "废弃传送门3", 2, 0},
    {Ruined_Portal, -1, 4, 0, "废弃传送门4", 3, 0},
    {Ruined_Portal, -1, 5, 0, "废弃传送门5", 4, 0},
    {Ruined_Portal, -1, 6, 0, "废弃传送门6", 5, 0},
    {Ruined_Portal, -1, 7, 0, "废弃传送门7", 6, 0},
    {Ruined_Portal, -1, 8, 0, "废弃传送门8", 7, 0},
    {Ruined_Portal, -1, 9, 0, "废弃传送门9", 8, 0},
    {Ruined_Portal, -1, 10, 0, "废弃传送门10", 9, 0},
    {-1,0,0,0,0,0,0}
};

inline QString getStartPieceName(int stype, const StructureVariant *sv)
{
    QString name;
    for (size_t i = 0; ; i++)
    {
        const StartPiece *sp = g_start_pieces + i;
        if (sp->stype < 0) break;
        if (sp->stype != stype) continue;
        if (sp->biome >= 0 && sp->biome != sv->biome) continue;
        if (sp->giant >= 0 && sp->giant != sv->giant) continue;
        if (sp->start != sv->start) continue;
        name += sp->name;
        break;
    }
    return name;
}


// get a random 64-bit integer
static inline uint64_t getRnd64()
{
    static QMutex mutex;
    static std::random_device rd;
    static std::mt19937_64 mt(rd());
    static uint64_t x = (uint64_t) time(0);
    uint64_t ret = 0;
    mutex.lock();
    if (rd.entropy())
    {
        std::uniform_int_distribution<int64_t> d;
        ret = d(mt);
    }
    else
    {
        const uint64_t c = 0xd6e8feb86659fd93ULL;
        x ^= x >> 32;
        x *= c;
        x ^= x >> 32;
        x *= c;
        x ^= x >> 32;
        ret = x;
    }
    mutex.unlock();
    return ret;
}

enum { S_TEXT, S_NUMERIC, S_RANDOM };
inline int str2seed(const QString &str, uint64_t *out)
{
    if (str.isEmpty())
    {
        *out = getRnd64();
        return S_RANDOM;
    }

    bool ok = false;
    *out = (uint64_t) str.toLongLong(&ok);
    if (ok)
    {
        return S_NUMERIC;
    }

    // String.hashCode();
    int32_t hash = 0;
    const ushort *chars = str.utf16();
    for (int i = 0; chars[i] != 0; i++)
    {
        hash = 31 * hash + chars[i];
    }
    *out = hash;
    return S_TEXT;
}

struct IdCmp
{
    enum
    {
        SORT_ID,
        SORT_LEX,
        SORT_DIM,
    };

    IdCmp(int mode, int mc, int dim) : mode(mode),mc(mc),dim(dim)
    {
    }

    int mode;
    int mc;
    int dim;
    bool operator() (int id1, int id2)
    {
        if (mode == SORT_ID)
            return id1 < id2;
        int v1 = 1, v2 = 1;
        if (mc >= 0)
        {   // biomes not in this version go to the back
            v1 &= biomeExists(mc, id1);
            v2 &= biomeExists(mc, id2);
        }
        if (dim != DIM_UNDEF)
        {   // biomes in other dimensions go to the back
            v1 &= getDimension(id1) == dim;
            v2 &= getDimension(id2) == dim;
        }
        if (v1 ^ v2)
            return v1;
        if (mode == SORT_DIM)
        {
            int d1 = getDimension(id1);
            int d2 = getDimension(id2);
            if (d1 != d2)
                return (d1==0 ? 0 : d1==-1 ? 1 : 2) < (d2==0 ? 0 : d2==-1 ? 1 : 2);
        }
        const char *s1 = biome2str(mc, id1);
        const char *s2 = biome2str(mc, id2);
        if (!s1 && !s2) return id1 < id2;
        if (!s1) return false; // move non-biomes to back
        if (!s2) return true;
        return strcmp(s1, s2) < 0;
    }

    bool isPrimary(int id)
    {
        if (mode == SORT_ID)
            return true;
        if (mc >= 0 && !biomeExists(mc, id))
            return false;
        if (dim != DIM_UNDEF && getDimension(id) != dim)
            return false;
        return true;
    }
};


#endif // CUTIL_H
