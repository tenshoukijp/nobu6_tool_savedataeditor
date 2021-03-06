#pragma once

#pragma pack(1)
////////////////////////////////////////////////////////////////////////////
// NB6 VãÄL
struct NB6HEAD
{
	WORD year;
	char season;
	char name[13];
	char scenario;
	char difficulty;
};

//y«ÌASYz
//0c¼«ÆÄÎê½l¨BL¼«É½¢B
//M·GgÆNÙ©AÖ¡¹Oâ¼ivGàB
//IÈoXÌÇ¢^Cv¾ªA
//comÌêºÍÎ©èâµA ÜèíµÈ¢B
//1cÒ«É½¢BÄcÆâ`èiÆâì°­çB
//2cí¬É­¢«ª½¢BÈÇB
//3cí¬¢B¡ì^çB­¡D«H
//4cÂª]áAVåElÌ¶»ln«B­¡D«H
//5cEÒ«âAve¼Eàn@`ÈÇB²ªD«B
//6cÚ§½È¢UR«B³\^CvH
//7cIWiPB«iÍs¾B
//255c_PB«iÍs¾B

struct NB6GENERAL
{
	WORD prev;
	WORD next;
	WORD _undef2;		//must 0xFFFF
	byte maxgov;		//­Ë
	byte maxbat;		//íË
	byte maxint;		//qË
	WORD expgov;		//­n
	WORD expbat;		//ín
	WORD expint;		//qn
	char charm;			//£Í
	char ambition;		//ì]
	char loyal;			//½
	WORD merits;		//M÷
	byte blood;			//Ø 0xFFÍ³
	WORD attach;		//å¼
	WORD gundan;		//Rc
	WORD castle;		//é
	WORD rise;			//àæ 0xFFFF
	byte soldier;		//º
	byte training;		//Pû
	byte solloyal;		//mC
	byte form;			//ºÔ
	byte work;			//d¯
	byte spouse;		//È
	byte birth;			//¶N - 1454(VãÄLîN)
	WORD parent;		//eçNO
	byte algo;			//vl
	byte duty:4;		//`
	byte affinity:4;	//«
						//00aa bbcc a-qK b-íK c-­K
	byte aptitudegov:2;	//c-­K (2=A,1=B,0=C)
	byte aptitudebat:2;	//b-íK (2=A,1=B,0=C)
	byte aptitudeint:2;	//a-qK (2=A,1=B,0=C)
	byte undef_5:2;		//
	byte tech_ikkatsu:1;	//Z\Eê
	byte tech_ansatsu:1;	//Z\EÃE
	byte tech_ryusyutsu:1;	//Z\E¬o
	byte tech_sendou:1;		//Z\Eø®
	byte tech_ryugen:1;		//Z\E¬¾
	byte tech_benzetsu:1;	//Z\EÙã
	byte tech_yakiuchi:1;	//Z\EÄ¢
	byte tech_chohatsu:1;	//Z\E§­
	byte job;			//EÆ 00³ 0x10 EÒ 20 l 30  40 mµ

	byte aptitudeasi:3; //«yK³				(0=E,1=D,2=C,3=B,4=A,5=S)
	byte aptitudekib:3; //RnK³				(0=E,1=D,2=C,3=B,4=A,5=S)
	byte _aptitudegun_hi:2; //SCK³ãÊrbg(0=E,1=D,2=C,3=B,4=A,5=S) Wrbg«EÌ«ÚÉ écÐ¥`
	byte _aptitudegun_lo:1; //SCK³ºÊrbg(0=E,1=D,2=C,3=B,4=A,5=S) Wrbg«EÌ«ÚÉ écÐ¥`
	byte aptitudesui:3; //RK³				(0=E,1=D,2=C,3=B,4=A,5=S)

	byte biggun:1;		//åC
	byte horsegun:1;	//RnSC
	byte steelship:1;	//SbD
	byte _undef:1;		//0
	byte life:3;		//õ½
	byte brave:3;		//EÒ
	byte _independence_hi:2;//Æ§ãÊrbg Wrbg«EÌ«ÚÉ écÐ¥`
	byte _independence_lo:1;//Æ§ºÊrbg Wrbg«EÌ«ÚÉ écÐ¥`
	byte tone:3;		//û²
	byte bEnd:1;		//s®
	byte bRise:1;		//à
	byte grudge:1;		//â¦
	byte flag:1;		//ï©
};

