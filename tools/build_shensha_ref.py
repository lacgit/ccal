#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""build_shensha_ref.py -- build tools/shensha_ref.db, the 神煞 reference database.

Sources (read only, never modified):
  shensha.h        -- enum order + 吉/凶 category (trailing enum comment)
  shensha.cpp      -- the ShenShaTbl rows (ASCII / GB / Big5 / UTF-8 simp / UTF-8 trad)
                      and, for markers with no plan row, the rule comment block
  shensha_plan.md  -- the 「已完成」/「待做 — 吉神」/「吉神實作記錄」/「吉神批次」
                      rule tables and the (C) four-year 通勝 verification table
  git log          -- commit_hash (subject match)

Everything is derived; re-running the script regenerates the DB from scratch.
Run from anywhere:  python3 tools/build_shensha_ref.py [--out tools/shensha_ref.db]
"""
import argparse
import os
import re
import sqlite3
import subprocess
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
# when run from <repo>/tools/ the sources live one level up; SHENSHA_REPO overrides
# that for out-of-tree runs.
REPO = Path(os.environ.get('SHENSHA_REPO', HERE.parent))

# ---------------------------------------------------------------- helpers ----

KEYINGS = [
    '月建(節氣月)', '農曆月', '日柱', '年(太歲,立春換歲)', '星期', '月朔', '綜合',
]

# markers whose rule needs BOTH the 月建 分區 and the 日柱 (干支) at once; their
# case body mixes mbranch with 日支/日干 so the plain "mbranch => 月建" default
# would mislabel them (見 shensha_plan.md 卷四 義例二 陰陽不將 條,「陰陽不將」family).
ZONGHE_MBRANCH = {'陽將', '陰將', '俱將', '不將'}

# 伏斷 的起例是「該日值宿 == 日支所配之宿」(協紀卷三十六 辨譌「伏斷日…皆以值宿起算,
# 八十四日而一周」), 即日柱/日支 keyed; shensha.cpp 之所以另外讀 wday, 只因 ccal 唔直接
# 算二十八宿, 而值宿 = (jdn−17) mod 28 由 (日支,星期) 唯一決定.  故標 日柱 (密日 才是
# 唯一 星期-keyed 之神煞, 見 shensha_plan.md 密日行).
KEYING_OVERRIDE = {'伏斷': '日柱'}

# shensha.h enum entries with no trailing /* 名 吉/凶 */ comment; category taken
# from the surrounding code/plan (all four are 吉神, 見 shensha_plan.md §吉神批次
# 及 §待做—吉神: 通勝吉神欄(row2)印之).
CAT_FALLBACK = {'月空': '吉', '金堂': '吉', '青龍': '吉', '除神': '吉'}


def read(path):
    return (REPO / path).read_text(encoding='utf-8')


def cstring_to_bytes(lit):
    """Decode a C string-literal chunk (with \\xNN escapes) to raw bytes."""
    parts = re.findall(r'"((?:[^"\\]|\\.)*)"', lit)
    out = bytearray()
    for part in parts:
        i = 0
        while i < len(part):
            ch = part[i]
            if ch == '\\':
                nxt = part[i + 1]
                if nxt == 'x':
                    out += bytes([int(part[i + 2:i + 4], 16)])
                    i += 4
                elif nxt == 'n':
                    out += b'\n'; i += 2
                elif nxt == 't':
                    out += b'\t'; i += 2
                elif nxt in '\\"':
                    out += nxt.encode(); i += 2
                else:
                    out += nxt.encode('latin-1'); i += 2
            else:
                out += ch.encode('latin-1')
                i += 1
    return bytes(out)


def strip_comments(text):
    return re.sub(r'/\*.*?\*/', ' ', text, flags=re.S)


def clean_md(text):
    """Drop markdown emphasis / inline code, collapse whitespace."""
    text = re.sub(r'\*\*(.+?)\*\*', r'\1', text)
    text = re.sub(r'\*(.+?)\*', r'\1', text)
    text = text.replace('`', '')
    return re.sub(r'\s+', ' ', text).strip()


# ------------------------------------------------------- shensha.h parsing ----

def parse_enum():
    """-> [(enum_name, cat)] in declaration order, SS_COUNT excluded."""
    h = read('shensha.h')
    body = re.search(r'enum \{(.*?)\n\};', h, re.S).group(1)
    out = []
    for line in body.split('\n'):
        line = line.strip()
        if not line.startswith('SS_'):
            continue
        m = re.match(r'(SS_[A-Z0-9_]+)\s*(?:=\s*\d+)?\s*,?\s*(?:/\*\s*(.*?)\s*\*/)?', line)
        name, comment = m.group(1), (m.group(2) or '')
        if name == 'SS_COUNT':
            break
        cat = '吉' if '吉' in comment else ('凶' if '凶' in comment else '')
        out.append((name, cat))
    return out


# ----------------------------------------------------- shensha.cpp parsing ----

def parse_table():
    """-> [(ascii, gb_bytes, b5_bytes, u8sim, u8tra)] in ShenShaTbl order."""
    c = read('shensha.cpp')
    body = c[c.index('static const struct ShenShaDef ShenShaTbl[SS_COUNT] = {'):]
    body = body[:body.index('\n};')]
    row_re = re.compile(
        r'\{\s*("(?:[^"\\]|\\.)*"(?:\s*"(?:[^"\\]|\\.)*")*)\s*,\s*'
        r'("(?:[^"\\]|\\.)*"(?:\s*"(?:[^"\\]|\\.)*")*)\s*,\s*'
        r'("(?:[^"\\]|\\.)*"(?:\s*"(?:[^"\\]|\\.)*")*)\s*,\s*'
        r'("(?:[^"\\]|\\.)*"(?:\s*"(?:[^"\\]|\\.)*")*)\s*,\s*'
        r'("(?:[^"\\]|\\.)*"(?:\s*"(?:[^"\\]|\\.)*")*)\s*\}', re.S)
    rows = []
    for m in row_re.finditer(body):
        g = [cstring_to_bytes(x) for x in m.groups()]
        rows.append((g[0].decode(), g[1], g[2],
                     g[3].decode('utf-8'), g[4].decode('utf-8')))
    return rows


def parse_cases():
    """-> {enum_name: body} for ShenShaOn() switch cases."""
    c = read('shensha.cpp')
    body = c[c.index('int ShenShaOn('):]
    body = body[:body.index('\n}\n', body.index('default:'))] if 'default:' in body \
        else body
    out = {}
    for m in re.finditer(r'case (SS_[A-Z0-9_]+):(.*?)(?=\n    case |\n    default|\n    \})',
                         body, re.S):
        out[m.group(1)] = m.group(2)
    return out


def parse_static_tables():
    """-> {array_name: 'declaration text'} for the file-scope const lookup tables."""
    c = read('shensha.cpp')
    out = {}
    for m in re.finditer(
            r'static const [^;=]*?\b([A-Za-z_][A-Za-z0-9_]*)\s*(\[[^;]*?)\s*=\s*(\{[^;]*?\});',
            c, re.S):
        out[m.group(1)] = (m.group(2) + ' = ' + m.group(3))
    return out


def comment_block_before(source_text, anchor_regex):
    """The /* ... */ block immediately preceding the first line matching anchor."""
    idx = None
    for m in re.finditer(anchor_regex, source_text):
        idx = m.start()
        break
    if idx is None:
        return ''
    head = source_text[:idx]
    end = head.rfind('/*')
    if end == -1:
        return ''
    close = head.find('*/', end)
    if close == -1:
        return ''
    # must be the nearest block (nothing but whitespace between it and the anchor)
    if head[close + 2:].strip():
        return ''
    return head[end:close + 2]


def named_passage(cpp, name, ascii_name):
    """The 註釋 passage that starts with the marker's name (" * 月恩 (YueEn): …"),
    used as a rule/source fallback when shensha_plan.md has no row for it."""
    for needle in (name, ascii_name):
        if not needle:
            continue
        m = re.search(r'\*\s*' + re.escape(needle) + r'\s*[（(:：]', cpp)
        if not m:
            continue
        tail = cpp[m.start():m.start() + 1500]
        # stop at the end of the comment or at the next marker entry (" * 名 (Ascii)")
        cut = re.search(r'\*/|\n\s*\*\s*[\u4e00-\u9fff]{2,6}\s*\([A-Za-z][A-Za-z0-9_]{2,}\)',
                        tail[3:])
        if cut:
            tail = tail[:3 + cut.start()]
        return normalize_block('/*' + tail + '*/')
    return ''


def normalize_block(block):
    txt = strip_comments(block) if not block.startswith('/*') else block
    txt = re.sub(r'^/\*', '', txt)
    txt = re.sub(r'\*/$', '', txt)
    lines = []
    for ln in txt.split('\n'):
        ln = ln.strip()
        ln = re.sub(r'^\*\s?', '', ln)
        if ln:
            lines.append(ln)
    return clean_md(' '.join(lines))


# -------------------------------------------------------- plan.md parsing ----

def parse_plan():
    """-> (rule_rows, c_rows).

    rule_rows: {name: {'rule':.., 'oracle':.., 'commit':.., 'line':.., 'section':..}}
    c_rows:    {name: (我, 通勝, 相同, J, L, 出處)}  -- from section (C) only
    """
    lines = read('shensha_plan.md').split('\n')
    section = ''
    rule_rows, c_rows = {}, {}
    numeric = re.compile(r'^[0-9]+ ?★?$')

    for i, line in enumerate(lines, 1):
        if line.startswith('#'):
            section = line.lstrip('# ').strip()
            continue
        if not line.startswith('|') or re.match(r'^\|[\s\-|]+\|$', line):
            continue
        cells = [c.strip() for c in line.strip().strip('|').split('|')]
        if len(cells) < 3 or not cells[0]:
            continue
        # the (C) verification table is the only 6+ column table whose cells 1..5
        # are bare numbers; the (D) spot-check table also looks like that, so the
        # section heading has to match as well.
        if (len(cells) >= 6 and section.startswith('C.')
                and all(numeric.match(x) for x in cells[1:6])
                and cells[0] not in c_rows):
            c_rows[cells[0]] = tuple(cells[1:6]) + ((cells[6],) if len(cells) > 6 else ('',))
            continue
        name = cells[0]
        for one in split_names(name):
            row = rule_rows.setdefault(one, {'rule': '', 'oracle': '', 'commit': '',
                                             'line': i, 'section': section})
            cand = clean_md(cells[1])
            if len(cand) > len(row['rule']):
                row['rule'] = cand
                row['line'] = i
                row['section'] = section
            if len(cells) >= 3:
                o = clean_md(cells[2])
                # the 吉神批次 table carries the commit in the 4th column
                if re.match(r'^[0-9a-f]{7,}$', cells[3] if len(cells) > 3 else ''):
                    row['commit'] = cells[3][:7]
                if o and ('通勝' in o or '我=' in o or o.startswith('✅') or o.startswith('⚠')):
                    if len(o) > len(row['oracle']):
                        row['oracle'] = o
    return rule_rows, c_rows


def split_names(name):
    """'三娘煞 / 真三娘煞' -> both; '歲支德 (協紀作 歲枝德)' -> 歲支德."""
    name = clean_md(name)
    name = re.sub(r'[（(].*$', '', name).strip()
    parts = [p.strip() for p in re.split(r'\s*/\s*|\s*／\s*', name) if p.strip()]
    return parts or [name]


# ------------------------------------------------------- per-marker derive ----

def derive_keying(enum_name, name, case_body):
    used = set(re.findall(r'\b(mbranch|lmonth|lday|ganzhi|suigz|wday|dbranch|mlendays|mm)\b',
                          strip_comments(case_body)))
    if 'mm' in used:            # mmonth_of(mbranch) computed in the function prologue
        used.add('mbranch')
    if name in KEYING_OVERRIDE:
        return KEYING_OVERRIDE[name]
    if name in ZONGHE_MBRANCH:
        # 陰陽不將 family: the day must satisfy BOTH the 月建-derived 前/後區 split
        # and the 日柱 (日支 + 日干之祿支) at once.
        return '綜合'
    if 'suigz' in used:
        return '年(太歲,立春換歲)'
    if 'wday' in used:
        return '星期'
    if 'lmonth' in used or 'lday' in used:
        if 'ganzhi' in used:
            return '綜合'                 # 農曆日 + 日柱 (三娘煞/真三娘煞)
        if 'dbranch' in used:
            return '月朔'                 # 反支: 日數由該月朔支決定
        return '農曆月'
    if 'mbranch' in used:
        return '月建(節氣月)'
    return '日柱'


# a clause verb only counts when it starts a clause: either it follows the 其日/之日
# lead-in, or the character before it is a punctuation/space boundary.  This keeps
# compounds such as 血忌 / 歸忌 / 宜忌 out of the extraction.
EFFECT_VERB = r'(不忌|不宜|不利|不可|止注|忌|宜)'
EFFECT_PAT = re.compile(r'(?:其日|之日)?' + EFFECT_VERB)
# compounds that must not be read as a clause start: 血忌 / 歸忌 / 宜忌 / 月忌 /
# 忌忌 / 無忌 (不忌 is filtered on its own).
EFFECT_PREV_BAD = set('血歸宜忌月無不')
EFFECT_STOP = re.compile(
    r'[。；;，,、（）()「」《》"\'\n/／]|按語|按\b|卷[0-9一二三四五六七八九十]|曆例|'
    r'曹震圭|考原|李鼎祚|通書|總要曆|樞要曆|神樞經|廣聖曆|天寶曆')
EFFECT_BAD_START = ('忌', '宜', '日', '神', '者', '不', '止', '之', '"', "'")


def derive_effect(rule_text):
    """Conservative extraction of 宜/忌 clauses from the rule cell (原文 wording)."""
    if not rule_text:
        return ''
    out = []
    for m in EFFECT_PAT.finditer(rule_text):
        verb = m.group(1)
        if verb == '不忌':
            continue
        if m.start() == m.start(1):          # no 其日/之日 lead-in
            prev = rule_text[m.start(1) - 1] if m.start(1) else ''
            if prev in EFFECT_PREV_BAD:
                continue
        rest = rule_text[m.end():m.end() + 90]
        if rest[:1] in EFFECT_BAD_START:
            continue
        cut = EFFECT_STOP.split(rest)[0].strip(' 、')
        if len(cut) < 2 or re.match(r'^[\u4e00-\u9fff]{0,2}者', cut):
            continue
        item = verb + cut
        if item not in out:
            out.append(item)
    return '；'.join(out)[:400]


def derive_source(rule_text, ctab_source, extra_text=''):
    """《書名》[卷N [義例M|立成|辨譌|宜忌]] list, in first-appearance order."""
    hits = []
    for text in (rule_text, extra_text):
        if not text:
            continue
        for m in re.finditer(r'《([^》]{1,30})》\s*(卷[0-9一二三四五六七八九十百]{1,4})?\s*'
                             r'(義例[一二三四五六七八九十]{1,2}|立成|辨譌|辨訛|宜忌|'
                             r'建除|總要曆|樞要曆|民俗吉凶日篇)?', text):
            book, vol, hint = m.group(1), m.group(2) or '', m.group(3) or ''
            book = book.replace(' ', '')     # undo markdown line-wrap inside 《…》
            item = '《%s》' % book
            if vol:
                item += vol
            if hint:
                item += ' ' + hint
            if item not in hits:
                hits.append(item)
    if len(hits) > 1:
        hits = [h for h in hits if not any(h != o and h + ' ' in o + ' ' for o in hits)]
    src = '；'.join(hits)
    if not src and rule_text:
        # 曆例 mentions that name 協紀 without the 《》 brackets (e.g. 除神 row)
        m = re.search(r'協紀\s*(卷[0-9一二三四五六七八九十]{1,2})?\s*'
                      r'(義例[一二三四五六七八九十]{1,2}|立成|辨譌|辨訛)?', rule_text)
        if m:
            src = '《欽定協紀辨方書》' + (m.group(1) or '') + (
                (' ' + m.group(2)) if m.group(2) else '')
    if not src and ctab_source and ctab_source not in ('—', '-', ''):
        src = ctab_source
    return src

def derive_rule_lut(rule_text, case_body, statics):
    """The lookup-table value(s) behind the rule, e.g. yuehai_branch[子..亥]={...}."""
    if rule_text:
        m = re.search(r'([A-Za-z_][A-Za-z0-9_]*\s*\[[^\]]{0,40}\]\s*=\s*\{[^}]{0,160}\})',
                      rule_text)
        if m:
            return clean_md(m.group(1))
    body = strip_comments(case_body)
    names = [n for n in re.findall(r'\b([a-z_][a-z0-9_]*)\s*\[', body)
             if n in statics]
    if not names:
        return ''
    out = []
    for n in dict.fromkeys(names):
        decl = re.sub(r'\s+', ' ', statics[n]).strip()
        out.append(n + decl)
    return '; '.join(out)[:400]


def commit_for(name, ascii_name, repo, extra=None):
    """-> (hash, how): how is 'plan' | 'subject' | 'pickaxe' | ''."""
    if extra:
        return extra, 'plan'
    log = subprocess.run(['git', 'log', '--format=%h%x09%s'], cwd=str(repo),
                         capture_output=True, text=True, check=True).stdout
    strict, loose = [], []
    for line in log.split('\n'):
        if '\t' not in line:
            continue
        h, subj = line.split('\t', 1)
        for needle in (name, ascii_name):
            if not needle or needle not in subj:
                continue
            if re.search(r'(^Add\s+%s\b|^shensha[:：]%s\b)' % (re.escape(needle),
                                                              re.escape(needle)), subj):
                strict.append(h)
            else:
                loose.append(h)
            break
    if strict:
        return strict[-1], 'subject'   # git log is newest-first -> last == earliest add
    if loose:
        return loose[-1], 'subject'
    # last resort: the commit that introduced the marker's ASCII key into shensha.cpp
    pick = subprocess.run(['git', 'log', '--format=%h', '-S' + ascii_name,
                           '--', 'shensha.cpp'], cwd=str(repo),
                          capture_output=True, text=True).stdout.split()
    return (pick[-1], 'pickaxe') if pick else ('', '')


# ------------------------------------------------------------------ build ----

def build(out_path):
    enums = parse_enum()
    table = parse_table()
    cases = parse_cases()
    statics = parse_static_tables()
    rule_rows, c_rows = parse_plan()
    cpp = read('shensha.cpp')
    if not (len(enums) == len(table) == len(cases)):
        sys.exit('count mismatch: enum=%d table=%d case=%d'
                 % (len(enums), len(table), len(cases)))

    if out_path.exists():
        out_path.unlink()
    con = sqlite3.connect(str(out_path))
    cur = con.cursor()
    cur.executescript("""
