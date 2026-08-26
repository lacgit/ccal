/*
   Copyright (c) 2000-2012, by Zhuo Meng (zxm8@case.edu).
   All rights reserved.

   Distributed under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/* Calendar with Chinese calendar */

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "lunaryear.h"
#include "htmlmonth.h"
#include "psmonth.h"
#include "jianchu.h"
#include "verstr.h"

extern "C" {
#include "novas.h"
}


#ifndef NO_NAMESPACE
using namespace std;
#endif

#ifndef WIN32
#define HILIGHTTODAY
#endif

static char jieqi[24][3] = {"XH", "DH", "LC", "YS", "JZ", "CF", "QM", "GY",
                            "LX", "XM", "MZ", "XZ", "XS", "DS", "LQ", "CS",
                            "BL", "QF", "HL", "SJ", "LD", "XX", "DX", "DZ"};
char monnames[12][10] = {"January", "February", "March", "April",
                         "May", "June", "July", "August",
                         "September", "October", "November", "December"};
char daynames[7][10]= {"Sunday", "Monday", "Tuesday", "Wednesday",
                       "Thursday", "Friday", "Saturday"};
char daynamesGB[7][10]= {"Sun  \xc8\xd5", "Mon  \xd2\xbb", "Tue  \xb6\xfe",
                         "Wed  \xc8\xfd", "Thu  \xcb\xc4", "Fri  \xce\xe5",
                         "Sat  \xc1\xf9"};
char daynamesB5[7][10]= {"Sun  \xa4\xe9", "Mon  \xa4\x40", "Tue  \xa4\x47",
                         "Wed  \xa4\x54", "Thu  \xa5\x7c", "Fri  \xa4\xad",
                         "Sat  \xa4\xbb"};
char daynamesU8[7][10]= {"Sun  \xe6\x97\xa5", "Mon  \xe4\xb8\x80",
                         "Tue  \xe4\xba\x8c", "Wed  \xe4\xb8\x89",
                         "Thu  \xe5\x9b\x9b", "Fri  \xe4\xba\x94",
                         "Sat  \xe5\x85\xad"};
unsigned short int daysinmonth[12] = {31, 28, 31, 30, 31, 30,
                                      31, 31, 30, 31, 30, 31};
static char tiangan[10][5] = {"Jia", "Yi", "Bing", "Ding", "Wu",
                              "Ji", "Geng", "Xin", "Ren", "Gui"};
static char dizhi[12][5] = {"Zi", "Chou", "Yin", "Mou", "Chen", "Si",
                            "Wu", "Wei", "Shen", "You", "Xu", "Hai"};
/* 建除十二神 ASCII codes (for -a and PS output) */
static char jianchu_ascii[12][3] = {"JN", "CU", "MN", "PG", "DG", "ZH",
                                    "PO", "WE", "CG", "SO", "KI", "BI"};
/* 月建 (month branch) for each 節, k=0..11 = 小寒,立春,驚蟄,清明,立夏,芒種,
   小暑,立秋,白露,寒露,立冬,大雪.  Branch: 子=0 丑=1 寅=2 ... 亥=11. */
static int jieyuejian[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0};

/* 月建 (month branch) for the day with julian day jd: the 12 節 (even
   indices of vterms) set the month branch.  0=子 1=丑 2=寅 ... 11=亥. */
int GetMonthBranch(double jd, vdouble& vterms)
{
    int jdi = int(jd);
    if (jdi < int(vterms[0]))
        return 0;  /* before 小寒 -> 子月 (prev-year 大雪) */
    for (int k = 11; k >= 0; k--)
        if (jdi >= int(vterms[2 * k]))
            return jieyuejian[k];
    return 0;
}

/* 建除十二神 index (0..11 = 建除滿平定執破危成收開閉) for the day with
   julian day jd (whole-day noon JD).  Reuses the solar terms in vterms to
   derive 月建, and the day's Earthly branch from the Julian day number. */
int GetJianChu(double jd, vdouble& vterms)
{
    int jdi = int(jd);
    int daybranch = (jdi + 1) % 12;
    int monthbranch = GetMonthBranch(jd, vterms);
    return (daybranch - monthbranch + 12) % 12;
}

/* 月柱 天干: 年上起月 (五虎遁).  ystem: 年干 index 0=甲;  mbranch: 月建
   branch 0=子.  Returns the 月干 index 0=甲. */
int GetMonthStem(int ystem, int mbranch)
{
    return (ystem * 2 + 2 + (mbranch - 2 + 12) % 12) % 10;
}

/* 時柱 天干: 日上起時 (五鼠遁).  dstem: 日干 index 0=甲;  hb: 時支 0=子.
   Returns the 時干 index 0=甲. */
int GetHourStem(int dstem, int hb)
{
    return (dstem * 2 + hb) % 10;
}
extern char GBjieqi[24][7];
extern char B5jieqi[24][7];
extern char U8jieqi[24][7];
extern char PSjieqi[24][9];
extern char GBmiscchar[22][4];
extern char B5miscchar[22][4];
extern char U8miscchar[22][4];
extern char PSbigmchar[21][5];
extern char GBtiangan[10][4];
extern char B5tiangan[10][4];
extern char U8tiangan[10][4];
extern char PStiangan[10][5];
extern char GBdizhi[12][4];
extern char B5dizhi[12][4];
extern char U8dizhi[12][4];
extern char PSdizhi[12][5];

typedef char c24_7[24][7];
typedef c24_7 *pc24_7;
typedef char c22_4[22][4];
typedef c22_4 *pc22_4;
typedef char c10_4[10][4];
typedef c10_4 *pc10_4;
typedef char c12_4[12][4];
typedef c12_4 *pc12_4;
typedef char c7_10[7][10];
typedef c7_10 *pc7_10;

void j2hms(double d, int& hr, int& min, int& sec)
{
	double frac = d	- (int) d;
	double tsec	= frac * 86400;
	double fhr	= tsec/3600;
	hr	= (int) fhr;
	double fmin	= (fhr - hr)*60;
	min	= (int) fmin;
	double fsec = (fmin - min)*60;
	sec = (int) fsec;
}

/* Input:
   year: year number in AD
   Return:
   true -- Leap year, false -- normal year
*/
bool IsLeapYear(short int year)
{
    if (year / 100 * 100 != year) /* Non century year */
    {
        if (year / 4 * 4 == year)
            return true;
        else
            return false;
    }
    else
    {
        if (year / 400 * 400 == year)
            return true;
        else
            return false;
    }
}

/* Inputs:
   mnumber: month number with .5 indicating leap month
   Output:
   month: integer month number
   leap: zero for normal month, 'R' for leap month
*/
void GetMonthNumber(double mnumber, short int& month, char* leap)
{
    month = int(mnumber);
    if (mnumber - month == 0.0)
        *leap = 0;
    else
        *leap = 'R';
}

/* Input:
   mnumber: month number with .5 indicating leap month
*/
void PrintMonthNumber(double mnumber)
{
    short int month;
    char leap[2] = {0x00, 0x00};
    GetMonthNumber(mnumber, month, leap);
	//	lc180710 -	enhance format
	//	lc260211 -	moved the leap prefix to front
	//				and use M instead of Y to indicate new lunar month.
    printf(" %1s[%2d]M     ", leap, month);
}

/* Inputs:
   year: year number in AD
   month: month number (Gregorian Calendar)
   vterms: vector of days of solarterms in the year
   lastnew: julian day of last new moon of the previous year
   lastmon: month number for that started on last new moon day of previous year
   vmoons: vector of days of new moons in the year
   vmonth: vector of month numbers with .5 indicating leap month
   nextnew: julian day of the first new moon of the next year
   pmode: mode of printing, 0: ASCII, 1: HTML Table, 2: PostScript, 3: XML
   bSingle: true--single month, false--part of whole year
   nEncoding: 'a' for ASCII, 'g' for GB, 'b' for BIG5, 'u' for UTF-8
   bNeedsRun: true if character Run is needed, false otherwise
*/
const int PMODE_ASCII	= 0;
const int PMODE_HTML	= 1;
const int PMODE_PS		= 2;
const int PMODE_XML		= 3;

const int FUNC_CAL		= 4;
const int FUNC_JIEQI	= 5;
const int FUNC_LIST		= 6;
const int FUNC_ICAL		= 7;

void SetChinese(int nEncoding, int pmode, pc10_4& CHtiangan, pc12_4& CHdizhi,
				pc22_4& CHmiscchar, pc24_7& CHjieqi, pc7_10& daynamesCH,
				int& nCHchars, char*& sp)
{
    CHtiangan	= &GBtiangan;
    CHdizhi		= &GBdizhi;
    CHmiscchar	= &GBmiscchar;
    CHjieqi		= &GBjieqi;
    daynamesCH	= &daynames;
    if (nEncoding == 'u')
    {
        CHtiangan = &U8tiangan;
        CHdizhi = &U8dizhi;
        CHmiscchar = &U8miscchar;
        CHjieqi = &U8jieqi;
        daynamesCH = &daynamesU8;
    }
    else if (nEncoding == 'g')
    {
        CHtiangan = &GBtiangan;
        CHdizhi = &GBdizhi;
        CHmiscchar = &GBmiscchar;
        CHjieqi = &GBjieqi;
        daynamesCH = &daynamesGB;
    }
    else if (nEncoding == 'b')
    {
        CHtiangan = &B5tiangan;
        CHdizhi = &B5dizhi;
        CHmiscchar = &B5miscchar;
        CHjieqi = &B5jieqi;
        daynamesCH = &daynamesB5;
    }
    nCHchars = (int)strlen((*CHmiscchar)[14]);
    if (pmode == PMODE_HTML)
        sp = (char *)"&#160;";
    else
        sp = (char *)" ";
}

void PrepareMonthInitials(short int year, short int month,
		int lastnew,
		vdouble& vterms, vdouble& vmoons,
		double& jdcnt, double& jdnext, int& termcnt, int& moncnt,
		int& ldcnt, int& dcnt)
{
    /* Set julian day counter to 1st of the month */
    jdcnt = julian_date(year, month, 1, 12.0);
    /* Find julian day of the start of the next month */
    if (month < 12)
        jdnext = julian_date(year, month + 1, 1, 12.0);
    else
        jdnext = julian_date(year + 1, 1, 1, 12.0);
    /* Set solarterm counter to day of 1st term of the calendar month */
    termcnt = (month - 1) * 2;
    if (vterms[termcnt] < jdcnt)
    	termcnt ++;
    /* Set lunar month counter to the 1st lunar month of the calendar month */
    moncnt = 0;
    while (moncnt < int(vmoons.size()) && vmoons[moncnt] < jdcnt)
        moncnt++;
    /* Initialize counters for lunar days and days of month */
    ldcnt, dcnt = 1;
    if (month != 1)
        ldcnt = int(jdcnt - vmoons[moncnt - 1] + 1);
    else
        ldcnt = int(jdcnt - lastnew + 1);
    if (jdcnt == vmoons[moncnt])
        ldcnt = 1;
}

