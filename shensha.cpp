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
 *
 * 咸池 (XianChi): 凶.  《欽定協紀辨方書》卷五 義例三 引淮南子「斗柄為小歲…
 * 咸池為大歲, 正月建夘, 月從右行四仲, 終而復始」, 又「大時者, 咸池也;
 * 小時者, 月建也」; 神樞經「大時者, 將軍之象也, 所值之日忌出軍、攻戰、築室、會親」;
 * 李鼎祚「大時者正月起夘, 逆行四仲」; 曹震圭「大時者, 乃月建三合五行沐浴之辰也,
 * 蓋五行至此則敗絕, 是最凶之辰也」.  故 咸池 = 大時 = 月建三合局之沐浴位
 * (亦即 桃花/四仲 之位): 寅午戌月->卯日, 亥卯未月->子日, 申子辰月->酉日,
 * 巳酉丑月->午日.  Keyed to the 節氣月/月建 (mbranch) 與 日支 (dbranch),
 * 同其兄弟 大時/月破/大耗/災煞/月厭/月刑.  四本通勝 兩名並用: 印 咸池 的 31 日 中
 * 27 日 同時 (或另日) 見 大時 之名.
 *
 * 班煞 (BanSha): 凶.  通勝凶星欄之「班煞」-- 「班」指魯班 (公輸班), 即沖尅魯班
 * 先師之煞, 俗稱「殺師日」, 營造、造葬之家所忌.  其口訣為「春子秋午, 夏卯冬酉」
 * (取天地四正子午卯酉剛強之氣為地師之煞), 即 春(寅卯辰月)子日、夏(巳午未月)卯日、
 * 秋(申酉戌月)午日、冬(亥子丑月)酉日.  Keyed to the 節氣月/月建 (mbranch) 與
 * 日支 (dbranch), 同其兄弟 月破/大耗/災煞/月厭/月刑.  四本通勝印 班煞 的 49 日 中
 * 46 日 全中 (其餘 3 日皆 DB 行 conf=low 之錯錨行).
 *
 * 歲德 (SuiDe): the day whose 日干 is the 德 of the 太歲 干 -- a 太歲-keyed
 * 吉神 (not the 節氣月).  《欽定協紀辨方書》卷三 義例一: 曾門經曰「歲德者
 * 歲中德神也, 十干之中五為陽五為陰, 陽者君道也, 陰者臣道也, 君德自處, 臣德從君也,
 * 所理之地萬福咸集, 眾殃自避」; 廣聖曆曰「甲德在甲, 乙德在庚, 丙德在丙, 丁德在壬,
 * 戊德在戊, 己德在甲, 庚德在庚, 辛德在丙, 壬德在壬, 癸德在戊」; 考原曰「五陽干
 * 當位自得, 五陰干則取其合」 (陽以自得為德, 陰以從陽為德).  即 日干 = 德(年干):
 *   甲年->甲日, 乙年->庚日, 丙年->丙日, 丁年->壬日, 戊年->戊日, 己年->甲日,
 *   庚年->庚日, 辛年->丙日, 壬年->壬日, 癸年->戊日.
 * 用戶 通勝 (2006 丙戌 / 2010 庚寅 / 2025 乙巳 / 2026 丙午) 印「歲德」於 丙/庚/庚/丙 日,
 * 即本規則; 年界用立春換歲 (干支年).
 *
 * 歲德合 (SuiDeHe): 歲德之五合干.  同卷 考原曰「歲德合者, 歲德五合之干是也 --
 * 甲年在己, 乙年在乙, 丙年在辛, 丁年在丁, 戊年在癸, 己年在己, 庚年在乙,
 * 辛年在辛, 壬年在丁, 癸年在癸, 故歲德屬陽, 歲德合屬陰」.  即 日干 = 五合(德(年干)):
 *   甲年->己日, 乙年->乙日, 丙年->辛日, 丁年->丁日, 戊年->癸日, 己年->己日,
 *   庚年->乙日, 辛年->辛日, 壬年->丁日, 癸年->癸日.  (通勝 印作「歲德合日」,
 *   即 歲德 + 合日 兩欄; 同卷 並記 歲干合 = 年干之五合, 與 歲德/歲德合 恆為同一干,
 *   故 通書 只印其一 -- 乙年 印「歲干」於庚日.)
 *
 * 歲祿 (SuiLu): the day whose 日支 is the 臨官 (祿) of the 太歲 干 -- 十干祿
 * 甲寅 乙卯 丙巳 丁午 戊巳 己午 庚申 辛酉 壬亥 癸子.  通勝 印「歲祿」於:
 *   2006 丙戌年 巳日, 2010 庚寅年 申日, 2025 乙巳年 卯日, 2026 丙午年 午日.
 *
 * 歲馬 (SuiMa): the day whose 日支 is the 驛馬 (三合 長生之沖) of the 太歲 支:
 *   寅午戌年->申日, 申子辰年->寅日, 巳酉丑年->亥日, 亥卯未年->巳日.
 *   (2006 丙戌年 落申日, 2010 庚寅年 與 歲祿 同落申日, 2025 乙巳年 落亥日.)
 *
 * 歲支 (SuiZhi): the day whose 日支 forms 三合 or 六合 with the 太歲 支 (the 太歲
 * 支 itself excluded) -- 通書 印作「歲支三合」/「歲支六合」, 即
 * 日支 ∈ {年支+4, 年支+8, 六合(年支)}:  戌年->寅/午/卯日,
 * 寅年->午/戌/亥日, 巳年->酉/丑/申日, 午年->戌/寅/未日.
 *
 * 歲支德 (SuiZhiDe): 協紀 作「歲枝德」 -- 日支 = 太歲 支前五辰 (通勝 印作
 * 「歲支德日」, 故與 歲支 共用同一欄名。  《欽定協紀辨方書》卷三 義例一:
 * 神樞經曰「歲枝德者歲中德神也, 德者得也, 得福之謂也, 主救危而濟弱」;
 * 李鼎祚曰「常居歲前五辰」; 曹震圭曰「枝德者從太歲向前五合之边辰也」.  即
 * 日支 = (年支+5) mod 12:  戌年->卯日, 寅年->未日, 巳年->戌日, 午年->亥日.
 * (協紀 按語「其辰又為死符又為小耗」.)
 *
 * 以上六神皆以 太歲 (干支年, 立春換歲) keying -- 唯一非月建/非日柱/
 * 非晚朝 keying 之一組, 故 ShenShaMask()/ShenShaOn() 多一個 suigz 入參 (太歲
 * 60-甲子 index, ccal.cpp 之 GetSuiGanzhi()).
 * 紅沙 (HongSha): 三煞/破碎 之一 -- 三合局之三煞辰 (巳/酉/丑) 之中一支,
 * 取該月代表的一支.  口訣「四孟金雞四仲蛇,四季丑日是紅沙」:
 *   四孟月 (寅巳申亥) -> 酉日, 四仲月 (子卯午酉) -> 巳日,
 *   四季月 (辰未戌丑) -> 丑日.
 * 即 子月->巳, 丑月->丑, 寅月->酉, 卯月->巳, 辰月->丑, 巳月->酉, 午月->巳,
 *   未月->丑, 申月->酉, 酉月->巳, 戌月->丑, 亥月->酉
 * (index = mbranch 0=子..11=亥; i.e. mbranch%3: 0->巳(5), 1->丑(1), 2->酉(9)).
 * 日支恒為 巳/酉/丑 -- 《欽定協紀辨方書》卷五 義例
 * 「巳酉丑謂之三煞,又謂之破碎,又謂之紅沙」, 即通書一系之紅沙日;
 * 卷十 宜忌亦列紅沙為嫁娶之忌.  按 節氣月/月建 mbranch keyed,
 * 同 月破/大耗/災煞/月厭/月刑/大時/天吏/血忌/血支 等.  四年通勝 DB 無任何
 * 「紅沙/紅紗/破碎」印記, 故無 oracle 可核, 依曆例 (同 受死/五墓).
 *
 * 除神 (ChuShen): 日支 申/酉 -- 五離之別名 (通書吉神欄用之吉神名). 《欽定協紀辨方書》
 * 卷五 義例三 五離條下 考原「按通書以申酉日為除神,與五合對衝之日也,宜沐浴祓除
 * 災眚」; 卷九 立成「日神取一定干支者: 五合 寅夘 / 除神〈五離〉申酉 / 重日 己亥」.
 * 日神, 不從月建起義.
 *
 * 陽錯 (YangCuo): 凶.  《欽定協紀辨方書》卷四 義例二「陽錯 隂錯 隂陽俱錯」引
 * 堪輿經曰「以陽建之支配當方之干, 陰陽自相配合為日, 以值所衝之宿為陽錯 -- 如正月
 * 陽建在寅, 近於甲, 支干相配為甲寅日, 寅衝於申, 故正月甲寅之日…為陽錯; 二月陽建
 * 在夘…乙夘日…為陽錯; 三月陽建在辰…得甲辰日…為陽錯也; 餘月倣此」.  協紀按語
 * 「陽建疊陽建為陽錯, 隂建疊隂建為隂錯, 五月十一月隂陽同建又疊同建之干為隂陽俱錯」.
 * 故 陽錯 = 日柱 為 月建之支 配其當方之干 (陽支配陽干、陰支配陰干, 干取該方位之干):
 *   寅月->甲寅日, 卯月->乙卯日, 辰月->甲辰日, 巳月->丁巳日, 午月->丙午日,
 *   未月->丁未日, 申月->庚申日, 酉月->辛酉日, 戌月->庚戌日, 亥月->癸亥日,
 *   子月->壬子日, 丑月->癸丑日.  Keyed to the 節氣月/月建 (mbranch) 與 日柱 (ganzhi).
 * 天恩 (TianEn): 日柱 ∈ {甲子,乙丑,丙寅,丁卯,戊辰, 己卯,庚辰,辛巳,壬午,癸未,
 *   己酉,庚戌,辛亥,壬子,癸丑} (15 日).  《欽定協紀辨方書》卷五 義例三 天寶曆
 *   「天恩者施德寛下之辰也」+ 曆例「常以甲子至戊辰、己卯至癸未、己酉至癸丑，
 *   凡一十五日」; 卷九 立成「日神取一定干支者」; 卷十 宜忌「天恩 宜覃恩肆赦…」.
 *   日神 (不從月建起義), keyed to 日柱 (ganzhi).
 * 母倉 (MuCang): 日支 ∈ 該四時所生之辰 -- 春(寅卯辰月)->亥子日,
 *   夏(巳午未月)->寅卯日, 秋(申酉戌月)->辰戌丑未日, 冬(亥子丑月)->申酉日.
 *   《欽定協紀辨方書》卷五 義例三 天寶曆「母倉者五行當王所生者為母倉…其日
 *   宜飬育羣畜栽植種蒔」+ 曆例「春亥子、夏寅卯、秋辰戌丑未、冬申酉、土王後巳午」;
 *   卷十 宜忌「母倉 宜納財栽種牧養納畜」.  按 節氣月/月建 mbranch keying (同其兄弟).
 *   (協紀另有「土王後巳午」一說, 四季月下半月, ccal 無此粒度, 未實現.)
 * 月恩 (YueEn): 日干 = 月建所生之干.  《欽定協紀辨方書》卷五 義例三 五行論
 *   「月恩者陽建所生之干也, 子母相從謂之月恩」+ 曆例「月恩者正月丙、二月丁、
 *   三月庚、四月己、五月戊、六月辛、七月壬、八月癸、九月庚、十月乙、十一月甲、
 *   十二月辛」; 卷十 宜忌「月恩 四相 時德 宜祭祀祈福求嗣施恩封拜…」.
 *   按 節氣月/月建 mbranch keying (同其兄弟 月破/大耗/災煞…).
 * 四相 (SiXiang): 日干 ∈ 該四時王相之干 -- 春(寅卯辰月)->丙丁日,
 *   夏(巳午未月)->戊己日, 秋(申酉戌月)->壬癸日, 冬(亥子丑月)->甲乙日.
 *   《欽定協紀辨方書》卷五 義例三 總要曆「四相者四時王相之辰也…」+ 曆例
 *   「春丙丁、夏戊己、秋壬癸、冬甲乙」; 曹震圭「四相者養育之道, 母生子也」;
 *   卷十 宜忌「月恩 四相 時德 宜祭祀祈福…」.  按 節氣月/月建 mbranch keying.
 * 時德 (ShiDe, 亦名 四時天德): 日支 = 該四時所生之陽辰 -- 春(寅卯辰月)->午日,
 *   夏(巳午未月)->辰日, 秋(申酉戌月)->子日, 冬(亥子丑月)->寅日.  《欽定協紀
 *   辨方書》卷五 義例三 總要曆「四時天德者四序中德神也…亦名時德」+ 曆例
 *   「春午、夏辰、秋子、冬寅」; 曹震圭「子寅辰午乃東方生育之陽辰…」; 按
 *   「時德與四相同義, 春木生午火…四相取天干時德取地支」.  按節氣月/月建 keying.
 * 天喜 (TianXi): 日支 = 月建後… = 月建+8, 即建除之「成」日.  《欽定協紀辨方書》
 *   卷四 建除十二神所合叢辰「成〈天醫 天喜〉」+ 卷五/四 選擇宗鏡「天喜者正月戌、
 *   二月亥、三月子、四月丑順輪十二月, 乃日支與月建三合故曰天喜…與成日同位」;
 *   協紀 按「今通書與宗鏡皆以成日為天喜…故改從畫一」; 卷十 宜忌「成日〈天喜 天醫〉」.
 *   按 節氣月/月建 mbranch keying.
 * 天醫 (TianYi): 日支 = 月建+8, 即建除之「成」日.  《欽定協紀辨方書》卷四
 *   建除十二神所合叢辰「成〈天醫 天喜〉」+ 總要曆「天醫者天之巫醫…」+ 曆例
 *   「天醫者正月起戌順行十二辰」; 曹震圭「天醫者三合後辰, 能使萬物死而復生」;
 *   卷十 宜忌「成日…乂為天醫宜求醫療病」.  按 節氣月/月建 mbranch keying.
 * 時陽 (ShiYang): 日支 = 月建後二辰, 即建除之「開」日.  《欽定協紀辨方書》卷四
 *   建除十二神所合叢辰「開〈時陽 生氣〉」+ 總要曆「時陽者月中陽神也…」+ 曆例
 *   「常居月建後二辰」; 曹震圭「時陽者月内掌陽氣之神…其日宜叙婚姻行宴樂」;
 *   卷十 宜忌「開日〈時陽 生氣〉」.  按 節氣月/月建 mbranch keying.
 * 生氣 (ShengQi): 日支 = 月建後二辰, 即建除之「開」日 (與 時陽 同辰).  《欽定協紀
 *   辨方書》卷四 建除十二神所合叢辰「開〈時陽 生氣〉」+ 五行論「生氣者極福之神也…」
 *   + 曆例「生氣者常居月建後二辰」; 卷十 宜忌「開日一陽始生故又為時陽又為生氣其日最吉」.
 *   按 節氣月/月建 mbranch keying.
 * 時陰 (ShiYin): 日支 = 月建前四辰, 即建除之「定」日.  《欽定協紀辨方書》卷四
 *   建除十二神所合叢辰「定〈時陰 官符 死氣〉」+ 總要曆「時陰者月中陰神也…」+曆例
 *   「常居月建前四辰」; 曹震圭「時陰者月内陰氣之神…其日宜運謀算畫計䇿」;
 *   卷十 宜忌「定日〈時陰〉…又為時陰宜運謀算畫計策」.  按 節氣月/月建 keying.
 * 陽德 (YangDe) / 陰德 (YinDe): 納甲取乾坤天地生成之義而以月配爻 (卷十 宜忌
 *   「阳德隂德…為乾坤納甲取天地生成之義而以月配爻」).  月建 -> 日支 (正月寅起):
 *   陽德 正月戌、二月子、三月寅、四月辰、五月午、六月申、七月戌… 六辰循環
 *   (即 日支 = (2*月建 + 6) mod 12); 陰德 正月酉、二月未、三月巳、四月卯、
 *   五月丑、六月亥、七月酉… (即 日支 = (13 - 2*月建) mod 12).  卷十 宜忌「陽德 陰德
 *   宜施恩惠恤孤惸行惠愛雪寃枉緩刑獄」.  按 節氣月/月建 mbranch keying.
 *
 * 陰錯 (YinCuo): 凶.  同 卷四 義例二 堪輿經 陰錯 條「以陰建之支配當方之干…如正月
 * 陰建於戌, 近於庚, 支干相配為庚戌日, 戌衝辰…為陰錯; 二月陰建於酉…辛酉日;
 * 三月陰建在申…庚申日…; 唯五月十一月陰陽二建會於子午, 故無陰錯陽錯」.  即
 * 陰錯 = 日柱 為 月厭之支 配其當方之干 (與 陽錯 共用同一張 支->干 表 cuo_gz):
 *   寅月->庚戌日, 卯月->辛酉日, 辰月->庚申日, 巳月->丁未日, 未月->丁巳日,
 *   申月->甲辰日, 酉月->乙卯日, 戌月->甲寅日, 亥月->癸丑日, 丑月->癸亥日;
 *   午月/子月 (陰陽二建同辰) 無 陰錯陽錯 (該日為 陰陽俱錯: 五月丙午、十一月壬子).
 * Keyed to the 節氣月/月建 (mbranch, 經 月厭) 與 日柱 (ganzhi).
 *
* 福生 (FuSheng): 日支 = 該月(節氣月)之福生支。
 * 《欽定協紀辨方書》卷六 義例四 樞要曆「福生者月中福神也,所值之日宜
 * 祈福求恩祀神致祭」; 曆例「福生者正月酉二月夘三月戌四月辰五月亥六月巳
 * 七月子八月午九月丑十月未十一月寅十二月申」。
 *
 *
* 福德 (FuDe): 建除「滿」日: 日支 = 月建前二辰。
 * 《欽定協紀辨方書》卷四 義例二 建除同位異名「滿〈福徳 天巫 天狗〉」;
 * 總要曆「福德者月中福德之神也,其日宜祀神祗求福願修宫室獻封章」,
 * 曆例「福德常居月建前二辰」; 曹震圭「建前二辰者是月所生之子」。
 *
 *
* 吉期 (JiQi): 建除「除」日: 日支 = 月建後一辰。
 * 《欽定協紀辨方書》卷四 義例二 建除同位異名「除〈吉期 兵寶〉」;
 * 總要曆「吉期者吉慶之神也,所值之日宜出軍行師攻城寨興弔伐㑹親姻」,
 * 曆例「常居月建前一辰也」; 曹震圭「吉期者是月建腹心同契之侍臣也」。
 *
 *
* 官日 (GuanRi): 四時臨官之辰 (春卯 夏午 秋酉 冬子), 按節氣月所屬季節。
 * 《欽定協紀辨方書》卷五 義例三 壇經「王官相民守日者皆月内視事之吉辰也」;
 * 曆例「官日者春卯夏午秋酉冬子」(曹震圭「官日者四時臨官之辰,諸侯之象」)。
 * 異文: 四庫本注「今易為王日」-- 通書間有把官日/王日互易者(見王日條),
 * 本表採 協紀 按語所主之舊說(王日寅巳申亥、官日卯午酉子), 與四年通勝 DB 相合。
 *
 *
* 守日 (ShouRi): 四序當王之土神 (春辰 夏未 秋戌 冬丑), 按節氣月所屬季節。
 * 《欽定協紀辨方書》卷五 義例三「至於春辰夏未秋戌冬丑者,四序當王之土神也,
 * 故曰守日,守封疆之臣用之」; 協紀 又按「曆例以春酉夏子秋卯冬午為守日,
 * 以春辰夏未秋戌冬丑為牢日,邵泰衢…謂守與牢二字相近而訛易之,則吉凶各以類聚,
 * 且與五勝義恰符,甚為有理,今從之」-- 故 協紀 定 守日 = 辰未戌丑。
 * 異文: 曆例 守日 = 春酉夏子秋卯冬午; 四年通勝 DB 印 辰未戌丑 之日,
 * 與 協紀 改正本相合 (亦即 三喪 之辰位)。
 *
 *
* 相日 (XiangRi): 四時相氣之辰 (春巳 夏申 秋亥 冬寅), 按節氣月所屬季節。
 * 《欽定協紀辨方書》卷五 義例三 曆例「相日者春巳夏申秋亥冬寅」
 * (按語「春巳夏申秋亥冬寅,四序之相氣,則自元子以下王侯卿相通用之吉辰也」)。
 *
 *
* 民日 (MinRi): 四序當令之真子 (春午 夏酉 秋子 冬卯), 按節氣月所屬季節。
 * 《欽定協紀辨方書》卷五 義例三 曆例「民日者春午夏酉秋子冬卯」
 * (按語「午酉子夘則專火專金專水專木,為四序當令之真子,吉莫大焉,故為民日」)。
 *
 *
* 王日 (WangRi): 四時正王之辰 (春寅 夏巳 秋申 冬亥), 按節氣月所屬季節。
 * 《欽定協紀辨方書》卷五 義例三 曆例「王日者春寅夏巳秋申冬亥」(今易為官日),
 * 曹震圭「王日者四時正王之辰,四正之位,帝王之象」; 協紀 按語 主 舊說
 * 「王日向為寅巳申亥…今悉刪去…帝居帝位官居官位皆臨官也」-- 採 寅巳申亥。
 * 異文: 通書有把 王日/官日 互易者(見官日條); 四年通勝 DB 印 寅巳申亥。
 *
 *
* 臨日 (LinRi): 日支 = 該月(節氣月)之臨日支。
 * 《欽定協紀辨方書》卷六 義例四 樞要曆「臨日者上臨下之義也,其日忌臨民訴訟」;
 * 曆例「臨日者正月午二月亥三月申四月丑五月戌六月夘七月子八月巳九月寅
 * 十月未十一月辰十二月酉」; 曹震圭「建陽之月在三合前辰…建隂之月在三合後辰」
 * (即 陽月取 定日/官符、陰月取 成日/白虎 之位)。
 *
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
      "\xd4\xc2\xd1\xe1", "\xa4\xeb\xb9\xbd",
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
      "\xb8\xb4\xc8\xd5", "\xb4\x5f\xa4\xe9",
      "\xe5\xa4\x8d\xe6\x97\xa5",
      "\xe5\xbe\xa9\xe6\x97\xa5" },   /* 復日 */
    { "TianZei",
      "\xcc\xec\xd4\xf4", "\xa4\xd1\xb8\xe9",
      "\xe5\xa4\xa9\xe8\xb4\xbc",
      "\xe5\xa4\xa9\xe8\xb3\x8a" },   /* 天賊 */
    { "GuiJi",
      "\xb9\xe9\xbc\xc9", "\xc2\x6b\xa7\xd2",
      "\xe5\xbd\x92\xe5\xbf\x8c",
      "\xe6\xad\xb8\xe5\xbf\x8c" },   /* 歸忌 */
    { "YueSha",
      "\xd4\xc2\xc9\xb7", "\xa4\xeb\xb7\xd9",
      "\xe6\x9c\x88\xe7\x85\x9e",
      "\xe6\x9c\x88\xe7\x85\x9e" },   /* 月煞 */
    { "YangJiang",
      "\xd1\xf4\xbd\xab", "\xb6\xa7\xb1\x4e",
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
      "\xd2\xf5\xbd\xab", "\xb3\xb1\xb1\x4e",
      "\xe9\x98\xb4\xe5\xb0\x86",
      "\xe9\x99\xb0\xe5\xb0\x87" },   /* 陰將 */
    { "WuLi",
      "\xce\xe5\xc0\xeb", "\xa4\xad\xc2\xf7",
      "\xe4\xba\x94\xe7\xa6\xbb",
      "\xe4\xba\x94\xe9\x9b\xa2" },   /* 五離 */
    { "XiaoHao",
      "\xd0\xa1\xba\xc4", "\xa4\x70\xaf\xd3",
      "\xe5\xb0\x8f\xe8\x80\x97",
      "\xe5\xb0\x8f\xe8\x80\x97" },   /* 小耗 */
    { "SanSang",
      "\xc8\xfd\xc9\xa5", "\xa4\x54\xb3\xe0",
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
      "\xcb\xc0\xc6\xf8", "\xa6\xba\xae\xf0",
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
      "\xb9\xb4\xb3\xc2", "\xa4\xc4\xb3\xaf",
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
    { "SiJi",
      "\xcb\xc4\xbc\xc9", "\xa5\x7c\xa7\xd2",
      "\xe5\x9b\x9b\xe5\xbf\x8c",
      "\xe5\x9b\x9b\xe5\xbf\x8c" },   /* 四忌 */
    { "SiQiong",
      "\xcb\xc4\xb8\x46", "\xa5\x7c\xbd\x61",
      "\xe5\x9b\x9b\xe7\xaa\xae",
      "\xe5\x9b\x9b\xe7\xaa\xae" },   /* 四窮 */
    { "JiuKong",
      "\xbe\xc5\xbf\xd5", "\xa4\x45\xaa\xc5",
      "\xe4\xb9\x9d\xe7\xa9\xba",
      "\xe4\xb9\x9d\xe7\xa9\xba" },   /* 九空 */
    { "JiuKan",
      "\xbe\xc5\xbf\xb2", "\xa4\x45\xa7\xa2",
      "\xe4\xb9\x9d\xe5\x9d\x8e",
      "\xe4\xb9\x9d\xe5\x9d\x8e" },   /* 九坎 */
    { "XianChi",
      "\xcf\xcc\xb3\xd8",
      "\xab\x77\xa6\xc0",
      "\xe5\x92\xb8\xe6\xb1\xa0",
      "\xe5\x92\xb8\xe6\xb1\xa0" },   /* XianChi */
    { "BanSha",
      "\xb0\xe0\xc9\xb7",
      "\xaf\x5a\xb7\xd9",
      "\xe7\x8f\xad\xe7\x85\x9e",
      "\xe7\x8f\xad\xe7\x85\x9e" },   /* BanSha */
    { "SuiDe",
      "\xcb\xea\xb5\xc2", "\xb7\xb3\xbc\x77",
      "\xe5\xb2\x81\xe5\xbe\xb7",
      "\xe6\xad\xb2\xe5\xbe\xb7" },   /* 歲德   吉 */
    { "SuiDeHe",
      "\xcb\xea\xb5\xc2\xba\xcf", "\xb7\xb3\xbc\x77\xa6\x58",
      "\xe5\xb2\x81\xe5\xbe\xb7\xe5\x90\x88",
      "\xe6\xad\xb2\xe5\xbe\xb7\xe5\x90\x88" },   /* 歲德合 吉 */
    { "SuiLu",
      "\xcb\xea\xc2\xbb", "\xb7\xb3\xb8\x53",
      "\xe5\xb2\x81\xe7\xa6\x84",
      "\xe6\xad\xb2\xe7\xa5\xbf" },   /* 歲祿   吉 */
    { "SuiMa",
      "\xcb\xea\xc2\xed", "\xb7\xb3\xb0\xa8",
      "\xe5\xb2\x81\xe9\xa9\xac",
      "\xe6\xad\xb2\xe9\xa6\xac" },   /* 歲馬   吉 */
    { "SuiZhi",
      "\xcb\xea\xd6\xa7", "\xb7\xb3\xa4\xe4",
      "\xe5\xb2\x81\xe6\x94\xaf",
      "\xe6\xad\xb2\xe6\x94\xaf" },   /* 歲支   吉 */
    { "SuiZhiDe",
      "\xcb\xea\xd6\xa7\xb5\xc2", "\xb7\xb3\xa4\xe4\xbc\x77",
      "\xe5\xb2\x81\xe6\x94\xaf\xe5\xbe\xb7",
      "\xe6\xad\xb2\xe6\x94\xaf\xe5\xbe\xb7" },   /* 歲支德 吉 */
    { "HongSha",
      "\xba\xec\xc9\xb3", "\xac\xf5\xa8\x46",
      "\xe7\xba\xa2\xe6\xb2\x99",
      "\xe7\xb4\x85\xe6\xb2\x99" },   /* 紅沙 */
    { "ChuShen",
      "\xb3\xfd\xc9\xf1", "\xb0\xa3\xaf\xab",
      "\xe9\x99\xa4\xe7\xa5\x9e",
      "\xe9\x99\xa4\xe7\xa5\x9e" },   /* 除神 */
    { "JiuJiao",
      "\xbe\xc5\xbd\xb9", "\xa4\x45\xb5\x4a",
      "\xe4\xb9\x9d\xe7\x84\xa6",
      "\xe4\xb9\x9d\xe7\x84\xa6" },   /* 九焦 */
    { "BaFeng",
      "\xb0\xcb\xef\x4c", "\xa4\x4b\xad\xb7",
      "\xe5\x85\xab\xe9\xa2\xa8",
      "\xe5\x85\xab\xe9\xa2\xa8" },   /* 八風 */
    { "YangCuo",
      "\xd1\xf4\xb4\xed",
      "\xb6\xa7\xbf\xf9",
      "\xe9\x98\xb3\xe9\x94\x99",
      "\xe9\x99\xbd\xe9\x8c\xaf" },   /* YangCuo */
    { "TianEn",
      "\xcc\xec\xb6\xf7", "\xa4\xd1\xae\xa6",
      "\xe5\xa4\xa9\xe6\x81\xa9",
      "\xe5\xa4\xa9\xe6\x81\xa9" },   /* 天恩 */
    { "MuCang",
      "\xc4\xb8\xb2\xd6", "\xa5\xc0\xad\xdc",
      "\xe6\xaf\x8d\xe4\xbb\x93",
      "\xe6\xaf\x8d\xe5\x80\x89" },   /* 母倉 */
    { "YueEn",
      "\xd4\xc2\xb6\xf7", "\xa4\xeb\xae\xa6",
      "\xe6\x9c\x88\xe6\x81\xa9",
      "\xe6\x9c\x88\xe6\x81\xa9" },   /* 月恩 */
    { "SiXiang",
      "\xcb\xc4\xcf\xe0", "\xa5\x7c\xac\xdb",
      "\xe5\x9b\x9b\xe7\x9b\xb8",
      "\xe5\x9b\x9b\xe7\x9b\xb8" },   /* 四相 */
    { "ShiDe",
      "\xca\xb1\xb5\xc2", "\xae\xc9\xbc\x77",
      "\xe6\x97\xb6\xe5\xbe\xb7",
      "\xe6\x99\x82\xe5\xbe\xb7" },   /* 時德 */
    { "TianXi",
      "\xcc\xec\xcf\xb2", "\xa4\xd1\xb3\xdf",
      "\xe5\xa4\xa9\xe5\x96\x9c",
      "\xe5\xa4\xa9\xe5\x96\x9c" },   /* 天喜 */
    { "TianYi",
      "\xcc\xec\xd2\xbd", "\xa4\xd1\xc2\xe5",
      "\xe5\xa4\xa9\xe5\x8c\xbb",
      "\xe5\xa4\xa9\xe9\x86\xab" },   /* 天醫 */
    { "ShiYang",
      "\xca\xb1\xd1\xf4", "\xae\xc9\xb6\xa7",
      "\xe6\x97\xb6\xe9\x98\xb3",
      "\xe6\x99\x82\xe9\x99\xbd" },   /* 時陽 */
    { "ShengQi",
      "\xc9\xfa\xc6\xf8", "\xa5\xcd\xae\xf0",
      "\xe7\x94\x9f\xe6\xb0\x94",
      "\xe7\x94\x9f\xe6\xb0\xa3" },   /* 生氣 */
    { "ShiYin",
      "\xca\xb1\xd2\xf5", "\xae\xc9\xb3\xb1",
      "\xe6\x97\xb6\xe9\x98\xb4",
      "\xe6\x99\x82\xe9\x99\xb0" },   /* 時陰 */
    { "YangDe",
      "\xd1\xf4\xb5\xc2", "\xb6\xa7\xbc\x77",
      "\xe9\x98\xb3\xe5\xbe\xb7",
      "\xe9\x99\xbd\xe5\xbe\xb7" },   /* 陽德 */
    { "YinCuo",
      "\xd2\xf5\xb4\xed",
      "\xb3\xb1\xbf\xf9",
      "\xe9\x98\xb4\xe9\x94\x99",
      "\xe9\x99\xb0\xe9\x8c\xaf" },   /* YinCuo */
    { "YinDe",
      "\xd2\xf5\xb5\xc2", "\xb3\xb1\xbc\x77",
      "\xe9\x98\xb4\xe5\xbe\xb7",
      "\xe9\x99\xb0\xe5\xbe\xb7" },   /* 陰德 */
    { "YueKong",
      "\xd4\xc2\xbf\xd5", "\xa4\xeb\xaa\xc5",
      "\xe6\x9c\x88\xe7\xa9\xba",
      "\xe6\x9c\x88\xe7\xa9\xba" },   /* 月空 */
    { "FuSheng",
      "\xb8\xa3\xc9\xfa", "\xba\xd6\xa5\xcd",
      "\xe7\xa6\x8f\xe7\x94\x9f",
      "\xe7\xa6\x8f\xe7\x94\x9f" },   /* 福生 */
    { "FuDe",
      "\xb8\xa3\xb5\xc2", "\xba\xd6\xbc\x77",
      "\xe7\xa6\x8f\xe5\xbe\xb7",
      "\xe7\xa6\x8f\xe5\xbe\xb7" },   /* 福德 */
    { "JiQi",
      "\xbc\xaa\xc6\xda", "\xa6\x4e\xb4\xc1",
      "\xe5\x90\x89\xe6\x9c\x9f",
      "\xe5\x90\x89\xe6\x9c\x9f" },   /* 吉期 */
    { "GuanRi",
      "\xb9\xd9\xc8\xd5", "\xa9\x78\xa4\xe9",
      "\xe5\xae\x98\xe6\x97\xa5",
      "\xe5\xae\x98\xe6\x97\xa5" },   /* 官日 */
    { "ShouRi",
      "\xca\xd8\xc8\xd5", "\xa6\x75\xa4\xe9",
      "\xe5\xae\x88\xe6\x97\xa5",
      "\xe5\xae\x88\xe6\x97\xa5" },   /* 守日 */
    { "XiangRi",
      "\xcf\xe0\xc8\xd5", "\xac\xdb\xa4\xe9",
      "\xe7\x9b\xb8\xe6\x97\xa5",
      "\xe7\x9b\xb8\xe6\x97\xa5" },   /* 相日 */
    { "MinRi",
      "\xc3\xf1\xc8\xd5", "\xa5\xc1\xa4\xe9",
      "\xe6\xb0\x91\xe6\x97\xa5",
      "\xe6\xb0\x91\xe6\x97\xa5" },   /* 民日 */
    { "WangRi",
      "\xcd\xf5\xc8\xd5", "\xa4\xfd\xa4\xe9",
      "\xe7\x8e\x8b\xe6\x97\xa5",
      "\xe7\x8e\x8b\xe6\x97\xa5" },   /* 王日 */
    { "LinRi",
      "\xc1\xd9\xc8\xd5", "\xc1\x7b\xa4\xe9",
      "\xe4\xb8\xb4\xe6\x97\xa5",
      "\xe8\x87\xa8\xe6\x97\xa5" },   /* 臨日 */
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

/* 四忌: 春甲子 夏丙子 秋庚子 冬壬子 (協紀卷五 義例三 神煞起例). */
static const char siji_gz[4] = {0, 12, 36, 48};

/* 四窮: 春乙亥 夏丁亥 秋辛亥 冬癸亥 (協紀卷五 義例三 總要曆). */
static const char siqiong_gz[4] = {11, 23, 47, 59};

/* 九空: 正月在辰逆行四季 (辰丑戌未循環), keyed to 月建 (協紀卷五 廣聖曆). */
static const char jiukong_branch[12] = {10, 7, 4, 1, 10, 7, 4, 1, 10, 7, 4, 1};

/* 九坎/九焦: 正月在辰逆行四季、五月在卯逆行四仲、九月在寅逆行四孟 (協紀卷五 廣聖曆). */
static const char jiukan_branch[12] = {8, 5, 4, 1, 10, 7, 3, 0, 9, 6, 2, 11};

/* 咸池 (XianChi) -- 見檔頭: 與 大時 同辰 (月建三合局之沐浴位), 即 `dashi_branch`. */
/* 班煞 (BanSha) -- 見檔頭: 春(寅卯辰月)->子日, 夏(巳午未月)->卯日,
   秋(申酉戌月)->午日, 冬(亥子丑月)->酉日 (取四正之氣為地師之煞).
   Index = season_of(mbranch) 0春1夏2秋3冬. */
static const char bansha_branch[4] = {0, 3, 6, 9};
/* 太歲 (歲) keyed 吉神 -- see the header comment.  suigz = 太歲 60-甲子 index
   (0 = 甲子, from ccal's GetSuiGanzhi(): the 干支年 turns at 立春). */
/* 歲德: 德干 of each 年干 (廣聖曆 甲德在甲 乙德在庚 丙德在丙 丁德在壬 戊德在戊
   己德在甲 庚德在庚 辛德在丙 壬德在壬 癸德在戊).  index = 年干 0=甲..9=癸. */
static const char suide_stem[10] = {0, 6, 2, 8, 4, 0, 6, 2, 8, 4};
/* 歲祿: the 十干祿 branch -- 甲寅 乙卯 丙巳 丁午 戊巳 己午 庚申 辛酉 壬亥 癸子.
   index = 年干 0=甲..9=癸. */
static const char suilu_branch[10] = {2, 3, 5, 6, 5, 6, 8, 9, 11, 0};
/* 歲馬: the 驛馬 branch -- 寅午戌->申, 申子辰->寅, 巳酉丑->亥, 亥卯未->巳.
   index = 年支 0=子..11=亥. */
static const char suima_branch[12] = {2, 11, 8, 5, 2, 11, 8, 5, 2, 11, 8, 5};


/* 紅沙 by 月建 (solar-term month): 「四孟金雞四仲蛇,四季丑日是紅沙」 on the day branch.
   Index = mbranch 0=子..11=亥: 子(0)->巳(5), 丑(1)->丑(1), 寅(2)->酉(9), 卯(3)->巳(5),
   辰(4)->丑(1), 巳(5)->酉(9), 午(6)->巳(5), 未(7)->丑(1), 申(8)->酉(9), 酉(9)->巳(5),
   戌(10)->丑(1), 亥(11)->酉(9)  (孟月->酉, 仲月->巳, 季月->丑). */
static const char hongsha_branch[12] = {5, 1, 9, 5, 1, 9, 5, 1, 9, 5, 1, 9};
/* 八風: 春丁丑丁巳 夏甲申甲辰 秋丁未丁亥 冬甲戌甲寅 (協紀卷五 樞要曆 + 按語所正). */
static const char bafeng_gz[4][2] = {{13, 53}, {20, 40}, {43, 23}, {10, 50}};

/* 陽錯 / 陰錯 (YangCuo / YinCuo) -- 見檔頭: 日柱 == 該建之支配「當方之干」.
   堪輿經 (協紀卷四 義例二 引) 以 陽建 (月建) 之支、陰建 (月厭) 之支 各配其方之干:
   寅甲寅 卯乙卯 辰甲辰 巳丁巳 午丙午 未丁未 申庚申 酉辛酉 戌庚戌 亥癸亥 子壬子 丑癸丑.
   Index = 支 0=子..11=亥; 五月(午月)/十一月(子月) 陰陽二建同辰 -> 無 陰錯陽錯. */
static const char cuo_gz[12] = {48, 49, 50, 51, 40, 53, 42, 43, 56, 57, 46, 59};
/* 天恩 日柱 (60-甲子 index): 甲子..戊辰 己卯..癸未 己酉..癸丑 */
static const char tianen_gz[15] = {0,1,2,3,4,15,16,17,18,19,45,46,47,48,49};

/* 母倉 by 四時 (月建/節氣月): 春(寅卯辰)->亥子, 夏(巳午未)->寅卯,
   秋(申酉戌)->辰戌丑未, 冬(亥子丑)->申酉. */
static const char mucang_branch[4][4] = {
    {11, 0, -1, -1}, {2, 3, -1, -1}, {4, 10, 1, 7}, {8, 9, -1, -1} };

/* 月恩 by 月建 (節氣月) -> 日干: 寅丙 卯丁 辰庚 巳己 午戊 未辛 申壬 酉癸 戌庚 亥乙 子甲 丑辛. */
static const char yueen_stem[12] = {0, 7, 2, 3, 6, 5, 4, 7, 8, 9, 6, 1};

/* 四相 by 四時 (月建) -> 日干: 春丙丁 夏戊己 秋壬癸 冬甲乙. */
static const char sixiang_stem[4][2] = {{2, 3}, {4, 5}, {8, 9}, {0, 1}};

/* 時德 by 四時 (月建) -> 日支: 春午 夏辰 秋子 冬寅. */
static const char shide_branch[4] = {6, 4, 0, 2};

/* 陰錯 (YinCuo) -- 見檔頭 陽錯 條: 同一張支干表, 但鍵於 陰建 (月厭) 之支
   (月厭 = (12 - 月建) mod 12, 即 `yueyan_branch`), 而非 月建. */

/* 月空: 月德之沖干 -- 《欽定協紀辨方書》卷五 義例三 曆例「寅午戌月壬 亥卯未月庚
   申子辰月丙 巳酉丑月甲」; 曆神原始「月德自南而東丙甲壬庚, 月空自北而西壬庚丙甲, 乃天德之衝神也」
   (寅甲 卯乙 辰戊 巳丙 午丁 未己 申庚 酉辛 戌戊 亥壬 子癸 丑己 之沖). 宜上表章. */
static const char yuekong_stem[12] = {2, 0, 8, 6, 2, 0, 8, 6, 2, 0, 8, 6};
/* 福生 (FuSheng): 日支 = 該月(節氣月)之福生支。
 * 《欽定協紀辨方書》卷六 義例四 樞要曆「福生者月中福神也,所值之日宜
 * 祈福求恩祀神致祭」; 曆例「福生者正月酉二月夘三月戌四月辰五月亥六月巳
 * 七月子八月午九月丑十月未十一月寅十二月申」。
 */
static const char fusheng_branch[12] = {2, 8, 9, 3, 10, 4, 11, 5, 0, 6, 1, 7};

/* 福德 (FuDe): 建除「滿」日: 日支 = 月建前二辰。
 * 《欽定協紀辨方書》卷四 義例二 建除同位異名「滿〈福徳 天巫 天狗〉」;
 * 總要曆「福德者月中福德之神也,其日宜祀神祗求福願修宫室獻封章」,
 * 曆例「福德常居月建前二辰」; 曹震圭「建前二辰者是月所生之子」。
 */

/* 吉期 (JiQi): 建除「除」日: 日支 = 月建後一辰。
 * 《欽定協紀辨方書》卷四 義例二 建除同位異名「除〈吉期 兵寶〉」;
 * 總要曆「吉期者吉慶之神也,所值之日宜出軍行師攻城寨興弔伐㑹親姻」,
 * 曆例「常居月建前一辰也」; 曹震圭「吉期者是月建腹心同契之侍臣也」。
 */

/* 官日 (GuanRi): 四時臨官之辰 (春卯 夏午 秋酉 冬子), 按節氣月所屬季節。
 * 《欽定協紀辨方書》卷五 義例三 壇經「王官相民守日者皆月内視事之吉辰也」;
 * 曆例「官日者春卯夏午秋酉冬子」(曹震圭「官日者四時臨官之辰,諸侯之象」)。
 * 異文: 四庫本注「今易為王日」-- 通書間有把官日/王日互易者(見王日條),
 * 本表採 協紀 按語所主之舊說(王日寅巳申亥、官日卯午酉子), 與四年通勝 DB 相合。
 */
static const char guanri_branch[4] = {3, 6, 9, 0};

/* 守日 (ShouRi): 四序當王之土神 (春辰 夏未 秋戌 冬丑), 按節氣月所屬季節。
 * 《欽定協紀辨方書》卷五 義例三「至於春辰夏未秋戌冬丑者,四序當王之土神也,
 * 故曰守日,守封疆之臣用之」; 協紀 又按「曆例以春酉夏子秋卯冬午為守日,
 * 以春辰夏未秋戌冬丑為牢日,邵泰衢…謂守與牢二字相近而訛易之,則吉凶各以類聚,
 * 且與五勝義恰符,甚為有理,今從之」-- 故 協紀 定 守日 = 辰未戌丑。
 * 異文: 曆例 守日 = 春酉夏子秋卯冬午; 四年通勝 DB 印 辰未戌丑 之日,
 * 與 協紀 改正本相合 (亦即 三喪 之辰位)。
 */
static const char shouri_branch[4] = {4, 7, 10, 1};

/* 相日 (XiangRi): 四時相氣之辰 (春巳 夏申 秋亥 冬寅), 按節氣月所屬季節。
 * 《欽定協紀辨方書》卷五 義例三 曆例「相日者春巳夏申秋亥冬寅」
 * (按語「春巳夏申秋亥冬寅,四序之相氣,則自元子以下王侯卿相通用之吉辰也」)。
 */
static const char xiangri_branch[4] = {5, 8, 11, 2};

/* 民日 (MinRi): 四序當令之真子 (春午 夏酉 秋子 冬卯), 按節氣月所屬季節。
 * 《欽定協紀辨方書》卷五 義例三 曆例「民日者春午夏酉秋子冬卯」
 * (按語「午酉子夘則專火專金專水專木,為四序當令之真子,吉莫大焉,故為民日」)。
 */
static const char minri_branch[4] = {6, 9, 0, 3};

/* 王日 (WangRi): 四時正王之辰 (春寅 夏巳 秋申 冬亥), 按節氣月所屬季節。
 * 《欽定協紀辨方書》卷五 義例三 曆例「王日者春寅夏巳秋申冬亥」(今易為官日),
 * 曹震圭「王日者四時正王之辰,四正之位,帝王之象」; 協紀 按語 主 舊說
 * 「王日向為寅巳申亥…今悉刪去…帝居帝位官居官位皆臨官也」-- 採 寅巳申亥。
 * 異文: 通書有把 王日/官日 互易者(見官日條); 四年通勝 DB 印 寅巳申亥。
 */
static const char wangri_branch[4] = {2, 5, 8, 11};

/* 臨日 (LinRi): 日支 = 該月(節氣月)之臨日支。
 * 《欽定協紀辨方書》卷六 義例四 樞要曆「臨日者上臨下之義也,其日忌臨民訴訟」;
 * 曆例「臨日者正月午二月亥三月申四月丑五月戌六月夘七月子八月巳九月寅
 * 十月未十一月辰十二月酉」; 曹震圭「建陽之月在三合前辰…建隂之月在三合後辰」
 * (即 陽月取 定日/官符、陰月取 成日/白虎 之位)。
 */
static const char linri_branch[12] = {4, 9, 6, 11, 8, 1, 10, 3, 0, 5, 2, 7};

int ShenShaOn(int i, int lmonth, int lday, int ganzhi, int mbranch, int dbranch,
              int wday, int suigz)
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
    case SS_SIJI:
        /* 四忌 (SiJi): 《欽定協紀辨方書》卷五 義例三 神煞起例「四忌 春甲子 夏丙子
           秋庚子 冬壬子」-- 本令陽干加於辰首(子), keyed to the season (節氣月). */
        return ganzhi == siji_gz[season_of(mbranch)];
    case SS_SIQIONG:
        /* 四窮 (SiQiong): 《欽定協紀辨方書》卷五 義例三 總要曆「四窮者謂亥為隂絶之辰,
           以四時旺干臨之…曆例曰春乙亥 夏丁亥 秋辛亥 冬癸亥」, keyed to the season. */
        return ganzhi == siqiong_gz[season_of(mbranch)];
    case SS_JIUKONG:
        /* 九空 (JiuKong): 《欽定協紀辨方書》卷五 義例三 廣聖曆「九空者月内殺神也,
           其日忌修造倉庫出入貨財」+ 曆例「九空者正月在辰逆行四季」; 曹震圭
           「九空者墓庫破散之神也…今歴家所傳與九坎九焦同行者非」-- 即月建三合局之
           墓辰受衝 (寅午戌月火庫戌, 辰衝之). Keyed to the 節氣月/月建. */
        return dbranch == jiukong_branch[mbranch];
    case SS_JIUKAN:
        /* 九坎 (JiuKan): 《欽定協紀辨方書》卷五 義例三 廣聖曆「九坎者月中殺神也,
           其日忌乘船渡水修堤防築垣墻苫蓋屋舍」+ 曆例「九坎者正月在辰逆行四季,
           五月在卯逆行四仲, 九月在寅逆行四孟」(寅月辰日 卯月丑日 辰月戌日 巳月未日
           午月卯日 未月子日 申月酉日 酉月午日 戌月寅日 亥月亥日 子月申日 丑月巳日),
           keyed to the 節氣月/月建. */
        return dbranch == jiukan_branch[mbranch];
    case SS_XIANCHI:
        /* 咸池 = 大時: 日支 == 月建三合局之沐浴位 (協紀卷五 "大時者咸池也"). */
        return dbranch == dashi_branch[mbranch];
    case SS_BANSHA:
        /* 班煞 (魯班煞/殺師日): 日支 == 該季節之四正 (見檔頭). */
        return dbranch == bansha_branch[season_of(mbranch)];
    case SS_SUIDE:
        /* 歲德: 日干 = 太歲干之德 (甲甲 乙庚 丙丙 丁壬 戊戊 己甲 庚庚 辛丙 壬壬 癸戊). */
        return (ganzhi % 10) == suide_stem[suigz % 10];
    case SS_SUIDEHE:
        /* 歲德合: 日干 = 歲德干之五合 (甲己 乙庚 丙辛 丁壬 戊癸). */
        return (ganzhi % 10) == (suide_stem[suigz % 10] + 5) % 10;
    case SS_SUILU:
        /* 歲祿: 日支 = 太歲干之祿 (甲寅 乙卯 丙巳 丁午 戊巳 己午 庚申 辛酉 壬亥 癸子). */
        return dbranch == suilu_branch[suigz % 10];
    case SS_SUIMA:
        /* 歲馬: 日支 = 太歲支之驛馬 (寅午戌->申, 申子辰->寅, 巳酉丑->亥, 亥卯未->巳). */
        return dbranch == suima_branch[suigz % 12];
    case SS_SUIZHI:
    {
        /* 歲支 (通書 印作 「歲支三合」/「歲支六合」):
           日支 與 太歲支 三合或六合 (不含 太歲支 自身). */
        int yb = suigz % 12;
        return dbranch == (yb + 4) % 12 || dbranch == (yb + 8) % 12 ||
               dbranch == (13 - yb) % 12;
    }
    case SS_SUIZHIDE:
        /* 歲支德 (協紀 作 歲枝德): 日支 = 太歲支前五辰 (通勝 印作 「歲支德日」). */
        return dbranch == (suigz % 12 + 5) % 12;
    case SS_HONGSHA:
        /* 紅沙: 日支 == 該月(節氣月)三煞/破碎之紅沙辰 -- 孟月酉日/仲月巳日/季月丑日. */
        return dbranch == hongsha_branch[mbranch];
    case SS_CHUSHEN:
        /* 除神 = 五離之吉神名: 日支 申/酉 (協紀卷五 考原「以申酉日為除神,與五合對衝
           之日也,宜沐浴祓除災眚」; 卷九 立成「除神〈五離〉申酉」). */
        return dbranch == 8 || dbranch == 9;
    case SS_JIUJIAO:
        /* 九焦 (JiuJiao): 《欽定協紀辨方書》卷五 義例三 廣聖曆「九焦者月中殺神也,
           其日忌爐冶鑄造種植修築園圃」+ 曆例與九坎同 (正月在辰逆行四季、五月在卯
           逆行四仲、九月在寅逆行四孟); 曹震圭「坎者陷也險也不平也,義與九焦同」. */
        return dbranch == jiukan_branch[mbranch];
    case SS_BAFENG:
        /* 八風 (BaFeng): 《欽定協紀辨方書》卷五 義例三 樞要曆「咸池招搖八風觸水龍所值
           之日忌乘船渡水涉江河」+ 卷首 奏議「八風乃取四時對衝三合之辰以木火二干配之…
           春衝酉用丁丑丁巳日, 夏衝子用甲申甲辰日, 秋衝卯用丁亥丁未日, 冬衝午用甲寅
           甲戌日」-- 即五虛之三支去其正位, 配丁/甲. Keyed to the season (節氣月). */
        return ganzhi == bafeng_gz[season_of(mbranch)][0] ||
               ganzhi == bafeng_gz[season_of(mbranch)][1];
    case SS_YANGCUO:
        /* 陽錯: 日柱 == 陽建(月建)之支所配之干 (五月/十一月陰陽同建, 無). */
        if (mbranch == 0 || mbranch == 6)
            return 0;
        return ganzhi == cuo_gz[mbranch];
    case SS_TIANEN:
    {
        /* 天恩: 日柱 ∈ {甲子..戊辰, 己卯..癸未, 己酉..癸丑} (15 日). 日神, 不從月建. */
        int k;
        for (k = 0; k < 15; k++) if (ganzhi == tianen_gz[k]) return 1;
        return 0;
    }
    case SS_MUCANG:
    {
        /* 母倉: 日支 ∈ 該四時所生之辰 (春亥子/夏寅卯/秋辰戌丑未/冬申酉). */
        int k; const char* t = mucang_branch[season_of(mbranch)];
        for (k = 0; k < 4; k++) if (t[k] >= 0 && dbranch == t[k]) return 1;
        return 0;
    }
    case SS_YUEEN:
        /* 月恩: 日干 = 月建所生之干 (曆例 正月丙 二月丁 …). */
        return ganzhi % 10 == yueen_stem[mbranch];
    case SS_SIXIANG:
    {
        /* 四相: 日干 ∈ 該四時所生之干 (春丙丁/夏戊己/秋壬癸/冬甲乙). */
        int k; const char* t = sixiang_stem[season_of(mbranch)];
        for (k = 0; k < 2; k++) if (ganzhi % 10 == t[k]) return 1;
        return 0;
    }
    case SS_SHIDE:
        /* 時德: 日支 = 該四時所生之陽辰 (春午/夏辰/秋子/冬寅). */
        return dbranch == shide_branch[season_of(mbranch)];
    case SS_TIANXI:
        /* 天喜: 日支 = 月建+8 (= 建除 成日; 選擇宗鏡「與成日同位」, 協紀改從成日). */
        return dbranch == (mbranch + 8) % 12;
    case SS_TIANYI:
        /* 天醫: 日支 = 月建+8 (= 建除 成日; 曆例「正月起戌順行十二辰」). */
        return dbranch == (mbranch + 8) % 12;
    case SS_SHIYANG:
        /* 時陽: 日支 = 月建後二辰 (= 建除 開日). */
        return dbranch == (mbranch + 10) % 12;
    case SS_SHENGQI:
        /* 生氣: 日支 = 月建後二辰 (= 建除 開日). */
        return dbranch == (mbranch + 10) % 12;
    case SS_SHIYIN:
        /* 時陰: 日支 = 月建前四辰 (= 建除 定日, 死氣同位). */
        return dbranch == (mbranch + 4) % 12;
    case SS_YANGDE:
        /* 陽德: 日支 = (2*月建 + 6) mod 12 (納甲天地生成; 正月戌 順行六辰循環). */
        return dbranch == (2 * mbranch + 6) % 12;
    case SS_YINCUO:
        /* 陰錯: 日柱 == 陰建(月厭)之支所配之干 (五月/十一月陰陽同建, 無). */
        if (mbranch == 0 || mbranch == 6)
            return 0;
        return ganzhi == cuo_gz[yueyan_branch[mbranch]];
    case SS_YINDE:
        /* 陰德: 日支 = (13 - 2*月建) mod 12 (正月酉 逆行六辰循環). */
        return dbranch == (13 - 2 * mbranch + 24) % 12;
    case SS_YUEKONG:
        /* 月空 = 日干 為月德之五合(沖)干 (寅午戌月壬/亥卯未月庚/申子辰月丙/巳酉丑月甲). */
        return (ganzhi % 10) == yuekong_stem[mbranch];
    case SS_FUSHENG:
        /* 福生: 日支 == 該月(節氣月)之福生支 (正月酉, 二月夘 ... 十二月申). */
        return dbranch == fusheng_branch[mbranch];
    case SS_FUDE:
        /* 福德: 滿日 = 月建前二辰 (日支 == mbranch + 2). */
        return dbranch == (mbranch + 2) % 12;
    case SS_JIQI:
        /* 吉期: 除日 = 月建後一辰 (日支 == mbranch + 1). */
        return dbranch == (mbranch + 1) % 12;
    case SS_GUANRI:
        /* 官日: 春卯 夏午 秋酉 冬子 (season_of(mbranch)). */
        return dbranch == guanri_branch[season_of(mbranch)];
    case SS_SHOURI:
        /* 守日: 春辰 夏未 秋戌 冬丑 (協紀改正本; season_of(mbranch)). */
        return dbranch == shouri_branch[season_of(mbranch)];
    case SS_XIANGRI:
        /* 相日: 春巳 夏申 秋亥 冬寅 (season_of(mbranch)). */
        return dbranch == xiangri_branch[season_of(mbranch)];
    case SS_MINRI:
        /* 民日: 春午 夏酉 秋子 冬卯 (season_of(mbranch)). */
        return dbranch == minri_branch[season_of(mbranch)];
    case SS_WANGRI:
        /* 王日: 春寅 夏巳 秋申 冬亥 (season_of(mbranch)). */
        return dbranch == wangri_branch[season_of(mbranch)];
    case SS_LINRI:
        /* 臨日: 日支 == 該月(節氣月)之臨日支 (正月午, 二月亥 ... 十二月酉). */
        return dbranch == linri_branch[mbranch];
    }
    return 0;
}

