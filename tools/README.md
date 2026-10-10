# tools/ — 神煞參考資料庫

神煞（ccal 日層 almanac 標記）嘅參考資料庫：每個神煞嘅**起例(rule)**、**對擇日嘅影響(宜/忌)**、
**出處**、**四年通勝 DB 核對數字**，收埋喺一個 SQLite 表度，方便日後查閱。

工具**唯讀**讀 source（`shensha.h` / `shensha.cpp` / `shensha_plan.md` / `git log`），
唔會改動任何現有 code 或輸出格式；DB 由 source 重新 build，可以隨時重跑。

| 檔案 | 用途 |
|---|---|
| `build_shensha_ref.py` | 生成器：由 source 重新 build 個 DB（可重跑、idempotent） |
| `shensha_ref.db` | 生成出嚟嘅 SQLite（可重建，仍然 commit 一份方便查） |
| `ssq.py` | 查詢小工具（CLI，只用 python3 標準庫 + sqlite3 模組） |
| `README.md` | 本檔：schema、資料來源、keying 分類法、用法 |

## 用法

```sh
# 重建 DB（喺 repo 根目錄）
python3 tools/build_shensha_ref.py                 # -> tools/shensha_ref.db
python3 tools/build_shensha_ref.py --out /tmp/x.db # 指定輸出

# 查一個神煞（繁體名／簡體名／ASCII key／enum 皆可）
python3 tools/ssq.py 月害
python3 tools/ssq.py YueHai
python3 tools/ssq.py SS_YUEHAI
python3 tools/ssq.py 月害 --field rule        # 只印一欄
python3 tools/ssq.py 月害 --json              # 一行一個 JSON

# 列／過濾
python3 tools/ssq.py --list                   # 全部 133 個
python3 tools/ssq.py --list --cat 凶          # 只列凶煞
python3 tools/ssq.py --list --keying 月建      # 只列按節氣月/月建起例嘅
python3 tools/ssq.py --search 月               # 名/key/enum 模糊搜

# 直接查 DB（sqlite3 CLI 唔一定裝；python3 一定有）
python3 -c "import sqlite3;print(sqlite3.connect('tools/shensha_ref.db').execute('select name_trad,keying from shensha limit 5').fetchall())"
```

## Schema

```sql
CREATE TABLE shensha (
  idx INTEGER PRIMARY KEY,   -- ShenShaTbl 次序(0-based) = enum 次序
  key TEXT UNIQUE NOT NULL,  -- ASCII 名 (YueHai)
  enum TEXT NOT NULL,        -- SS_YUEHAI
  name_trad TEXT, name_simp TEXT,   -- 由表行 UTF-8 欄解出
  gb_hex TEXT, big5_hex TEXT,       -- 表行 GB2312 / Big5 bytes (hex)
  cat TEXT,                  -- 吉 / 凶（由 shensha.h enum 注釋）
  keying TEXT,               -- 見下面「keying 七值」
  rule TEXT,                 -- 起例（plan 原文，含曆例引文）
  rule_lut TEXT,             -- 查表值, 例 yuehai_branch[mbranch 子..亥] = {7,6,5,…}
  effect TEXT,               -- 對擇日嘅影響(宜/忌), 盡量引原文；抽唔到留空
  source TEXT,               -- 出處（《欽定協紀辨方書》卷X 義例Y / 通書 / 《玉匣記》…）
  oracle TEXT,               -- 四年通勝 DB 核對: 我=N／通勝=M／相同=K／J=A／L=B
  status TEXT,               -- 完成 / 略(非日神煞) / 未實作
  commit_hash TEXT,
  notes TEXT                 -- 逐行 provenance（邊個欄由邊度抽、邊樣留空）
);
CREATE TABLE keying_dict (keying TEXT PRIMARY KEY, desc TEXT);
```

## 資料來源（優先次序；實作相符）

1. `shensha_plan.md` 嘅規則表 —— 「已完成」表（88 行）、「待做 — 吉神」表（30 行）、
   「吉神實作記錄」表（三合/六合/五合/進神/解神/不將）、「吉神批次」表（17 行）。
   同名多行時取**最長嘅 rule cell**，commit 欄（吉神批次表）優先。
2. `shensha.h` enum 行 + 注釋 → `enum`（次序／名）同 `cat`（吉/凶）。
3. `shensha.cpp` 表行（ASCII/GB/Big5/U8簡/U8繁）→ `key`、`gb_hex`、`big5_hex`、名。
4. plan 冇 rule 行嘅神煞（反支、玄武、勾陳、白虎、朱雀、天刑、天火、紅沙）→
   由 `shensha.cpp` 註釋抽（玄武等五個共用「黃黑道十二神」那段；其餘用 `* 名 (Ascii): …` 段落）。
   呢 8 行喺 `notes` 有記。