void PrepareMonthHead(short int year, short int month, vdouble& vterms,
				vdouble& vmoons, vdouble& vmonth, double nextnew,
				int	jdcnt, int jdnext, int moncnt, char* sp,
				bool bSingle, int pmode, int nEncoding,
				pc10_4 CHtiangan,
				pc12_4 CHdizhi,
				pc22_4 CHmiscchar,
				pc24_7 CHjieqi,
				pc7_10 daynamesCH,
				char* monthhead,
				char* cmonname,
				short int& cmonth)
{
    int nCHchars = (int)strlen((*CHmiscchar)[14]);
    /* Header of the month */
    short int cyear = (year - 1984) % 60;
    if (cyear < 0)
        cyear += 60;
    int tiancnt = cyear % 10;
    int dicnt = cyear % 12;
    char leap[2] = {0x00, 0x00};
    //char monthhead[200], cmonname[100];
    int nstartlm = int(vmoons[moncnt] - jdcnt + 1);
    int ndayslm;
    if (moncnt < int(vmoons.size()) - 1)
        ndayslm = int(vmoons[moncnt + 1] - vmoons[moncnt]);
    else
        ndayslm = int(nextnew - vmoons.back());
    GetMonthNumber(vmonth[moncnt], cmonth, leap);
    if ((pmode == PMODE_ASCII && nEncoding != 'a') || pmode == PMODE_HTML || pmode == PMODE_XML)
        Number2MonthCH(vmonth[moncnt], nstartlm, ndayslm, nEncoding, cmonname);
    else if (pmode == PMODE_PS)
        Number2MonthPS(vmonth[moncnt], nstartlm, ndayslm, !bSingle, cmonname);
    /* January is special if lunar New Year is in February */
    if (month == 1 && cmonth != 1)
    {
        int tiancnt0 = (cyear + 59) % 10;
        int dicnt0 = (cyear + 59) % 12;
        if (vmoons[moncnt + 1] < jdnext) /* Two lunar months in one month */
        {
            short int cmonth1;
            char leap1[2] = {0x00, 0x00};
            int nstartlm1 = int(vmoons[moncnt + 1] - jdcnt + 1);
            int ndayslm1;
            char cmonname1[100];
            if (moncnt < int(vmoons.size()) - 2)
                ndayslm1 = int(vmoons[moncnt + 2] - vmoons[moncnt + 1]);
            else
                ndayslm1 = int(nextnew - vmoons.back());
            if (pmode == PMODE_ASCII && nEncoding == 'a')
            {
                GetMonthNumber(vmonth[moncnt + 1], cmonth1, leap1);
                sprintf(monthhead,
                    "%s %d (Year %s%s, Month %s%d%c S%d, Year %s%s, Month %s%d%c S%d)",
                    monnames[month - 1], year, tiangan[tiancnt0], dizhi[dicnt0],
                    leap, cmonth, (ndayslm == 30) ? 'D' : 'X', nstartlm,
                    tiangan[tiancnt], dizhi[dicnt],
                    leap1, cmonth1, (ndayslm1 == 30) ? 'D' : 'X', nstartlm1);
            }
            else if ((pmode == PMODE_ASCII && nEncoding != 'a') || pmode == PMODE_HTML)
            {
                Number2MonthCH(vmonth[moncnt + 1], nstartlm1, ndayslm1, nEncoding, cmonname1);
                sprintf(monthhead,
                    "%s %d%s%s%s%s%s%s%s%s%s%s%s",
                    monnames[month - 1], year, sp, sp, (*CHtiangan)[tiancnt0], (*CHdizhi)[dicnt0],
                    (*CHmiscchar)[16], cmonname, (*CHmiscchar)[19], (*CHtiangan)[tiancnt],
                    (*CHdizhi)[dicnt], (*CHmiscchar)[16], cmonname1);
            }
            else if (pmode == PMODE_XML)
            {
                Number2MonthCH(vmonth[moncnt + 1], nstartlm1, ndayslm1, nEncoding, cmonname1);
                sprintf(monthhead,
                    "%s%s%s%s%s%s%s%s%s",
                    (*CHtiangan)[tiancnt0], (*CHdizhi)[dicnt0],
                    (*CHmiscchar)[16], cmonname, (*CHmiscchar)[19], (*CHtiangan)[tiancnt],
                    (*CHdizhi)[dicnt], (*CHmiscchar)[16], cmonname1);
            }
            else if (bSingle)
            {
                Number2MonthPS(vmonth[moncnt + 1], nstartlm1, ndayslm1, !bSingle, cmonname1);
                sprintf(monthhead,
                    "20 1 SF (%s %d) S\n/fsc 21 def gsave ptc\n"
                    "%s%s%s%s%s\n%s%s%s%s%sgrestore",
                    monnames[month - 1], year, PSbigmchar[20], PStiangan[tiancnt0],
                    PSdizhi[dicnt0], PSbigmchar[16], cmonname, PSbigmchar[19],
                    PStiangan[tiancnt], PSdizhi[dicnt], PSbigmchar[16], cmonname1);
            }
        }
        else
        {
            if (pmode == PMODE_ASCII && nEncoding == 'a')
            {
                sprintf(monthhead, "%s %d (Year %s%s, Month %s%d%c S%d)",
                    monnames[month - 1], year, tiangan[tiancnt0], dizhi[dicnt0],
                    leap, cmonth, (ndayslm == 30) ? 'D' : 'X', nstartlm);
            }
            else if ((pmode == PMODE_ASCII && nEncoding != 'a') || pmode == PMODE_HTML)
            {
                sprintf(monthhead,
                    "%s %d%s%s%s%s%s%s",
                    monnames[month - 1], year, sp, sp, (*CHtiangan)[tiancnt0],
                    (*CHdizhi)[dicnt0], (*CHmiscchar)[16], cmonname);
            }
            else if (pmode == PMODE_XML)
            {
                sprintf(monthhead,
                    "%s%s%s%s",
                    (*CHtiangan)[tiancnt0],
                    (*CHdizhi)[dicnt0], (*CHmiscchar)[16], cmonname);
            }
            else if (bSingle)
            {
                sprintf(monthhead,
                    "20 1 SF (%s %d) S\n/fsc 21 def gsave ptc\n"
                    "%s%s%s%s%sgrestore",
                    monnames[month - 1], year, PSbigmchar[20], PStiangan[tiancnt0],
                    PSdizhi[dicnt0], PSbigmchar[16], cmonname);
            }
        }
    }
    else
    {
        if (moncnt < int(vmoons.size()) - 1 && vmoons[moncnt + 1] < jdnext)
        /* Two lunar months in one month */
        {
            short int cmonth1;
            char leap1[2] = {0x00, 0x00};
            int nstartlm1 = int(vmoons[moncnt + 1] - jdcnt + 1);
            int ndayslm1;
            char cmonname1[100];
            if (moncnt < int(vmoons.size()) - 2)
                ndayslm1 = int(vmoons[moncnt + 2] - vmoons[moncnt + 1]);
            else
                ndayslm1 = int(nextnew - vmoons.back());
            if (pmode == PMODE_ASCII && nEncoding == 'a')
            {
                GetMonthNumber(vmonth[moncnt + 1], cmonth1, leap1);
                sprintf(monthhead,
                    "%s %d (Year %s%s, Month %s%d%c S%d, %s%d%c S%d)",
                    monnames[month - 1], year, tiangan[tiancnt], dizhi[dicnt],
                    leap, cmonth, (ndayslm == 30) ? 'D' : 'X', nstartlm,
                    leap1, cmonth1, (ndayslm1 == 30) ? 'D' : 'X', nstartlm1);
            }
            else if ((pmode == PMODE_ASCII && nEncoding != 'a') || pmode == PMODE_HTML)
            {
                Number2MonthCH(vmonth[moncnt + 1], nstartlm1, ndayslm1, nEncoding, cmonname1);
                sprintf(monthhead,
                    "%s %d%s%s%s%s%s%s%s%s", monnames[month - 1], year, sp, sp,
                    (*CHtiangan)[tiancnt], (*CHdizhi)[dicnt],
                    (*CHmiscchar)[16], cmonname, (*CHmiscchar)[19], cmonname1);
            }
            else if (pmode == PMODE_XML)
            {
                Number2MonthCH(vmonth[moncnt + 1], nstartlm1, ndayslm1, nEncoding, cmonname1);
                sprintf(monthhead,
                    "%s%s%s%s%s%s",
                    (*CHtiangan)[tiancnt], (*CHdizhi)[dicnt],
                    (*CHmiscchar)[16], cmonname, (*CHmiscchar)[19], cmonname1);
            }
            else if (bSingle)
            {
                Number2MonthPS(vmonth[moncnt + 1], nstartlm1, ndayslm1, !bSingle, cmonname1);
                sprintf(monthhead,
                    "20 1 SF (%s %d) S\n/fsc 21 def gsave ptc\n"
                    "%s%s%s%s%s%s%sgrestore",
                    monnames[month - 1], year, PSbigmchar[20], PStiangan[tiancnt],
                    PSdizhi[dicnt], PSbigmchar[16], cmonname, PSbigmchar[19],
                    cmonname1);
            }
        }
        else if (month == 2 && vmoons[moncnt] >= jdnext)
        /* No new moon in February */
        {
            ndayslm = int(vmoons[moncnt] - vmoons[moncnt - 1]);
            if (pmode == PMODE_ASCII && nEncoding == 'a')
            {
                GetMonthNumber(vmonth[moncnt - 1], cmonth, leap);
                sprintf(monthhead, "%s %d (Year %s%s, Month %s%d%c)",
                    monnames[month - 1], year, tiangan[tiancnt], dizhi[dicnt],
                    leap, cmonth, (ndayslm == 30) ? 'D' : 'X');
            }
            else if ((pmode == PMODE_ASCII && nEncoding != 'a') || pmode == PMODE_HTML)
            {
                Number2MonthCH(vmonth[moncnt - 1], nstartlm, ndayslm, nEncoding, cmonname);
                char *p = strstr(cmonname, (*CHmiscchar)[14]) + 2 * nCHchars;
                *p = 0;
                sprintf(monthhead,
                    "%s %d%s%s%s%s%s%s", monnames[month - 1], year, sp, sp,
                    (*CHtiangan)[tiancnt], (*CHdizhi)[dicnt],
                    (*CHmiscchar)[16], cmonname);
            }
            else if (pmode == PMODE_XML)
            {
                Number2MonthCH(vmonth[moncnt - 1], nstartlm, ndayslm, nEncoding, cmonname);
                char *p = strstr(cmonname, (*CHmiscchar)[14]) + 2 * nCHchars;
                *p = 0;
                sprintf(monthhead,
                    "%s%s%s%s",
                    (*CHtiangan)[tiancnt], (*CHdizhi)[dicnt],
                    (*CHmiscchar)[16], cmonname);
            }
            else if (bSingle)
            {
                Number2MonthPS(vmonth[moncnt - 1], nstartlm, ndayslm, !bSingle, cmonname);
                char *p = strstr(cmonname, PSbigmchar[14]) + 8;
                *p = 0;
                sprintf(monthhead,
                    "20 1 SF (%s %d) S\n/fsc 21 def gsave ptc\n"
                    "%s%s%s%s%sgrestore",
                    monnames[month - 1], year, PSbigmchar[20], PStiangan[tiancnt],
                    PSdizhi[dicnt], PSbigmchar[16], cmonname);
            }
        }
        else
        {
            if (pmode == PMODE_ASCII && nEncoding == 'a')
            {
                sprintf(monthhead, "%s %d (Year %s%s, Month %s%d%c S%d)",
                    monnames[month - 1], year, tiangan[tiancnt], dizhi[dicnt],
                    leap, cmonth, (ndayslm == 30) ? 'D' : 'X', nstartlm);
            }
            else if ((pmode == PMODE_ASCII && nEncoding != 'a') || pmode == PMODE_HTML)
            {
                sprintf(monthhead,
                    "%s %d%s%s%s%s%s%s", monnames[month - 1], year, sp, sp,
                    (*CHtiangan)[tiancnt], (*CHdizhi)[dicnt],
                    (*CHmiscchar)[16], cmonname);
            }
            else if (pmode == PMODE_XML)
            {
                sprintf(monthhead,
                    "%s%s%s%s",
                    (*CHtiangan)[tiancnt], (*CHdizhi)[dicnt],
                    (*CHmiscchar)[16], cmonname);
            }
            else if (bSingle)
            {
                sprintf(monthhead,
                    "20 1 SF (%s %d) S\n/fsc 21 def gsave ptc\n"
                    "%s%s%s%s%sgrestore",
                    monnames[month - 1], year, PSbigmchar[20], PStiangan[tiancnt],
                    PSdizhi[dicnt], PSbigmchar[16], cmonname);
            }
        }
    }

}

