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

enum Subscription {
	SUB_BRONZE,
	SUB_SILVER,
	SUB_GOLD
};