5. plan 嘅 (C)「各神煞驗證數字總表」→ `oracle`（我/通勝/相同/J/L，`★` = 該神煞原卡無數字、由收尾卡重算）。
6. `git log` —— 先揀 subject 含名（`Add 名`／`shensha:名` 優先），冇先算全文 grep，
   再冇就用 `git log -S<key> -- shensha.cpp`（唯一一例：四廢 → `83a4f34`）。
7. `effect` —— 由 rule 原文抽「其日忌X」「宜X」「止注X」「不利X」「不可X」等子句（保留原文用詞）。
   抽唔到就**留空**（133 行中 39 行有 effect），`notes` 會標「rule 無宜/忌原文, effect 留空」。
8. `source` —— 由 rule 原文抽 `《書名》卷X 義例Y`；plan cell 冇書名時依次用
   `shensha.cpp` 註釋、plan (C) 表「出處」欄補。真係冇就留空（17 行，多數係 plan cell 只有起例、
   冇引書嘅列，例：天赦「春戊寅 夏甲午 秋戊申 冬甲子」）。
9. `rule_lut` —— rule cell 內嘅查表（`name[索引] = {…}`），冇就由 `shensha.cpp` 嘅
   `static const` 表抄（91 行有值）。

## keying 七值

| keying | 意思 | 數 |
|---|---|---|
| `月建(節氣月)` | 按節氣月/月建 mbranch 起例（非農曆月） | 101 |
| `日柱` | 按日柱 60 甲子／日支起例，不從月建 | 12 |
| `年(太歲,立春換歲)` | 按干支年 suigz（立春換歲） | 7 |
| `綜合` | 同一條規則同時需兩種 keying | 6 |
| `農曆月` | 按農曆月/日（水痕/債不 另需 mlendays 分大月小月） | 5 |
| `月朔` | 按該農曆月朔支取日數（唯一：反支） | 1 |
| `星期` | 按星期 wday（唯一：密日＝房/虛/昴/星 值宿＝星期日） | 1 |

分類法（`build_shensha_ref.py` 自動判，唔靠人手填）：

* case body（**剔走註釋**）用到 `suigz` → 年；用到 `wday` → 星期；用到 `lmonth`/`lday` →
  農曆月（若同時用 `ganzhi` → 綜合；若用 `dbranch` 取朔支 → 月朔）；用到 `mbranch`（包括
  函數開頭 `mm = mmonth_of(mbranch)`）→ 月建；其餘 → 日柱。
* 兩個明確 override（各有理由，寫喺 code 註釋）：
  * `陽將/陰將/俱將/不將` → 綜合（要同時用月建分後區**同**日柱干支，唔可以只當月建）；
  * `伏斷` → 日柱（起例係「該日值宿 == 日支所配之宿」，協紀卷三十六 辨譌）；
    `shensha.cpp` 之所以另讀 `wday`，只因 ccal 唔直接算 28 宿，而值宿 `(jdn−17) mod 28`
    由 (日支,星期) 唯一決定。密日 才是唯一 星期-keyed 之神煞。

## 內容統計（build 出嚟 133 行 = `SS_COUNT`）

* `key` / `name_trad` / `enum` 133 行全部非空；`keying` 只出現上表 7 個值。
* `cat`：吉 63、凶 70。其中 4 個（月空/金堂/青龍/除神）`shensha.h` 冇 enum 注釋，
  依通勝吉神欄 + 協紀判為吉，`notes` 有記。
* `oracle` 133/133（全部有四年通勝數字）、`commit_hash` 133/133、
  `rule` 133/133、`rule_lut` 91/133、`effect` 39/133、`source` 116/133。
* `status` 全部 `完成` —— DB 只收 `ShenShaTbl` 內實際實作咗嘅神煞（每行都有 `enum`）。

## 未收落 DB 嘅神煞（唔喺 `ShenShaTbl`，故無 `enum`）

* **略去**（無日曆例、非通書可算之日神煞）：天財 / 地財 / 月財 / 故墓。
* **未實作(todo)**：天貴（非日神煞，規則核唔實）、披麻、狼籍、猶忌、人隔、山隔、鼓輪、
  陰陽俱錯（五月丙午、十一月壬子）。

（見 `shensha_plan.md` §E、§F。呢批冇 `enum`／表行，加落 DB 會整污糟「每行 key/name/enum 非空」
嘅驗收條件，故只喺此處列出。）

## 注意

* 容器/部分環境冇 `sqlite3` CLI；`ssq.py` 只用 python3 `sqlite3` 模組，唔需要 CLI。
* `build_shensha_ref.py` 呢個 repo 內嘅版本預設由 `<repo>/tools/` 上一層讀 source；
  想喺其他位置跑可以設 `SHENSHA_REPO=<repo 路徑>`。
* `rule` / `oracle` / `notes` 嘅文字係 plan/code 原文（只清 markdown 強調號同收窄空白），
  刻意保留原文用詞（包括「通勝版面收錄差異」「J」等 jargon）以便回溯。
