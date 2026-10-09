/* shensha.h - day-level 神煞 (almanac markers) for the Chinese calendar. */
#ifndef SHENSHA_H
#define SHENSHA_H

/* Evaluate the whole 神煞 table for one day, then print the names of the
   applicable ones with ShenShaText() ("、" separated).  See shensha.cpp for the
   rules and for how to add more 神煞.  Inputs are plain ints:
     lmonth  lunar month 1..12 (a leap month uses its own number)
     lday    lunar day 1..30
     ganzhi  日柱 index 0 = 甲子      (ccal: (int(jdcnt) + 49) % 60)
     mbranch 月建 branch 0=子..11=亥  (ccal: GetMonthBranch(jd, vterms))
     dbranch 日支 0=子..11=亥         (ccal: (int(jdcnt) + 1) % 12)
     wday    weekday 0=Sunday..6=Saturday (ccal: (int(jdcnt) + 1) % 7)
   enc: 'a' ASCII, 'g' GB/GBK ('GB2312' plus a few GBK-extension
 *   characters such as the 厭 in 月厭), 'b' Big5, 'u' UTF-8. */
enum {
    SS_TIANSHE = 0,     /* 天赦   吉 */
    SS_TIANDE,          /* 天德   吉 */
    SS_TIANDEHE,        /* 天德合 吉 */
    SS_YUEDE,           /* 月德   吉 */
    SS_YUEDEHE,         /* 月德合 吉 */
    SS_YUEPO,           /* 月破   凶 */
    SS_DAHAO,           /* 大耗   凶 */
    SS_SHOUSI,          /* 受死   凶 */
    SS_WANGWANG,        /* 往亡   凶 */
    SS_SIFEI,           /* 四廢   凶 */
    SS_SHIEDABAI,       /* 十惡大敗 凶 */
    SS_YANGGONGJI,      /* 楊公忌 凶 */
    SS_SANNIANGSHA,     /* 三娘煞 凶 */
    SS_ZHENSANNIANGSHA, /* 真三娘煞 凶 */
    SS_ZAISHA,          /* 災煞   凶 */
    SS_YUEYAN,          /* 月厭   凶 */
    SS_YUEXING,         /* 月刑   凶 */
    SS_DASHI,           /* 大時   凶 */
    SS_TIANLI,          /* 天吏   凶 */
    SS_XUEJI,           /* 血忌   凶 */
    SS_XUEZHI,          /* 血支   凶 */
    SS_WUMU,            /* 五墓   凶 */
    SS_FURI,            /* 復日   凶 */
    SS_TIANZEI,         /* 天賊   凶 */
    SS_GUIJI,           /* 歸忌   凶 */
    SS_YUESHA,          /* 月煞   凶 */
    SS_YANGJIANG,       /* 陽將   凶 */
    SS_DUANXING,        /* 短星   凶 */
    SS_MIRI,            /* 密日   凶 */
    SS_CHONGRI,         /* 重日   凶 */
    SS_FANZHI,          /* 反支   凶 */
    SS_HUOXING,         /* 火星   凶 */
    SS_YINJIANG,        /* 陰將   凶 */
    SS_WULI,           /* 五離   凶 */
    SS_XIAOHAO,         /* 小耗   凶 */
    SS_SANSANG,        /* 三喪   凶 */
    SS_YUEJI,           /* 月忌   凶 */
    SS_TUFU,            /* 土符   凶 */
    SS_YUEJIAN,         /* 月建   凶 */
    SS_SISHEN,         /* 死神   凶 */
    SS_TUFU_MANSION,    /* 土府   凶 */
    SS_JUJIANG,         /* 俱將   凶 */
    SS_SIQI,           /* 死氣   凶 */
    SS_DINANG,          /* 地囊   凶 */
    SS_WUXU,            /* 五虛   凶 */
    SS_JIESHA,             /* 劫煞   凶 */
    SS_YOUHUO,             /* 遊禍   凶 */
    SS_ZHAOYAO,            /* 招搖   凶 */
    SS_XUANWU,             /* 玄武   凶 */
    SS_GOUCHEN,            /* 勾陳   凶 */
    SS_BAIHU,              /* 白虎   凶 */
    SS_ZHUQUE,             /* 朱雀   凶 */
    SS_TIANXING,           /* 天刑   凶 */
    SS_TIANHUO,            /* 天火   凶 */
    SS_SIJI,            /* 四忌   凶 */
    SS_COUNT
};

int ShenShaCount(void);
const char* ShenShaName(int i, int enc);
int ShenShaOn(int i, int lmonth, int lday, int ganzhi, int mbranch, int dbranch,
              int wday);
unsigned long long ShenShaMask(int lmonth, int lday, int ganzhi, int mbranch, int dbranch, int wday);
void ShenShaText(unsigned long long mask, int enc, char* buf, int bufsize);

#endif /* SHENSHA_H */
