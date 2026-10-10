#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""ssq.py -- 神煞查詢小工具 (query the 神煞 reference database).

  python3 tools/ssq.py 月害                 一個神煞嘅完整資料
  python3 tools/ssq.py 月害 --field rule    只印某一欄 (rule/effect/source/oracle/…)
  python3 tools/ssq.py --list               全部 133 個 (idx key enum 名 簡 吉凶 keying …)
  python3 tools/ssq.py --list --cat 凶      只列凶煞
  python3 tools/ssq.py --list --keying 月建  只列以節氣月/月建起例嘅神煞
  python3 tools/ssq.py --list --json        逐行一個 JSON object
  python3 tools/ssq.py --search 月          名/key/enum 模糊搜

名可以係繁體名、簡體名、ASCII key (YueHai) 或 enum (SS_YUEHAI)；搵唔到會列出近似項。
只讀 DB，唔會改任何嘢。
"""
import argparse
import json
import signal
import sqlite3
import sys
from pathlib import Path

try:                      # keep `ssq.py --list | head` from spewing BrokenPipeError
    signal.signal(signal.SIGPIPE, signal.SIG_DFL)
except (AttributeError, ValueError):
    pass

FIELDS = ['idx', 'key', 'enum', 'name_trad', 'name_simp', 'gb_hex', 'big5_hex',
          'cat', 'keying', 'rule', 'rule_lut', 'effect', 'source', 'oracle',
          'status', 'commit_hash', 'notes']
DEFAULT_DB = Path(__file__).resolve().parent / 'shensha_ref.db'
LIST_COLS = ['idx', 'key', 'enum', 'name_trad', 'name_simp', 'cat', 'keying',
             'effect', 'source', 'oracle', 'commit_hash']


def load(db):
    if not Path(db).exists():
        sys.exit('搵唔到 %s — 先跑 python3 tools/build_shensha_ref.py 生成' % db)
    con = sqlite3.connect(str(db))
    con.row_factory = sqlite3.Row
    return con


def find(con, term):
    """Exact match on 名/key/enum first, then a substring match on any of them."""
    for col in ('name_trad', 'name_simp', 'key', 'enum'):
        rows = con.execute('SELECT * FROM shensha WHERE %s = ?' % col, (term,)).fetchall()
        if rows:
            return rows
    pat = '%%%s%%' % term
    return con.execute(
        'SELECT * FROM shensha WHERE name_trad LIKE ? OR name_simp LIKE ? '
        'OR key LIKE ? OR enum LIKE ? ORDER BY idx', (pat, pat, pat, pat)).fetchall()


def show(con, row):
    for f in FIELDS:
        v = row[f]
        if v in (None, ''):
            v = '—'
        print('%-11s %s' % (f, v))


def main():
    ap = argparse.ArgumentParser(add_help=True, description='神煞參考資料庫查詢')
    ap.add_argument('name', nargs='?', help='神煞名 / ASCII key / enum')
    ap.add_argument('--db', default=str(DEFAULT_DB))
    ap.add_argument('--list', action='store_true', help='列全部（可加 --cat/--keying）')
    ap.add_argument('--search', metavar='TERM', help='模糊搜名/key/enum')
    ap.add_argument('--cat', help='吉 / 凶')
    ap.add_argument('--keying', help='月建 / 農曆月 / 日柱 / 年 / 星期 / 月朔 / 綜合')
    ap.add_argument('--field', help='只印某一欄 (%s)' % '/'.join(FIELDS))
    ap.add_argument('--json', action='store_true', help='JSON 輸出')
    a = ap.parse_args()
    con = load(a.db)

    if a.list or a.search or (a.cat and not a.name) or (a.keying and not a.name):
        sql, args = 'SELECT * FROM shensha', []
        wheres = []
        if a.cat:
            wheres.append('cat = ?'); args.append(a.cat)
        if a.keying:
            wheres.append('keying LIKE ?'); args.append(a.keying + '%')
        if a.search:
            wheres.append('(name_trad LIKE ? OR name_simp LIKE ? OR key LIKE ? OR enum LIKE ?)')
            args += ['%%%s%%' % a.search] * 4
        if wheres:
            sql += ' WHERE ' + ' AND '.join(wheres)
        sql += ' ORDER BY idx'
        rows = con.execute(sql, args).fetchall()
        if a.json:
            for r in rows:
                print(json.dumps({f: r[f] for f in FIELDS}, ensure_ascii=False))
        else:
            print('\t'.join(LIST_COLS))
            for r in rows:
                out = []
                for c in LIST_COLS:
                    v = r[c] if r[c] not in (None, '') else '—'
                    v = str(v).replace('\t', ' ')
                    if c in ('effect', 'source', 'oracle') and len(v) > 48:
                        v = v[:47] + '…'
                    out.append(v)
                print('\t'.join(out))
            print('# %d 個' % len(rows))
        return 0

    if not a.name:
        ap.print_help()
        print('\n提示: python3 tools/ssq.py 月害 ；python3 tools/ssq.py --list --cat 凶')
        return 2

    rows = find(con, a.name)
    if not rows:
        print('冇「%s」呢個神煞。' % a.name)
        like = con.execute('SELECT name_trad FROM shensha ORDER BY idx').fetchall()
        print('全部 133 名: ' + '、'.join(r[0] for r in like))
        return 1
    if a.json:
        for r in rows:
            print(json.dumps({f: r[f] for f in FIELDS}, ensure_ascii=False))
        return 0
    if a.field:
        if a.field not in FIELDS:
            sys.exit('--field 只可以是 %s' % '/'.join(FIELDS))
        for r in rows:
            print(r[a.field] if r[a.field] not in (None, '') else '—')
        return 0
    for i, r in enumerate(rows):
        if i:
            print('-' * 60)
        show(con, r)
    if len(rows) > 1:
        print('# 命中 %d 個（要精確就畀完整名）' % len(rows))
    return 0


if __name__ == '__main__':
    sys.exit(main())
