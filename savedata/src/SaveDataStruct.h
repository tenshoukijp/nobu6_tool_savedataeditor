#pragma once

#pragma pack(1)
////////////////////////////////////////////////////////////////////////////
// NB6 �V�ċL
struct NB6HEAD
{
	WORD year;
	char season;
	char name[13];
	char scenario;
	char difficulty;
};

//�y�����̃A���S���Y���z
//0�c�����ƌĂ΂ꂽ�l���B�L�������ɑ����B
//�M���G�g�ƍN�ق��A�֓����O�⏼�i�v�G���B
//�����I�ȃo�����X�̗ǂ��^�C�v�����A
//com�̏ꍇ���͂΂��葝�₵�A���܂�푈���Ȃ��B
//1�c�ҏ��ɑ����B�ēc���Ƃ�`��i�Ƃ�암������B
//2�c�퓬�ɋ��������������B�����ȂǁB
//3�c�퓬�����B���쎁�^��B�����D���H
//4�c�����]�ᓙ�A�V��E���l���̕����l�n�����B�����D���H
//5�c�E�ҕ�����A�v���e���E���n�@���`�ȂǁB�����D���B
//6�c�ڗ����Ȃ��U�R�����B���\�^�C�v�H
//7�c�I���W�i���P�B���i�͕s���B
//255�c�����_���P�B���i�͕s���B

struct NB6GENERAL
{
	WORD prev;
	WORD next;
	WORD _undef2;		//must 0xFFFF
	byte maxgov;		//����
	byte maxbat;		//���
	byte maxint;		//�q��
	WORD expgov;		//���n
	WORD expbat;		//��n
	WORD expint;		//�q�n
	char charm;			//����
	char ambition;		//��]
	char loyal;			//����
	WORD merits;		//�M��
	byte blood;			//���� 0xFF�͖���
	WORD attach;		//�喼
	WORD gundan;		//�R�c
	WORD castle;		//����
	WORD rise;			//������ 0xFFFF
	byte soldier;		//����
	byte training;		//�P��
	byte solloyal;		//�m�C
	byte form;			//����
	byte work;			//�d��
	byte spouse;		//��
	byte birth;			//���N - 1454(�V�ċL��N)
	WORD parent;		//�e��NO
	byte algo;			//�v�l
	byte duty:4;		//�`��
	byte affinity:4;	//����
						//00aa bbcc a-�q�K b-��K c-���K
	byte aptitudegov:2;	//c-���K (2=A,1=B,0=C)
	byte aptitudebat:2;	//b-��K (2=A,1=B,0=C)
	byte aptitudeint:2;	//a-�q�K (2=A,1=B,0=C)
	byte undef_5:2;		//
	byte tech_ikkatsu:1;	//�Z�\�E�ꊅ
	byte tech_ansatsu:1;	//�Z�\�E�ÎE
	byte tech_ryusyutsu:1;	//�Z�\�E���o
	byte tech_sendou:1;		//�Z�\�E����
	byte tech_ryugen:1;		//�Z�\�E����
	byte tech_benzetsu:1;	//�Z�\�E�ِ�
	byte tech_yakiuchi:1;	//�Z�\�E�ē�
	byte tech_chohatsu:1;	//�Z�\�E����
	byte job;			//�E�� 00�� 0x10 �E�� 20 ���l 30 ���� 40 �m��

	byte aptitudeasi:3; //���y�K��				(0=E,1=D,2=C,3=B,4=A,5=S)
	byte aptitudekib:3; //�R�n�K��				(0=E,1=D,2=C,3=B,4=A,5=S)
	byte _aptitudegun_hi:2; //�S�C�K����ʃr�b�g(0=E,1=D,2=C,3=B,4=A,5=S) �W�r�b�g���E�̋��ڂɂ���c�Ђ��`
	byte _aptitudegun_lo:1; //�S�C�K�����ʃr�b�g(0=E,1=D,2=C,3=B,4=A,5=S) �W�r�b�g���E�̋��ڂɂ���c�Ђ��`
	byte aptitudesui:3; //���R�K��				(0=E,1=D,2=C,3=B,4=A,5=S)

