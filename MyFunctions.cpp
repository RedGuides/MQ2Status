#include "MyFunctions.h"

void StatusCmd(PSPAWNINFO pChar, PCHAR szLine) {
	if (!FindPlugin("MQ2EQBC")) {
		WriteChatf("\ar[\a-tMQ2Status\ar]\ao:: \arYou don't appear to have MQ2EQBC Loaded!");
		return;
	}
	else {
		char amConnected[64] = "${EQBC.Connected}";
		ParseMacroData(amConnected, 64);
		if (!_stricmp(amConnected, "FALSE")) {
			WriteChatf("\ar[\a-tMQ2Status\ar]\ao:: \arYou don't appear to be connected to the EQBC server!");
			return;
		}
	}
	char buffer[MAX_STRING] = "/bc ";
	bool classPlugin = false;

	DWORD classID = GetCharInfo2()->Class;
	if (classID == EQData::Berserker || classID == EQData::Rogue || classID == EQData::Warrior || classID == EQData::Shadowknight) {
		char temp[MAX_STRING] = "Class Plugin:";
		switch (classID) {
			case EQData::Berserker:
				if (FindPlugin("MQ2BerZerker")) {
					classPlugin = true;
				}
				break;
			case EQData::Rogue:
				if (FindPlugin("MQ2Rogue")) {
					classPlugin = true;
				}
				break;
			case EQData::Warrior:
				if (FindPlugin("MQ2War")) {
					classPlugin = true;
				}
				break;
			case EQData::Shadowknight:
				if (FindPlugin("MQ2Eskay")) {
					classPlugin = true;
				}
				break;
			default:
				break;
		}
		if (classPlugin) {
			strcat_s(temp, "[+g+] Loaded! [+w+] ");
		}
		else {
			strcat_s(temp, "[+r+] Not Loaded! [+w+] ");
		}
		strcat_s(buffer, temp);
	}


	//Am I running a macro.
	if (gMacroStack && strlen(gszMacroName)) {
		char temp[MAX_STRING] = "Macro:";

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
	int amHidden = 0;
	amHidden = pChar->HideMode;
	strcat_s(buffer, (amHidden ? "Hiding:[+r+] TRUE[+w+]" : "Hiding:[+g+] FALSE[+w+]"));





	//Do the command we've decided on.
	EzCommand(buffer);

}

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

bool IsDefined(PCHAR szLine) {
	return (FindMQ2DataVariable(szLine) != 0);
}