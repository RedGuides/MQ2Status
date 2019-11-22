#include "StatusFunctions.h"

void StatusCmd(PSPAWNINFO pChar, PCHAR szLine) {
	//Check to see if MQ2EQBC Plugin is loaded, if not output an error and return out without doing anything. 
	if (!FindPlugin("MQ2EQBC")) {
		WriteChatf("\ar[\a-tMQ2Status\ar]\ao:: \arYou don't appear to have MQ2EQBC Loaded!");
		return;
	}
	else {//If the MQ2EQBC was loaded, let's see if we're connected to the EQBC Server, if not output an error and return out without doing anything.
		char amConnected[24] = "${EQBC.Connected}";
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

	/// Get our Parameters
	CHAR Arg[MAX_STRING] = { 0 };
	GetArg(Arg, szLine, 1);
	if (!_stricmp(Arg, "item") || !_stricmp(Arg, "itembank") || !_stricmp(Arg, "stat") || !_stricmp(Arg, "merc") || !_stricmp(Arg, "campfire") || !_stricmp(Arg, "aa") || !_stricmp(Arg, "help") || !_stricmp(Arg, "bagspace") || !strlen(Arg) || !_stricmp(Arg, "fellow") || !_stricmp(Arg, "fellowship")) {
		/// /status item stuff - this is doing a search for how many of these items we have on our person.
		if (!_stricmp(Arg, "help")) {
			WriteChatf("Welcome to MQ2Status");
			WriteChatf("By \aoChatWithThisName\aw & \agSic\aw Exclusively for \arRedGuides\aw.");
			WriteChatf("\agValid Status options are:\aw");
			WriteChatf("/status will output : If we have a CWTN Class Plugin loaded, if we have a macro, if our macro is kiss - it will say what our role is, if we are paused, if we are hidden, and if we have a merc that is alive.");
			WriteChatf("/status \agitem\aw \ayitem name\aw : how many \ayitem name\aw you have in your inventory.");
			WriteChatf("/status \agitembank\aw \ayitem name\aw : how many \ayitem name\aw you have in your bank.");
			WriteChatf("/status \agstat\aw \ayoption\aw : options: Hdex, HStr, HSta, HInt, HAgi, HWis, HCha, HPS, Mana, and, Money.");
			WriteChatf("/status \agaa\aw : How many \"banked\" AA points you have.");
			WriteChatf("/status \agmerc\aw : This returns mercenary information including class, and role.");
			WriteChatf("/status \agcampfire\aw : This returns campfire information including Active, Duration, and Zone.");
		}
		if (!_stricmp(Arg, "item")) {
			GetArg(Arg, szLine, 2);
			if (!strlen(Arg)) {
				WriteChatf("\arPlease provide a valid Item to search for\aw");
				WriteChatf("\arExamples: Bone Chips, Diamond, Blue Diamond, etc.\aw");
			}
			else {
				char findItem[MAX_STRING] = "${FindItemCount["; // better "MQ2" way of doing this?
				for (int i = 2; i < 20; i++) {
					GetArg(Arg, szLine, i); // grab all params after szLine 1
					if (strlen(Arg)) {
						if (i > 2) strcat_s(findItem, " ");
						strcat_s(findItem, Arg);
					}
					else {
						break;
					}
				}
				strcat_s(findItem, "]} ");
				//Cycling through to get what item we're looking for again to output the text 
				//likely should store the information when we first get it before we concatenate it to reuse?
				for (int i = 2; i < 20; i++) {
					GetArg(Arg, szLine, i);
					if (strlen(Arg)) {
						if (i > 2) strcat_s(findItem, " ");
						strcat_s(findItem, Arg);
					}
					else {
						break;
					}
				}
				strcat_s(findItem, " in our inventory.");
				strcat_s(buffer, findItem);
				EzCommand(buffer);
			}
		}
		/// /status itembank stuff
		if (!_stricmp(Arg, "itembank")) {
			GetArg(Arg, szLine, 2);
			if (!strlen(Arg)) {
				WriteChatf("\arPlease provide a valid Item to search for\aw");
				WriteChatf("\arExamples: Bone Chips, Diamond, Blue Diamond, etc.\aw");
			}
			else {
				char findItemBank[MAX_STRING] = "${FindItemBankCount["; // better "MQ2" way of doing this?
				for (int i = 2; i < 20; i++) {
					GetArg(Arg, szLine, i);
					if (strlen(Arg)) {
						if (i > 2) strcat_s(findItemBank, " ");
						strcat_s(findItemBank, Arg);
					}
					else {
						break;
					}
				}
				strcat_s(findItemBank, "]} ");
				//Cycling through to get what item we're looking for again to output the text 
				//likely should store the information when we first get it before we concatenate it to reuse?
				for (int i = 2; i < 20; i++) {
					GetArg(Arg, szLine, i);
					if (strlen(Arg)) {
						if (i > 2) strcat_s(findItemBank, " ");
						strcat_s(findItemBank, Arg);
					}
					else {
						break;
					}
				}
				strcat_s(findItemBank, " in the bank.");
				strcat_s(buffer, findItemBank);
				EzCommand(buffer);
			}
		}
		// /status stat <whateverstat>
		if (!_stricmp(Arg, "stat")) {
			PCHARINFO pChar = GetCharInfo(); // need this for heroic stats
			PCHARINFO2 pChar2 = GetCharInfo2(); // need this for money
			PSPAWNINFO me = GetCharInfo()->pSpawn;
			GetArg(Arg, szLine, 2);
			if (!strlen(Arg)) {
				WriteChatf("\arPlease provide a valid MQ2Status stat\aw");
				WriteChatf("\arThese are currently: hstr, hsta, hint, hwis, hagi, hdex, hcha, hps, money, and mana.\aw");
			}
			else {
				//char stat[256] = "[+g+]${Me.";
				//					pChar->HeroicSTRBonus
				char stat[MAX_STRING] = "We have ";
				bool bFound = true;
				if (!_stricmp(Arg, "hstr")) {
					sprintf_s(stat, "I have [+g+]%lu[+w+] [+y+]HStr[+w+].", pChar->HeroicSTRBonus);
				}
				else if (!_stricmp(Arg, "hsta")) {
					sprintf_s(stat, "I have [+g+]%lu[+w+] [+y+]HSta[+w+].", pChar->HeroicSTABonus);
				}
				else if (!_stricmp(Arg, "hint")) {
					sprintf_s(stat, "I have [+g+]%lu[+w+] [+y+]HInt[+w+].", pChar->HeroicINTBonus);
				}
				else if (!_stricmp(Arg, "hwis")) {
					sprintf_s(stat, "I have [+g+]%lu[+w+] [+y+]HWis[+w+].", pChar->HeroicWISBonus);
				}
				else if (!_stricmp(Arg, "hagi")) {
					sprintf_s(stat, "I have [+g+]%lu[+w+] [+y+]HAgi[+w+].", pChar->HeroicAGIBonus);
				}
				else if (!_stricmp(Arg, "hdex")) {
					sprintf_s(stat, "I have [+g+]%lu[+w+] [+y+]HDex[+w+].", pChar->HeroicDEXBonus);
				}
				else if (!_stricmp(Arg, "hcha")) {
					sprintf_s(stat, "I have [+g+]%lu[+w+] [+y+]HCHA[+w+].", pChar->HeroicCHABonus);
				}
				else if (!_stricmp(Arg, "hps")) {
					sprintf_s(stat, "Current HPS: [+g+]%llu[+w+] Max HPs: [+g+]%llu[+w+] Percent Health: [+g+]%2.2f %%[+w+]", me->HPCurrent, me->HPMax, PercentHealth(me));
				}
				else if (!_stricmp(Arg, "mana")) {
					strcat_s(stat, "CurrentMana} / ${Me.MaxMana}[+w+] at [+g+]${Me.PctMana}%[+w+] Mana");
					sprintf_s(stat, "Current Mana: [+g+]%i[+w+] Max Mana: [+g+]%i[+w+] Percent Mana: [+g+]%2.2f %%[+w+]", me->GetCurrentMana(), me->GetMaxMana(), PercentMana(me));
				}
				else if (!_stricmp(Arg, "money")) {
					unsigned long myPlat = pChar2->Plat;
					char szmyPlat[MAX_STRING] = "";
					_ltoa_s(myPlat, szmyPlat, 10);
					PutCommas(szmyPlat);
					sprintf_s(stat, "[+y+]I have [+g+]%s [+y+]platinum! [+w+]", szmyPlat);
				}
				else {
					WriteChatf("\arThat was not a valid stat, please use hstr, hsta, hint, hwis, hagi, hdex, hcha, hps, mana, or money for this option!\aw");
					bFound = false;
				}
				if (bFound) {
					strcat_s(buffer, stat);
					EzCommand(buffer);
				}
			}
		}
		if (!_stricmp(Arg, "aa")) {
			char myAABank[MAX_STRING] = "";
			PCHARINFO2 pChar2 = GetCharInfo2();
			sprintf_s(myAABank,"We have [+g+] %lu [+w+] banked AA points.", pChar2->AAPoints);
			strcat_s(buffer, myAABank);
			EzCommand(buffer);
			return;
		}
		if (!_stricmp(Arg, "merc")) {
			char temp[32] = { 0 };
			char mercStatInfo[MAX_STRING] = "";
			char mercClass[128] = "";
			if (pMercInfo) {
				ZeroMemory(&MercenarySpawn, sizeof(MercenarySpawn));
				if (pMercInfo->HaveMerc == 1) {
					switch (pMercInfo->MercState)
					{
					case 0:
						strcpy_s(MercenarySpawn.Name, "[+r+]DEAD [+w+]");
						sprintf_s(mercStatInfo, "Mercenary State: %s  ", MercenarySpawn.Name);
						break;
					case 1:
						strcpy_s(MercenarySpawn.Name, "[+r+]SUSPENDED [+w+]");
						sprintf_s(mercStatInfo, "Mercenary State: %s  ", MercenarySpawn.Name);
						break;
					case 5:
						strcpy_s(MercenarySpawn.Name, "[+g+]ALIVE [+w+]");

						{
							DWORD mercStance = pMercInfo->ActiveStance;
							if (PSPAWNINFO myMerc = (PSPAWNINFO)GetSpawnByID(pMercInfo->MercSpawnId)) {
								switch (myMerc->GetClass()) {
								case EQData::Cleric:
									sprintf_s(mercClass, "[+g+]Cleric [+w+]");
									{
										switch (mercStance) {
										case 0:
											sprintf_s(temp, "[+g+]Balanced [+w+]");
											//status Balanced
											break;
										case 1:
											sprintf_s(temp, "[+g+]Efficient [+w+]");
											//status Efficient
											break;
										case 2:
											sprintf_s(temp, "[+g+]Reactive [+w+]");
											//status Reactive
											break;
										case 3:
											sprintf_s(temp, "[+r+]Passive [+w+]");
											//status Passive
											break;
										default:
											sprintf_s(temp, "Unknown [+w+]");
											break;
										}
									}
									break;
								case EQData::Warrior:
									sprintf_s(mercClass, "[+g+]Warrior [+w+]");
									{
										switch (mercStance) {
										case 0:
											sprintf_s(temp, "\apAggressive [+w+]");
											//status Aggressive
											break;
										case 1:
											sprintf_s(temp, "[+g+]Assist [+w+]");
											//status Assist
											break;
										case 2:
											sprintf_s(temp, "[+r+]Passive [+w+]");
											//status Passive
											break;
										default:
											sprintf_s(temp, "Unknown [+w+]");
											break;
										}
									}
									break;
								case EQData::Wizard:
									sprintf_s(mercClass, "[+g+]Wizard [+w+]");
									{
										switch (mercStance) {
										case 0:
											sprintf_s(temp, "[+r+]Passive [+w+]");
											//status Passive
											break;
										case 1:
											sprintf_s(temp, "[+g+]Balanced [+w+]");
											//status Balanced
											break;
										case 2:
											sprintf_s(temp, "[+o+]Burn [+w+]");
											//status Burn
											break;
										case 3:
											sprintf_s(temp, "[+o+]Burn AE [+w+]");
											//status Burn AE
											break;
										default:
											sprintf_s(temp, "Unknown [+w+]");
											break;
										}
									}
									break;
								case EQData::Rogue:
									sprintf_s(mercClass, "[+g+]Rogue [+w+]");
									{
										switch (mercStance) {
										case 0:
											sprintf_s(temp, "[+r+]Passive [+w+]");
											//status Passive
											break;
										case 1:
											sprintf_s(temp, "[+g+]Balanced [+w+]");
											//status Balanced
											break;
										case 2:
											sprintf_s(temp, "[+o+]Burn [+w+]");
											//status Burn
											break;
										default:
											sprintf_s(temp, "Unknown [+w+]");
											break;
										}
									}
									break;
								default:
									sprintf_s(temp, "Unknown class [+w+]");
									break;
								}
								sprintf_s(mercStatInfo, "Mercenary State: %s Mercenary Class: %s Mercenary Stance: %s ", MercenarySpawn.Name, mercClass, temp);
							}
						}
						break;
					default:
						strcpy_s(MercenarySpawn.Name, "UNKNOWN");
						WriteChatf("%s", MercenarySpawn.Name);
						break;
					}
				}
				else {
					sprintf_s(mercStatInfo, "[+r+]It does not appear we have a merc.[+w+]");
				}
			}
			else {
				sprintf_s(mercStatInfo, "[+r+]It does not appear we have a merc.[+w+]");
			}
			strcat_s(buffer, mercStatInfo);
			EzCommand(buffer);
		}
		if (!_stricmp(Arg, "campfire")) {
			if (pLocalPlayer && ((PSPAWNINFO)pLocalPlayer)->Campfire) {
				char cfStatus[MAX_STRING] = "";
				char cfInfo[MAX_STRING] = "";
				char cfTimeRemainHMS[MAX_STRING] = "";
				char cfZoneLongName[MAX_STRING] = "";
				if (unsigned long cfTimeRemain = ((PSPAWNINFO)pLocalPlayer)->CampfireTimestamp - GetFastTime()) {
					unsigned long Hrs = ((cfTimeRemain / 60) / 60);
					unsigned long Mins = ((cfTimeRemain / 60) - (Hrs * 60));
					unsigned long Secs = ((cfTimeRemain) - ((Mins + (Hrs * 60)) * 60));
					sprintf_s(cfTimeRemainHMS, "%lu:%02lu:%02lu ", Hrs, Mins, Secs);
					if (DWORD ZoneID = (((PSPAWNINFO)pLocalPlayer)->CampfireZoneID & 0x7FFF)) {
						if (ZoneID < MAX_ZONES && pWorldData) {
							 if (PZONELIST pZoneID = ((PWORLDDATA)pWorldData)->ZoneArray[ZoneID]) {
								 //WriteChatf("Zone Long Name is: %s", pZoneID->LongName);
								 strcat_s(cfZoneLongName, pZoneID->LongName);
							 }
							 else {
								 strcat_s(buffer, "[+r+]I don't appear to have a campfire[+w+]");
							 }
						}
						else {
							if (ZoneID > MAX_ZONES) {
								strcat_s(buffer, "[+r+]ZoneID is bad?![+w+] ");
							}
							if (!pWorldData) {
								strcat_s(buffer, "[+r+]There was no pWorldData, are you in game?![+w+] ");
							}
						}
					}
					else {
						return;
					}
					PCHARINFO2 pChar2 = GetCharInfo2();
					sprintf_s(cfStatus, "[+g+]Active[+w+]");
				}
				sprintf_s(cfInfo, "Campfire: [+g+]%s[+w+] Time Left: [+g+]%s[+w+] Campfire Zone: [+g+]%s[+w+] ", cfStatus, cfTimeRemainHMS, cfZoneLongName);
				strcat_s(buffer, cfInfo);
				EzCommand(buffer);
			}
			else {
				strcat_s(buffer, "[+r+]We do not appear to have a campfire in a usable location![+w+]"); // if the fellowship is in an instance we can't fellowship to - it returns out
				EzCommand(buffer);
			}
		}
		if (!_stricmp(Arg, "fellow") || !_stricmp(Arg, "fellowship")) {
			if (PCHARINFO pChar = GetCharInfo()) {
				char ClassName[64] = { 0 };
				PFELLOWSHIPINFO Fellowship = (PFELLOWSHIPINFO)&pChar->pSpawn->Fellowship;
				WriteChatf("FS MoTD: %s", Fellowship->MotD);
				WriteChatf("FS Leader is: %s , We have : %lu members", Fellowship->Leader, Fellowship->Members);
				if (PFELLOWSHIPINFO pFellowship = (PFELLOWSHIPINFO)&pChar->pSpawn->Fellowship) {
					if (int NumMembers = pFellowship->Members) {
						for (int i = 0; i < NumMembers; i++) {
							if (PFELLOWSHIPMEMBER thisMember = &pFellowship->FellowshipMember[i]) {

								char ClassDesc[64] = { 0 };
								sprintf_s(ClassDesc, GetClassDesc(thisMember->Class));
								WriteChatf("%s - %lu - Class Name: %s ", thisMember->Name, thisMember->Level, ClassDesc);
							}
						}
					}
				}
			}

		}
		if (!_stricmp(Arg, "bagspace")) {
			char stat[MAX_STRING] = "I have ";
			GetFreeInventory(0);
			int getbagspace = GetFreeInventory(4);
			char bagspace[MAX_STRING] = "";
			_ltoa_s(getbagspace, bagspace, 10);
			sprintf_s(stat, "[+o+]Free Inventory Slots: [+g+]%s[+w+]", bagspace);
			strcat_s(buffer, stat);
			EzCommand(buffer);
			return;
		}
		
		if (!strlen(szLine)) {
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
			else if (notLoaded) {//Only outputs if there is a CWTN classPlugin available for that class, and it wasn't loaded.
				strcat_s(buffer, "Class Plugin:[+r+] Not Loaded! [+w+] ");
			}
			//Am I running a macro.
			if (gMacroStack && strlen(gszMacroName)) {
				char temp[MAX_STRING] = "Macro:";
				//Is the currently running macro "kiss"Assist? Where any macro with the word "kiss" will be found for people running custom KA's or older KA's etc. 
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
						//Seem to not have a role. That's weird. 
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
					strcat_s(buffer, "Macro:[+r+] FALSE [+w+] ");
				}
			}
			if (pMercInfo) {
				if (pMercInfo->HaveMerc == 1) {
					switch (pMercInfo->MercState)
					{
					case 0:
						//strcat_s(buffer, "Mercenary State: [+r+]DEAD [+w+]");
						break;
					case 1:
						//strcat_s(buffer, "Mercenary State: [+r+]SUSPENDED [+w+]");
						break;
					case 5:
						strcat_s(buffer, "Mercenary State: [+g+]ALIVE [+w+]");
						break;
					default:
						break;
					}
				}
			}

		//Am I Invis?
			if (int amHidden = pChar->HideMode) {
				strcat_s(buffer, "Hiding:[+r+] TRUE[+w+]");
			}

			//Do the command we've decided on.
			EzCommand(buffer);
		}
	}
	else {
		WriteChatf("\ap%s\ar is not a valid option. Valid options are stat, item, itembank, merc, aa, fellowship, campfire, bagspace, or no argument at all.", Arg);
	}
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

void ReverseString(PCHAR szLine) {
	std::string temp2 = szLine;
	std::reverse(temp2.rbegin(), temp2.rend());
	sprintf_s(szLine, MAX_STRING, temp2.c_str());
}

void PutCommas(PCHAR szLine) {
	ReverseString(szLine);
	unsigned int j = 0;
	char temp[MAX_STRING] = { 0 };
	for (unsigned int i = 0; i < strlen(szLine) + 1; i++) {
		if (i % 3 == 0 && i != 0 && i != strlen(szLine)) {
			temp[j] = ',';
			j++;
		}
		temp[j] = szLine[i];
		j++;
	}
	sprintf_s(szLine, MAX_STRING, temp);
	ReverseString(szLine);
}

inline float PercentHealth(PSPAWNINFO& pSpawn)
{
	return (float)pSpawn->HPCurrent / (float)pSpawn->HPMax * 100.0f;
}

inline float PercentEndurance(PSPAWNINFO& pSpawn)
{
	return (float)pSpawn->GetCurrentEndurance() / (float)pSpawn->GetMaxEndurance() * 100.0f;
}

inline float PercentMana(PSPAWNINFO& pSpawn)
{
	if (GetCharInfo()->pSpawn->GetMaxMana() == 0) {
		return 0;
	}
	return (float)pSpawn->GetCurrentMana() / (float)pSpawn->GetMaxMana() * 100.0f;
}