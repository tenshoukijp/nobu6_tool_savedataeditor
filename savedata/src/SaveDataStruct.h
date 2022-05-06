#pragma once

#pragma pack(1)
////////////////////////////////////////////////////////////////////////////
// NB6 天翔記
struct NB6HEAD
{
	WORD year;
	char season;
	char name[13];
	char scenario;
	char difficulty;
};

//【武将のアルゴリズム】
//0…名将と呼ばれた人物。有名武将に多い。
//信長秀吉家康ほか、斎藤道三や松永久秀も。
//総合的なバランスの良いタイプだが、
//comの場合兵力ばかり増やし、あまり戦争しない。
//1…猛将に多い。柴田勝家や柿崎景家や南部晴政ら。
//2…戦闘に強い武将が多い。剣豪など。
//3…戦闘嫌い。今川氏真ら。政治好き？
//4…板部岡江雪等、坊主・茶人等の文化人系武将。政治好き？
//5…忍者武将や、久武親直・金地院崇伝など。調略好き。
//6…目立たないザコ武将。無能タイプ？
//7…オリジナル姫。性格は不明。
//255…ランダム姫。性格は不明。

struct NB6GENERAL
{
	WORD prev;
	WORD next;
	WORD _undef2;		//must 0xFFFF
	byte maxgov;		//政才
	byte maxbat;		//戦才
	byte maxint;		//智才
	WORD expgov;		//政熟
	WORD expbat;		//戦熟
	WORD expint;		//智熟
	char charm;			//魅力
	char ambition;		//野望
	char loyal;			//忠誠
	WORD merits;		//勲功
	byte blood;			//血筋 0xFFは無縁
	WORD attach;		//大名
	WORD gundan;		//軍団
	WORD castle;		//居城
	WORD rise;			//内応先 0xFFFF
	byte soldier;		//兵数
	byte training;		//訓練
	byte solloyal;		//士気
	byte form;			//兵態
	byte work;			//仕官
	byte spouse;		//妻
	byte birth;			//生年 - 1454(天翔記基準年)
	WORD parent;		//親顔NO
	byte algo;			//思考
	byte duty:4;		//義理
	byte affinity:4;	//相性
						//00aa bbcc a-智適 b-戦適 c-政適
	byte aptitudegov:2;	//c-政適 (2=A,1=B,0=C)
	byte aptitudebat:2;	//b-戦適 (2=A,1=B,0=C)
	byte aptitudeint:2;	//a-智適 (2=A,1=B,0=C)
	byte undef_5:2;		//
	byte tech_ikkatsu:1;	//技能・一喝
	byte tech_ansatsu:1;	//技能・暗殺
	byte tech_ryusyutsu:1;	//技能・流出
	byte tech_sendou:1;		//技能・煽動
	byte tech_ryugen:1;		//技能・流言
	byte tech_benzetsu:1;	//技能・弁舌
	byte tech_yakiuchi:1;	//技能・焼討
	byte tech_chohatsu:1;	//技能・挑発
	byte job;			//職業 00無 0x10 忍者 20 茶人 30 剣豪 40 僧侶

	byte aptitudeasi:3; //足軽適正				(0=E,1=D,2=C,3=B,4=A,5=S)
	byte aptitudekib:3; //騎馬適正				(0=E,1=D,2=C,3=B,4=A,5=S)
	byte _aptitudegun_hi:2; //鉄砲適正上位ビット(0=E,1=D,2=C,3=B,4=A,5=S) ８ビット境界の境目にある…ひぇ～
	byte _aptitudegun_lo:1; //鉄砲適正下位ビット(0=E,1=D,2=C,3=B,4=A,5=S) ８ビット境界の境目にある…ひぇ～
	byte aptitudesui:3; //水軍適正				(0=E,1=D,2=C,3=B,4=A,5=S)

