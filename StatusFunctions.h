#pragma once
#include "../MQ2Plugin.h"

void StatusCmd(PSPAWNINFO pChar, PCHAR szLine);
PMQPLUGIN FindPlugin(PCHAR szLine);
bool IsDefined(PCHAR szLine);
bool HaveAlias(PCHAR ShortCommand);