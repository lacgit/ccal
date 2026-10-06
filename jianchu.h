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

/* ---- 四離四絕日 ----
   四離 = 春分 夏至 秋分 冬至 之前一日 (陰陽分離之日);
   四絕 = 立春 立夏 立秋 立冬 之前一日 (四時之絕).
   kind: 0 = 四離, 1 = 四絕.  enc: 'g'/'b'/'u' as above. */

/* The single marker character 離 / 絕 (2 columns wide in every encoding). */
const char* LiJueChar(int kind, int enc);

/* The full name 四離 / 四絕. */
const char* LiJueName(int kind, int enc);

/* 2-character ASCII marker for -a and PostScript output: "LI" / "JE". */
const char* LiJueAscii(int kind);

/* ---- 節 / 氣 ----
   The 24 solar terms are twelve 節 (小寒, 立春, 驚蟄, 清明, 立夏, 芒種, 小暑,
   立秋, 白露, 寒露, 立冬, 大雪) and twelve 氣 (中氣).  bQi: 0 = 節, 1 = 氣. */
const char* JieQiKindName(int bQi, int enc);

/* ---- 三娘煞 ----
   三娘煞 = 農曆每月 初三、初七、十三、十八、廿二、廿七 (通勝歌訣: 上旬初三
   與初七, 中旬十三二十八當, 下旬廿二與廿七, 作事求謀定不昌, 迎親嫁娶無男女).
   真三娘煞 = 這六日之中日柱干支相合者: 初三逢庚午、初七逢辛未、十三逢戊申、
   十八逢己酉、廿二逢丙午、廿七逢丁未 (嫁娶尤忌).
   idx: 0 = 三娘煞, 1 = 真三娘煞.  enc: 'g'/'b'/'u' as above. */
const char* SanNiangShaName(int idx, int enc);

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
