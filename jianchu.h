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

#endif /* JIANCHU_H */