	byte biggun:1;		//大砲
	byte horsegun:1;	//騎馬鉄砲
	byte steelship:1;	//鉄甲船
	byte _undef:1;		//0
	byte life:3;		//寿命
	byte brave:3;		//勇猛
	byte _independence_hi:2;//独立上位ビット ８ビット境界の境目にある…ひぇ～
	byte _independence_lo:1;//独立下位ビット ８ビット境界の境目にある…ひぇ～
	byte tone:3;		//口調
	byte bEnd:1;		//行動
	byte bRise:1;		//内応
	byte grudge:1;		//遺恨
	byte flag:1;		//会見
};

struct NB6GENERALNAME
{
	char familyname[7];
	char fastname[7];
	WORD number;		//顔？
	byte State:4;		//0大名,1軍長,2現役,3隠居,4浪人,5姫,6待機,7死亡
	byte position:4;	//身分
	byte sex;			//性 0-男 1-女
};

struct NB6CASTLE
{
	WORD prev;		//軍団所属城リスト 前の城
	WORD next;		//軍団所属城リスト 後の城
	WORD master;
	WORD ronin;
	char name[9];
	WORD attach;	//所属
	byte build;		//城郭
	WORD grain;		//石高
	byte town;		//商業
	WORD population;//人口
	byte loyal;		//民忠
	WORD soldier;	//徴兵可
	byte quality;	//兵質 0-2
	byte _undef0:2;	//本城 居城
	byte bSmith:1;	//鍛冶
	byte bHorse:1;	//馬産地
	byte _undef3:1;	//0
	byte bPort:1;	//港
	byte blPort:1;	//国際港
	byte _undef4:1;	//
	byte bSilver:1;	//銀山
	byte bGold:1;	//金山
	byte _undef5:2;	//
	byte bRevolt:1;	//一揆扇動
	byte _undef6:3;	//
	byte bgback:2;	//｢絵｣を表示した時の、背景の種類 城称(0=山) (1=林) (2=海) (3=平野)
	byte title:6;	//城称。
	byte _undef7:3;	//
	byte bLarge:1;	//巨城
	byte _undef8:4;	//
};


struct NB6CASTLEMAX
{
    byte name;		//地域名(所属地域番号)
	byte _undef1;	//must 0
	byte maxgrain;	//最石高/10
	byte maxtown;	//最商業
	byte _x_unknown;	//セル中のXのずれ
	byte _y_unknown;	//セル中のYのずれ
	byte x;			//セルX座標    Ｘは３違いぐらいでも隣接していることがある。２以下は確実に隣接している。
	byte y;			//セルY座標	   Ｙは２以下なら隣接している。
};

struct NB6GUNDAN
{
	WORD prev;
	WORD next;
	WORD residence;
	WORD leader;
	WORD attach;	//所属
	byte act;
	WORD money;		//金
	WORD rise;		//米
	WORD horse;		//馬
	WORD gun;		//鉄砲
	byte _undef;	//must 0?
	byte number:4;	//1-8
	byte position:4;//役職
};

struct NB6DAIMYO
{
    WORD attach;	//大名
	WORD gundan;	//軍団
	WORD castle;	//居城
	WORD retire;	//隠居リスト先頭
	byte symbol;	//家紋
    byte tyotei;	//朝廷
	byte hostile;	//敵対大名
	byte friendship;//友好大名
	byte _undef0;	//
	byte merchant;	// 商人
	byte bPropagate:1;//布教
	byte _undef1:3;	 // must 0 (布教の残り)
	byte bHyotei:1;	//評定
	byte _undef2:3;	// must 0 (評定の残り)
	byte position;	//役職
	byte _undef3[6];
};

struct NB6KAHOU
{
	char name[13];
	byte type;
	byte face;
	WORD attach;	//所有者
	byte param:4;	//上昇値
	byte _undef0:4;	//0
	byte _undef1:6;	//
	byte bMitojo:1;	//未登場
	byte bLost:1;	//消失
};


/*
			[官位(18*250)]

			0-12	名前
*/
struct NB6KANI
{
	char name[13];	//名前
	byte rank;		//官位のランク(正一位とかを数値にしたもの 一番高いのが0、低いほど数値が大きい)
	WORD attach;	//所有者
	byte param;		//上昇値
	byte _undef0;	//
};
	

#pragma pack()
