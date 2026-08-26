/* 建除十二神 (Jianchu twelve deities) support.
   index 0..11 = 建 除 滿 平 定 執 破 危 成 收 開 閉 */
#ifndef JIANCHU_H
#define JIANCHU_H

/* Call once after option parsing: -b -> traditional, otherwise simplified. */
void SetJianChuTrad(bool bTrad);

/* The 建除 name character for index idx (0..11).
   enc: 'g' GB2312 (simplified), 'b' Big5 (traditional), 'u' UTF-8. */
const char* JianChuName(int idx, int enc);

/* Legend text for index idx.
   field: 0=神煞(name), 1=吉凶, 2=含义, 3=宜, 4=忌.
   enc: 'g'/'b'/'u' as above. */
const char* JianChuField(int field, int idx, int enc);

/* Legend column header text. col: 0=神煞 1=吉凶 2=含义 3=宜 4=忌. */
const char* JianChuLabel(int col, int enc);

/* ---- 時辰吉凶 (double-hour auspiciousness) ----
   The 建除 twelve-officer cycle also applies to the 12 時辰 (double
   hours): the hour whose branch equals the day's branch is 建時
   (以日支起建), then 除滿平定執破危成收開閉 follow the branch order.
   hb: hour branch 0=子..11=亥; db: day branch 0=子..11=亥;
   yearbranch: 年支 (0=子..11=亥), used for 歲破 (沖年支) hours. */

/* 時辰建除 index 0..11 for hour branch hb on a day with branch db. */
int GetShiChenJianChu(int hb, int db);

/* 吉凶 character (吉/凶/中) for hour hb on a day with branch db:
   沖年支/沖日支 = 凶, 伏吟 (時支=日支) = 吉, 六害 = 中, otherwise the
   時建 officer rated by the 十二建星黃黑道.
   enc: 'g'/'b'/'u' as above. */
const char* ShiChenJixiong(int hb, int db, int yearbranch, int enc);

/* hh:mm range of hour hb, e.g. "23:00-01:00" for 子時 (0). */
const char* ShiChenHHMM(int hb);

#endif /* JIANCHU_H */