/* The mask must be wider than 64 bits: SS_COUNT now exceeds 64 and `1ULL << i`
   for i >= 64 wraps (x86-64 takes the shift count mod 64), so the newest markers
   silently lit up bit 0/1/2 -- 天赦/天德/天德合.  天赦 was printed on 94 days
   of 2006 instead of the 25 its rule allows.  unsigned __int128 holds all of
   them (gcc/clang on 64-bit; the rest of this table already assumes a 64-bit
   target for `1ULL`). */
unsigned __int128 ShenShaMask(int lmonth, int lday, int ganzhi, int mbranch, int dbranch,
                               int wday, int suigz)
{
    int i;
    unsigned __int128 mask = 0;
    for (i = 0; i < SS_COUNT; i++)
        if (ShenShaOn(i, lmonth, lday, ganzhi, mbranch, dbranch, wday, suigz))
            mask |= (unsigned __int128)1 << i;
    return mask;
}

/* Join the names of the set bits with "、", in table order. */
void ShenShaText(unsigned __int128 mask, int enc, char* buf, int bufsize)
{
    const char* sep = (enc == 'a') ? ", " :
                      (enc == 'g') ? "\xa1\xa2" :
                      (enc == 'b') ? "\xa1\x42" : "\xe3\x80\x81";
    int i, first = 1;
    buf[0] = 0;
    for (i = 0; i < SS_COUNT; i++)
    {
        const char* n;
        if (!((mask >> i) & 1))
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
