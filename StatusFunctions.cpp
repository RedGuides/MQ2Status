#include "StatusFunctions.h"

void StatusCmd(PSPAWNINFO pChar, PCHAR szLine) {
	//Check to see if MQ2EQBC Plugin is loaded, if not output an error and return out without doing anything. 
	if (!FindPlugin("MQ2EQBC")) {
		WriteChatf("\ar[\a-tMQ2Status\ar]\ao:: \arYou don't appear to have MQ2EQBC Loaded!");
		return;
	}
	else {//If the MQ2EQBC was loaded, let's see if we're connected to the EQBC Server, if not output an error and return out without doing anything.
		char amConnected[64] = "${EQBC.Connected}";
		ParseMacroData(amConnected, 64);
		if (!_stricmp(amConnected, "FALSE")) {
			WriteChatf("\ar[\a-tMQ2Status\ar]\ao:: \arYou don't appear to be connected to the EQBC server!");
			return;
		}
	}
	//This is what I want to send the status output to, default is /bc, more options later perhaps? Is max_string too big? Maybe, but I don't know what you're naming your macros!
	char buffer[MAX_STRING] = "/bc ";

	bool classPlugin = false;//only true if there is a class plugin for this class, and the plugin was loaded.
	bool notLoaded = false;//would only be true if one of the classes in the switch has a plugin, but it's not loaded

	DWORD classID = GetCharInfo2()->Class;
	switch (classID) {
		case EQData::Berserker:
			if (FindPlugin("MQ2BerZerker")) {
				classPlugin = true;
			}
			else {
				notLoaded = true;
			}
			break;
		case EQData::Rogue:
			if (FindPlugin("MQ2Rogue")) {
				classPlugin = true;
			}
			else {
				notLoaded = true;
			}
			break;
		case EQData::Warrior:
			if (FindPlugin("MQ2War")) {
				classPlugin = true;
			}
			else {
				notLoaded = true;
			}
			break;
		case EQData::Shadowknight:
			if (FindPlugin("MQ2Eskay")) {
				classPlugin = true;
			}
			else {
				notLoaded = true;
			}
			break;
		default:
			break;
	}
	if (classPlugin) {
		strcat_s(buffer, "Class Plugin:[+g+] Loaded! [+w+] ");
	}
	else if (notLoaded) {//Only outputs if there is a classPlugin available for that class, and it wasn't loaded.
		strcat_s(buffer, "Class Plugin:[+r+] Not Loaded! [+w+] ");
	}

	//Am I running a macro.
	if (gMacroStack && strlen(gszMacroName)) {
		char temp[MAX_STRING] = "Macro:";
		//Is the currently running macro "kiss"Assist? Where any macro with the word "kiss" will be found for people running customer KA's or older KA's etc. 
		if (strstr(gszMacroName, "kiss")) {
			if (IsDefined("Role")) {
				strcat_s(temp, "[+g+] ");
				strcat_s(temp, gszMacroName);
				strcat_s(temp, "[+w+] Role: [+g+]");
				//Get the value of the Role variable
				char theRole[64] = "${Role}";
				ParseMacroData(theRole, 64);
				strcat_s(temp, theRole);
				strcat_s(temp, "[+w+] ");
			}
			else {
				strcat_s(temp, "[+r+] ");
				//Seem to not have a role. That's fucking weird. 
			}
		}
		else {
			strcat_s(temp, "[+r+] ");
			strcat_s(temp, gszMacroName);
			strcat_s(temp, "[+w+] ");
		}
		if (PMACROBLOCK pBlock = GetCurrentMacroBlock()) {
			if (pBlock->Paused) {
				strcat_s(temp, "[+r+]***PAUSED***[+w+] ");
			}
		}
		strcat_s(buffer, temp);
	}
	else {
		if (!classPlugin) {
			strcat_s(buffer, "Macro:[+r+] FALSE [+w+]");
		}
	}

	//Am I Invis?
	if (int amHidden = pChar->HideMode) {
		strcat_s(buffer, "Hiding:[+r+] TRUE[+w+]");
	}

	//Do the command we've decided on.
	EzCommand(buffer);
}

//Check to see if a plugin is loaded.
PMQPLUGIN FindPlugin(PCHAR szLine)
{
	if (!strlen(szLine)) return false;
	PMQPLUGIN pPlugin = pPlugins;
	while (pPlugin)
	{
		if (!_stricmp(szLine, pPlugin->szFilename))
		{
			return pPlugin;
		}

		pPlugin = pPlugin->pNext;
	}
	return false;
}

//Check to see if a macro variable is defined.
bool IsDefined(PCHAR szLine) {
	return (FindMQ2DataVariable(szLine) != 0);
}

//Check to see if an alias exists with the name of "ShortCommand"
bool HaveAlias(PCHAR ShortCommand) {
	std::string sName = ShortCommand;
	std::transform(sName.begin(), sName.end(), sName.begin(), tolower);
	if (mAliases.find(sName) != mAliases.end()) {
		return true;
	}
	return false;
}