void PrintMonth(short int year, short int month, vdouble& vterms,
                double lastnew, double lastmon, vdouble& vmoons,
                vdouble& vmonth, double nextnew, int pmode,
                bool bSingle, int nEncoding, bool bNeedsRun,
                bool bJianChu, vdouble& vtermhours)
{
#ifdef HILIGHTTODAY
#define ANSI_REV "\x1b[7m"
#define ANSI_NORMAL "\x1b[0m"
    time_t now = time(NULL);
    struct tm *today = localtime(&now);
#endif
    pc10_4 CHtiangan;
    pc12_4 CHdizhi;
    pc22_4 CHmiscchar;
    pc24_7 CHjieqi;
    pc7_10 daynamesCH;
    bool bIsSim = (nEncoding == 'g');
    int nCHchars;
    char *sp;
	SetChinese(nEncoding, pmode, CHtiangan, CHdizhi, CHmiscchar, CHjieqi, daynamesCH, nCHchars, sp);

    /* Set julian day counter to 1st of the month */
    double jdcnt;
    /* Find julian day of the start of the next month */
    double jdnext;
    /* Set solarterm counter to day of 1st term of the calendar month */
    int termcnt;
    /* Set lunar month counter to the 1st lunar month of the calendar month */
    int moncnt = 0;
    /* Initialize counters for lunar days and days of month */
    int ldcnt, dcnt;
	PrepareMonthInitials(year, month, lastnew, vterms, vmoons,
		jdcnt, jdnext, termcnt, moncnt, ldcnt, dcnt);

    /* Set julian day counter to 1st of the month */
//    double jdcnt = julian_date(year, month, 1, 12.0);
    /* Find julian day of the start of the next month */
//    double jdnext;
//    if (month < 12)
//        jdnext = julian_date(year, month + 1, 1, 12.0);
//    else
//        jdnext = julian_date(year + 1, 1, 1, 12.0);
    /* Set solarterm counter to day of 1st term of the calendar month */
//    int termcnt = (month - 1) * 2;
//    if (vterms[termcnt] < jdcnt)
//    	termcnt ++;
    /* Set lunar month counter to the 1st lunar month of the calendar month */
//    int moncnt = 0;
//    while (moncnt < int(vmoons.size()) && vmoons[moncnt] < jdcnt)
//        moncnt++;
    /* In case solarterm and 1st of lunar month falls on the same day */
    bool sameday = false;
    /* Initialize counters for lunar days and days of month */
//    int ldcnt, dcnt = 1;
//    if (month != 1)
//        ldcnt = int(jdcnt - vmoons[moncnt - 1] + 1);
//    else
//        ldcnt = int(jdcnt - lastnew + 1);
//    if (jdcnt == vmoons[moncnt])
//        ldcnt = 1;

    /* Day of week of the 1st of month */
    int dofw = (int(jdcnt) + 1) % 7;
    int nWeeks = 5;
    if ((dofw > 4 && daysinmonth[month - 1] == 31) || (dofw > 5 && daysinmonth[month - 1] == 30))
    	nWeeks = 6;

    char monthhead[200], cmonname[100];
    short int cmonth;
    char leap[2] = {0x00, 0x00};
	PrepareMonthHead(year, month, vterms,
				vmoons, vmonth, nextnew,
				jdcnt, jdnext, moncnt, sp,
				bSingle, pmode, nEncoding,
				CHtiangan,
				CHdizhi,
				CHmiscchar,
				CHjieqi,
				daynamesCH,
				monthhead,
				cmonname,
				cmonth);

    int nmove, i;
    if (pmode == PMODE_ASCII)
    {
        int nHeadLen = strlen(monthhead);
        if (nEncoding == 'u')
        {
            int nasc = 0;
            for (i = 0; i < nHeadLen; i++)
                if ((unsigned char)(monthhead[i]) < 0x80)
                    nasc++;
            nHeadLen = (nHeadLen - nasc) * 2 / 3 + nasc;
        }
        nmove = ((bJianChu ? 82 : 68) - nHeadLen) / 2;
        if (nmove < 0)
            nmove = 0;
        for (i = 0; i < nmove; i++)
            printf(" ");
        printf("%s\n", monthhead);
    }
    else if (pmode == PMODE_HTML)
    {
        printf("<tr>\n<th colspan=\"7\" width=\"100%%\">");
        printf("%s</th>\n</tr>\n", monthhead);
    }
    else if (pmode == PMODE_XML)
    {
        printf("<ccal:month value=\"%d\" name=\"%s\" cname=\"%s\">\n",
            month, monnames[month - 1], monthhead);
    }
    else if (bSingle)
    {
        PrintHeaderMonthPS(monthhead, month, true, bIsSim, bNeedsRun, nWeeks);
        char *p1 = strstr(monthhead, "(");
        char *p2 = strstr(monthhead, ")");
        int nlen = int(p2 - p1) - 1;
        int nheadlen = (int)strlen(monthhead);
        if (nheadlen - nlen - 66 > 56)
            nmove = (4200 - 78 * (nlen + (nheadlen - nlen - 86) / 2 + 3)) / 2;
        else if (nheadlen - nlen - 66 > 30)
            nmove = (4200 - 78 * (nlen + (nheadlen - nlen - 66) / 2 + 3)) / 2;
        else
            nmove = (4200 - 78 * (nlen + (nheadlen - nlen - 46) / 2 + 2)) / 2;
        if (nmove < 0)
            nmove = 0;
        printf("%d 2475 m\n", nmove);
        printf("%s\n", monthhead);
    }
    else
    {
        printf("%% %s %d\n", monnames[month - 1], year);
        int posx = (month - 1) % 4 * 140;
        int posy = 3450 - (month - 1) / 4 * 1150 - 173;
        printf("%d lpts %d m gsave ct\n",
               posx, posy);
        printf("%% Month number\n%d lpts %d m gsave\n100 1 SF 1 0.9 1 K",
               ((month < 10) ? 35 : 10), -750);
        printf(" (%d) S grestore\n", month);
    }
    /* Day of week */
    char dayshort[4];
    if (pmode == PMODE_ASCII)
    {
        for (i = 0; i < 7; i++)
        {
            if (nEncoding != 'u')
			//	lc180710 -	enhance format
                printf(bJianChu ? "%-10s       " : "%-10s    ", (*daynamesCH)[i]);
            else
			//	lc180710 -	enhance format
                printf(bJianChu ? "%s          " : "%s       ", (*daynamesCH)[i]);
        }
        printf("\n");
    }
    else if (pmode == PMODE_HTML)
    {
        printf("<tr align=\"center\">\n");
        for (i = 0; i < 7; i++)
        {
            strncpy(dayshort, daynames[i], 3);
            dayshort[3] = 0;
            if (i == 0)
                printf("<td width=\"15%%\"><font color=\"#FF0000\">"
                       "%s %s</font></td>\n", dayshort, (*CHmiscchar)[15]);
            else if (i == 6)
                printf("<td width=\"15%%\"><font color=\"#00E600\">"
                       "%s %s</font></td>\n", dayshort, (*CHmiscchar)[i]);
            else
                printf("<td width=\"14%%\">%s %s</td>\n",
                       dayshort, (*CHmiscchar)[i]);
        }
        printf("</tr>\n");
    }
    else if (pmode == PMODE_PS)
    {
        if (bSingle)
        {
            printf("%% Day names\n14 1 SF\n/fsc 15 def\n");
            for (i = 0; i < 7; i++)
            {
                strncpy(dayshort, daynames[i], 3);
                dayshort[3] = 0;
                if (i == 0)
                    printf("gsave 1 0 0 K\n100 2270 m (%s) S\n400 2270 m "
                           "gsave ptc %s grestore\ngrestore\n", dayshort, PSbigmchar[15]);
                else if (i == 6)
                    printf("gsave 0 0.8 0 K\n3700 2270 m (%s) S\n4000 2270 m "
                           "gsave ptc %s grestore\ngrestore\n", dayshort, PSbigmchar[i]);
                else
                {
                    int posx = i * 600 + 100;
                    printf("%d 2270 m (%s) S\n%d 2270 m gsave ptc "
                           "%s grestore\n", posx, dayshort,
                           (posx + 300), PSbigmchar[i]);
                }
            }
            printf("%% Days\n17 1 SF\n/fsc 7.5 def\n");
        }
        else
        {
            printf("%% Week heading\n0 0 m Ymh\n");
            printf("%% Days\n9 0 SF\n");
        }
    }
    /* At most can be six weeks */
    int w;
    char cdayname[21];
    for (w = 0; w < nWeeks; w++)
    {
        if (pmode == PMODE_HTML)
        {
            printf("<tr align=\"right\">\n");
        }
        if (pmode == PMODE_XML)
        {
            printf("<ccal:week>\n");
        }
        for (i = 0; i < 7; i++)
        {
            if (pmode == PMODE_HTML)
            {
                if (i == 0)
                    printf("<td width=\"15%%\"><font color=\"#FF0000\">");
                else if (i == 6)
                    printf("<td width=\"15%%\"><font color=\"#00E600\">");
                else
                    printf("<td width=\"14%%\">");
            }
            if (pmode == PMODE_XML)
            {
                printf("<ccal:day ");
            }
            if (dcnt > daysinmonth[month - 1])
            {
                if (pmode == PMODE_HTML)
                {
                    if (i == 0 || i == 6)
                        printf("&#160;</font></td>\n");
                    else
                        printf("&#160;</td>\n");
                    continue;
                }
                if (pmode == PMODE_XML)
                {
                    printf("/>\n");
                    continue;
                }
                break;
            }
            if (w == 0)
            {
                if (i < dofw)
                {
                    if (pmode == PMODE_ASCII)
					//	lc180710 -	enhance format
                        printf(bJianChu ? "%10s       " : "%10s    ", " ");
                    else if (pmode == PMODE_HTML)
                    {
                        if (i == 0 || i == 6)
                            printf("&#160;</font></td>\n");
                        else
                            printf("&#160;</td>\n");
                    }
                    else if (pmode == PMODE_XML)
                    {
                        printf("/>\n");
                    }
                    continue;
                }
            }
            if (dcnt == 1 || ldcnt == 1)
            {
                int mcnt = moncnt;
                if (ldcnt != 1)
                    mcnt --;
                if (mcnt >= 0)
                    GetMonthNumber(vmonth[mcnt], cmonth, leap);
                else
                    GetMonthNumber(lastmon, cmonth, leap);
                if (nEncoding != 'a')
                {
                    if (mcnt >= 0)
                        Number2MonthCH(vmonth[mcnt], 1, 30, nEncoding, cmonname);
                    else
                        Number2MonthCH(lastmon, 1, 30, nEncoding, cmonname);
                    char *p = strstr(cmonname, (*CHmiscchar)[14]) + nCHchars;
                    *p = 0;
                }
            }
#ifdef HILIGHTTODAY
            if (today->tm_year + 1900 == (int)year && today->tm_mon + 1 == (int)month && today->tm_mday == (int)dcnt)
            {
            	if (pmode == PMODE_ASCII)
                    printf(ANSI_REV);
            }
#endif
            int jc = (bJianChu) ? GetJianChu(jdcnt, vterms) : 0;
            if (pmode == PMODE_ASCII || pmode == PMODE_HTML)
            {
                printf("%2d", dcnt);
                if (pmode == PMODE_ASCII && bJianChu)
                    printf(" %s", (nEncoding == 'a') ? jianchu_ascii[jc] : JianChuName(jc, nEncoding));
                else if (pmode == PMODE_HTML && bJianChu)
                    printf(" <span class=\"jianchu\">%s%s</span>", JianChuName(jc, nEncoding),
                           JianChuField(1, jc, nEncoding));
            }
            else if (pmode == PMODE_XML)
            {
                printf("value=\"%d\" cmonth=\"%d\" leap=\"%s\" cdate=\"%d\" ", dcnt, cmonth, leap, ldcnt);
                Number2DayCH(ldcnt, nEncoding, cdayname);
            }
            else if (bSingle)
            {
                int posx = i * 600 + 50;
                int posy = 1875 - w * 375 + 220;
                if (nWeeks == 5)
                	posy = 1800 - w * 450 + 295;
                if (i == 0)
                    printf("gsave 1 0 0 K\n");
                else if (i == 6)
                    printf("gsave 0 0.8 0 K\n");
                printf("%d %d m (%2d) S\ngsave 8 0 SF ", posx, posy, dcnt);
                if (bJianChu)
                    printf("(%s) 2 0 rmoveto show ", jianchu_ascii[jc]);
                posx += 170;
                if (dcnt == 1 || ldcnt == 1)
                {
                    if (*leap == 'R')
                        printf("%d %d m (%dR.%d) S grestore\n%d %d m",
                            (posx + 5), (posy + 56), cmonth, ldcnt, posx,
                            (posy - 2));
                    else
                        printf("%d %d m (%d.%d) S grestore\n%d %d m",
                            (posx + 5), (posy + 56), cmonth, ldcnt, posx,
                            (posy - 2));
                }
                else
                    printf("%d %d m (%2d) S grestore\n%d %d m",
                        (posx + 5), (posy + 56), ldcnt, posx, (posy - 2));
                printf(" gsave ptc");
            }
            else
            {
                int posx = i * 19;
                int posy = -w * 19 - 14;
                if (i == 0)
                    printf("gsave 1 0 0 K\n");
                else if (i == 6)
                    printf("gsave 0 0.8 0 K\n");
                printf("%d %d moveto (%2d) S\n", posx, posy, dcnt);
                if (bJianChu)
                    printf("(%s) 2 0 rmoveto show\n", jianchu_ascii[jc]);
                posy -= 7;
                printf("%d %d moveto gsave ptc", posx, posy);
            }

			//	lc220715 -	below print jieqi
			if (!sameday && (termcnt >= int(vterms.size()) || jdcnt != vterms[termcnt]) && (moncnt >= int(vmoons.size()) || jdcnt != vmoons[moncnt]))
            {
                if (pmode == PMODE_ASCII)
                {
                    if (nEncoding == 'a')
						//	lc180710 -	enhance format
                        printf("  [%2d]      ", ldcnt);
                    else
                    {
                        Number2DayCH(ldcnt, nEncoding, cdayname);
						//	lc180710 -	enhance format
                        printf("  %s      ", cdayname);
                    }
                }
                else if (pmode == PMODE_HTML)
                {
                    if (dcnt == 1)
                        printf(" %s", cmonname);
                    else
                        printf(" ");
                    Number2DayCH(ldcnt, nEncoding, cdayname);
                    int nlen = (int)strlen(cdayname);
                    printf("%s", cdayname);
                    if (i == 0 || i == 6)
                        printf("</font>");
                    if (nlen == 2 * nCHchars && dcnt != 1)
                        printf("&#160;&#160;</td>\n");
                    else
                        printf("</td>\n");
                }
                else if (pmode == PMODE_PS && bSingle)
                {
                    if (dcnt == 1)
                    {
                        if (moncnt > 0)
                            Number2MonthPS(vmonth[moncnt - 1], 1, 30, true, cmonname);
                        else
                            Number2MonthPS(lastmon, 1, 30, true, cmonname);
                        printf(" %s", cmonname);
                    }
                    else
                        printf(" ");
                    Number2DayPS(ldcnt, cdayname);
                    printf("%s\n", cdayname);
                    if (i == 0 || i == 6)
                        printf("grestore\n");
                    printf("grestore\n");
                }
                else if (pmode == PMODE_XML)
                {
                    /* The day name is emitted in the closing
                       <ccal:day ... cdatename="..."/> element. */
                }
            }
            else if (sameday)
            {
                if (pmode == PMODE_ASCII)
                {
                    if (nEncoding == 'a')
                        PrintMonthNumber(vmonth[moncnt++]);
                    else
                    {
                        Number2MonthCH(vmonth[moncnt++], 1, 30, nEncoding, cmonname);
                        char *p = strstr(cmonname, (*CHmiscchar)[14]) + nCHchars;
                        *p = 0;
                        int nlen = (int)strlen(cmonname);
                        if (nlen <= 3 * nCHchars)
                            printf("  ");
						char day2[8];
						Number2DayCH(2, nEncoding, day2);
                        printf("%s%s", cmonname, day2);
                        if (nlen == 2 * nCHchars)
                            printf("  ");
                    //	if (nlen == 3 * nCHchars)
                    //		printf(" ");
                    }
                }
                else if (pmode == PMODE_HTML)
                {
                    Number2DayCH(ldcnt, nEncoding, cdayname);
                    int nlen = (int)strlen(cdayname);
                    printf(" %s", cdayname);
                    if (i == 0 || i == 6)
                        printf("</font>");
                    if (nlen == 2 * nCHchars)
                        printf("&#160;&#160;</td>\n");
                    else
                        printf("</td>\n");
                }
                else if (pmode == PMODE_PS)
                {
                    Number2DayPS(ldcnt, cdayname);
                    printf(" %s\n", cdayname);
                    if (i == 0 || i == 6)
                        printf("grestore\n");
                    printf("grestore\n");
                }
                sameday = false;
            }
            else if (termcnt < int(vterms.size()) && jdcnt == vterms[termcnt])
            {
                if (moncnt < int(vmoons.size()) && jdcnt == vmoons[moncnt])
                    sameday = true;
                if (pmode == PMODE_ASCII)
                {
					int hr, min, sec;
					j2hms(vtermhours[termcnt], hr, min, sec);
                    if (nEncoding == 'a') {
					//	lc180710 -	enhance format
                        printf("  %s %02d:%02d  ", jieqi[termcnt++], hr, min);
					}
                    else
                    {
					//	lc180710 -	enhance format
                        printf("  %s%02d:%02d ", (*CHjieqi)[termcnt++], hr, min);
                    }
                }
                else if (pmode == PMODE_HTML)
                {
                    if (sameday)
                        printf("  %s", cmonname);
                    else
                        printf(" ");
					int hr, min, sec;
					j2hms(vtermhours[termcnt], hr, min, sec);
					//	lc180710 -	enhance format
                    printf("%s %02d:%02d", (nEncoding == 'a' ? jieqi[termcnt++] : (*CHjieqi)[termcnt++]), hr, min);
                    if (i == 0 || i == 6)
                        printf("</font>");
                    if (!sameday)
                        printf("&#160;&#160;</td>\n");
                    else
                        printf("</td>\n");
                }
                else if (pmode == PMODE_XML)
                {
                    strcpy(cdayname, (*CHjieqi)[termcnt++]);
                }
                else if (pmode == PMODE_PS)
                {
                    if (sameday)
                    {
                        Number2MonthPS(vmonth[moncnt++], 1, 30, true, cmonname);
                        int nlen = (int)strlen(cmonname);
                        if (!bSingle)
                            printf(" grestore -6 0 rmoveto gsave ptc");
                        if (!bSingle && nlen > 8)
                        {
                            double fac = 16.0 / (nlen + 8.0);
                            printf(" gsave %f 1 scale %s", fac, cmonname);
                        }
                        else
                            printf(" %s", cmonname);
                    }
                    else
                        printf(" ");
                    printf("%s\n", PSjieqi[termcnt++]);
                    if (sameday && !bSingle && strlen(cmonname) > 8)
                        printf("grestore\n");
                    if (i == 0 || i == 6)
                        printf("grestore\n");
                    printf("grestore\n");
                }
            }
            else
            {
                if (pmode == PMODE_ASCII)
                {
                    if (nEncoding == 'a')
                        PrintMonthNumber(vmonth[moncnt++]);
                    else
                    {
                        Number2MonthCH(vmonth[moncnt++], 1, 30, nEncoding, cmonname);
                        char *p = strstr(cmonname, (*CHmiscchar)[14]) + nCHchars;
                        *p = 0;
                        int nlen = (int)strlen(cmonname);
                        if (nlen <= 3 * nCHchars)
                            printf("  ");
						//	lc180710 -	enhance format
                        printf("%s    ", cmonname);
                        if (nlen == 2 * nCHchars)
                            printf("  ");
						//	lc260825 -	when month >= 11, in chinese, one space less to align text print
                        //if (nlen == 3 * nCHchars)
                        //    printf("");
                    }
                }
                else if (pmode == PMODE_HTML || pmode == PMODE_XML)
                {
                    Number2MonthCH(vmonth[moncnt++], 1, 30, nEncoding, cmonname);
                    char *p = strstr(cmonname, (*CHmiscchar)[14]) + nCHchars;
                    *p = 0;
                    if (pmode == PMODE_HTML)
                    {
                        int nlen = (int)strlen(cmonname);
                        printf(" %s", cmonname);
                        if (i == 0 || i == 6)
                            printf("</font>");
                        if (nlen == 2 * nCHchars)
                            printf("&#160;&#160;</td>\n");
                        else
                            printf("</td>\n");
                    }
                }
                else if (pmode == PMODE_PS && bSingle)
                {
                    Number2MonthPS(vmonth[moncnt++], 1, 30, true, cmonname);
                    printf(" %s\n", cmonname);
                    if (i == 0 || i == 6)
                        printf("grestore\n");
                    printf("grestore\n");
                }
                else if (pmode == PMODE_PS)
                {
                    Number2MonthPS(vmonth[moncnt++], 1, 30, true, cmonname);
                    int nlen = (int)strlen(cmonname);
                    if (nlen > 8)
                        printf(" grestore -%d 0 rmoveto gsave ptc",
                               ((nlen - 8) / 4 * 3));
                    printf(" %s\n", cmonname);
                    if (i == 0 || i == 6)
                        printf("grestore\n");
                    printf("grestore\n");
                }
            }
            if (pmode == PMODE_XML)
            {
                if (bJianChu)
                {
                    /* shichen: 12 吉凶 chars for 子時..亥時; yuezhu: 月柱 */
                    char szShichen[40] = "";
                    char szYue[8];
                    int db = (int(jdcnt) + 1) % 12;
                    int yb = (year - 4) % 12;
                    if (yb < 0)
                        yb += 12;
                    int mbranch = GetMonthBranch(jdcnt, vterms);
                    int ystem = (year - 4) % 10;
                    if (jdcnt < vterms[2])
                        ystem = (ystem - 1 + 10) % 10;
                    sprintf(szYue, "%s%s",
                            (*CHtiangan)[GetMonthStem(ystem, mbranch)],
                            (*CHdizhi)[mbranch]);
                    for (int h = 0; h < 12; h++)
                        strcat(szShichen, ShiChenJixiong(h, db, yb, nEncoding));
                    printf("cmonthname=\"%s\" cdatename=\"%s\" jianchu=\"%s\" jixiong=\"%s\" yuezhu=\"%s\" shichen=\"%s\" />\n",
                           cmonname, cdayname, JianChuName(jc, nEncoding),
                           JianChuField(1, jc, nEncoding), szYue, szShichen);
                }
                else
                    printf("cmonthname=\"%s\" cdatename=\"%s\" />\n", cmonname, cdayname);
            }
#ifdef HILIGHTTODAY
            if (today->tm_year + 1900 == (int)year && today->tm_mon + 1 == (int)month && today->tm_mday == (int)dcnt)
            {
            	if (pmode == PMODE_ASCII)
                    printf(ANSI_NORMAL);
            }
#endif
            dcnt++;
            jdcnt++;
            if (moncnt < int(vmoons.size()) && jdcnt == vmoons[moncnt])
                ldcnt = 1;
            else
                ldcnt++;
        }
        if (pmode == PMODE_ASCII)
        {
            printf("\n");
        }
        else if (pmode == PMODE_HTML)
        {
            printf("</tr>\n");
        }
        else if (pmode == PMODE_XML)
        {
            printf("</ccal:week>\n");
        }
    }
    if (pmode == PMODE_PS && !bSingle)
    {
        printf("grestore\n");
    }
    else if (pmode == PMODE_XML)
    {
        printf("</ccal:month>\n");
    }
}

