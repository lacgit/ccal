/* Day-level 神煞 (almanac markers other than the 建除 officer).
 *
 * The table below is both the rule set and the display order.  Add a 神煞 by
 * appending an entry (ASCII, GB2312, Big5, UTF-8 simplified, UTF-8
 * traditional) plus its enum value in shensha.h and a case in ShenShaOn();
 * the mask, the joined text and every output then follow automatically.
 *
 * The rules are the classical 通書 ones and are checked against the user's
 * 通勝 volumes (webui/attachments/LunarCalendarRecords): their day cells carry
 * the same 神煞 names, so every rule can be verified by grepping them.
 */
#include <string.h>
#include "shensha.h"
#include "jianchu.h"

struct ShenShaDef {
    const char* ascii;
    const char* gb;
    const char* b5;
    const char* u8sim;
    const char* u8tra;
};

static const struct ShenShaDef ShenShaTbl[SS_COUNT] = {
    { "TianShe",
      "\xcc\xec\xc9\xe2", "\xa4\xd1\xb3\x6a",
      "\xe5\xa4\xa9\xe8\xb5\xa6",
      "\xe5\xa4\xa9\xe8\xb5\xa6" },   /* 天赦 */
    { "TianDe",
      "\xcc\xec\xb5\xc2", "\xa4\xd1\xbc\x77",
      "\xe5\xa4\xa9\xe5\xbe\xb7",
      "\xe5\xa4\xa9\xe5\xbe\xb7" },   /* 天德 */
    { "TianDeHe",
      "\xcc\xec\xb5\xc2\xba\xcf", "\xa4\xd1\xbc\x77\xa6\x58",
      "\xe5\xa4\xa9\xe5\xbe\xb7\xe5\x90\x88",
      "\xe5\xa4\xa9\xe5\xbe\xb7\xe5\x90\x88" },   /* 天德合 */
    { "YueDe",
      "\xd4\xc2\xb5\xc2", "\xa4\xeb\xbc\x77",
      "\xe6\x9c\x88\xe5\xbe\xb7",
      "\xe6\x9c\x88\xe5\xbe\xb7" },   /* 月德 */
    { "YueDeHe",
      "\xd4\xc2\xb5\xc2\xba\xcf", "\xa4\xeb\xbc\x77\xa6\x58",
      "\xe6\x9c\x88\xe5\xbe\xb7\xe5\x90\x88",
      "\xe6\x9c\x88\xe5\xbe\xb7\xe5\x90\x88" },   /* 月德合 */
    { "YuePo",
      "\xd4\xc2\xc6\xc6", "\xa4\xeb\xaf\x7d",
      "\xe6\x9c\x88\xe7\xa0\xb4",
      "\xe6\x9c\x88\xe7\xa0\xb4" },   /* 月破 */
    { "ShouSi",
      "\xca\xdc\xcb\xc0", "\xa8\xfc\xa6\xba",
      "\xe5\x8f\x97\xe6\xad\xbb",
      "\xe5\x8f\x97\xe6\xad\xbb" },   /* 受死 */
    { "WangWang",
      "\xcd\xf9\xcd\xf6", "\xa9\xb9\xa4\x60",
      "\xe5\xbe\x80\xe4\xba\xa1",
      "\xe5\xbe\x80\xe4\xba\xa1" },   /* 往亡 */
    { "SiFei",
      "\xcb\xc4\xb7\xcf", "\xa5\x7c\xbc\x6f",
      "\xe5\x9b\x9b\xe5\xba\x9f",
      "\xe5\x9b\x9b\xe5\xbb\xa2" },   /* 四廢 */
    { "ShiEDaBai",
      "\xca\xae\xb6\xf1\xb4\xf3\xb0\xdc", "\xa4\x51\xb4\x63\xa4\x6a\xb1\xd1",
      "\xe5\x8d\x81\xe6\x81\xb6\xe5\xa4\xa7\xe8\xb4\xa5",
      "\xe5\x8d\x81\xe6\x83\xa1\xe5\xa4\xa7\xe6\x95\x97" },   /* 十惡大敗 */
    { "YangGongJi",
      "\xd1\xee\xb9\xab\xbc\xc9", "\xb7\xa8\xa4\xbd\xa7\xd2",
      "\xe6\x9d\xa8\xe5\x85\xac\xe5\xbf\x8c",
      "\xe6\xa5\x8a\xe5\x85\xac\xe5\xbf\x8c" },   /* 楊公忌 */
    { "SanNiangSha",
      "\xc8\xfd\xc4\xef\xc9\xb7", "\xa4\x54\xae\x51\xb7\xd9",
      "\xe4\xb8\x89\xe5\xa8\x98\xe7\x85\x9e",
      "\xe4\xb8\x89\xe5\xa8\x98\xe7\x85\x9e" },   /* 三娘煞 */
    { "ZhenSanNiangSha",
      "\xd5\xe6\xc8\xfd\xc4\xef\xc9\xb7", "\xaf\x75\xa4\x54\xae\x51\xb7\xd9",
      "\xe7\x9c\x9f\xe4\xb8\x89\xe5\xa8\x98\xe7\x85\x9e",
      "\xe7\x9c\x9f\xe4\xb8\x89\xe5\xa8\x98\xe7\x85\x9e" },   /* 真三娘煞 */
};