	byte biggun:1;		//��C
	byte horsegun:1;	//�R�n�S�C
	byte steelship:1;	//�S�b�D
	byte _undef:1;		//0
	byte life:3;		//����
	byte brave:3;		//�E��
	byte _independence_hi:2;//�Ɨ���ʃr�b�g �W�r�b�g���E�̋��ڂɂ���c�Ђ��`
	byte _independence_lo:1;//�Ɨ����ʃr�b�g �W�r�b�g���E�̋��ڂɂ���c�Ђ��`
	byte tone:3;		//����
	byte bEnd:1;		//�s��
	byte bRise:1;		//����
	byte grudge:1;		//�⍦
	byte flag:1;		//�
};

struct NB6GENERALNAME
{
	char familyname[7];
	char fastname[7];
	WORD number;		//��H
	byte State:4;		//0�喼,1�R��,2����,3�B��,4�Q�l,5�P,6�ҋ@,7���S
	byte position:4;	//�g��
	byte sex;			//�� 0-�j 1-��
};

struct NB6CASTLE
{
	WORD prev;		//�R�c�����郊�X�g �O�̏�
	WORD next;		//�R�c�����郊�X�g ��̏�
	WORD master;
	WORD ronin;
	char name[9];
	WORD attach;	//����
	byte build;		//��s
	WORD grain;		//�΍�
	byte town;		//����
	WORD population;//�l��
	byte loyal;		//����
	WORD soldier;	//������
	byte quality;	//���� 0-2
	byte _undef0:2;	//�{�� ����
	byte bSmith:1;	//�b��
	byte bHorse:1;	//�n�Y�n
	byte _undef3:1;	//0
	byte bPort:1;	//�`
	byte blPort:1;	//���ۍ`
	byte _undef4:1;	//
	byte bSilver:1;	//��R
	byte bGold:1;	//���R
	byte _undef5:2;	//
	byte bRevolt:1;	//�Ꝅ�
	byte _undef6:3;	//
	byte bgback:2;	//��G���\���������́A�w�i�̎�� ���(0=�R) (1=��) (2=�C) (3=����)
	byte title:6;	//��́B
	byte _undef7:3;	//
	byte bLarge:1;	//����
	byte _undef8:4;	//
};


struct NB6CASTLEMAX
{
    byte name;		//�n�於(�����n��ԍ�)
	byte _undef1;	//must 0
	byte maxgrain;	//�Ő΍�/10
	byte maxtown;	//�ŏ���
	byte _x_unknown;	//�Z������X�̂���
	byte _y_unknown;	//�Z������Y�̂���
	byte x;			//�Z��X���W    �w�͂R�Ⴂ���炢�ł��אڂ��Ă��邱�Ƃ�����B�Q�ȉ��͊m���ɗאڂ��Ă���B
	byte y;			//�Z��Y���W	   �x�͂Q�ȉ��Ȃ�אڂ��Ă���B
};

struct NB6GUNDAN
{
	WORD prev;
	WORD next;
	WORD residence;
	WORD leader;
	WORD attach;	//����
	byte act;
	WORD money;		//��
	WORD rise;		//��
	WORD horse;		//�n
	WORD gun;		//�S�C
	byte _undef;	//must 0?
	byte number:4;	//1-8
	byte position:4;//��E
};

struct NB6DAIMYO
{
    WORD attach;	//�喼
	WORD gundan;	//�R�c
	WORD castle;	//����
	WORD retire;	//�B�����X�g�擪
	byte symbol;	//�Ɩ�
    byte tyotei;	//����
	byte hostile;	//�G�Α喼
	byte friendship;//�F�D�喼
	byte _undef0;	//
	byte merchant;	// ���l
	byte bPropagate:1;//�z��
	byte _undef1:3;	 // must 0 (�z���̎c��)
	byte bHyotei:1;	//�]��
	byte _undef2:3;	// must 0 (�]��̎c��)
	byte position;	//��E
	byte _undef3[6];
};

struct NB6KAHOU
{
	char name[13];
	byte type;
	byte face;
	WORD attach;	//���L��
	byte param:4;	//�㏸�l
	byte _undef0:4;	//0
	byte _undef1:6;	//
	byte bMitojo:1;	//���o��
	byte bLost:1;	//����
};


/*
			[����(18*250)]

			0-12	���O
*/
struct NB6KANI
{
	char name[13];	//���O
	byte rank;		//���ʂ̃����N(����ʂƂ��𐔒l�ɂ������� ��ԍ����̂�0�A�Ⴂ�قǐ��l���傫��)
	WORD attach;	//���L��
	byte param;		//�㏸�l
	byte _undef0;	//
};
	

#pragma pack()