void PrintMonthList(short int year, short int month, vdouble& vterms,
                double lastnew, double lastmon, vdouble& vmoons,
                vdouble& vmonth, double nextnew, int pmode,
                bool bSingle, int nEncoding, bool bNeedsRun,
                bool bJianChu, vdouble& vtermhours)
{
#ifdef HILIGHTTODAY
#define ANSI_REV "\x1b[7m"
#define ANSI_NORMAL "\x1b[0m"
    time_t now = time(NULL);
    struct tm *today = localtime(&now);
#endif
    pc10_4 CHtiangan;
    pc12_4 CHdizhi;
    pc22_4 CHmiscchar;
    pc24_7 CHjieqi;
    pc7_10 daynamesCH;
    bool bIsSim = (nEncoding == 'g');
    int nCHchars;
    char *sp;
	SetChinese(nEncoding, pmode, CHtiangan, CHdizhi, CHmiscchar, CHjieqi, daynamesCH, nCHchars, sp);

    /* Set julian day counter to 1st of the month */
    double jdcnt;
    /* Find julian day of the start of the next month */
    double jdnext;
    /* Set solarterm counter to day of 1st term of the calendar month */
    int termcnt;
    /* Set lunar month counter to the 1st lunar month of the calendar month */
    int moncnt = 0;
    /* Initialize counters for lunar days and days of month */
    int ldcnt, dcnt;
	PrepareMonthInitials(year, month, lastnew, vterms, vmoons,
		jdcnt, jdnext, termcnt, moncnt, ldcnt, dcnt);

    /* In case solarterm and 1st of lunar month falls on the same day */
    bool sameday = false;

    /* Day of week of the 1st of month */
    int dofw = (int(jdcnt) + 1) % 7;
    int nWeeks = 5;
    if ((dofw > 4 && daysinmonth[month - 1] == 31) || (dofw > 5 && daysinmonth[month - 1] == 30))
    	nWeeks = 6;

    char monthhead[200], cmonname[100];
    short int cmonth;
    char leap[2] = {0x00, 0x00};
	PrepareMonthHead(year, month, vterms,
				vmoons, vmonth, nextnew,
				jdcnt, jdnext, moncnt, sp,
				bSingle, pmode, nEncoding,
				CHtiangan,
				CHdizhi,
				CHmiscchar,
				CHjieqi,
				daynamesCH,
				monthhead,
				cmonname,
				cmonth);

    int nmove, i;
    if (pmode == PMODE_ASCII)
    {
        int nHeadLen = strlen(monthhead);
        if (nEncoding == 'u')
        {
            int nasc = 0;
            for (i = 0; i < nHeadLen; i++)
                if ((unsigned char)(monthhead[i]) < 0x80)
                    nasc++;
            nHeadLen = (nHeadLen - nasc) * 2 / 3 + nasc;
        }
        nmove = ((bJianChu ? 82 : 68) - nHeadLen) / 2;
        if (nmove < 0)
            nmove = 0;
        for (i = 0; i < nmove; i++)
            printf(" ");
        printf("%s\n", monthhead);
    }
    else if (pmode == PMODE_HTML)
    {
        printf("<tr>\n<th colspan=\"7\" width=\"100%%\">");
        printf("%s</th>\n</tr>\n", monthhead);
    }
    else if (pmode == PMODE_XML)
    {
        printf("<ccal:month value=\"%d\" name=\"%s\" cname=\"%s\">\n",
            month, monnames[month - 1], monthhead);
    }
    else if (bSingle)
    {
        PrintHeaderMonthPS(monthhead, month, true, bIsSim, bNeedsRun, nWeeks);
        char *p1 = strstr(monthhead, "(");
        char *p2 = strstr(monthhead, ")");
        int nlen = int(p2 - p1) - 1;
        int nheadlen = (int)strlen(monthhead);
        if (nheadlen - nlen - 66 > 56)
            nmove = (4200 - 78 * (nlen + (nheadlen - nlen - 86) / 2 + 3)) / 2;
        else if (nheadlen - nlen - 66 > 30)
            nmove = (4200 - 78 * (nlen + (nheadlen - nlen - 66) / 2 + 3)) / 2;
        else
            nmove = (4200 - 78 * (nlen + (nheadlen - nlen - 46) / 2 + 2)) / 2;
        if (nmove < 0)
            nmove = 0;
        printf("%d 2475 m\n", nmove);
        printf("%s\n", monthhead);
    }
    else
    {
        printf("%% %s %d\n", monnames[month - 1], year);
        int posx = (month - 1) % 4 * 140;
        int posy = 3450 - (month - 1) / 4 * 1150 - 173;
        printf("%d lpts %d m gsave ct\n",
               posx, posy);
        printf("%% Month number\n%d lpts %d m gsave\n100 1 SF 1 0.9 1 K",
               ((month < 10) ? 35 : 10), -750);
        printf(" (%d) S grestore\n", month);
    }
    /* Day of week */
    char dayshort[4];
    if (pmode == PMODE_ASCII)
    {
        for (i = 0; i < 7; i++)
        {
            if (nEncoding != 'u')
			//	lc180710 -	enhance format
                printf(bJianChu ? "%-10s      " : "%-10s   ", (*daynamesCH)[i]);
            else
			//	lc180710 -	enhance format
                printf(bJianChu ? "%s         " : "%s      ", (*daynamesCH)[i]);
        }
        printf("\n");
    }
    else if (pmode == PMODE_HTML)
    {
        printf("<tr align=\"center\">\n");
        for (i = 0; i < 7; i++)
        {
            strncpy(dayshort, daynames[i], 3);
            dayshort[3] = 0;
            if (i == 0)
                printf("<td width=\"15%%\"><font color=\"#FF0000\">"
                       "%s %s</font></td>\n", dayshort, (*CHmiscchar)[15]);
            else if (i == 6)
                printf("<td width=\"15%%\"><font color=\"#00E600\">"
                       "%s %s</font></td>\n", dayshort, (*CHmiscchar)[i]);
            else
                printf("<td width=\"14%%\">%s %s</td>\n",
                       dayshort, (*CHmiscchar)[i]);
        }
        printf("</tr>\n");
    }
    else if (pmode == PMODE_PS)
    {
        if (bSingle)
        {
            printf("%% Day names\n14 1 SF\n/fsc 15 def\n");
            for (i = 0; i < 7; i++)
            {
                strncpy(dayshort, daynames[i], 3);
                dayshort[3] = 0;
                if (i == 0)
                    printf("gsave 1 0 0 K\n100 2270 m (%s) S\n400 2270 m "
                           "gsave ptc %s grestore\ngrestore\n", dayshort, PSbigmchar[15]);
                else if (i == 6)
                    printf("gsave 0 0.8 0 K\n3700 2270 m (%s) S\n4000 2270 m "
                           "gsave ptc %s grestore\ngrestore\n", dayshort, PSbigmchar[i]);
                else
                {
                    int posx = i * 600 + 100;
                    printf("%d 2270 m (%s) S\n%d 2270 m gsave ptc "
                           "%s grestore\n", posx, dayshort,
                           (posx + 300), PSbigmchar[i]);
                }
            }
            printf("%% Days\n17 1 SF\n/fsc 7.5 def\n");
        }
        else
        {
            printf("%% Week heading\n0 0 m Ymh\n");
            printf("%% Days\n9 0 SF\n");
        }
    }
    /* At most can be six weeks */
    int w;
    char cdayname[21];
    for (w = 0; w < nWeeks; w++)
    {
        if (pmode == PMODE_HTML)
        {
            printf("<tr align=\"right\">\n");
        }
        if (pmode == PMODE_XML)
        {
            printf("<ccal:week>\n");
        }
        for (i = 0; i < 7; i++)
        {
            if (pmode == PMODE_HTML)
            {
                if (i == 0)
                    printf("<td width=\"15%%\"><font color=\"#FF0000\">");
                else if (i == 6)
                    printf("<td width=\"15%%\"><font color=\"#00E600\">");
                else
                    printf("<td width=\"14%%\">");
            }
            if (pmode == PMODE_XML)
            {
                printf("<ccal:day ");
            }
            if (dcnt > daysinmonth[month - 1])
            {
                if (pmode == PMODE_HTML)
                {
                    if (i == 0 || i == 6)
                        printf("&#160;</font></td>\n");
                    else
                        printf("&#160;</td>\n");
                    continue;
                }
                if (pmode == PMODE_XML)
                {
                    printf("/>\n");
                    continue;
                }
                break;
            }
            if (w == 0)
            {
                if (i < dofw)
                {
                    if (pmode == PMODE_ASCII)
					//	lc180710 -	enhance format
                        printf(bJianChu ? "%10s      " : "%10s   ", " ");
                    else if (pmode == PMODE_HTML)
                    {
                        if (i == 0 || i == 6)
                            printf("&#160;</font></td>\n");
                        else
                            printf("&#160;</td>\n");
                    }
                    else if (pmode == PMODE_XML)
                    {
                        printf("/>\n");
                    }
                    continue;
                }
            }
            if (dcnt == 1 || ldcnt == 1)
            {
                int mcnt = moncnt;
                if (ldcnt != 1)
                    mcnt --;
                if (mcnt >= 0)
                    GetMonthNumber(vmonth[mcnt], cmonth, leap);
                else
                    GetMonthNumber(lastmon, cmonth, leap);
                if (nEncoding != 'a')
                {
                    if (mcnt >= 0)
                        Number2MonthCH(vmonth[mcnt], 1, 30, nEncoding, cmonname);
                    else
                        Number2MonthCH(lastmon, 1, 30, nEncoding, cmonname);
                    char *p = strstr(cmonname, (*CHmiscchar)[14]) + nCHchars;
                    *p = 0;
                }
            }
#ifdef HILIGHTTODAY
            if (today->tm_year + 1900 == (int)year && today->tm_mon + 1 == (int)month && today->tm_mday == (int)dcnt)
            {
            	if (pmode == PMODE_ASCII)
                    printf(ANSI_REV);
            }
#endif
            int jc = (bJianChu) ? GetJianChu(jdcnt, vterms) : 0;
            if (pmode == PMODE_ASCII || pmode == PMODE_HTML)
            {
                printf("%2d", dcnt);
                if (pmode == PMODE_ASCII && bJianChu)
                    printf(" %s", (nEncoding == 'a') ? jianchu_ascii[jc] : JianChuName(jc, nEncoding));
                else if (pmode == PMODE_HTML && bJianChu)
                    printf(" <span class=\"jianchu\">%s%s</span>", JianChuName(jc, nEncoding),
                           JianChuField(1, jc, nEncoding));
            }
            else if (pmode == PMODE_XML)
            {
                printf("value=\"%d\" cmonth=\"%d\" leap=\"%s\" cdate=\"%d\" ", dcnt, cmonth, leap, ldcnt);
                Number2DayCH(ldcnt, nEncoding, cdayname);
            }
            else if (bSingle)
            {
                int posx = i * 600 + 50;
                int posy = 1875 - w * 375 + 220;
                if (nWeeks == 5)
                	posy = 1800 - w * 450 + 295;
                if (i == 0)
                    printf("gsave 1 0 0 K\n");
                else if (i == 6)
                    printf("gsave 0 0.8 0 K\n");
                printf("%d %d m (%2d) S\ngsave 8 0 SF ", posx, posy, dcnt);
                if (bJianChu)
                    printf("(%s) 2 0 rmoveto show ", jianchu_ascii[jc]);
                posx += 170;
                if (dcnt == 1 || ldcnt == 1)
                {
                    if (*leap == 'R')
                        printf("%d %d m (%dR.%d) S grestore\n%d %d m",
                            (posx + 5), (posy + 56), cmonth, ldcnt, posx,
                            (posy - 2));
                    else
                        printf("%d %d m (%d.%d) S grestore\n%d %d m",
                            (posx + 5), (posy + 56), cmonth, ldcnt, posx,
                            (posy - 2));
                }
                else
                    printf("%d %d m (%2d) S grestore\n%d %d m",
                        (posx + 5), (posy + 56), ldcnt, posx, (posy - 2));
                printf(" gsave ptc");
            }
            else
            {
                int posx = i * 19;
                int posy = -w * 19 - 14;
                if (i == 0)
                    printf("gsave 1 0 0 K\n");
                else if (i == 6)
                    printf("gsave 0 0.8 0 K\n");
                printf("%d %d moveto (%2d) S\n", posx, posy, dcnt);
                if (bJianChu)
                    printf("(%s) 2 0 rmoveto show\n", jianchu_ascii[jc]);
                posy -= 7;
                printf("%d %d moveto gsave ptc", posx, posy);
            }

			//	lc220715 -	below print jieqi
			if (!sameday && (termcnt >= int(vterms.size()) || jdcnt != vterms[termcnt]) && (moncnt >= int(vmoons.size()) || jdcnt != vmoons[moncnt]))
            {
                if (pmode == PMODE_ASCII)
                {
                    if (nEncoding == 'a')
						//	lc180710 -	enhance format
                        printf("  [%2d]      ", ldcnt);
                    else
                    {
                        Number2DayCH(ldcnt, nEncoding, cdayname);
						//	lc180710 -	enhance format
                        printf("  %s      ", cdayname);
                    }
                }
                else if (pmode == PMODE_HTML)
                {
                    if (dcnt == 1)
                        printf(" %s", cmonname);
                    else
                        printf(" ");
                    Number2DayCH(ldcnt, nEncoding, cdayname);
                    int nlen = (int)strlen(cdayname);
                    printf("%s", cdayname);
                    if (i == 0 || i == 6)
                        printf("</font>");
                    if (nlen == 2 * nCHchars && dcnt != 1)
                        printf("&#160;&#160;</td>\n");
                    else
                        printf("</td>\n");
                }
                else if (pmode == PMODE_PS && bSingle)
                {
                    if (dcnt == 1)
                    {
                        if (moncnt > 0)
                            Number2MonthPS(vmonth[moncnt - 1], 1, 30, true, cmonname);
                        else
                            Number2MonthPS(lastmon, 1, 30, true, cmonname);
                        printf(" %s", cmonname);
                    }
                    else
                        printf(" ");
                    Number2DayPS(ldcnt, cdayname);
                    printf("%s\n", cdayname);
                    if (i == 0 || i == 6)
                        printf("grestore\n");
                    printf("grestore\n");
                }
                else if (pmode == PMODE_XML)
                {
                    /* The day name is emitted in the closing
                       <ccal:day ... cdatename="..."/> element. */
                }
            }
            else if (sameday)
            {
                if (pmode == PMODE_ASCII)
                {
                    if (nEncoding == 'a')
                        PrintMonthNumber(vmonth[moncnt++]);
                    else
                    {
                        Number2MonthCH(vmonth[moncnt++], 1, 30, nEncoding, cmonname);
                        char *p = strstr(cmonname, (*CHmiscchar)[14]) + nCHchars;
                        *p = 0;
                        int nlen = (int)strlen(cmonname);
                        if (nlen <= 3 * nCHchars)
                            printf(" ");
                        printf("%s", cmonname);
                        if (nlen == 2 * nCHchars)
                            printf("   ");
                        if (nlen == 3 * nCHchars)
                            printf(" ");
                    }
                }
                else if (pmode == PMODE_HTML)
                {
                    Number2DayCH(ldcnt, nEncoding, cdayname);
                    int nlen = (int)strlen(cdayname);
                    printf(" %s", cdayname);
                    if (i == 0 || i == 6)
                        printf("</font>");
                    if (nlen == 2 * nCHchars)
                        printf("&#160;&#160;</td>\n");
                    else
                        printf("</td>\n");
                }
                else if (pmode == PMODE_PS)
                {
                    Number2DayPS(ldcnt, cdayname);
                    printf(" %s\n", cdayname);
                    if (i == 0 || i == 6)
                        printf("grestore\n");
                    printf("grestore\n");
                }
                sameday = false;
            }
            else if (termcnt < int(vterms.size()) && jdcnt == vterms[termcnt])
            {
                if (moncnt < int(vmoons.size()) && jdcnt == vmoons[moncnt])
                    sameday = true;
                if (pmode == PMODE_ASCII)
                {
					int hr, min, sec;
					j2hms(vtermhours[termcnt], hr, min, sec);
                    if (nEncoding == 'a') {
					//	lc180710 -	enhance format
                        printf("  %s %02d:%02d ", jieqi[termcnt++], hr, min);
					}
                    else
                    {
					//	lc180710 -	enhance format
                        printf("  %s%02d:%02d ", (*CHjieqi)[termcnt++], hr, min);
                    }
                }
                else if (pmode == PMODE_HTML)
                {
                    if (sameday)
                        printf("  %s", cmonname);
                    else
                        printf(" ");
					int hr, min, sec;
					j2hms(vtermhours[termcnt], hr, min, sec);
					//	lc180710 -	enhance format
                    printf("%s %02d:%02d", (nEncoding == 'a' ? jieqi[termcnt++] : (*CHjieqi)[termcnt++]), hr, min);
                    if (i == 0 || i == 6)
                        printf("</font>");
                    if (!sameday)
                        printf("&#160;&#160;</td>\n");
                    else
                        printf("</td>\n");
                }
                else if (pmode == PMODE_XML)
                {
                    strcpy(cdayname, (*CHjieqi)[termcnt++]);
                }
                else if (pmode == PMODE_PS)
                {
                    if (sameday)
                    {
                        Number2MonthPS(vmonth[moncnt++], 1, 30, true, cmonname);
                        int nlen = (int)strlen(cmonname);
                        if (!bSingle)
                            printf(" grestore -6 0 rmoveto gsave ptc");
                        if (!bSingle && nlen > 8)
                        {
                            double fac = 16.0 / (nlen + 8.0);
                            printf(" gsave %f 1 scale %s", fac, cmonname);
                        }
                        else
                            printf(" %s", cmonname);
                    }
                    else
                        printf(" ");
                    printf("%s\n", PSjieqi[termcnt++]);
                    if (sameday && !bSingle && strlen(cmonname) > 8)
                        printf("grestore\n");
                    if (i == 0 || i == 6)
                        printf("grestore\n");
                    printf("grestore\n");
                }
            }
            else
            {
                if (pmode == PMODE_ASCII)
                {
                    if (nEncoding == 'a')
                        PrintMonthNumber(vmonth[moncnt++]);
                    else
                    {
                        Number2MonthCH(vmonth[moncnt++], 1, 30, nEncoding, cmonname);
                        char *p = strstr(cmonname, (*CHmiscchar)[14]) + nCHchars;
                        *p = 0;
                        int nlen = (int)strlen(cmonname);
                        if (nlen <= 3 * nCHchars)
                            printf(" ");
						//	lc180710 -	enhance format
                        printf("%s   ", cmonname);
                        if (nlen == 2 * nCHchars)
                            printf("   ");
                        if (nlen == 3 * nCHchars)
                            printf(" ");
                    }
                }
                else if (pmode == PMODE_HTML || pmode == PMODE_XML)
                {
                    Number2MonthCH(vmonth[moncnt++], 1, 30, nEncoding, cmonname);
                    char *p = strstr(cmonname, (*CHmiscchar)[14]) + nCHchars;
                    *p = 0;
                    if (pmode == PMODE_HTML)
                    {
                        int nlen = (int)strlen(cmonname);
                        printf(" %s", cmonname);
                        if (i == 0 || i == 6)
                            printf("</font>");
                        if (nlen == 2 * nCHchars)
                            printf("&#160;&#160;</td>\n");
                        else
                            printf("</td>\n");
                    }
                }
                else if (pmode == PMODE_PS && bSingle)
                {
                    Number2MonthPS(vmonth[moncnt++], 1, 30, true, cmonname);
                    printf(" %s\n", cmonname);
                    if (i == 0 || i == 6)
                        printf("grestore\n");
                    printf("grestore\n");
                }
                else if (pmode == PMODE_PS)
                {
                    Number2MonthPS(vmonth[moncnt++], 1, 30, true, cmonname);
                    int nlen = (int)strlen(cmonname);
                    if (nlen > 8)
                        printf(" grestore -%d 0 rmoveto gsave ptc",
                               ((nlen - 8) / 4 * 3));
                    printf(" %s\n", cmonname);
                    if (i == 0 || i == 6)
                        printf("grestore\n");
                    printf("grestore\n");
                }
            }
            if (pmode == PMODE_XML)
            {
                if (bJianChu)
                {
                    /* shichen: 12 吉凶 chars for 子時..亥時; yuezhu: 月柱 */
                    char szShichen[40] = "";
                    char szYue[8];
                    int db = (int(jdcnt) + 1) % 12;
                    int yb = (year - 4) % 12;
                    if (yb < 0)
                        yb += 12;
                    int mbranch = GetMonthBranch(jdcnt, vterms);
                    int ystem = (year - 4) % 10;
                    if (jdcnt < vterms[2])
                        ystem = (ystem - 1 + 10) % 10;
                    sprintf(szYue, "%s%s",
                            (*CHtiangan)[GetMonthStem(ystem, mbranch)],
                            (*CHdizhi)[mbranch]);
                    for (int h = 0; h < 12; h++)
                        strcat(szShichen, ShiChenJixiong(h, db, yb, nEncoding));
                    printf("cmonthname=\"%s\" cdatename=\"%s\" jianchu=\"%s\" jixiong=\"%s\" yuezhu=\"%s\" shichen=\"%s\" />\n",
                           cmonname, cdayname, JianChuName(jc, nEncoding),
                           JianChuField(1, jc, nEncoding), szYue, szShichen);
                }
                else
                    printf("cmonthname=\"%s\" cdatename=\"%s\" />\n", cmonname, cdayname);
            }
#ifdef HILIGHTTODAY
            if (today->tm_year + 1900 == (int)year && today->tm_mon + 1 == (int)month && today->tm_mday == (int)dcnt)
            {
            	if (pmode == PMODE_ASCII)
                    printf(ANSI_NORMAL);
            }
#endif
            dcnt++;
            jdcnt++;
            if (moncnt < int(vmoons.size()) && jdcnt == vmoons[moncnt])
                ldcnt = 1;
            else
                ldcnt++;
        }
        if (pmode == PMODE_ASCII)
        {
            printf("\n");
        }
        else if (pmode == PMODE_HTML)
        {
            printf("</tr>\n");
        }
        else if (pmode == PMODE_XML)
        {
            printf("</ccal:week>\n");
        }
    }
    if (pmode == PMODE_PS && !bSingle)
    {
        printf("grestore\n");
    }
    else if (pmode == PMODE_XML)
    {
        printf("</ccal:month>\n");
    }
}

