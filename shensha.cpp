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
 *
 * 大耗: 《欽定協紀辨方書》卷四 "厯例曰大耗者正月起申順行十二辰";
 * 曹震圭 "大耗者月建擊衝破散之辰也,與月破同位" -- the day whose 日支
 * clashes the 月建 (same position as 月破).
 *
 * 災煞: 《欽定協紀辨方書》卷二十 monthly tables -- one of the 三煞,
 * keyed to the solar-term month (節氣月, i.e. the 月建 used by 月破/大耗).
 * Within each month's 三合局 it sits on the 胎 position (劫煞前一辰):
 *   寅午戌月->子日, 亥卯未月->酉日, 巳酉丑月->卯日, 申子辰月->午日.
 *
 * 月厭: the day whose 日支 sits on "正月起戌逆行十二辰", keyed to the
 * solar-term month (節氣月/月建, same convention as 月破/大耗/災煞):
 *   寅月->戌日, 卯月->酉日, 辰月->申日, 巳月->未日, 午月->午日, 未月->巳日,
 *   申月->辰日, 酉月->卯日, 戌月->寅日, 亥月->丑日, 子月->子日, 丑月->亥日.
 *
 * 月刑 (YueXing): the day whose 日支 is punished by the month's build branch
 * (月建之刑, three-punishment / 建刑), keyed to the solar-term month (節氣月/月建)
 * like its siblings 月破/大耗/災煞/月厭.  Cycles yin-si-shen / chou-xu-wei,
 * mutual zi-mao, self-punishments chen/wu/you/hai:
   寅->巳(5), 卯->子(0), 辰->辰(4), 巳->申(8),
   午->午(6), 未->丑(1), 申->寅(2), 酉->酉(9),
   戌->未(7), 亥->亥(11), 子->卯(3), 丑->戌(10).
 *
 * 大時 (DaShi): the day whose 日支 sits on the 咸池/桃花 position of the
 * month's 三合局 (曹震圭 "大時者乃月建三合五行沐浴之辰也"; 李鼎祚 "大時者
 * 正月起卯逆行四仲" -- 卯/子/酉/午 cycle), keyed to the solar-term month
 * (節氣月/月建) like its siblings 月破/大耗/災煞/月厭/月刑:
   寅月->卯日, 卯月->子日, 辰月->酉日, 巳月->午日, 午月->卯日, 未月->子日,
   申月->酉日, 酉月->午日, 戌月->卯日, 亥月->子日, 子月->酉日, 丑月->午日.
 *
 * 天吏 (TianLi): 《欽定協紀辨方書》卷五 義例三 -- 樞要曆曰"天吏者,月中凶神也,
 * 其日忌臨官赴任,遠行詞訟", 曆例曰"天吏者,正月起酉,逆行四仲", 曹震圭曰"天吏
 * 者,三合五行死氣之位"; 卷十 宜忌 "按天吏又名致死,為三合死地".  So the day
 * whose 日支 is the 死 position of the month's 三合局, keyed to the 節氣月/月建
 * like its siblings 月破/大耗/災煞/月厭/月刑 (寅申巳亥月->危日, 辰戌丑未月->閉日,
 * 子午卯酉月->平日 -- the 建除 officer the rule lands on, also per 卷十):
 *   寅午戌月->酉日, 亥卯未月->午日, 巳酉丑月->子日, 申子辰月->卯日.
 *
 * 血忌 (XueJi): the day whose 日支 is the month's 血忌 branch, keyed to the
 * solar-term month (節氣月/月建/斗建) like its siblings.  《攝生月令》
 * (雲笈七籤卷三十六) lists all twelve by 斗建:
 *   斗建寅->丑日, 卯->未日, 辰->寅日, 巳->申日, 午->卯日, 未->酉日,
 *   申->辰日, 酉->戌日, 戌->巳日, 亥->亥日, 子->午日, 丑->子日,
 * i.e. the classical 通書 曆例 正月丑 二月未 三月寅 四月申 五月卯 六月酉
 * 七月辰 八月戌 九月巳 十月亥 十一月午 十二月子.  忌針刺出血之事.
 *
 * 血支 (XueZhi): the day whose 日支 is the month-build's previous branch
 * (月建前一位), i.e. the 建除 "閉" day, keyed to the solar-term month
 * (節氣月/月建) like its siblings 月破/大耗/災煞/月厭/月刑/大時/天吏/血忌.
 * 《星歷考原》歷例「血支者，正月在丑，順行十二辰」 -> 正月(寅月)丑日,
 * 卯月寅日, 辰月卯日 ... 丑月子日; 《欽定協紀辨方書》義例 equates 閉 with
 * 血支 (「閉之為血忌...閉又為血支」/「閉在十二辰,猶小冬也」), 其日忌針刺出血:
 *   寅月->丑日, 卯月->寅日, 辰月->卯日, 巳月->辰日, 午月->巳日, 未月->午日,
 *   申月->未日, 酉月->申日, 戌月->酉日, 亥月->戌日, 子月->亥日, 丑月->子日.
 *
 * 五墓 (WuMu): the day whose 日柱 is the 墓辰 of that month's 旺干, per
 * 《欽定協紀辨方書》卷五 義例三 (廣聖曆 "五墓者,四旺之墓辰也,其日忌營造起土
 * 嫁娶出軍"; 曆例 "五墓者正二月乙未、四五月丙戌、七八月辛丑、十月十一月
 * 壬辰、四季月戊辰也"; 曹震圭 "五墓者五行旺干自臨墓辰也").  Keyed to the
 * 節氣月/月建 like its siblings 月破/大耗/災煞/月厭/月刑:
 *   寅月/卯月->乙未日, 巳月/午月->丙戌日, 申月/酉月->辛丑日,
 *   亥月/子月->壬辰日, 辰月/未月/戌月/丑月(四季月)->戊辰日.
 *
 * 復日 (FuRi): the day whose 日干 repeats the intrinsic (本氣) stem of the 月建
 * branch, keyed to the solar-term month (節氣月/月建) like its siblings
 * 月破/大耗/災煞/月厭/月刑/大時/天吏/血忌/血支/五墓.  《欽定協紀辨方書》卷五 義例三:
 * 天寶曆「復日者為魁罡所系之辰也, 其日忌為凶事利為吉事」; 曆例「復日者,
 * 正七月甲庚, 二八月乙辛, 四十月丙壬, 五十一月丁癸,
 * 三九六十二月戊己日也」; 曹震圭「復者重見也, 為本建之辰與所遇之干同也,
 * 假令正月寅即甲也, 而又見甲是復也」; 地理新書「正月甲、七月庚、二月乙、八月辛、
 * 四月丙、十月壬、五月丁、十一月癸、三月九月戊、六月十二月己」.
 * I.e. 日干 == 本氣干 of the 月建 branch -- 寅甲 卯乙 辰戊 巳丙 午丁 未己 申庚 酉辛 戌戊 亥壬 子癸 丑己:
 *   寅月->甲日, 卯月->乙日, 辰月->戊日, 巳月->丙日, 午月->丁日, 未月->己日,
 *   申月->庚日, 酉月->辛日, 戌月->戊日, 亥月->壬日, 子月->癸日, 丑月->己日.
 * 其日忌為凶事 (尤忌安葬破土), 利為吉事 *
 * 天賊 (TianZei): the day whose 日支 sits on "正月在丑,逆行十二辰" (李鼎祎;
 * 《欽定協紀辨方書》卷五 義例 "天賊者,盜神也,常居天倉後辰", i.e. 月厭之收日),
 * keyed to the solar-term month (節氣月/月建) like its siblings
 * 月破/大耗/災煞/月厭/月刑/大時/天吏/血忌/血支.  天賊 = 月厭 + 3
 * (月厭 正月起戌逆行, 天賊 正月在丑):
 *   寅月->丑日, 卯月->子日, 辰月->亥日, 巳月->戌日, 午月->酉日, 未月->申日,
 *   申月->未日, 酉月->午日, 戌月->巳日, 亥月->辰日, 子月->卯日, 丑月->寅日.
 *
 * 歸忌 (GuiJi): the day whose 日支 is the 歸忌 branch of the month, keyed to the
 * 節氣月/月建 like its siblings 月破/大耗/災煞/月厭/月刑/大時/天吏/血忌/血支.
 * 《欽定協紀辨方書》義例「孟月丑,仲月寅,季月子」, 考原 曰「孟月忌丑、仲月忌寅、
 * 季月忌子者,皆忌退後一辰,所謂歸忌也」 (六壬 "章光日,孟月乙丑,仲月丙寅,季月
 * 甲子,即歸忌煞也"); 陰陽書曆法 「歸忌日,四孟在丑,四仲在寅,四季在子,其日不可
 * 遠行、歸家及徙也」.  Index = mbranch 0=子..11=亥:
 *   四孟(寅巳申亥)->丑日, 四仲(子卯午酉)->寅日, 四季(辰未戌丑)->子日.
 *
 * 月煞 (YueSha): 《欽定協紀辨方書》 廣聖曆「月殺者,月內之殺神也,其日忌停賓客,
 * 興穿掘,營種殖,納群畜」; 曆例「月煞者,正月起丑, 逆行四季」, 按「在歲為歲煞,
 * 在月為月煞, 無二義也」 -- 即月建三合局之養位 (三煞之歲煞; 《三命通會》「月煞:
 * 寅午戌月丑, 亥卯未月戌, 申子辰月未, 巳酉丑月辰」).  Keyed to the 節氣月/月建
 * like its siblings 月破/大耗/災煞/月厭/月刑:
 *   寅月->丑日, 卯月->戌日, 辰月->未日, 巳月->辰日, 午月->丑日, 未月->戌日,
 *   申月->未日, 酉月->辰日, 戌月->丑日, 亥月->戌日, 子月->未日, 丑月->辰日.
 *
 * 密日 (MiRi): the day whose 值宿 is one of the four 日曜 mansions 房/虛/昴/星
 * (房日兔、虛日鼠、昴日雞、星日馬) -- the 日曜日, i.e. a Sunday.
 * 《欽定協紀辨方書》卷三十六 辨譌: "伏斷日密日裁衣日皆以值宿起算, 查二十八宿選擇
 * 之法來自西域 ... 密日者乃房虛星昴四宿, 七政屬日, 西語曰密, 彼地以為主喜事,
 * 而中國遂以其日忌安葬啟攢凶事"; 同卷又列 密日 為奏准刪去之造葬條.
 * The 二十八宿 cycle is 28 = 4x7 days, so the four 日曜 mansions always land on
 * the same weekday; in all four oracle 通勝 volumes (2006/2010/2025/2026) the day
 * whose own 值宿 column reads 房/虛/昴/星 is the Sunday -- their 值宿 column fits
 * (jdn - 17) % 28 for 97% of rows and every one of those 密日 days is a Sunday.
 * 其日忌安葬、啟攢 (凶).  This is the only marker keyed to the weekday, hence the
 * extra wday input (0 = Sunday .. 6 = Saturday).
 *
 * 重日 (ChongRi): the day whose 日支 is 巳 or 亥 (the 陽極/陰極 positions).
 * 《欽定協紀辨方書》卷五 義例三: 天寶歷「重日者,以陰陽混合於亥, 陽起於甲子而順,
 * 陰起於甲戌而逆, 至己亥而同, 故曰重日, 其日忌為凶事利為吉事」;
 * 歷例「重日者,己亥二日也」 (四庫本作「己」, 巳/己 形近之訝);
 * 曹震圭「亥為陰極之位, 坤辟在焉; 巳為陽極之位, 乾辟在焉,
 * 是陽中陽而陰中陰也, 故曰重」 -- 不論天干, 亦不論月建:
 *   日支 巳(5) 或 亥(11) -> 重日.
 *
 * 短星 (DuanXing): 時憲書/通書 之日神煞, 按 **農曆月/日** keying（非其兄弟神煞所用之
 *  節氣月/月建）, 即每月固定一日、八月九月各二日, 無論月之大小有閏無閏每年皆然:
 *   正月廿一、二月十九、三月十六、四月廿五、五月廿五、六月二十、七月廿二、
 *   八月十八/十九、九月十六/十七、十月十四、十一月廿二、十二月廿五.
 *  《欽定協紀辨方書》卷九 立成「短星 正月〈二十一日〉二月〈十九日〉三月〈十六日〉
 *  四月〈二十五日〉五月〈二十五日〉六月〈二十日〉七月〈二十二日〉八月〈十八…」;
 *  御定星曆考原「短星者,正月二十一,二月十九,三月十六,四月二十五,五月二十五,
 *  六月二十,七月二十二,八月十八、十九,九月十六、十七,十月十四,十一月二十二,
 *  十二月二十五日是也」.  曹震圭 以 長星為金、短星為火;
 *  《協紀辨方書》卷三十六 辨訛 引時憲書「長星短星每月各一日八九月各二日
 *  無論月之大小有閏無閏每年皆然」, 謂其日只忌 裁衣、開市、立券、交易、納財.

 *
 * 反支 (FanZhi): 凶.  《欽定協紀辨方書》卷六 義例四 (引《後漢書》王符傳注,
 * "見陰陽書也"): 「反支日用月朔為正 -- 戌亥朔一日反支, 申酉朔二日反支,
 * 午未朔三日反支, 辰巳朔四日反支, 寅夘朔五日反支, 子丑朔六日反支」;
 * 卷九 立成 「日神按月朔取日數者」 同; 卷十 宜忌 「忌上冊進表章陳詞訟」.
 * 按 「反支之義, 惡其將盡也」 -- 反支 = 該農曆月的第 (6 - 朔支/2) 日
 * (朔支 子丑=六日, 寅夘=五日, 辰巳=四日, 午未=三日, 申酉=二日, 戌亥=一日).
 * 唯一按月朔 (農曆月) 取日數的神煞 -- 與 月建(節氣月)-keyed 的兄弟不同.
 *
 * 陽將 (YangJiang): 《欽定協紀辨方書》卷四 義例二 天寶曆「陰陽不將者,以月建為陽謂之陽建(正月起寅順行十二辰),
 * 月厭為陰謂之陰建(正月起戌逆行十二辰); 分於卯酉,會於子午。厭前枝幹自相配者為陽將,厭後枝幹自相配者為陰將;
 * 厭後幹配厭前枝者為陰陽俱將,厭前幹配厭後枝者為陰陽不將也。陽將傷夫,陰將傷婦」。  Keyed to the 節氣月/月建 like its
 * siblings (月破/大耗/災煞/月厭/月刑/大時/天吏/血忌/血支/復日/天賊/歸忌/五墓):
 *   月厭 A = (12 - 月建) mod 12;  前區 = {A+6 .. A+11} (6 辰), 後區 = {A .. A+5};
 *   「自相配」 = the day's 日支 AND the 日干's 祿支 (甲寅 乙卯 丙巳 丁午 戊丑 己未 庚申 辛酉 壬亥 癸子)
 *   both lie in 前區 -- so 陽將 = 日支 ∈ 前區 且 祿(日干) ∈ 前區.
 *   (陽將/陰將 are the 凶 halves of the 不將 family; the 不將 day lists of 卷四 are
 *   reproduced exactly by 不將 = 日支 ∈ 後區\{月厭} 且 祿(日干) ∈ 前區.)
 *
 * 陰將 (YinJiang): 同段 曆例 -- 陽將/陰將 是 陰陽不將 家族的一對凶日.
 *   「厭後支幹自相配者為陰將」 -> 陰將 = 日支 ∈ 後區 且 日干之祿支 ∈ 後區,
 *   月厭 A = (12 - 月建) mod 12;  後區(支) = {A+1 .. A+5},
 *   後區(干) = 祿支 ∈ {A .. A+5} (甲寅 乙卯 丙巳 丁午 戊丑 己未 庚申 辛酉 壬亥 癸子).
 *   「支幹自相配」 取 日支 與 日干之祿支 同區; 後區 只取 A+1..A+5 (月厭自身與其沖 厭對
 *   不取), 與 卷四 不將 日表 的取法一致 (該表列出的正月至十二月不將日, 用
 *   不將 = 日支 ∈ 後區\{月厭} 且 祿 ∈ 前區 全部對中).  陽將傷夫, 陰將傷婦,
 *   其日忌嫁娶.  Keyed to the 節氣月/月建 like its siblings. *
 * 火星 (HuoXing): the day whose 日柱 is one of the month's 火星日, keyed to the
 * solar-term month (節氣月/月建) like its siblings 月破/大耗/災煞/月厭/月刑/
 * 大時/天吏/血忌/血支/復日/天賊/歸忌/五墓.  通書/《玉匣記》民俗吉凶日篇「火星日」
 * (忌修造、起蓋、砌灶、裁衣等事) lists them by 月之孟仲季:
 *   正四七十月 (四孟: 寅巳申亥月) 乙丑 甲戌 癸未 壬辰 辛丑 庚戌 己未;
 *   二五八十一月 (四仲: 子卯午酉月) 甲子 癸酉 壬午 辛卯 庚子 己酉 戊午;
 *   三六九十二月 (四季: 辰未戌丑月) 壬申 辛巳 庚寅 己亥 戊申 丁巳.
 * Each set is the 60-甲子 arithmetic progression with common difference 9
 * (日干退一、日支退三).  《欽定協紀辨方書》卷三十四 火星條 只以火星為方位凶神
 * (「火星乃三合旺方即打頭火也」; 卷三十三 又斥日家諸火星為偽造), 故日家之火星
 * 以通書為準.  Index = mbranch % 3 (0 = 四仲月, 1 = 四季月, 2 = 四孟月); every
 * listed 干支 has 序號 % 9 = 0 (仲) / 8 (季) / 1 (孟), i.e.
  hx_off[mbranch 子..亥] = {0, 8, 1, 0, 8, 1, 0, 8, 1, 0, 8, 1}.
 *
 * 五離 (WuLi): 凶.  日支 = 申 或 酉 -- 日神取一定干支者, 不從月建, 亦不論天干
 * (與五合「寅卯」對衝).  《欽定協紀辨方書》卷五 義例三「五離〈除神〉」:
 * 樞要曆「五離者,月中離神也,其日忌結婚姻、會親友、作交關、立契券」; 曆例
 * 「五離者,申酉日是也」; 曹震圭「五離者是陰陽重會之辰,再合則離也」; 考原
 * 「按通書以申酉日為除神,與五合對衝之日也,宜沐浴祓除災眚」.  卷九 立成
 * 「日神取一定干支者」: 五合 寅卯 / 除神〈五離〉申酉 / 重日 己亥.
 * → 日支 ∈ {申(8), 酉(9)}, 不論天干、不論月建 (同其兄弟 重日 之 keying).
 *
 * 小耗 (XiaoHao): 《欽定協紀辨方書》卷四 義例二 建除同位異名「執〈四利之死符、
 * 小耗〉」; 樞要曆「小耗者亦月內之耗神也,其日忌經營種蒔、納財交易、開市」;
 * 曆例「小耗者常居月建前五辰」/「小耗者正月在未,順行十二辰」 (正月建寅 -> 未 = 寅+5),
 * i.e. the 建除 「執」 day, keyed to the 節氣月/月建 like its siblings:
 *   寅月->未日, 卯月->申日, 辰月->酉日, 巳月->戌日, 午月->亥日, 未月->子日,
 *   申月->丑日, 酉月->寅日, 戌月->卯日, 亥月->辰日, 子月->巳日, 丑月->午日.
 *
 * 三喪 (SanSang): 凶.  日支 = 該月(節氣月/月建)所屬季節之土辰 -- 春(寅卯辰月)辰日、
 * 夏(巳午未月)未日、秋(申酉戌月)戌日、冬(亥子丑月)丑日; 即四序當旺之土, 與《欽定協紀
 * 辨方書》卷五「守日」同辰位 (春辰夏未秋戌冬丑).  通書/通勝凶星欄之三喪: 「三喪日以
 * 每一季之衰日論: 春季(寅卯辰月)以辰日、夏季(巳午未月)以未日、秋季(申酉戌月)以戌日、
 * 冬季(亥子丑月)以丑日」, 其日忌安葬嫁娶入宅.  Keyed to 節氣月/月建 mbranch
 * (同 月破/大耗/災煞/月厭/月刑/大時/天吏/血忌/血支/復日/天賊/歸忌/五墓/月煞).
 *
 * 月忌 (YueJi): 《欽定協紀辨方書》「月忌日」條 -- 曆例曰「月忌日,止注祭祀、宴會、
 * 沐浴、整容、剃頭、整手足甲、求醫療病、補垣、掃舍宇、修飾垣牆、平治道塗、
 * 破屋壞垣,餘事不注」; 齊東野語「俗以每月初五、十四、二十三日為月忌,凡事必避之,
 * 其說不經 -- 此三日即河圖之中宮五宮五數耳,五為君象,故民庶不可用」;
 * 通書「俗忌初五、十四、二十三,以五黃值日,配廉貞火,生起中宮之土也」;
 * 考原「以其日為九五之數,自初一至初五五數也,自初五至十四、自十四至二十三
 * 皆距九日,世不敢用,故忌也」; 《玉匣記》民俗吉凶日篇「月忌日 百事忌 --
 * 初五十四二十三,年年月月在人間」.  Keyed to the 農曆 day (lmonth/lday),
 * like 短星/楊公忌/三娘煞 (NOT the 節氣月/月建):
 *   農曆 初五(5) 十四(14) 廿三(23) 日 -> 月忌 (閏月亦照該月之農曆日).
 *
 * 土符 (TuFu): 日支 = 月建所屬之土符支, keyed to the 節氣月/月建 like its siblings
 * 月破/大耗/災煞/月厭/月刑/大時/天吏/血忌/血支.  The only 協紀 曆例 that lists all
 * twelve months and is confirmed by 曹震圭's seasonal grouping:
 *   《欽定協紀辨方書》卷六 義例四 總要曆「土符土神也, 其日忌破土穿井開渠築墻」;
 *   曆例「土符者 正月丑 二月巳 三月酉 四月寅 五月午 六月戌 七月卯 八月未
 *   九月亥 十月辰 十一月申 十二月子」; 曹震圭「春木旺土受其尅, 故托子金以制之,
 *   是春三月歷巳酉丑也; 夏火旺土休, 頼火為母以飬之, 故夏三月歷寅午戌也;
 *   秋金旺土相, 不畏木制, 故秋三月歷亥卯未也; 冬水旺土亦剛堅, 頼水以柔和之,
 *   故冬三月歷申子辰也」(按語以 寅巳申亥長生 / 子卯午酉帝旺 / 辰未戌丑收藏 三組
 *   各配 丑寅卯辰 / 巳午未申 / 酉戌亥子 三支而正之).  Map (mbranch 0=子..11=亥):
 *   寅月->丑日, 卯月->巳日, 辰月->酉日, 巳月->寅日, 午月->午日, 未月->戌日,
 *   申月->卯日, 酉月->未日, 戌月->亥日, 亥月->辰日, 子月->申日, 丑月->子日.
 *
 * 月建 (YueJian): the 建除 「建」 day -- the day whose 日支 equals the 節氣月/月建
 * branch.  《欽定協紀辨方書》卷四 義例二 建除同位異名「建〈兵福、小時、土府〉」,
 * so the 通書 prints the 建日 凶煞 as 「土府／月建」 (also 「火星月建」「三喪月建」
 * 「重日月建」 ...); 卷四 又「建為歲君為元神,為吉凶眾神之主帥」.  Keyed to the
 * 節氣月/月建 like its siblings:
 *   建日 -> 日支 == 月建支 (寅月寅日, 卯月卯日 ... 丑月丑日).
 *
 * 死神 (SiShen): 凶.  日支 = 月建前三辰 = 建除「平」日 (死氣前一辰).  《欽定協紀
 * 辨方書》卷四 義例二: 平〈陽月天罡 陰月河魁 死神〉; 神樞經「死神者,月中凶神也,
 * 其日忌請醫服藥、出師征討、種植樹木、進人納畜」; 李鼎祚「常居月建前三辰」;
 * 曹震圭「死神者,以月建為旺辰前逢死位也,大抵死氣之前必有死神,其義同前」.
 * 六壬「死神者,死氣後一位,月建向前三位為死神,正月巳、二月午、三月未…」.
 * Keyed to the 節氣月/月建 (同其兄弟 月破/大耗/災煞/月厭/月刑/大時/天吏/死氣).
 *
 * 土府 (TuFuMansion): the 建日 -- the day whose 日支 equals the 月建 itself.
 * 《欽定協紀辨方書》卷六 義例四, 土符 按語: 「其有所忌者亦猶月建之為土府尊之,
 * 故不敢犯也」, i.e. 月建之日 is the 土府.  Keyed to the 節氣月/月建 like its
 * siblings 月破/大耗/災煞/月厭/月刑/大時/天吏/血忌/血支/土符:
 *   日支 == mbranch  ->  土府 (建日); 其日忌動土、修築垣牆.
 * (The 通勝 cells agree: of the four volumes' 土府 prints, 60 are 建日 and 4 more
 * are the 建日 of the OLD 月建 on a 節氣 day.)
 *
 * 俱將 (JuJiang -- 陰陽俱將): the 4th and last member of the
 * 陰陽不將 (不將) family -- the day whose 日干's 祿支 and whose 日支
 * fall in OPPOSITE 區s: 祿(日干) ∈ 後區 AND 日支 ∈ 前區.
 * 《欽定協紀辨方書》卷四 義例二 天寶曆「陰陽不將者,以月建為陽謂之陽建(正月起寅順行十二辰),月厭為陰謂之陰建(正月起戌逆行十二辰);
 * 分於卯酉,會於子午。厭前枝幹自相配者為陽將,厭後枝幹自相配者為陰將;厭後幹配厭前枝者為陰陽俱將,
 * 厭前幹配厭後枝者為陰陽不將也。陽將傷夫,陰將傷婦」+ 「陰陽俱將夫婦俱傷」;
 * 考原「厭後幹配厭前枝者為陰陽俱將,陰非其陰陽非其陽,其道相乖也」.  Same 節氣月/月建 keying as its siblings
 * (月厭 A = (12 - 月建) mod 12; 前區 = {A+6 .. A+11}, 後區 = {A .. A+5}).  The 月厭 day
 * itself has 支 ∈ 後區, so it can never be 俱將.
 *
 * 死氣 (SiQi): 凶.  日支 = 月建前四辰 = 建除「定」日 (生氣之對衝, 死神後一辰).
 * 《欽定協紀辨方書》卷四 義例二: 定〈時陰 官符 死氣〉; 卷十 宜忌別列「死氣」為所忌.
 * 六壬「死氣者,月建向前四位(即生氣對沖為死氣),正月在午、二月在未、三月申…」.
 * Keyed to the 節氣月/月建 (同其兄弟 月破/大耗/災煞/月厭/月刑/大時/天吏/血忌/死神).
 *
 * 地囊 (DiNang): the day whose 日柱 is one of the two 納甲 days of that month's
 * 三合卦 (the internal and external initial 爻 / 世應 pair), keyed to the
 * 節氣月/月建 like its siblings.  《欽定協紀辨方書》卷六 義例四 曆例
 * 「地囊者 正月庚子庚午 二月癸未癸丑 三月甲子甲寅 四月己卯己丑
 * 五月戊辰戊午 六月癸未癸巳 七月丙寅丙申 八月丁卯丁巳 九月戊辰戊子
 * 十月庚戌庚子 十一月辛未辛酉 十二月乙酉乙未」 -- but the 四庫 傳本 is corrupt:
 * the 協紀 按語 counts 23 of its 48 characters as 傳寫之誤 and rebuilds every month
 * from 當時三合卦內外兩初爻之納甲 (春 亥卯未局 震坤乾 / 夏 寅午戌局 離乾艮 /
 * 秋 巳酉丑局 兌艮巽 / 冬 申子辰局 坎巽坤; 一卦兩用者再用世應二爻):
 *   正月 震 庚子庚午, 二月 坤 乙未癸丑, 三月 乾 甲子壬午, 四月 離 己卯己酉,
 *   五月 乾(世應) 壬戌甲辰, 六月 艮 丙辰丙戌, 七月 兌 丁巳丁亥,
 *   八月 艮(世應) 丙寅丙申, 九月 巽 辛丑辛未, 十月 坎 戊寅戊申,
 *   十一月 巽(世應) 辛卯辛酉, 十二月 坤(世應) 癸酉乙卯.
 * (協紀 辨訛 also lists 「七八月互易」 among the corruptions.)  Keyed to the
 * 節氣月/月建 (mbranch 0=子..11=亥):
 *   寅月->庚子/庚午日, 卯月->乙未/癸丑日, 辰月->甲子/壬午日, 巳月->己卯/己酉日,
 *   午月->壬戌/甲辰日, 未月->丙辰/丙戌日, 申月->丁巳/丁亥日, 酉月->丙寅/丙申日,
 *   戌月->辛丑/辛未日, 亥月->戊寅/戊申日, 子月->辛卯/辛酉日, 丑月->癸酉/乙卯日.
 * 其日忌營建穿鑿.  The 通勝 oracle settles transmitted vs corrected: corrected
 * matches 19 of the 22 printed 地囊 days, the transmitted list only 7.
 * 五虛 (WuXu): 凶.  《欽定協紀辨方書》卷五 義例 樞要曆
 * 「五虚者四時絶辰也,其日忌開倉庫營種蒔出財寶施債負」;
 * 曆例「五虚者春巳酉丑、夏申子辰、秋亥卯未、冬寅午戌也」;
 * 曹震圭「五虚者四時逢絶之辰也,物絶則損朽其中虚而無實也,
 * 故曰虚 -- 如春木旺巳酉丑金絶也, 夏火旺申子辰水絶也,
 * 秋金旺亥卯未木絶也, 冬水旺寅午戌火絶也」;
 * 卷十 宜忌「五虚 -- 忌修倉庫開倉庫出貨財」「與天德月德
 * 天德合月德合六合併則不忌」.  So 五虛 = the day whose 日支 lies in the
 * 三合局 of the branch that is 絶 in the current season (四時絶辰), keyed to the
 * 節氣月/月建 like its siblings 月破/大耗/災煞/月厭/月刑/大時/天吏/血忌/血支/復日/
 * 天賊/歸忌/五墓/月煞/月害/陽將/陰將:
 *   春(寅卯辰月)->巳酉丑日, 夏(巳午未月)->申子辰日, 秋(申酉戌月)->亥卯未日,
 *   冬(亥子丑月)->寅午戌日.
 *
 * 劫煞 (JieSha): 三煞之一 -- 日支 = 月建(節氣月)三合局五行之絕地, 即 災煞前一辰.
 * 《欽定協紀辨方書》卷六 義例四: 神樞經曰「刼煞者,刼害之辰也,其日忌臨官視事
 * 納禮成親戰伐行軍出入興販」; 李鼎祚曰「正月起亥,逆行四孟」; 又按
 * 「月刼煞義與嵗刼煞同」.  卷十 宜忌: 「按三煞為三合敵對,刼煞為絶地,
 * 災煞為正衝,月煞為盡地」 -- 即該月三合局之絕辰:
 *   寅午戌月->亥日, 亥卯未月->申日, 申子辰月->巳日, 巳酉丑月->寅日
 * (index = mbranch 0=子..11=亥; 劫煞 = 災煞 前一辰).
 *
 * 遊禍 (YouHuo): 三合五行臨官之神 (對衝劫煞之位) -- 日支 = 月建三合局之臨官辰.
 * 《欽定協紀辨方書》卷六 義例四: 神樞經曰「遊禍者,月中惡神也,其日忌服藥請醫
 * 祝神致祭」; 李鼎祚曰「遊禍者,正月起巳,逆行四孟」; 曹震圭曰「遊禍者,
 * 三合五行臨官之神,是對衝刼煞之位也」; 考原曰「以其流行於四隅故曰遊,
 * 以其過旺故曰禍」.  卷十 宜忌: 「遊禍 忌祈福求嗣解除求醫療病」,
 * 「與德合天赦併猶忌」.  孟支 巳→寅→亥→申 逐月逆行:
 *   寅午戌月->巳日, 亥卯未月->寅日, 申子辰月->亥日, 巳酉丑月->申日
 * (index = mbranch 0=子..11=亥; 遊禍 = 劫煞之對衝, 即該局之臨官).
 *
 * 招搖 (ZhaoYao): 厭對之別名 -- 日支 = 月厭之對衝辰, 即 正月起辰, 逆行十二辰.
 * 《欽定協紀辨方書》卷四 義例二 天寶曆曰「厭對者,月厭所衝之辰也,其日忌嫁娶,
 * 又為招搖,忌乘船渡水」; 厯例「厭對者,正月起辰,逆行十二辰」; 卷五 又曰
 * 「招搖即六儀,六儀即天罡,隂建之衝也」.  故 招搖 = 月厭之衝:
 *   月厭 A = (12 - 月建) mod 12, 招搖 = (A + 6) mod 12, 即
 *   寅月->辰日, 卯月->卯日, 辰月->寅日, 巳月->丑日, 午月->子日, 未月->亥日,
 *   申月->戌日, 酉月->酉日, 戌月->申日, 亥月->未日, 子月->午日, 丑月->巳日
 * (index = mbranch 0=子..11=亥).  卷十 宜忌「厭對忌嫁娶,又為招搖忌取魚乘船渡水」.
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
    { "DaHao",
      "\xb4\xf3\xba\xc4", "\xa4\x6a\xaf\xd3",
      "\xe5\xa4\xa7\xe8\x80\x97",
      "\xe5\xa4\xa7\xe8\x80\x97" },   /* 大耗 */
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
    { "ZaiSha",
      "\xd4\xd6\xc9\xb7", "\xa8\x61\xb7\xd9",
      "\xe7\x81\xbe\xe7\x85\x9e",
      "\xe7\x81\xbd\xe7\x85\x9e" },   /* 災煞 */
    { "YueYan",
      "\xd4\xc2\x85\x92", "\xa4\xeb\xb9\xbd",
      "\xe6\x9c\x88\xe5\x8e\x8c",
      "\xe6\x9c\x88\xe5\x8e\xad" },   /* 月厭 */
    { "YueXing",
      "\xd4\xc2\xd0\xcc", "\xa4\xeb\xa6\x44",
      "\xe6\x9c\x88\xe5\x88\x91",
      "\xe6\x9c\x88\xe5\x88\x91" },   /* 月刑 */
    { "DaShi",
      "\xb4\xf3\xca\xb1", "\xa4\x6a\xae\xc9",
      "\xe5\xa4\xa7\xe6\x97\xb6",
      "\xe5\xa4\xa7\xe6\x99\x82" },   /* 大時 */
    { "TianLi",
      "\xcc\xec\xc0\xf4", "\xa4\xd1\xa6\x4f",
      "\xe5\xa4\xa9\xe5\x90\x8f",
      "\xe5\xa4\xa9\xe5\x90\x8f" },   /* 天吏 */
    { "XueJi",
      "\xd1\xaa\xbc\xc9", "\xa6\xe5\xa7\xd2",
      "\xe8\xa1\x80\xe5\xbf\x8c",
      "\xe8\xa1\x80\xe5\xbf\x8c" },   /* 血忌 */
    { "XueZhi",
      "\xd1\xaa\xd6\xa7", "\xa6\xe5\xa4\xe4",
      "\xe8\xa1\x80\xe6\x94\xaf",
      "\xe8\xa1\x80\xe6\x94\xaf" },   /* 血支 */
    { "WuMu",
      "\xce\xe5\xc4\xb9", "\xa4\xad\xb9\xd3",
      "\xe4\xba\x94\xe5\xa2\x93",
      "\xe4\xba\x94\xe5\xa2\x93" },   /* 五墓 */
    { "FuRi",
      "\x8f\xcd\xc8\xd5", "\xb4\x5f\xa4\xe9",
      "\xe5\xa4\x8d\xe6\x97\xa5",
      "\xe5\xbe\xa9\xe6\x97\xa5" },   /* 復日 */
    { "TianZei",
      "\xcc\xec\xd4\xf4", "\xa4\xd1\xb8\xe9",
      "\xe5\xa4\xa9\xe8\xb4\xbc",
      "\xe5\xa4\xa9\xe8\xb3\x8a" },   /* 天賊 */
    { "GuiJi",
      "\x9a\x77\xbc\xc9", "\xc2\x6b\xa7\xd2",
      "\xe5\xbd\x92\xe5\xbf\x8c",
      "\xe6\xad\xb8\xe5\xbf\x8c" },   /* 歸忌 */
    { "YueSha",
      "\xd4\xc2\xc9\xb7", "\xa4\xeb\xb7\xd9",
      "\xe6\x9c\x88\xe7\x85\x9e",
      "\xe6\x9c\x88\xe7\x85\x9e" },   /* 月煞 */
    { "YangJiang",
      "\xea\x96\x8c\xa2", "\xb6\xa7\xb1\x4e",
      "\xe9\x98\xb3\xe5\xb0\x86",
      "\xe9\x99\xbd\xe5\xb0\x87" },   /* 陽將 */
    { "DuanXing",
      "\xb6\xcc\xd0\xc7", "\xb5\x75\xac\x50",
      "\xe7\x9f\xad\xe6\x98\x9f",
      "\xe7\x9f\xad\xe6\x98\x9f" },   /* 短星 */
    { "MiRi",
      "\xc3\xdc\xc8\xd5", "\xb1\x4b\xa4\xe9",
      "\xe5\xaf\x86\xe6\x97\xa5",
      "\xe5\xaf\x86\xe6\x97\xa5" },   /* 密日 */
    { "ChongRi",
      "\xd6\xd8\xc8\xd5", "\xad\xab\xa4\xe9",
      "\xe9\x87\x8d\xe6\x97\xa5",
      "\xe9\x87\x8d\xe6\x97\xa5" },   /* 重日 */
    { "FanZhi",
      "\xb7\xb4\xd6\xa7", "\xa4\xcf\xa4\xe4",
      "\xe5\x8f\x8d\xe6\x94\xaf",
      "\xe5\x8f\x8d\xe6\x94\xaf" },   /* 反支 */
    { "HuoXing",
      "\xbb\xf0\xd0\xc7", "\xa4\xf5\xac\x50",
      "\xe7\x81\xab\xe6\x98\x9f",
      "\xe7\x81\xab\xe6\x98\x9f" },   /* 火星 */
    { "YinJiang",
      "\xea\x8e\x8c\xa2", "\xb3\xb1\xb1\x4e",
      "\xe9\x98\xb4\xe5\xb0\x86",
      "\xe9\x99\xb0\xe5\xb0\x87" },   /* 陰將 */
    { "WuLi",
      "\xce\xe5\xeb\x78", "\xa4\xad\xc2\xf7",
      "\xe4\xba\x94\xe7\xa6\xbb",
      "\xe4\xba\x94\xe9\x9b\xa2" },   /* 五離 */
    { "XiaoHao",
      "\xd0\xa1\xba\xc4", "\xa4\x70\xaf\xd3",
      "\xe5\xb0\x8f\xe8\x80\x97",
      "\xe5\xb0\x8f\xe8\x80\x97" },   /* 小耗 */
    { "SanSang",
      "\xc8\xfd\x86\xca", "\xa4\x54\xb3\xe0",
      "\xe4\xb8\x89\xe4\xb8\xa7",
      "\xe4\xb8\x89\xe5\x96\xaa" },   /* 三喪 */
    { "YueJi",
      "\xd4\xc2\xbc\xc9", "\xa4\xeb\xa7\xd2",
      "\xe6\x9c\x88\xe5\xbf\x8c",
      "\xe6\x9c\x88\xe5\xbf\x8c" },   /* 月忌 */
    { "TuFu",
      "\xcd\xc1\xb7\xfb", "\xa4\x67\xb2\xc5",
      "\xe5\x9c\x9f\xe7\xac\xa6",
      "\xe5\x9c\x9f\xe7\xac\xa6" },   /* 土符 */
    { "YueJian",
      "\xd4\xc2\xbd\xa8", "\xa4\xeb\xab\xd8",
      "\xe6\x9c\x88\xe5\xbb\xba",
      "\xe6\x9c\x88\xe5\xbb\xba" },   /* 月建 */
    { "SiShen",
      "\xcb\xc0\xc9\xf1", "\xa6\xba\xaf\xab",
      "\xe6\xad\xbb\xe7\xa5\x9e",
      "\xe6\xad\xbb\xe7\xa5\x9e" },   /* 死神 */
    { "TuFuMansion",
      "\xcd\xc1\xb8\xae", "\xa4\x67\xa9\xb2",
      "\xe5\x9c\x9f\xe5\xba\x9c",
      "\xe5\x9c\x9f\xe5\xba\x9c" },   /* 土府 */
    { "JuJiang",
      "\xbe\xe3\xbd\xab", "\xad\xd1\xb1\x4e",
      "\xe4\xbf\xb1\xe5\xb0\x86",
      "\xe4\xbf\xb1\xe5\xb0\x87" },   /* 俱將 */
    { "SiQi",
      "\xcb\xc0\x9a\xe2", "\xa6\xba\xae\xf0",
      "\xe6\xad\xbb\xe6\xb0\x94",
      "\xe6\xad\xbb\xe6\xb0\xa3" },   /* 死氣 */
    { "DiNang",
      "\xb5\xd8\xc4\xd2", "\xa6\x61\xc5\x6e",
      "\xe5\x9c\xb0\xe5\x9b\x8a",
      "\xe5\x9c\xb0\xe5\x9b\x8a" },   /* 地囊 */
    { "WuXu",
      "\xce\xe5\xd0\xe9", "\xa4\xad\xb5\xea",
      "\xe4\xba\x94\xe8\x99\x9a",
      "\xe4\xba\x94\xe8\x99\x9b" },   /* 五虛 */
    { "JieSha",
      "\xbd\xd9\xc9\xb7", "\xa7\x54\xb7\xd9",
      "\xe5\x8a\xab\xe7\x85\x9e",
      "\xe5\x8a\xab\xe7\x85\x9e" },   /* 劫煞 */
    { "YouHuo",
      "\xd3\xce\xbb\xf6", "\xb9\x43\xba\xd7",
      "\xe6\xb8\xb8\xe7\xa5\xb8",
      "\xe9\x81\x8a\xe7\xa6\x8d" },   /* 遊禍 */
    { "ZhaoYao",
      "\xd5\xd0\xd2\xa1", "\xa9\xdb\xb7\x6e",
      "\xe6\x8b\x9b\xe6\x91\x87",
      "\xe6\x8b\x9b\xe6\x90\x96" },   /* 招搖 */
    { "XuanWu",
      "\xd0\xfe\xce\xe4", "\xa5\xc8\xaa\x5a",
      "\xe7\x8e\x84\xe6\xad\xa6",
      "\xe7\x8e\x84\xe6\xad\xa6" },   /* 玄武 */
    { "GouChen",
      "\xb9\xb4\xea\x90", "\xa4\xc4\xb3\xaf",
      "\xe5\x8b\xbe\xe9\x99\x88",
      "\xe5\x8b\xbe\xe9\x99\xb3" },   /* 勾陳 */
    { "BaiHu",
      "\xb0\xd7\xbb\xa2", "\xa5\xd5\xaa\xea",
      "\xe7\x99\xbd\xe8\x99\x8e",
      "\xe7\x99\xbd\xe8\x99\x8e" },   /* 白虎 */
    { "ZhuQue",
      "\xd6\xec\xc8\xb8", "\xa6\xb6\xb3\xb6",
      "\xe6\x9c\xb1\xe9\x9b\x80",
      "\xe6\x9c\xb1\xe9\x9b\x80" },   /* 朱雀 */
    { "TianXing",
      "\xcc\xec\xd0\xcc", "\xa4\xd1\xa6\x44",
      "\xe5\xa4\xa9\xe5\x88\x91",
      "\xe5\xa4\xa9\xe5\x88\x91" },   /* 天刑 */
    { "TianHuo",
      "\xcc\xec\xbb\xf0", "\xa4\xd1\xa4\xf5",
      "\xe5\xa4\xa9\xe7\x81\xab",
      "\xe5\xa4\xa9\xe7\x81\xab" },   /* 天火 */
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

/* 災煞 by 月建 (solar-term month): the day whose 日支 is the 胎 position
   of that month's 三合局 (劫煞前一辰).  Index = mbranch 0=子..11=亥:
     寅午戌(2,6,10)->子(0)  亥卯未(11,3,7)->酉(9)
     巳酉丑(5,9,1)->卯(3)    申子辰(8,0,4)->午(6) */
static const char zai_branch[12] = {6, 3, 0, 9, 6, 3, 0, 9, 6, 3, 0, 9};

/* 月厭 by 月建 (solar-term month): "正月起戌逆行十二辰" on the day branch.
   Index = mbranch 0=子..11=亥:
     寅(2)->戌(10)  卯(3)->酉(9)  辰(4)->申(8)  巳(5)->未(7)
     午(6)->午(6)   未(7)->巳(5)  申(8)->辰(4)  酉(9)->卯(3)
     戌(10)->寅(2)  亥(11)->丑(1) 子(0)->子(0)  丑(1)->亥(11) */
static const char yueyan_branch[12] = {0, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};

/* 月刑 by 月建 (solar-term month): the day branch punished by that month's
   build branch (月建之刑).  Index = mbranch 0=子..11=亥:
     子->卯(3), 丑->戌(10), 寅->巳(5), 卯->子(0)
     辰->辰(4), 巳->申(8), 午->午(6), 未->丑(1)
     申->寅(2), 酉->酉(9), 戌->未(7), 亥->亥(11) */
static const char yuexing_branch[12] = {3, 10, 5, 0, 4, 8, 6, 1, 2, 9, 7, 11};

/* 大時 by 月建 (solar-term month): "正月起卯逆行四仲" (咸池/桃花 position,
   月建三合五行沐浴之辰).  Index = mbranch 0=子..11=亥:
     寅(2)->卯(3)  卯(3)->子(0)  辰(4)->酉(9)  巳(5)->午(6)
     午(6)->卯(3)  未(7)->子(0)  申(8)->酉(9)  酉(9)->午(6)
     戌(10)->卯(3) 亥(11)->子(0) 子(0)->酉(9)  丑(1)->午(6) */
static const char dashi_branch[12] = {9, 6, 3, 0, 9, 6, 3, 0, 9, 6, 3, 0};

/* 天吏 by 月建 (solar-term month): the day whose 日支 is the 死 position of
   that month's 三合局 (三合之死地/死氣 = "正月起酉逆行四仲").  Index = mbranch
   0=子..11=亥:
     寅午戌(2,6,10)->酉(9)  亥卯未(11,3,7)->午(6)
     巳酉丑(5,9,1)->子(0)    申子辰(8,0,4)->卯(3) */
static const char tianli_branch[12] = {3, 0, 9, 6, 3, 0, 9, 6, 3, 0, 9, 6};

/* 血忌 by 月建 (solar-term month), same 斗建 keying as its siblings:
   《攝生月令》(雲笈七籤卷三十六) "斗建X ... 血忌Y".  Index = mbranch 0=子..11=亥:
     寅(2)->丑(1)  卯(3)->未(7)  辰(4)->寅(2)  巳(5)->申(8)
     午(6)->卯(3)  未(7)->酉(9)  申(8)->辰(4)  酉(9)->戌(10)
     戌(10)->巳(5) 亥(11)->亥(11) 子(0)->午(6) 丑(1)->子(0) */
static const char xueji_branch[12] = {6, 0, 1, 7, 2, 8, 3, 9, 4, 10, 5, 11};

/* 血支 by 月建 (solar-term month): the 建除 "閉" position = 月建前一位
   (《星歷考原》歷例「血支者正月在丑順行十二辰」).  Index = mbranch 0=子..11=亥:
     子(0)->亥(11) 丑(1)->子(0)  寅(2)->丑(1)  卯(3)->寅(2)
     辰(4)->卯(3)  巳(5)->辰(4)  午(6)->巳(5)  未(7)->午(6)
     申(8)->未(7)  酉(9)->申(8)  戌(10)->酉(9) 亥(11)->戌(10) */
static const char xuezhi_branch[12] = {11, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

/* 五墓 by 月建 (solar-term month).  《欽定協紀辨方書》卷五 義例三:
   廣聖曆曰"五墓者,四旺之墓辰也,其日忌營造起土嫁娶出軍"; 曆例曰"五墓者
   正二月乙未、四五月丙戌、七八月辛丑、十月十一月壬辰、四季月戊辰也";
   曹震圭曰"五墓者,五行旺干自臨墓辰也" (木旺于正二月墓未, 火旺于四五月墓戌,
   金旺于七八月墓丑, 水旺于十月十一月墓辰, 土旺于四季月墓辰).  The day's
   日柱 (ganzhi index 0=甲子) for each 月建, index = mbranch 0=子..11=亥:
     子(0)->壬辰(28) 丑(1)->戊辰(4)  寅(2)->乙未(31) 卯(3)->乙未(31)
     辰(4)->戊辰(4)  巳(5)->丙戌(22) 午(6)->丙戌(22) 未(7)->戊辰(4)
     申(8)->辛丑(37) 酉(9)->辛丑(37) 戌(10)->戊辰(4) 亥(11)->壬辰(28) */
static const char wumu_gz[12] = {28, 4, 31, 31, 4, 22, 22, 4, 37, 37, 4, 28};

/* 復日 by 月建 (solar-term month): 日干 == 本氣干 of the 月建 branch
   (曹震圭 「本建之辰與所遇之干同」).  Index = mbranch 0=子..11=亥:
     子->癸(9)  丑->己(5)  寅->甲(0)  卯->乙(1)  辰->戊(4)  巳->丙(2)
     申->庚(6)  酉->辛(7)  戌->戊(4)  亥->壬(8)  子->癸(9)  丑->己(5) */
static const char furi_stem[12] = {9, 5, 0, 1, 4, 2, 3, 5, 6, 7, 4, 8};

/* 天賊 by 月建 (solar-term month): the 月厭 branch + 3 ("正月在丑逆行十二辰",
   李鼎祎; 曹震圭 "天賊者盜神也常居天倉後辰").  Index = mbranch 0=子..11=亥:
     子(0)->卯(3)  丑(1)->寅(2)  寅(2)->丑(1)  卯(3)->子(0)
     辰(4)->亥(11) 巳(5)->戌(10) 午(6)->酉(9)  未(7)->申(8)
     申(8)->未(7)  酉(9)->午(6)  戌(10)->巳(5) 亥(11)->辰(4) */
static const char tianzei_branch[12] = {3, 2, 1, 0, 11, 10, 9, 8, 7, 6, 5, 4};

/* 歸忌 by 月建 (solar-term month): 《欽定協紀辨方書》/考原「孟月丑、仲月寅、季月子」
   -- 四孟(寅巳申亥)丑日, 四仲(子卯午酉)寅日, 四季(辰未戌丑)子日.  Index = mbranch
   0=子..11=亥:
     子(0)->寅(2)  丑(1)->子(0)  寅(2)->丑(1)  卯(3)->寅(2)
     辰(4)->子(0)  巳(5)->丑(1)  午(6)->寅(2)  未(7)->子(0)
     申(8)->丑(1)  酉(9)->寅(2)  戌(10)->子(0) 亥(11)->丑(1) */
static const char guiji_branch[12] = {2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1};

/* 月煞 by 月建 (solar-term month): "正月起丑逆行四季" (丑戌未辰 cycle) on the day
   branch, i.e. the 養 position of the month's 三合局 (= the 歲煞 position applied to
   the 月建).  Index = mbranch 0=子..11=亥:
     子(0)->未(7)  丑(1)->辰(4)  寅(2)->丑(1)  卯(3)->戌(10)
     辰(4)->未(7)  巳(5)->辰(4)  午(6)->丑(1)  未(7)->戌(10)
     申(8)->未(7)  酉(9)->辰(4)  戌(10)->丑(1) 亥(11)->戌(10) */
static const char yuesha_branch[12] = {7, 4, 1, 10, 7, 4, 1, 10, 7, 4, 1, 10};
/* 短星 by 農曆 month/day (NOT the 節氣月/月建 keying of its 月-keyed siblings):
   《欽定協紀辨方書》卷九 立成「短星 正月〈二十一日〉二月〈十九日〉…八月〈十八…」;
   御定星曆考原「短星者,正月二十一,二月十九,三月十六,四月二十五,五月二十五,六月二十,
   七月二十二,八月十八、十九,九月十六、十七,十月十四,十一月二十二,十二月二十五日是也」.
   One fixed 農曆 date per lunar month; the 8th and 9th months have two.
   Index = 農曆 month 1..12 (lmonth), value = 農曆 day. */
static const char duanxing_mon[14] = {1, 2, 3, 4, 5, 6, 7, 8, 8, 9, 9, 10, 11, 12};
static const char duanxing_day[14] = {21, 19, 16, 25, 25, 20, 22, 18, 19, 16, 17, 14, 22, 25};

/* 火星 by 月建 (solar-term month): the 通書/玉匣記 火星日 干支 sets (see the
   header comment).  Index = mbranch % 3:
     0 = 四仲月 (子卯午酉) -> 甲子 癸酉 壬午 辛卯 庚子 己酉 戊午
     1 = 四季月 (辰未戌丑) -> 壬申 辛巳 庚寅 己亥 戊申 丁巳
     2 = 四孟月 (寅巳申亥) -> 乙丑 甲戌 癸未 壬辰 辛丑 庚戌 己未
   (-1 = pad; the 季月 set has only six entries.) */
static const char huoxing_gz[3][7] = {
    { 0, 9, 18, 27, 36, 45, 54},
    { 8, 17, 26, 35, 44, 53, -1},
    { 1, 10, 19, 28, 37, 46, 55}
};

/* 陰將 (YinJiang) -- see the header comment.  祿支 of the 日干 (identical table to
   陽將's): 甲寅 乙卯 丙巳 丁午 戊丑 己未 庚申 辛酉 壬亥 癸子, index = 干 0=甲..9=癸. */
static const char yinjiang_lu[10] = {2, 3, 5, 6, 1, 7, 8, 9, 11, 0};

/* 1 when branch br lies in the 陰將 支 後區: 月厭 A = (12 - mbranch) mod 12,
   後區 = {A+1 .. A+5} (月厭 itself and its 沖 厭對 excluded -- the same取法 as the
   卷四 不將 day lists, which never fall on a 月厭/厭對 支). */
static int yinjiang_back5(int mbranch, int br)
{
    int o = (br - (12 - mbranch) % 12 + 12) % 12;
    return o >= 1 && o <= 5;
}

/* 1 when branch br lies in the 陰將 干 後區: 月厭 A, 後區 = {A .. A+5}
   (the 干 half includes the 月厭 位, mirroring 陽將's 前區 which includes 厭對). */
static int yinjiang_back6(int mbranch, int br)
{
    int o = (br - (12 - mbranch) % 12 + 12) % 12;
    return o >= 0 && o <= 5;
}


/* 陽將 (YangJiang) -- see the header comment.  祿支 of the 日干:
   甲寅 乙卯 丙巳 丁午 戊丑 己未 庚申 辛酉 壬亥 癸子, index = 干 0=甲..9=癸. */
static const char yangjiang_lu[10] = {2, 3, 5, 6, 1, 7, 8, 9, 11, 0};

/* 1 when branch br lies in the 前區 of the 月建's 建/厭 pair:
   月厭 A = (12 - mbranch) mod 12, 前區 = {A+6 .. A+11}. */
static int yangjiang_front(int mbranch, int br)
{
    int a = (12 - mbranch) % 12;
    return ((br - a + 12) % 12) >= 6;
}
/* 三喪 by 月建 (solar-term month): 日支 = 該季節之土辰 (四序當旺之土).
   Index = season_of(mbranch): 春(寅卯辰)->辰(4) 夏(巳午未)->未(7)
   秋(申酉戌)->戌(10) 冬(亥子丑)->丑(1). */
static const char sansang_branch[4] = {4, 7, 10, 1};
/* 月忌 by 農曆 day: 每月 初五、十四、廿三 (see the header comment). */
static const char yueji_day[3] = { 5, 14, 23 };

/* 土符 by 月建 (solar-term month): the 曆例 branch of each of the twelve
   months, index = mbranch 0=子 .. 11=亥 (see the header comment). */
static const char tufu_branch[12] = {8, 0, 1, 5, 9, 2, 6, 10, 3, 7, 11, 4};

/* 地囊 by 月建 (solar-term month): the two 納甲 日柱 of the month's 三合卦,
   index = mbranch 0=子 .. 11=亥, value = 干支 index (甲子 = 0).  See the header
   comment -- this is the 協紀 按語 corrected 曆例, not the 四庫 傳本. */
static const char dinang_gz[12][2] = {
    {27, 57}, { 9, 51}, {36,  6}, {31, 49}, { 0, 18}, {15, 45},
    {58, 40}, {52, 22}, {53, 23}, { 2, 32}, {37,  7}, {14, 44}
};

/* 五虛 by 節氣月/月建 (four-season 絶辰): see the header comment.
   season_of(mbranch) = 0春 1夏 2秋 3冬; index = season, value = the three 日支
   of the 三合局 of the 絶 branch of that season's 旺行:
     春(寅卯辰月) -> 巳(5) 酉(9) 丑(1)   (木旺金絶)
     夏(巳午未月) -> 申(8) 子(0) 辰(4)   (火旺水絶)
     秋(申酉戌月) -> 亥(11) 卯(3) 未(7)  (金旺木絶)
     冬(亥子丑月) -> 寅(2) 午(6) 戌(10)  (水旺火絶) */
static const char wuxu_branch[4][3] = {
    { 5, 9, 1},
    { 8, 0, 4},
    {11, 3, 7},
    { 2, 6, 10}
};

/* 劫煞 (JieSha) -- see the header comment.  三合局之絕地 (災煞前一辰),
   index = mbranch 0=子..11=亥: 子月->巳, 丑月->寅, 寅月->亥, 卯月->申,
   辰月->巳, 巳月->寅, 午月->亥, 未月->申, 申月->巳, 酉月->寅, 戌月->亥, 亥月->申. */
static const char jiesha_branch[12] = {5, 2, 11, 8, 5, 2, 11, 8, 5, 2, 11, 8};
/* 遊禍 (YouHuo) -- see the header comment.  三合臨官 (劫煞之對衝),
   index = mbranch 0=子..11=亥: 子月->亥, 丑月->申, 寅月->巳, 卯月->寅,
   辰月->亥, 巳月->申, 午月->巳, 未月->寅, 申月->亥, 酉月->申, 戌月->巳, 亥月->寅. */
static const char youhuo_branch[12] = {11, 8, 5, 2, 11, 8, 5, 2, 11, 8, 5, 2};
/* 招搖 (ZhaoYao) -- see the header comment.  厭對 = 月厭之衝 = (18 - mbranch) mod 12,
   index = mbranch 0=子..11=亥: 子月->午, 丑月->巳, 寅月->辰, 卯月->卯, 辰月->寅,
   巳月->丑, 午月->子, 未月->亥, 申月->戌, 酉月->酉, 戌月->申, 亥月->未. */
static const char zhaoyao_branch[12] = {6, 5, 4, 3, 2, 1, 0, 11, 10, 9, 8, 7};

/* 黃黑道十二神 (日家值日神) -- 《欽定協紀辨方書》卷七 義例五 「黃道黑道」:
   神樞經「青龍明堂金匱天德玉堂司命皆月内天黄道之神也…天刑朱雀白虎天牢元武勾陳者月中黑道也」;
   御定星曆考原「黃黑二道者,黃道六黑道六共十有二,以配十有二辰: 一青龍 二明堂 三天刑 四朱雀
   五金匱 六天德 七白虎 八玉堂 九天牢 十元武 十一司命 十二勾陳; 其法則寅申青龍起子, 卯酉起寅,
   辰戌起辰, 巳亥起午, 子午起申, 丑未起戌, 順行十二辰; 月起日, 則建寅之月子日為青龍, 丑日為明堂」.
   即 青龍 支 = 2*((月建-2) mod 6); 第 i 神 (0-based: 0青龍 1明堂 2天刑 3朱雀 4金匱 5天德
   6白虎 7玉堂 8天牢 9元武 10司命 11勾陳) 值日 ⇔ 日支 = 青龍 + i (mod 12).
   按 節氣月/月建 mbranch 與 日支 dbranch keyed, 同 月破/大耗/災煞/月厭/月刑/大時/天吏/血忌/血支. */
static int huangdao_index(int mbranch, int dbranch)
{
    return ((dbranch - 2 * ((mbranch - 2 + 6) % 6)) % 12 + 12) % 12;
}

int ShenShaOn(int i, int lmonth, int lday, int ganzhi, int mbranch, int dbranch,
              int wday)
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
    case SS_DAHAO:
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
    case SS_ZAISHA:
        return dbranch == zai_branch[mbranch];
    case SS_YUEYAN:
        return dbranch == yueyan_branch[mbranch];
    case SS_YUEXING:
        return dbranch == yuexing_branch[mbranch];
    case SS_DASHI:
        return dbranch == dashi_branch[mbranch];
    case SS_TIANLI:
        return dbranch == tianli_branch[mbranch];
    case SS_XUEJI:
        return dbranch == xueji_branch[mbranch];
    case SS_XUEZHI:
        return dbranch == xuezhi_branch[mbranch];
    case SS_WUMU:
        return ganzhi == wumu_gz[mbranch];
    case SS_FURI:
        return (ganzhi % 10) == furi_stem[mbranch];
    case SS_TIANZEI:
        return dbranch == tianzei_branch[mbranch];
    case SS_GUIJI:
        return dbranch == guiji_branch[mbranch];
    case SS_YUESHA:
        return dbranch == yuesha_branch[mbranch];
    case SS_YANGJIANG:
        return yangjiang_front(mbranch, dbranch) &&
               yangjiang_front(mbranch, yangjiang_lu[ganzhi % 10]);
    case SS_DUANXING:
        for (k = 0; k < 14; k++)
            if (lmonth == duanxing_mon[k] && lday == duanxing_day[k])
                return 1;
        return 0;
    case SS_JUJIANG:
        /* 陰陽俱將 = 厭後幹配厭前枝: 祿(日干) ∈ 後區 且 日支 ∈ 前區
           (卷四 義例二 天寶曆「厭後幹配厭前枝者為陰陽俱將…夫婦俱傷」; 考原「陰非其陰,陽非其陽」). */
        return yangjiang_front(mbranch, dbranch) &&
               !yangjiang_front(mbranch, yangjiang_lu[ganzhi % 10]);
    case SS_CHONGRI:
        /* 歷例「重日者,己亥二日也」 -> 日支 巳/亥, 不論天干與月建. */
        return dbranch == 5 || dbranch == 11;
    case SS_MIRI:
        return wday == 0;   /* 房/虛/昴/星 = 日曜 = 星期日 */
    case SS_FANZHI:
    {
        /* 反支 (FanZhi): 《欽定協紀辨方書》卷六 義例四 (後漢書王符傳注,
           見陰陽書): "反支日用月朔為正 -- 戌亥朔一日反支,申酉朔二日反支,
           午未朔三日反支,辰巳朔四日反支,寅夘朔五日反支,子丑朔六日反支";
           卷九 立成 "日神按月朔取日數者": 子丑朔六日 寅夘朔五日 辰巳朔四日
           午未朔三日 申酉朔二日 戌亥朔一日.  卷十 宜忌 "忌上冊進表章陳詞訟".
           按 "反支之義,惡其將盡也": 反支 = 該農曆月的第 (6 - 朔支/2) 日,
           朔支 = (日支 - 日數 + 1) mod 12 (子=0..亥=11). 唯一按月朔取日數者. */
        int s = ((dbranch - lday + 1) % 12 + 12) % 12;
        return lday == 6 - s / 2;
    }
    case SS_YINJIANG:
        /* 陰將: 日支 ∈ 後區 (月厭+1..+5) 且 日干之祿支 ∈ 後區 (月厭..+5). */
        return yinjiang_back5(mbranch, dbranch) &&
               yinjiang_back6(mbranch, yinjiang_lu[ganzhi % 10]);
    case SS_HUOXING:
        /* 通書/玉匣記 火星日: 日柱 == 該月(孟/仲/季)之火星干支. */
        for (k = 0; k < 7; k++)
            if (ganzhi == huoxing_gz[mbranch % 3][k])
                return 1;
        return 0;
    case SS_WULI:
        /* 五離 = 除神: 日支 申/酉 (協紀卷五 義例三, 協紀卷九 立成). */
        return dbranch == 8 || dbranch == 9;
    case SS_XIAOHAO:
        /* 小耗: 日支 == 月建前五辰 (建除 執日). */
        return dbranch == (mbranch + 5) % 12;
    case SS_SANSANG:
        return dbranch == sansang_branch[season_of(mbranch)];
    case SS_YUEJI:
        /* 月忌: 農曆 初五、十四、廿三 (五黃值日). */
        for (k = 0; k < 3; k++)
            if (lday == yueji_day[k])
                return 1;
        return 0;
    case SS_TUFU:
        /* 土符: 日支 = 月建對應之土符支 (正月丑 二月巳 三月酉 四月寅 ...). */
        return dbranch == tufu_branch[mbranch];
    case SS_YUEJIAN:
        /* 月建: 日支 == 月建支 (建日; 通書 作 土府月建). */
        return dbranch == mbranch;
    case SS_SISHEN:
        /* 死神 = 月建前三辰 (建除「平」日). */
        return dbranch == (mbranch + 3) % 12;
    case SS_TUFU_MANSION:
        /* 土府 = 月建之日 (建日): 日支等於節氣月之月建支. */
        return dbranch == mbranch;
    case SS_SIQI:
        /* 死氣 = 月建前四辰 (建除「定」日). */
        return dbranch == (mbranch + 4) % 12;
    case SS_DINANG:
        /* 地囊: 日柱 = 該月(節氣月)三合卦兩初爻之納甲日 (九月辛丑辛未 ...). */
        for (k = 0; k < 2; k++)
            if (ganzhi == dinang_gz[mbranch][k])
                return 1;
        return 0;
    case SS_WUXU:
    {
        /* 五虛: 日支 ∈ 四時絶辰之三合局 (春巳酉丑 夏申子辰 秋亥卯未 冬寅午戌). */
        int s = season_of(mbranch);
        return dbranch == wuxu_branch[s][0] || dbranch == wuxu_branch[s][1] ||
               dbranch == wuxu_branch[s][2];
    }
    case SS_JIESHA:
        /* 劫煞: 日支 == 該月三合局之絕地 (災煞前一辰). */
        return dbranch == jiesha_branch[mbranch];
    case SS_YOUHUO:
        /* 遊禍: 日支 == 該月三合局之臨官 (劫煞之對衝). */
        return dbranch == youhuo_branch[mbranch];
    case SS_ZHAOYAO:
        /* 招搖 (= 厭對): 日支 == 月厭之對衝辰. */
        return dbranch == zhaoyao_branch[mbranch];
    case SS_XUANWU:
        /* 元武/玄武 = 黃黑道第 10 神 (黑道), 支 = 青龍+9 */
        return huangdao_index(mbranch, dbranch) == 9;
    case SS_GOUCHEN:
        /* 勾陳 = 黃黑道第 12 神 (黑道), 支 = 青龍+11 */
        return huangdao_index(mbranch, dbranch) == 11;
    case SS_BAIHU:
        /* 白虎 = 黃黑道第 7 神 (黑道), 支 = 青龍+6 */
        return huangdao_index(mbranch, dbranch) == 6;
    case SS_ZHUQUE:
        /* 朱雀 = 黃黑道第 4 神 (黑道), 支 = 青龍+3 */
        return huangdao_index(mbranch, dbranch) == 3;
    case SS_TIANXING:
        /* 天刑 = 黃黑道第 3 神 (黑道), 支 = 青龍+2 */
        return huangdao_index(mbranch, dbranch) == 2;
    case SS_TIANHUO:
        /* 天火: 逆行四仲 -- 《欽定協紀辨方書》卷六 義例四 玉帳經「天火者月中凶神也,其日忌苫蓋築壘
           垣牆振旅興師會親娶婦」; 曆例「天火者正月在子順行四仲」, 而卷内按語「是故天火亦應逆行
           四仲」⇒ 與 災煞 同支 (正月在子, 逆行四仲). */
        return dbranch == zai_branch[mbranch];
    }
    return 0;
}

/* The mask must be wider than int: SS_COUNT now exceeds 32, and `1 << 32` wraps
   to bit 0 on x86-64 (shift count taken mod 32), which would corrupt 天赦. */
unsigned long long ShenShaMask(int lmonth, int lday, int ganzhi, int mbranch, int dbranch,
                               int wday)
{
    int i;
    unsigned long long mask = 0;
    for (i = 0; i < SS_COUNT; i++)
        if (ShenShaOn(i, lmonth, lday, ganzhi, mbranch, dbranch, wday))
            mask |= 1ULL << i;
    return mask;
}

/* Join the names of the set bits with "、", in table order. */
void ShenShaText(unsigned long long mask, int enc, char* buf, int bufsize)
{
    const char* sep = (enc == 'a') ? ", " :
                      (enc == 'g') ? "\xa1\xa2" :
                      (enc == 'b') ? "\xa1\x42" : "\xe3\x80\x81";
    int i, first = 1;
    buf[0] = 0;
    for (i = 0; i < SS_COUNT; i++)
    {
        const char* n;
        if (!(mask & (1ULL << i)))
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