struct NB6GENERALNAME
{
	char familyname[7];
	char fastname[7];
	WORD number;		//çH
	byte State:4;		//0å¼,1R·,2»ð,3B,4Ql,5P,6Ò@,7S
	byte position:4;	//gª
	byte sex;			//« 0-j 1-
};

struct NB6CASTLE
{
	WORD prev;		//Rc®éXg OÌé
	WORD next;		//Rc®éXg ãÌé
	WORD master;
	WORD ronin;
	char name[9];
	WORD attach;	//®
	byte build;		//és
	WORD grain;		//Î
	byte town;		//¤Æ
	WORD population;//lû
	byte loyal;		//¯
	WORD soldier;	//¥ºÂ
	byte quality;	//º¿ 0-2
	byte _undef0:2;	//{é é
	byte bSmith:1;	//bè
	byte bHorse:1;	//nYn
	byte _undef3:1;	//0
	byte bPort:1;	//`
	byte blPort:1;	//Û`
	byte _undef4:1;	//
	byte bSilver:1;	//âR
	byte bGold:1;	//àR
	byte _undef5:2;	//
	byte bRevolt:1;	//êî®
	byte _undef6:3;	//
	byte bgback:2;	//¢G£ð\¦µ½ÌAwiÌíÞ éÌ(0=R) (1=Ñ) (2=C) (3=½ì)
	byte title:6;	//éÌB
	byte _undef7:3;	//
	byte bLarge:1;	//é
	byte _undef8:4;	//
};


struct NB6CASTLEMAX
{
    byte name;		//næ¼(®næÔ)
	byte _undef1;	//must 0
	byte maxgrain;	//ÅÎ/10
	byte maxtown;	//Å¤Æ
	byte _x_unknown;	//ZÌXÌ¸ê
	byte _y_unknown;	//ZÌYÌ¸ê
	byte x;			//ZXÀW    wÍRá¢®ç¢Åà×ÚµÄ¢é±Æª éBQÈºÍmÀÉ×ÚµÄ¢éB
	byte y;			//ZYÀW	   xÍQÈºÈç×ÚµÄ¢éB
};

struct NB6GUNDAN
{
	WORD prev;
	WORD next;
	WORD residence;
	WORD leader;
	WORD attach;	//®
	byte act;
	WORD money;		//à
	WORD rise;		//Ä
	WORD horse;		//n
	WORD gun;		//SC
	byte _undef;	//must 0?
	byte number:4;	//1-8
	byte position:4;//ðE
};

struct NB6DAIMYO
{
    WORD attach;	//å¼
	WORD gundan;	//Rc
	WORD castle;	//é
	WORD retire;	//BXgæª
	byte symbol;	//Æä
    byte tyotei;	//©ì
	byte hostile;	//GÎå¼
	byte friendship;//FDå¼
	byte _undef0;	//
	byte merchant;	// ¤l
	byte bPropagate:1;//z³
	byte _undef1:3;	 // must 0 (z³Ìcè)
	byte bHyotei:1;	//]è
	byte _undef2:3;	// must 0 (]èÌcè)
	byte position;	//ðE
	byte _undef3[6];
};

struct NB6KAHOU
{
	char name[13];
	byte type;
	byte face;
	WORD attach;	//LÒ
	byte param:4;	//ã¸l
	byte _undef0:4;	//0
	byte _undef1:6;	//
	byte bMitojo:1;	//¢oê
	byte bLost:1;	//Á¸
};


/*
			[¯Ê(18*250)]

			0-12	¼O
*/
struct NB6KANI
{
	char name[13];	//¼O
	byte rank;		//¯ÊÌN(³êÊÆ©ðlÉµ½àÌ êÔ¢Ìª0Aá¢ÙÇlªå«¢)
	WORD attach;	//LÒ
	byte param;		//ã¸l
	byte _undef0;	//
};
	

#pragma pack()