bool ProcessArg(int argc, char** argv, short int& year, short int& month,
                short int& day, int& pmode, int& fmode, bool& bSingle,
                int& nEncoding, bool& bJianChu)
{
    /* No argc limit: options are single-char flags and at most three
       numeric operands are accepted (4th numeric → "Too many parameters"
       below). */
    pmode = PMODE_ASCII;
    fmode = FUNC_CAL;
    bSingle = true;
    nEncoding = 'a';
    bJianChu = false;
    day = 0;
    bool bIsUTF8 = false;
    bool bTrad = false;
    if (argc == 1)
        return true;
    int i;
    int nParam = 0;
    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
        {
            if (nParam == 0)
            {
                year = atoi(argv[i]);
                bSingle = false;
                nParam++;
            }
            else if (nParam == 1)
            {
                month = year;
                year = atoi(argv[i]);
                bSingle = true;
                nParam++;
            }
            else if (nParam == 2)
            {
                day = month;
                month = year;
                year = atoi(argv[i]);
                bSingle = true;
                nParam++;
            }
            else
            {
                printf("ccal: Too many parameters.\n");
                return false;
            }
        }
        else if (argv[i][1] == 'j')
            fmode = FUNC_JIEQI;
        else if (argv[i][1] == 'l')
            fmode = FUNC_LIST;
        else if (argv[i][1] == 'i')
            fmode = FUNC_ICAL;
        else if (argv[i][1] == 'x')
            pmode = PMODE_XML;
        else if (argv[i][1] == 'p')
            pmode = PMODE_PS;
        else if (argv[i][1] == 't')
            pmode = PMODE_HTML;
        else if (argv[i][1] == 'g')
        { nEncoding = 'g'; bTrad = false; }
        else if (argv[i][1] == 'b')
        { nEncoding = 'b'; bTrad = true; }
        else if (argv[i][1] == 'u')
            bIsUTF8 = true;
        else if (argv[i][1] == 'c')
            bJianChu = true;
        else
        {
            printf("ccal: Unrecognized option.\n");
            return false;
        }
    }
    if (pmode != PMODE_PS)
    {
        if (bIsUTF8 || pmode == PMODE_XML)
        {
            if (nEncoding == 'b')
                SetU8Characters(false);
            nEncoding = 'u';
        }
    }
    if (pmode != PMODE_ASCII && pmode != FUNC_JIEQI)
    {
        if (nEncoding == 'a')
            nEncoding = 'u';
    }
    SetJianChuTrad(bTrad);
    return true;
}

