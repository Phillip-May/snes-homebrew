//this file generated with SNES GSS tool

#define SOUND_EFFECTS_ALL	10

#define MUSIC_ALL	0

//sound effect aliases

enum {
	SFX_BUTTON_B=0,
	SFX_WRONG_X=1,
	SFX_GAME_OVER=2,
	SFX_WRONG_A=3,
	SFX_BUTTON_Y=4,
	SFX_WRONG_B=5,
	SFX_SCORE_POINT=6,
	SFX_BUTTON_X=7,
	SFX_WRONG_Y=8,
	SFX_BUTTON_A=9
};

//sound effect names

const char* const soundEffectsNames[SOUND_EFFECTS_ALL]={
	"BUTTON B",	//0
	"WRONG X",	//1
	"GAME OVER",	//2
	"WRONG A",	//3
	"BUTTON Y",	//4
	"WRONG B",	//5
	"SCORE POINT",	//6
	"BUTTON X",	//7
	"WRONG Y",	//8
	"BUTTON A"	//9
};

extern const unsigned char spc700_code_1[];
extern const unsigned char spc700_code_2[];

const unsigned char* const musicData[MUSIC_ALL]={

};