CREATE TABLE shensha (
  idx INTEGER PRIMARY KEY,
  key TEXT UNIQUE NOT NULL,
  enum TEXT NOT NULL,
  name_trad TEXT, name_simp TEXT,
  gb_hex TEXT, big5_hex TEXT,
  cat TEXT,
  keying TEXT,
  rule TEXT,
  rule_lut TEXT,
  effect TEXT,
  source TEXT,
  oracle TEXT,
  status TEXT,
  commit_hash TEXT,
  notes TEXT
);
CREATE TABLE keying_dict (keying TEXT PRIMARY KEY, desc TEXT);
""")

    keying_desc = {
        '月建(節氣月)': '按節氣月/月建 mbranch 起例（立春-雨水為寅月…），非農曆月',
        '農曆月': '按農曆月/日 lmonth/lday 起例（水痕/債不 另需 mlendays 分大月小月）',
        '日柱': '按日柱 60 甲子 gz 或日支 dbranch 起例，不從月建',
        '年(太歲,立春換歲)': '按干支年 suigz（立春換歲, ccal GetSuiGanzhi()）起例',
        '星期': '按星期 wday 起例（密日＝房/虛/昴/星 值宿＝星期日）',
        '月朔': '按農曆月朔支起例，日數 = 6 − 朔支/2（唯一按月朔取日數之 反支）',
        '綜合': '同一條規則同時需 月建 分區與 日柱干支（陰陽不將家族、三娘煞/真三娘煞）',
    }
    for k in KEYINGS:
        cur.execute('INSERT INTO keying_dict VALUES (?,?)', (k, keying_desc[k]))

    notes_prov, problems = [], []
    for idx, (enum_name, cat) in enumerate(enums):
        ascii_name, gb, b5, u8sim, u8tra = table[idx]
        name = u8tra
        cat_from_enum = bool(cat)
        if not cat:
            cat = CAT_FALLBACK.get(name, '')
            if cat:
                notes_prov.append('%s: cat 由 shensha.h 無 enum 注釋, 依通勝吉神欄/協紀判為 %s'
                                  % (name, cat))
        rr = rule_rows.get(name, {})
        rule = rr.get('rule', '')
        ctab = c_rows.get(name)
        passage = named_passage(cpp, name, ascii_name)
        notes = []
        if rr.get('section'):
            notes.append('plan §%s' % rr['section'])
        if not cat_from_enum:
            notes.append('cat 由 shensha.h enum 無注釋, 依通勝吉神欄/協紀判為 %s' % cat)
        if not rule:
            # fall back to the rule comment block in shensha.cpp
            block = ''
            if enum_name in ('SS_XUANWU', 'SS_GOUCHEN', 'SS_BAIHU', 'SS_ZHUQUE',
                             'SS_TIANXING'):
                # 黃黑道十二神 share one rule; the block above huangdao_index()
                block = comment_block_before(cpp, r'static int huangdao_index')
            if not block and passage:
                block = '/*' + passage + '*/'
            if not block:
                case_body = cases[enum_name]
                m = re.search(r'/\*(.*?)\*/', case_body, re.S)
                if m:
                    block = m.group(0)
            rule = normalize_block(block)
            if rule:
                notes_prov.append('%s: rule 由 shensha.cpp 註釋抽（plan 無此行）' % name)
        if not rule:
            problems.append('%s: rule 空' % name)
        keying = derive_keying(enum_name, name, cases[enum_name])
        if keying not in KEYINGS:
            problems.append('%s: keying=%s 不在 7 值內' % (name, keying))
        effect = derive_effect(rule)
        source = derive_source(rule, ctab[5] if ctab else '', passage)
        if source and not derive_source(rule, ''):
            notes_prov.append('%s: source 由 shensha.cpp 註釋補（plan cell 無書名）' % name)
        rule_lut = derive_rule_lut(rule, cases[enum_name], statics)
        commit, how = commit_for(name, ascii_name, REPO, rr.get('commit') or '')
        if how == 'pickaxe':
            notes_prov.append('%s: commit 由 git log -S%s 定（subject 無此名）'
                              % (name, ascii_name))

        oracle = ''
        if ctab:
            oracle = '我=%s／通勝=%s／相同=%s／J=%s／L=%s' % (
                ctab[0].replace(' ', ''), ctab[1], ctab[2], ctab[3], ctab[4])
        elif rr.get('oracle'):
            oracle = rr['oracle']
        else:
            problems.append('%s: oracle 空' % name)

        if not rr.get('rule'):
            notes.append('rule 由 shensha.cpp 註釋抽（plan 無此行）')
        if how == 'pickaxe':
            notes.append('commit 由 git log -S%s 定（subject 無此名）' % ascii_name)
        elif how == 'plan':
            notes.append('commit 取自 plan 吉神批次表 commit 欄')
        if not ctab:
            notes.append('C 表無數字')
        if not commit:
            notes.append('git log 無專門 commit')
        if not effect:
            notes.append('rule 無宜/忌原文, effect 留空')
        if not source:
            notes.append('source 無出處可抽（plan cell 及 shensha.cpp 註釋皆無書名）')
        elif not derive_source(rule, ''):
            notes.append('source 非 plan cell 直出（由 shensha.cpp 註釋或 plan C 表出處欄補）')
        cur.execute('INSERT INTO shensha VALUES (' + ','.join(['?'] * 17) + ')',
                    (idx, ascii_name, enum_name, u8tra, u8sim,
                     gb.hex(), b5.hex(), cat, keying, rule, rule_lut, effect, source,
                     oracle, '完成', commit, '; '.join(notes)))
    con.commit()

    # hard acceptance checks
    bad = cur.execute("SELECT idx,key,enum,name_trad FROM shensha WHERE key IS NULL OR "
                      "key='' OR enum IS NULL OR enum='' OR name_trad IS NULL OR "
                      "name_trad=''").fetchall()
    if bad:
        problems.append('空洞: %r' % (bad,))
    dup = cur.execute('SELECT key, COUNT(*) c FROM shensha GROUP BY key HAVING c>1')\
        .fetchall()
    if dup:
        problems.append('duplicate key: %r' % (dup,))
    n = cur.execute('SELECT COUNT(*) FROM shensha').fetchone()[0]
    con.close()
    return n, problems, notes_prov


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--out', default=str(HERE / 'shensha_ref.db'))
    a = ap.parse_args()
    n, problems, notes_prov = build(Path(a.out))
    print('wrote %s: %d rows (= SS_COUNT)' % (a.out, n))
    if notes_prov:
        print('-- provenance notes (%d) --' % len(notes_prov))
        for x in notes_prov:
            print('  ', x)
    if problems:
        print('!! %d open items:' % len(problems))
        for x in problems:
            print('  ', x)
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