void PrintJieQiList(short int year, vdouble& vterms, vdouble& vtermhours,
		int pmode, int nEncoding)
{ 
		//	lc220715 -	enhanced to print the name, year, month, day
	    pc10_4 CHtiangan;
		pc12_4 CHdizhi;
		pc22_4 CHmiscchar;
        pc24_7 CHjieqi;
        pc7_10 daynamesCH;
    	int nCHchars;
    	char *sp;
		SetChinese(nEncoding, pmode, CHtiangan, CHdizhi, CHmiscchar, CHjieqi, daynamesCH, nCHchars, sp);
		printf("year %04d no of terms %d:\n", year, int(vterms.size()));
		for (int t=0; t<int(vtermhours.size()); t++) {
			double frac	= vtermhours[t];
			int hr, min, sec;
			j2hms(frac, hr, min, sec);
			short int lyear, month, day;
			double hour;
			cal_date(frac, &lyear, &month, &day, &hour);
			printf("%2d\t%.6f\t%s\t%04d-%02d-%02d\t%02d:%02d\n", t+1, vtermhours[t], nEncoding == 'a' ? jieqi[t] : (*CHjieqi)[t], lyear, month, day, hr, min);
		}
}

/* Print the 建除十二神 legend (神煞/吉凶/含义/宜/忌) for HTML/XML/PS. */
void PrintJianChuLegend(int pmode, int nEncoding, bool bSingle)
{
    int i;
    if (pmode == PMODE_HTML)
    {
        printf("<tr><td colspan=\"7\">\n");
        printf("<table border=\"1\" cellspacing=\"1\" width=\"100%%\">\n");
        printf("<tr align=\"center\">");
        for (i = 0; i < 5; i++)
            printf("<td><b>%s</b></td>", JianChuLabel(i, nEncoding));
        printf("</tr>\n");
        for (i = 0; i < 12; i++)
        {
            printf("<tr align=\"center\"><td>%s</td><td>%s</td>"
                   "<td align=\"left\">%s</td><td align=\"left\">%s</td>"
                   "<td align=\"left\">%s</td></tr>\n",
                   JianChuField(0, i, nEncoding), JianChuField(1, i, nEncoding),
                   JianChuField(2, i, nEncoding), JianChuField(3, i, nEncoding),
                   JianChuField(4, i, nEncoding));
        }
        printf("</table></td></tr>\n");
    }
    else if (pmode == PMODE_XML)
    {
        printf("<ccal:jianchu>\n");
        for (i = 0; i < 12; i++)
            printf("<ccal:jianchu value=\"%d\" shensha=\"%s\" jixiong=\"%s\" hanyi=\"%s\" yi=\"%s\" ji=\"%s\"/>\n",
                   i, JianChuField(0, i, nEncoding), JianChuField(1, i, nEncoding),
                   JianChuField(2, i, nEncoding), JianChuField(3, i, nEncoding),
                   JianChuField(4, i, nEncoding));
        printf("</ccal:jianchu>\n");
    }
    else if (pmode == PMODE_PS)
    {
        /* ASCII legend only: no CJK vector glyphs exist for the legend text.
           The single-month page has no free area, so render only on the
           whole-year page (absolute points, top-left, above the 12-month grid). */
        if (bSingle)
            return;
        static const char* jcpx[12] = {"Jian", "Chu", "Man", "Ping", "Ding", "Zhi",
                                       "Po", "Wei", "Cheng", "Shou", "Kai", "Bi"};
        static const char* jcjx[12] = {"Xiong", "Ji", "Xiong", "Xiong", "Ji", "Ji",
                                       "Xiong", "Ji", "Ji", "Xiong", "Ji", "Xiong"};
        printf("%% Jianchu (JianChu) twelve deities legend (ASCII)\n");
        printf("/Times-Bold findfont 9 scalefont setfont\n");
        for (i = 0; i < 12; i++)
            printf("%d %d moveto (%s %s %s) show\n",
                   40 + (i / 6) * 230, 700 - (i % 6) * 14, jianchu_ascii[i], jcpx[i], jcjx[i]);
    }
}

