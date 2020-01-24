#pragma once
#include "../MQ2Plugin.h"

void StatusCmd(PSPAWNINFO pChar, PCHAR szLine);
PMQPLUGIN FindPlugin(PCHAR szLine);
bool IsDefined(PCHAR szLine);
bool HaveAlias(PCHAR ShortCommand);
void ReverseString(PCHAR szLine);
void PutCommas(PCHAR szLine);
inline float PercentHealth(PSPAWNINFO& pSpawn);
inline float PercentEndurance(PSPAWNINFO& pSpawn);
inline float PercentMana(PSPAWNINFO& pSpawn);
int GetSubscriptionLevel();
bool atob(char x[MAX_STRING]);
void ParseBoolArg(PCHAR Arg, PCHAR Arg2, PCHAR Arg3, bool* theOption, char* INIsection);
void DoINIThings();
void VerifyINI(char* Section, char* Key, char* Default, char *plugin);

enum Subscription {
	SUB_BRONZE,
	SUB_SILVER,
	SUB_GOLD
};

// bool to toggle on/off all class plugins in the /status command
extern bool bShowPlugin; 

// bool to toggle on/off this specific class for class plugin
extern bool bShowWarrior; 
extern bool bShowCleric;
extern bool bShowPaladin;
extern bool bShowRanger;
extern bool bShowShadowknight;
extern bool bShowDruid;
extern bool bShowMonk;
extern bool bShowBard;
extern bool bShowRogue;
extern bool bShowShaman;
extern bool bShowNecromancer;
extern bool bShowWizard;
extern bool bShowMage;
extern bool bShowEnchanter;
extern bool bShowBeastlord;
extern bool bShowBerserker;