int ShenShaCount(void)
{
    return SS_COUNT;
}

const char* ShenShaName(int i, int enc)
{
    const struct ShenShaDef* d;
    if (i < 0 || i >= SS_COUNT)
        return "";
    d = &ShenShaTbl[i];
    if (enc == 'a') return d->ascii;
    if (enc == 'g') return d->gb;
    if (enc == 'b') return d->b5;
    return JianChuIsTrad() ? d->u8tra : d->u8sim;
}

/* ---- rules ---- */
static int mmonth_of(int mbranch)        /* 月建 -> 正月 = 1 */
{
    return ((mbranch - 2 + 12) % 12) + 1;
}

static int season_of(int mbranch)        /* 0 春 1 夏 2 秋 3 冬 */
{
    switch (mbranch)
    {
    case 2: case 3: case 4:  return 0;   /* 寅卯辰 */
    case 5: case 6: case 7:  return 1;   /* 巳午未 */
    case 8: case 9: case 10: return 2;   /* 申酉戌 */
    }
    return 3;                            /* 亥子丑 */
}

/* 受死: 正月戌, 二月辰, 三月亥, 四月巳, 五月子, 六月午, 七月丑, 八月未,
   九月寅, 十月申, 十一月卯, 十二月酉 (日支). */
static const char shousi_branch[12] = {10, 4, 11, 5, 0, 6, 1, 7, 2, 8, 3, 9};
/* 往亡: 正月寅, 二月巳, 三月申, 四月亥, 五月卯, 六月午, 七月酉, 八月子,
   九月辰, 十月未, 十一月戌, 十二月丑 (日支). */
static const char wangwang_branch[12] = {2, 5, 8, 11, 3, 6, 9, 0, 4, 7, 10, 1};
/* 天德: 正月丁, 二月申, 三月壬, 四月辛, 五月亥, 六月甲, 七月癸, 八月寅,
   九月丙, 十月乙, 十一月巳, 十二月庚 -- 卯午酉子月 take a 地支, the rest a 天干. */
static const char tiande_val[12] = {3, 8, 8, 7, 11, 0, 9, 2, 2, 1, 5, 6};
static const char tiande_isbranch[12] = {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0};
/* 天德合 = 六合 of 天德 (丁壬 申巳 壬丁 辛丙 亥寅 甲己 癸戊 寅亥 丙辛 乙庚 巳申 庚乙). */
static const char tiandehe_val[12] = {8, 5, 3, 2, 2, 5, 4, 11, 7, 6, 8, 1};
/* 月德 / 月德合: 寅午戌月 丙/辛, 申子辰月 壬/丁, 亥卯未月 甲/己, 巳酉丑月 庚/乙. */
static int yuede_stem(int mbranch)
{
    switch (mbranch)
    {
    case 2: case 6: case 10: return 2;
    case 8: case 0: case 4:  return 8;
    case 11: case 3: case 7: return 0;
    }
    return 6;
}

static int yuedehe_stem(int mbranch)
{
    switch (mbranch)
    {
    case 2: case 6: case 10: return 7;
    case 8: case 0: case 4:  return 3;
    case 11: case 3: case 7: return 5;
    }
    return 1;
}