/* Output the Chinese lunar month name (正月/二月/閏六月/十一月/十二月)
   without the trailing 大/小/start-day suffix of Number2MonthCH.
   monname must hold at least 24 bytes. */
void LunarMonthNameCH(double mnumber, int nEncoding, char* monname)
{
    pc22_4 miscchar;
    if (nEncoding == 'u')
        miscchar = &U8miscchar;
    else if (nEncoding == 'g')
        miscchar = &GBmiscchar;
    else
        miscchar = &B5miscchar;
    monname[0] = 0;
    int nmonth = int(mnumber);
    if (mnumber - nmonth == 0.5) /* Leap month */
        strcat(monname, (*miscchar)[13]);
    if (nmonth > 10)
    {
        strcat(monname, (*miscchar)[10]);
        nmonth -= 10;
    }
    if (nmonth == 1 && strlen(monname) == 0) /* Zheng month */
        strcat(monname, (*miscchar)[12]);
    else
        strcat(monname, (*miscchar)[nmonth]);
    strcat(monname, (*miscchar)[14]);
}

/* Escape a string for an iCalendar TEXT property value (RFC 5545). */
void IcsEscape(const char* in, char* out, int outsize)
{
    int o = 0;
    for (int i = 0; in[i] != 0 && o < outsize - 4; i++)
    {
        char c = in[i];
        if (c == '\\' || c == ';' || c == ',')
        {
            out[o++] = '\\';
            out[o++] = c;
        }
        else if (c == '\n')
        {
            out[o++] = '\\';
            out[o++] = 'n';
        }
        else
            out[o++] = c;
    }
    out[o] = 0;
}

/* Write one iCalendar content line with CRLF, folding at 75 octets per
   RFC 5545.  A fold never splits a UTF-8 sequence or a backslash escape. */
void IcsLine(const char* s)
{
    int len = (int)strlen(s);
    if (len <= 75)
    {
        printf("%s\r\n", s);
        return;
    }
    int pos = 0;
    int seg = 0;
    while (pos < len)
    {
        int maxoct = (seg == 0) ? 75 : 74; /* continuation space counts */
        int end = pos + maxoct;
        if (end > len)
            end = len;
        /* do not split a UTF-8 multi-byte character */
        while (end > pos && end < len && ((unsigned char)s[end] & 0xC0) == 0x80)
            end--;
        /* do not leave a lone backslash at the end of a segment */
        while (end > pos && s[end - 1] == '\\')
            end--;
        if (end <= pos) /* pathological: force progress */
            end = pos + 1;
        if (seg == 0)
            printf("%.*s\r\n", end - pos, s + pos);
        else
            printf(" %.*s\r\n", end - pos, s + pos);
        pos = end;
        seg++;
    }
}

/* Generate an iCalendar 2.0 (.ics) file on stdout: one all-day VEVENT per
   Gregorian day, showing the lunar date, the solar term when one falls on
   that day and (with -c) the Jianchu 建除 deity.  Compatible with Outlook,
   Google Calendar, Apple Calendar, OneCalendar etc.: CRLF line endings,
   UTF-8, 75-octet folding, DATE values for all-day events and stable UIDs
   so a re-import does not create duplicates. */
void PrintICalendar(short int year, short int month, short int day,
                    vdouble& vterms,
                    double lastnew, double lastmon, vdouble& vmoons,
                    vdouble& vmonth, double nextnew, bool bSingle,
                    bool bJianChu, vdouble& vtermhours)
{
    pc10_4 CHtiangan;
    pc12_4 CHdizhi;
    pc22_4 CHmiscchar;
    pc24_7 CHjieqi;
    pc7_10 daynamesCH;
    int nCHchars;
    char *sp;
    SetChinese('u', PMODE_ASCII, CHtiangan, CHdizhi, CHmiscchar, CHjieqi,
               daynamesCH, nCHchars, sp);

    char szStamp[32];
    time_t now = time(NULL);
    struct tm *tmnow = gmtime(&now);
    strftime(szStamp, sizeof(szStamp), "%Y%m%dT%H%M%SZ", tmnow);

    char line[1100];
    printf("BEGIN:VCALENDAR\r\n");
    printf("VERSION:2.0\r\n");
    printf("PRODID:-//chinesebay//ccal %s//EN\r\n", versionstr);
    printf("CALSCALE:GREGORIAN\r\n");
    printf("METHOD:PUBLISH\r\n");
    if (day > 0)
        sprintf(line, "X-WR-CALNAME:Chinese Calendar %d-%02d-%02d", year, month, day);
    else if (bSingle)
        sprintf(line, "X-WR-CALNAME:Chinese Calendar %d-%02d", year, month);
    else
        sprintf(line, "X-WR-CALNAME:Chinese Calendar %d", year);
    IcsLine(line);
    printf("X-WR-TIMEZONE:Asia/Hong_Kong\r\n");

    /* Julian day of 正月初一 (lunar new year), for the 年柱 label */
    double jdlny = 0.0;
    for (int i = 0; i < int(vmonth.size()); i++)
        if (vmonth[i] == 1.0)
        {
            jdlny = vmoons[i];
            break;
        }

    short int m1 = bSingle ? month : 1;
    short int m2 = bSingle ? month : 12;
    for (short int m = m1; m <= m2; m++)
    {
        double jdcnt, jdnext;
        int termcnt, moncnt = 0, ldcnt, dcnt;
        PrepareMonthInitials(year, m, lastnew, vterms, vmoons,
                             jdcnt, jdnext, termcnt, moncnt, ldcnt, dcnt);
        bool sameday = false;
        for (dcnt = 1; dcnt <= daysinmonth[m - 1]; dcnt++, jdcnt++)
        {
            bool bTerm = (termcnt < int(vterms.size()) && jdcnt == vterms[termcnt]);
            bool bNew  = (moncnt < int(vmoons.size()) && jdcnt == vmoons[moncnt]);

            /* Current lunar month, same numbering as PrintMonth */
            int mcnt = moncnt;
            if (ldcnt != 1)
                mcnt--;
            double mnum = (mcnt >= 0) ? vmonth[mcnt] : lastmon;
            short int cmonth;
            char leap[2] = {0x00, 0x00};
            GetMonthNumber(mnum, cmonth, leap);
            char cmonname[24];
            LunarMonthNameCH(mnum, 'u', cmonname);
            char cdayname[8];
            Number2DayCH(ldcnt, 'u', cdayname);

            if (day == 0 || dcnt == day)
            {
            char szSum[160], szDesc[1024], szEsc[1600];
            /* SUMMARY: 農曆日 [節氣 HH:MM] [建除] — day first */
            int sl = 0;
            sl += sprintf(szSum + sl, "%s%s", (ldcnt == 1) ? cmonname : "", cdayname);
            if (bTerm)
            {
                int hr, min, sec;
                j2hms(vtermhours[termcnt], hr, min, sec);
                sl += sprintf(szSum + sl, " %s %02d:%02d", (*CHjieqi)[termcnt], hr, min);
            }
            if (bJianChu)
            {
                int jc = GetJianChu(jdcnt, vterms);
                sl += sprintf(szSum + sl, " %s%s",
                              JianChuName(jc, 'u'), JianChuField(1, jc, 'u'));
            }

            /* DESCRIPTION */
            int dl = 0;
            int lyear = year;
            if (jdlny > 0.0 && jdcnt < jdlny)
                lyear--;
            int cyear = (lyear - 1984) % 60;
            if (cyear < 0)
                cyear += 60;
            /* 月柱 (五虎遁 年上起月; the 年干 for the 月柱 follows 立春) */
            int mbranch = GetMonthBranch(jdcnt, vterms);
            int ystem = (year - 4) % 10;
            if (jdcnt < vterms[2])
                ystem = (ystem - 1 + 10) % 10;
            dl += sprintf(szDesc + dl, "農曆：%s%s（%s%s年 %s%s月）\n", cmonname, cdayname,
                          (*CHtiangan)[cyear % 10], (*CHdizhi)[cyear % 12],
                          (*CHtiangan)[GetMonthStem(ystem, mbranch)],
                          (*CHdizhi)[mbranch]);
            int n = (int(jdcnt) + 49) % 60;
            dl += sprintf(szDesc + dl, "干支：%s%s日\n",
                          (*CHtiangan)[n % 10], (*CHdizhi)[n % 12]);
            if (bJianChu)
            {
                int jc = GetJianChu(jdcnt, vterms);
                const char* pyi = JianChuField(3, jc, 'u');
                const char* pji = JianChuField(4, jc, 'u');
                dl += sprintf(szDesc + dl, "建除：%s日（%s）\n",
                              JianChuName(jc, 'u'), JianChuField(1, jc, 'u'));
                if (pyi != 0 && pyi[0] != 0)
                    dl += sprintf(szDesc + dl, "宜：%s\n", pyi);
                if (pji != 0 && pji[0] != 0)
                    dl += sprintf(szDesc + dl, "忌：%s\n", pji);
                /* 時辰吉凶, with hh:mm ranges for reference */
                int db = (int(jdcnt) + 1) % 12;
                int yb = (lyear - 4) % 12;
                if (yb < 0)
                    yb += 12;
                dl += sprintf(szDesc + dl, "時辰吉凶：\n");
                for (int h = 0; h < 12; h++)
                    dl += sprintf(szDesc + dl, "%s時 %s%s %s %s\n",
                                  (*CHdizhi)[h],
                                  (*CHtiangan)[GetHourStem(n % 10, h)], (*CHdizhi)[h],
                                  ShiChenHHMM(h), ShiChenJixiong(h, db, yb, 'u'));
            }
            if (bTerm)
            {
                int hr, min, sec;
                j2hms(vtermhours[termcnt], hr, min, sec);
                dl += sprintf(szDesc + dl, "節氣：%s %02d:%02d\n",
                              (*CHjieqi)[termcnt], hr, min);
            }
            /* Drop the trailing newline: keeps the last fold clean */
            if (dl > 0 && szDesc[dl - 1] == '\n')
                szDesc[--dl] = 0;
            IcsEscape(szDesc, szEsc, sizeof(szEsc));

            /* Exclusive all-day DTEND = the following Gregorian day */
            short int ny, nm, nd;
            double nhr;
            cal_date(jdcnt + 1.0, &ny, &nm, &nd, &nhr);

            printf("BEGIN:VEVENT\r\n");
            printf("UID:ccal-%04d%02d%02d@chinesebay.com\r\n", year, m, dcnt);
            printf("DTSTAMP:%s\r\n", szStamp);
            printf("DTSTART;VALUE=DATE:%04d%02d%02d\r\n", year, m, dcnt);
            printf("DTEND;VALUE=DATE:%04d%02d%02d\r\n", ny, nm, nd);
            sprintf(line, "SUMMARY:%s", szSum);
            IcsLine(line);
            sprintf(line, "DESCRIPTION:%s", szEsc);
            IcsLine(line);
            printf("CATEGORIES:農曆\r\n");
            printf("TRANSP:TRANSPARENT\r\n");
            printf("X-MICROSOFT-CDO-ALLDAYEVENT:TRUE\r\n");
            printf("SEQUENCE:0\r\n");
            printf("END:VEVENT\r\n");
            }

            /* Advance counters exactly like PrintMonth's branches */
            if (bTerm)
            {
                termcnt++;
                if (bNew)
                    sameday = true;
            }
            else if (sameday)
            {
                sameday = false;
            }
            else if (bNew)
            {
                moncnt++;
            }
            /* End-of-day lunar day rollover (tomorrow == next new moon?) */
            if (moncnt < int(vmoons.size()) && jdcnt + 1.0 == vmoons[moncnt])
                ldcnt = 1;
            else
                ldcnt++;
        }
    }
    printf("END:VCALENDAR\r\n");
}

/* Print a single-day detail view: Gregorian date, weekday, lunar date,
   day ganzhi, 建除 with 宜/忌, and the 時辰吉凶 table with hh:mm ranges. */
