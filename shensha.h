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
    SS_COUNT
};

int ShenShaCount(void);
const char* ShenShaName(int i, int enc);
int ShenShaOn(int i, int lmonth, int lday, int ganzhi, int mbranch, int dbranch);
int ShenShaMask(int lmonth, int lday, int ganzhi, int mbranch, int dbranch);
void ShenShaText(int mask, int enc, char* buf, int bufsize);

#endif /* SHENSHA_H */