/* 天赦: 春戊寅, 夏甲午, 秋戊申, 冬甲子. */
static const char tianshe_gz[4] = {14, 30, 44, 0};
/* 四廢: 春庚申辛酉, 夏壬子癸亥, 秋甲寅乙卯, 冬丙午丁巳. */
static const char sifei_gz[4][2] = {{56, 57}, {48, 59}, {50, 51}, {42, 53}};
/* 十惡大敗: 甲辰 乙巳 丙申 丁亥 戊戌 己丑 庚辰 辛巳 壬申 癸亥. */
static const char shie_gz[10] = {40, 41, 32, 23, 14, 25, 16, 17, 8, 59};
/* 楊公忌: 正月十三, 二月十一, 三月初九, 四月初七, 五月初五, 六月初三,
   七月初一, 七月廿九, 八月廿七, 九月廿五, 十月廿三, 十一月廿一, 十二月十九. */
static const char yanggong_mon[13] = {1, 2, 3, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12};
static const char yanggong_day[13] = {13, 11, 9, 7, 5, 3, 1, 29, 27, 25, 23, 21, 19};
/* 三娘煞: 農曆 初三 初七 十三 十八 廿二 廿七; 真三娘煞 = 該日日柱為
   庚午 辛未 戊申 己酉 丙午 丁未 (a 真三娘煞 day shows 真三娘煞 only). */
static const char sanniang_day[6] = {3, 7, 13, 18, 22, 27};
static const char sanniang_gz[6] = {6, 7, 44, 45, 42, 43};

int ShenShaOn(int i, int lmonth, int lday, int ganzhi, int mbranch, int dbranch)
{
    int k, mm = mmonth_of(mbranch);
    switch (i)
    {
    case SS_TIANSHE:
        return ganzhi == tianshe_gz[season_of(mbranch)];
    case SS_TIANDE:
        return tiande_isbranch[mm - 1] ? (dbranch == tiande_val[mm - 1])
                                       : (ganzhi % 10 == tiande_val[mm - 1]);
    case SS_TIANDEHE:
        return tiande_isbranch[mm - 1] ? (dbranch == tiandehe_val[mm - 1])
                                       : (ganzhi % 10 == tiandehe_val[mm - 1]);
    case SS_YUEDE:
        return ganzhi % 10 == yuede_stem(mbranch);
    case SS_YUEDEHE:
        return ganzhi % 10 == yuedehe_stem(mbranch);
    case SS_YUEPO:
        return (dbranch + 6) % 12 == mbranch;
    case SS_SHOUSI:
        return dbranch == shousi_branch[mm - 1];
    case SS_WANGWANG:
        return dbranch == wangwang_branch[mm - 1];
    case SS_SIFEI:
        return ganzhi == sifei_gz[season_of(mbranch)][0] ||
               ganzhi == sifei_gz[season_of(mbranch)][1];
    case SS_SHIEDABAI:
        for (k = 0; k < 10; k++)
            if (ganzhi == shie_gz[k])
                return 1;
        return 0;
    case SS_YANGGONGJI:
        for (k = 0; k < 13; k++)
            if (lmonth == yanggong_mon[k] && lday == yanggong_day[k])
                return 1;
        return 0;
    case SS_SANNIANGSHA:
        for (k = 0; k < 6; k++)
            if (lday == sanniang_day[k])
                return (ganzhi == sanniang_gz[k]) ? 0 : 1;
        return 0;
    case SS_ZHENSANNIANGSHA:
        for (k = 0; k < 6; k++)
            if (lday == sanniang_day[k] && ganzhi == sanniang_gz[k])
                return 1;
        return 0;
    }
    return 0;
}

int ShenShaMask(int lmonth, int lday, int ganzhi, int mbranch, int dbranch)
{
    int i, mask = 0;
    for (i = 0; i < SS_COUNT; i++)
        if (ShenShaOn(i, lmonth, lday, ganzhi, mbranch, dbranch))
            mask |= 1 << i;
    return mask;
}

/* Join the names of the set bits with "、", in table order. */
void ShenShaText(int mask, int enc, char* buf, int bufsize)
{
    const char* sep = (enc == 'a') ? ", " :
                      (enc == 'g') ? "\xa1\xa2" :
                      (enc == 'b') ? "\xa1\x42" : "\xe3\x80\x81";
    int i, first = 1;
    buf[0] = 0;
    for (i = 0; i < SS_COUNT; i++)
    {
        const char* n;
        if (!(mask & (1 << i)))
            continue;
        n = ShenShaName(i, enc);
        if ((int)(strlen(buf) + strlen(n) + strlen(sep) + 1) >= bufsize)
            break;
        if (!first)
            strcat(buf, sep);
        strcat(buf, n);
        first = 0;
    }
}