void PrintDayASCII(short int year, short int month, short int day,
                   vdouble& vterms, double lastnew, double lastmon,
                   vdouble& vmoons, vdouble& vmonth, double nextnew,
                   vdouble& vtermhours, int nEncoding)
{
    double jdcnt, jdnext;
    int termcnt, moncnt = 0, ldcnt, dcnt;
    PrepareMonthInitials(year, month, lastnew, vterms, vmoons,
                         jdcnt, jdnext, termcnt, moncnt, ldcnt, dcnt);

    /* Advance the lunar/solar-term counters to the requested day,
       mirroring PrintMonth's day-loop advancement. */
    bool sameday = false;
    for (dcnt = 1; dcnt < day; dcnt++, jdcnt++)
    {
        bool bTerm = (termcnt < int(vterms.size()) && jdcnt == vterms[termcnt]);
        bool bNew  = (moncnt < int(vmoons.size()) && jdcnt == vmoons[moncnt]);
        if (bTerm)
        {
            termcnt++;
            if (bNew)
                sameday = true;
        }
        else if (sameday)
            sameday = false;
        else if (bNew)
            moncnt++;
        if (moncnt < int(vmoons.size()) && jdcnt + 1.0 == vmoons[moncnt])
            ldcnt = 1;
        else
            ldcnt++;
    }

    int db = (int(jdcnt) + 1) % 12;
    int jc = GetJianChu(jdcnt, vterms);
    int n = (int(jdcnt) + 49) % 60;
    int mcnt = moncnt;
    if (ldcnt != 1)
        mcnt--;
    double mnum = (mcnt >= 0) ? vmonth[mcnt] : lastmon;
    short int cmonth;
    char leap[2] = {0x00, 0x00};
    GetMonthNumber(mnum, cmonth, leap);
    bool bTerm = (termcnt < int(vterms.size()) && jdcnt == vterms[termcnt]);
    int dofw = (int(jdcnt) + 1) % 7;

    /* 年柱: flips at 正月初一 */
    int lyear = year;
    for (int i = 0; i < int(vmonth.size()); i++)
        if (vmonth[i] == 1.0)
        {
            if (jdcnt < vmoons[i])
                lyear--;
            break;
        }
    int cyear = (lyear - 1984) % 60;
    if (cyear < 0)
        cyear += 60;
    int yb = (lyear - 4) % 12;
    if (yb < 0)
        yb += 12;

    /* 月柱 (五虎遁 年上起月; the 年干 for the 月柱 follows 立春) */
    int mbranch = GetMonthBranch(jdcnt, vterms);
    int ystem = (year - 4) % 10;
    if (jdcnt < vterms[2])
        ystem = (ystem - 1 + 10) % 10;
    int mstem = GetMonthStem(ystem, mbranch);
    int dstem = n % 10;

    if (nEncoding == 'a')
    {
        static const char* jcpx[12] = {"Jian", "Chu", "Man", "Ping", "Ding", "Zhi",
                                       "Po", "Wei", "Cheng", "Shou", "Kai", "Bi"};
        static const char* jcjx[12] = {"Xiong", "Ji", "Xiong", "Xiong", "Ji", "Ji",
                                       "Xiong", "Ji", "Ji", "Xiong", "Ji", "Xiong"};
        char dayshort[4];
        strncpy(dayshort, daynames[dofw], 3);
        dayshort[3] = 0;
        printf("%04d-%02d-%02d %s  %s%s Year, %s%s Month, %s%s Day, Lunar %s%d/%d\n",
               year, month, day, dayshort,
               tiangan[cyear % 10], dizhi[cyear % 12],
               tiangan[mstem], dizhi[mbranch],
               tiangan[n % 10], dizhi[n % 12],
               leap[0] == 'R' ? "Leap " : "", cmonth, ldcnt);
        printf("JianChu: %s (%s)\n", jcpx[jc], jcjx[jc]);
        if (bTerm)
        {
            int hr, min, sec;
            j2hms(vtermhours[termcnt], hr, min, sec);
            printf("JieQi: %s %02d:%02d\n", jieqi[termcnt], hr, min);
        }
        printf("ShiChen JiXiong (day branch %s):\n", dizhi[db]);
        for (int h = 0; h < 12; h++)
        {
            const char* jx = ShiChenJixiong(h, db, yb, 'u');
            const char* pinyin = "Zhong";
            if (strcmp(jx, "\xe5\x90\x89") == 0)      /* 吉 */
                pinyin = "Ji";
            else if (strcmp(jx, "\xe5\x87\xb6") == 0) /* 凶 */
                pinyin = "Xiong";
            printf("  %s %s%s %s %s\n", dizhi[h],
                   tiangan[GetHourStem(dstem, h)], dizhi[h],
                   ShiChenHHMM(h), pinyin);
        }
    }
    else
    {
        pc10_4 CHtiangan;
        pc12_4 CHdizhi;
        pc22_4 CHmiscchar;
        pc24_7 CHjieqi;
        pc7_10 daynamesCH;
        int nCHchars;
        char *sp;
        SetChinese(nEncoding, PMODE_ASCII, CHtiangan, CHdizhi, CHmiscchar,
                   CHjieqi, daynamesCH, nCHchars, sp);
        char cmonname[24];
        LunarMonthNameCH(mnum, nEncoding, cmonname);
        char cdayname[8];
        Number2DayCH(ldcnt, nEncoding, cdayname);
        printf("%04d-%02d-%02d %s  農曆：%s%s（%s%s年 %s%s月）  干支：%s%s日\n",
               year, month, day, (*daynamesCH)[dofw],
               cmonname, cdayname,
               (*CHtiangan)[cyear % 10], (*CHdizhi)[cyear % 12],
               (*CHtiangan)[mstem], (*CHdizhi)[mbranch],
               (*CHtiangan)[n % 10], (*CHdizhi)[n % 12]);
        printf("建除：%s日（%s）\n", JianChuName(jc, nEncoding),
               JianChuField(1, jc, nEncoding));
        const char* pyi = JianChuField(3, jc, nEncoding);
        const char* pji = JianChuField(4, jc, nEncoding);
        if (pyi != 0 && pyi[0] != 0)
            printf("宜：%s\n", pyi);
        if (pji != 0 && pji[0] != 0)
            printf("忌：%s\n", pji);
        if (bTerm)
        {
            int hr, min, sec;
            j2hms(vtermhours[termcnt], hr, min, sec);
            printf("節氣：%s %02d:%02d\n", (*CHjieqi)[termcnt], hr, min);
        }
        printf("時辰吉凶：\n");
        for (int h = 0; h < 12; h++)
            printf("  %s時 %s%s %s %s\n", (*CHdizhi)[h],
                   (*CHtiangan)[GetHourStem(dstem, h)], (*CHdizhi)[h],
                   ShiChenHHMM(h), ShiChenJixiong(h, db, yb, nEncoding));
    }
}

int main(int argc, char** argv)
{
    time_t now = time(NULL);
    struct tm *tmnow = localtime(&now);
    short int year, month, day;
    int pmode;		//	ascii, html, xml, ps
    int fmode;		//	cal, jieqi, list, ical
    bool bSingle;
    bool bJianChu;
    int nEncoding;
    year = (short int) (tmnow->tm_year + 1900);
    month = (short int) (tmnow->tm_mon + 1);

	//	lc180716 -	new function FUNC_JIEQI
    if (!ProcessArg(argc, argv, year, month, day, pmode, fmode, bSingle, nEncoding,
                    bJianChu))
    {
        printf("ccal version %s: Displays Chinese calendar (Gregorian with Chinese dates).\n", versionstr);
        printf("Usage: ccal [-t|-p|-x] [-j|-l|-i] [-g|-b] [-u] [-c] [[<day>] <month>] <year>.\n");
        printf("\t-t:\tGenerates HTML table output.\n");
        printf("\t-p:\tGenerates encapsulated PostScript output.\n");
        printf("\t-x:\tGenerates XML output.\n");
        printf("\t-j:\tGenerates list of JieQis.\n");
        printf("\t-l:\tGenerates list of Dates.\n");
        printf("\t-i:\tGenerates iCalendar (.ics) output for import into Outlook, Google Calendar etc.\n");
        printf("\t-g:\tGenerates simplified Chinese output.\n");
        printf("\t-b:\tGenerates traditional Chinese output.\n");
        printf("\t-u:\tUses UTF-8 rather than GB or Big5 for Chinese output.\n");
        printf("\t-c:\tPrints the Jianchu (建除) twelve deities and the 時辰吉凶 for each day.\n");
        exit(1);
    }
    if (month < 1 || month > 12)
    {
        printf("ccal: Invalid month value: month 1-12.\n");
        exit(1);
    }
    if (year < 1645 || year > 7000)
    {
        printf("ccal: Invalid year value: year 1645-7000.\n");
        exit(1);
    }
    if (IsLeapYear(year))
        daysinmonth[1] = 29;
    if (day != 0 && (day < 1 || day > daysinmonth[month - 1]))
    {
        printf("ccal: Invalid day value: day 1-%d for month %d.\n",
               daysinmonth[month - 1], month);
        exit(1);
    }
    vdouble vterms, vmoons, vmonth, vtermhours;
    double lastnew, lastmon, nextnew;
    double lmon = lunaryear(year, vterms, lastnew, lastmon, vmoons, vmonth, nextnew, vtermhours);
    bool bIsSim = (nEncoding == 'g');
    char titlestr[20];

    if (fmode == FUNC_JIEQI)
    {
		//	lc220715 -	enhanced to print the name, year, month, day
		PrintJieQiList(year, vterms, vtermhours, pmode, nEncoding);
		return 0;
	}

    if (fmode == FUNC_LIST)
    {
		//	lc220716 -	enhanced to print date list of a month
	    if (bSingle)
		{
        	PrintMonthList(year, month, vterms, lastnew, lastmon, vmoons, vmonth, nextnew, pmode, bSingle, nEncoding,
        	           ((short int)(lmon) == month || (short int)(lmon + 0.9) == month), bJianChu, vtermhours);
		}
		else
		{
			short int i;
			for (i = 1; i <= 12; i++)
				PrintMonthList(year, i, vterms, lastnew, lastmon, vmoons, vmonth, nextnew, pmode, bSingle, nEncoding, false, bJianChu, vtermhours);
		}
		return 0;
	}

    if (fmode == FUNC_ICAL)
    {
		//	lc260825 -	iCalendar (.ics) output: always UTF-8, -b selects traditional
        if (nEncoding == 'b')
            SetU8Characters(false);
        nEncoding = 'u';
        PrintICalendar(year, month, day, vterms, lastnew, lastmon, vmoons, vmonth,
                       nextnew, bSingle, bJianChu, vtermhours);
        return 0;
    }

    if (day > 0 && pmode == PMODE_ASCII)
    {
        PrintDayASCII(year, month, day, vterms, lastnew, lastmon, vmoons,
                      vmonth, nextnew, vtermhours, nEncoding);
        return 0;
    }

	
	//	lc220716 -	default function FUNC_CAL
    if (pmode == PMODE_XML)
    {
        printf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        printf("<ccal:year value=\"%d\" xmlns:ccal=\"http://ccal.chinesebay.com/ccal/\">\n", year);
    }
    if (bSingle)
    {
        sprintf(titlestr, "%s %d", monnames[month - 1], year);
        if (pmode == PMODE_HTML)
            PrintHeaderHTML(titlestr, month, year, nEncoding);
        else if (pmode == PMODE_PS)
            PrintHeaderPS(titlestr, bIsSim, false);
        PrintMonth(year, month, vterms, lastnew, lastmon, vmoons, vmonth, nextnew, pmode, bSingle, nEncoding,
                   ((short int)(lmon) == month || (short int)(lmon + 0.9) == month), bJianChu, vtermhours);
    }
    else
    {
        sprintf(titlestr, "Year %d", year);
        if (pmode == PMODE_HTML)
            PrintHeaderHTML(titlestr, 0, year, nEncoding);
        else if (pmode == PMODE_PS)
            PrintHeaderPS(titlestr, bIsSim, (lmon != 0.0));
        short int i;
        for (i = 1; i <= 12; i++)
            PrintMonth(year, i, vterms, lastnew, lastmon, vmoons, vmonth, nextnew, pmode, bSingle, nEncoding, false, bJianChu, vtermhours);
    }
    if (bJianChu)
        PrintJianChuLegend(pmode, nEncoding, bSingle);
    if (pmode == PMODE_HTML)
        PrintClosingHTML();
    else if (pmode == PMODE_PS)
        PrintClosingPS();
    else if (pmode == PMODE_XML)
        printf("</ccal:year>\n");
    return 0;
}

