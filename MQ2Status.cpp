/*
//MQ2Status by Chatwiththisname and Sic
//	/status help - returns help information for the / commands
*/
#include "../MQ2Plugin.h"
#include <sstream>

PreSetup("MQ2Status");
PLUGIN_VERSION(2.0);

bool bShowPlugin = true;
bool bShowWarrior = true;
bool bShowCleric = true;
bool bShowPaladin = false;
bool bShowRanger = false;
bool bShowShadowknight = true;
bool bShowDruid = false;
bool bShowMonk = true;
bool bShowBard = false;
bool bShowRogue = true;
bool bShowShaman = false;
bool bShowNecromancer = false;
bool bShowWizard = false;
bool bShowMage = false;
bool bShowEnchanter = false;
bool bShowBeastlord = true;
bool bShowBerserker = true;
bool bConnectedToEQBC = false;
bool bConnectedToDannet = false;

const int MAX_ARGS = 20;

bool atob(char* pChar);
std::string ConnectedToReportOutput();
bool FindPlugin(const char* szPluginName);
bool HaveAlias(const std::string& aliasName);
bool IHaveSpa(int spa);
bool IsDefined(char* szLine);
bool VerifyINI(char* Section, char* Key, char* Default);
inline float PercentHealth(SPAWNINFO* pSpawn);
inline float PercentEndurance(SPAWNINFO* pSpawn);
inline float PercentMana(SPAWNINFO* pSpawn);
int GetSubscriptionLevel();
void DoINIThings();
void ParseBoolArg(const char* Arg, const char* Arg2, char* Arg3, bool* theOption, char* INIsection);
void PutCommas(char* szLine);
void ReverseString(char* szLine);
void StatusCmd(SPAWNINFO* pChar, char* szLine);

template <typename T>
std::string LabeledText(const std::string& Label, T Value);
std::string stringBuffer = { 0 };
std::string GetColorCode(char Color, bool Dark);

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
	std::ostringstream out;
	out.precision(n);
	out << std::fixed << a_value;
	return out.str();
}

enum Subscription {
	SUB_BRONZE,
	SUB_SILVER,
	SUB_GOLD
};

void StatusCmd(SPAWNINFO* pChar, char* szLine)
{
	std::string stringBuffer = ConnectedToReportOutput();
	char buffer[MAX_STRING] = { 0 };
	if (!bConnectedToDannet && !bConnectedToEQBC) return;

	bool classPlugin = false; // Only true if there is a class plugin for this class, and the plugin was loaded.
	bool notLoaded = false; // Would only be true if one of the classes in the switch has a plugin, but it's not loaded

	// Get our Parameters
	char Arg[MAX_STRING] = { 0 };
	char Arg2[MAX_STRING] = { 0 };
	char Arg3[MAX_STRING] = { 0 };
	GetArg(Arg, szLine, 1);
	GetArg(Arg2, szLine, 2);
	GetArg(Arg3, szLine, 3);
	PCHARINFO pCharInfo = GetCharInfo();
	PCHARINFO2 pCharInfo2 = GetCharInfo2();

	if (!_stricmp(Arg, "show")) {
		if (!_stricmp(Arg2, "plugin")) {
			ParseBoolArg(Arg, Arg2, Arg3, &bShowPlugin, "ShowPlugin");
			return;
		}

		if (!_stricmp(Arg2, "warrior")) {
			ParseBoolArg(Arg, Arg2, Arg3, &bShowWarrior, "ShowPlugin");
			return;
		}

		if (!_stricmp(Arg2, "cleric")) {
			ParseBoolArg(Arg, Arg2, Arg3, &bShowCleric, "ShowPlugin");
			return;
		}

		if (!_stricmp(Arg2, "paladin")) {
			ParseBoolArg(Arg, Arg2, Arg3, &bShowPaladin, "ShowPlugin");
			return;
		}

		if (!_stricmp(Arg2, "ranger")) {
			ParseBoolArg(Arg, Arg2, Arg3, &bShowRanger, "ShowPlugin");
			return;
		}

		if (!_stricmp(Arg2, "shadowknight")) {
			ParseBoolArg(Arg, Arg2, Arg3, &bShowShadowknight, "ShowPlugin");
			return;
		}

		if (!_stricmp(Arg2, "druid")) {
			ParseBoolArg(Arg, Arg2, Arg3, &bShowDruid, "ShowPlugin");
			return;
		}

		if (!_stricmp(Arg2, "monk")) {
			ParseBoolArg(Arg, Arg2, Arg3, &bShowMonk, "ShowPlugin");
			return;
		}

		if (!_stricmp(Arg2, "bard")) {
			ParseBoolArg(Arg, Arg2, Arg3, &bShowBard, "ShowPlugin");
			return;
		}

		if (!_stricmp(Arg2, "rogue")) {
			ParseBoolArg(Arg, Arg2, Arg3, &bShowRogue, "ShowPlugin");
			return;
		}

		if (!_stricmp(Arg2, "shaman")) {
			ParseBoolArg(Arg, Arg2, Arg3, &bShowShaman, "ShowPlugin");
			return;
		}

		if (!_stricmp(Arg2, "necromancer")) {
			ParseBoolArg(Arg, Arg2, Arg3, &bShowNecromancer, "ShowPlugin");
			return;
		}

		if (!_stricmp(Arg2, "wizard")) {
			ParseBoolArg(Arg, Arg2, Arg3, &bShowWizard, "ShowPlugin");
			return;
		}

		if (!_stricmp(Arg2, "magician")) {
			ParseBoolArg(Arg, Arg2, Arg3, &bShowMage, "ShowPlugin");
			return;
		}

		if (!_stricmp(Arg2, "enchanter")) {
			ParseBoolArg(Arg, Arg2, Arg3, &bShowEnchanter, "ShowPlugin");
			return;
		}

		if (!_stricmp(Arg2, "beastlord")) {
			ParseBoolArg(Arg, Arg2, Arg3, &bShowBeastlord, "ShowPlugin");
			return;
		}

		if (!_stricmp(Arg2, "berserker")) {
			ParseBoolArg(Arg, Arg2, Arg3, &bShowBerserker, "ShowPlugin");
			return;
		}

		WriteChatf("\arPlease provide a valid \agShow\aw option.\aw");
		WriteChatf("\ayImportant!\aw \agShow Plugin Off\aw will hide ALL the plugins.");
		WriteChatf("\ayImportant!\aw To display an individual plugin, you will need \agShow Plugin On\aw as well as the individual class plugin set to on.");
		WriteChatf("\arExamples: \agShow\ay Plugin, \agShow\ay Warrior, \agShow\ay Cleric, \agShow\ay Paladin, \agShow\ay Ranger, \agShow\ay Shadowknight, \agShow\ay Druid, \agShow\ay Monk, \agShow\ay Bard\aw");
		WriteChatf("\arExamples: \agShow\ay Rogue, \agShow\ay Shaman, \agShow\ay Necromancer, \agShow\ay Wizard, \agShow\ay Magician, \agShow\ay Enchanter, \agShow\ay Beastlord, \agShow\ay Berserker\aw");
		return;
	}

	if (!_stricmp(Arg, "help")) {
		WriteChatf("Welcome to MQ2Status");
		WriteChatf("By \aoChatWithThisName\aw & \agSic\aw Exclusively for \arRedGuides\aw.");
		WriteChatf("\agValid Status options are:\aw");
		WriteChatf("\ao/status will output to eqbc/dannet: If we have a CWTN Class Plugin loaded, if we have a macro, if our macro is kiss - it will say what our role is, if we are paused, if we are hidden, and if we have a merc that is alive.");
		WriteChatf("\ao/status \agitem\aw \ayitem name\aw: reports how many \ayitem name\aw you have in your inventory.");
		WriteChatf("\ao/status \agitembank\aw \ayitem name\aw: reports how many \ayitem name\aw you have in your bank.");
		WriteChatf("\ao/status \agstat\aw \ayoption\aw: reports the following options to eqbc: Hdex, HStr, HSta, HInt, HAgi, HWis, HCha, HPS, Mana, Endurance, and, Money.");
		WriteChatf("\ao/status \agaa\aw: Reports how many \"banked\" AA points you have.");
		WriteChatf("\ao/status \agmerc\aw: Reports mercenary information including class, and role.");
		WriteChatf("\ao/status \agcampfire\aw: Reports campfire information including Active, Duration, and Zone.");
		WriteChatf("\ao/status \agfellowship\aw: This returns to your mq2window (does not eqbc/dannet) information on your fellowship");
		WriteChatf("\ao/status \agbagspace\aw: Reports how many open bagspaces you have.");
		WriteChatf("\ao/status \agquest\aw or \agtask\aw \ayQuest name\aw: Reports if you have a quest/task matching \ayQuest name\aw.");
#if !defined(ROF2EMU)
		WriteChatf("\ao/status \agsub\aw: Reports to eqbc our subscription level, and if we are gold, how many days are left.");
#endif
		WriteChatf("\ao/status \agxp\aw: Reports to eqbc our level, Current XP %%, Banked AA, and our AAXP %%.");
		WriteChatf("\ao/status \agaaxp\aw: Reports to eqbc our Spent AA, our AAXP %%, and our Banked AA.");
		WriteChatf("\ao/status \agshow\aw: Allows toggling on/off of the CWTN Class Plugins to be visible during /status.");
		WriteChatf("\ao/status \agparcel\aw: Reports our \"Parcel\" status.");
		WriteChatf("\ao/status \aginvis\aw: Reports our Invis and IVU status, so we can check we are \"Double Invis\".");
		return;
	}

	if (!_stricmp(Arg, "item")) {
		GetArg(Arg, szLine, 2);
		if (!strlen(Arg)) {
			WriteChatf("\arPlease provide a valid Item to search for\aw");
			WriteChatf("\arExamples: Bone Chips, Diamond, Blue Diamond, etc.\aw");
		}
		else {
			std::string findItem;
			for (int i = 2; i < MAX_ARGS; i++) {
				GetArg(Arg, szLine, i); // grab all params after szLine 1
				if (strlen(Arg)) {
					if (i > 2) {
						findItem += " ";
					}
					findItem += Arg;
				}
				else {
					break;
				}
			}
			stringBuffer += LabeledText(findItem, FindItemCountByName(&findItem[0]));
			strcat_s(buffer, stringBuffer.c_str());
			EzCommand(buffer);
		}
		return;
	}

	if (!_stricmp(Arg, "itembank")) {
		GetArg(Arg, szLine, 2);
		if (!strlen(Arg)) {
			WriteChatf("\arPlease provide a valid Item to search for\aw");
			WriteChatf("\arExamples: Bone Chips, Diamond, Blue Diamond, etc.\aw");
		}
		else {
			std::string findItem;
			for (int i = 2; i < MAX_ARGS; i++) {
				GetArg(Arg, szLine, i); // grab all params after szLine 1
				if (strlen(Arg)) {
					if (i > 2) {
						findItem += " ";
					}
					findItem += Arg;
				}
				else {
					break;
				}
			}
			stringBuffer += LabeledText(findItem, FindBankItemCountByName(&findItem[0], 0)); // FindBankItemCountByName requires bExact
			strcat_s(buffer, stringBuffer.c_str());
			EzCommand(buffer);
		}
		return;
	}

	if (!_stricmp(Arg, "quest") || !_stricmp(Arg, "task")) {
		GetArg(Arg, szLine, 2);
		if (Arg[0] == 0) { // if an Argument after quest/task wasn't made, we need to ask for one
			WriteChatf("\arPlease provide a valid Quest/Task Name to search for.\aw");
		}
		else {
			const char* tempArg = GetNextArg(szLine);
			stringBuffer += GetColorCode('o', false);
			stringBuffer += "Quest/Task \"";
			stringBuffer += GetColorCode('t', true);
			stringBuffer += tempArg;
			stringBuffer += GetColorCode('o', false);
			stringBuffer += "\": ";
			char tempTask[MAX_STRING] = "";
			sprintf_s(tempTask, "${Task[%s]}", tempArg);
			ParseMacroData(tempTask, MAX_STRING);
			if (_stricmp(tempTask, "NULL")) {
				stringBuffer += GetColorCode('g', false);
			}
			else {
				stringBuffer += GetColorCode('r', false);
			}
			stringBuffer += tempTask;
			EzCommand(&stringBuffer[0]);
		}
		return;
	}

	if (!_stricmp(Arg, "stat")) {
		SPAWNINFO* me = GetCharInfo()->pSpawn;
		GetArg(Arg, szLine, 2);
		if (!strlen(Arg)) {
			WriteChatf("\arPlease provide a valid MQ2Status stat\aw");
			WriteChatf("\aoThese are currently: \aghstr, hsta, hint, hwis, hagi, hdex, hcha, hps, endurance, money, and mana.\aw");
		}
		else {
			bool bFound = true;
			if (!_stricmp(Arg, "hstr")) {
				stringBuffer += LabeledText("HSTR", pCharInfo->HeroicSTRBonus);
			}
			else if (!_stricmp(Arg, "hsta")) {
				stringBuffer += LabeledText("HSTA", pCharInfo->HeroicSTABonus);
			}
			else if (!_stricmp(Arg, "hint")) {
				stringBuffer += LabeledText("HINT", pCharInfo->HeroicINTBonus);
			}
			else if (!_stricmp(Arg, "hwis")) {
				stringBuffer += LabeledText("HWIS", pCharInfo->HeroicWISBonus);
			}
			else if (!_stricmp(Arg, "hagi")) {
				stringBuffer += LabeledText("HAGI", pCharInfo->HeroicAGIBonus);
			}
			else if (!_stricmp(Arg, "hdex")) {
				stringBuffer += LabeledText("HDEX", pCharInfo->HeroicDEXBonus);
			}
			else if (!_stricmp(Arg, "hcha")) {
				stringBuffer += LabeledText("HCHA", pCharInfo->HeroicCHABonus);
			}
			else if (!_stricmp(Arg, "hps")) {
				stringBuffer += LabeledText("Current HPs", me->HPCurrent) + " " +
					LabeledText("Max HPs", me->HPMax) + " " +
					LabeledText("Health Pct", PercentHealth(me));
			}
			else if (!_stricmp(Arg, "mana")) {
				stringBuffer += LabeledText("Current Mana", me->GetCurrentMana()) + " " +
					LabeledText("Current Mana", me->GetMaxMana()) + " " +
					LabeledText("Mana Pct", PercentMana(me));
			}
			else if (!_stricmp(Arg, "endurance")) {
				stringBuffer += LabeledText("Current Endurance", me->GetCurrentEndurance()) + " " +
					LabeledText("Current Endurance", me->GetMaxEndurance()) + " " +
					LabeledText("Endurance Pct", PercentEndurance(me));
			}
			else if (!_stricmp(Arg, "money") || !_stricmp(Arg, "plat")) {
				char szmyPlat[MAX_STRING] = "";
				_ltoa_s(pCharInfo2->Plat, szmyPlat, 10);
				PutCommas(szmyPlat);
				stringBuffer += LabeledText("Plat", szmyPlat);
			}
			else {
				WriteChatf("\arThat was not a valid stat, \agplease use hstr, hsta, hint, hwis, hagi, hdex, hcha, hps, mana, endurance, or money for this option!\aw");
				bFound = false;
			}
			if (bFound) {
				strcat_s(buffer, stringBuffer.c_str());
				EzCommand(buffer);
				return;
			}
		}
		return;
	}

	if (!_stricmp(Arg, "aa")) {
		stringBuffer += LabeledText("Available AA Points", pCharInfo2->AAPoints);
		strcat_s(buffer, stringBuffer.c_str());
		EzCommand(buffer);
		return;
	}

#if !defined(ROF2EMU) // Subscription doesn't exist in EMU'
	if (!_stricmp(Arg, "sub") || !_stricmp(Arg, "subscription")) {
		switch (GetSubscriptionLevel())
		{
		case SUB_GOLD:
			stringBuffer += GetColorCode('o', false) + "Sub: " + GetColorCode('g', false) + "Gold" + " ";
			break;
		case SUB_SILVER:
			stringBuffer += GetColorCode('o', false) + "Sub: " + GetColorCode('r', false) + "Silver" + " ";
			break;
		case SUB_BRONZE:
			stringBuffer += GetColorCode('o', false) + "Sub: " + GetColorCode('r', false) + "Bronze" + " ";
			break;
		default:
			break;
		}
		if (GetSubscriptionLevel() == SUB_GOLD) {
			if (pCharInfo->SubscriptionDays == -1) {
				stringBuffer += GetColorCode('o', false) + "& I appear to have a lifetime subscription.";
			}
			else if (pCharInfo->SubscriptionDays == 0) {
				stringBuffer += GetColorCode('o', false) + "& I have a 0 days left or a lifetime subscription.";
			}
			else if (pCharInfo->SubscriptionDays) {
				stringBuffer += LabeledText("& I have ", pCharInfo->SubscriptionDays) + " days remaining.";
			}
		}
		strcat_s(buffer, stringBuffer.c_str());
		EzCommand(buffer);
		return;
	}
#endif

	if (!_stricmp(Arg, "merc")) {
		if (pMercInfo) {
			//ZeroMemory(&MercenarySpawn, sizeof(MercenarySpawn));
			if (pMercInfo->HaveMerc == 1) {
				switch (pMercInfo->MercState)
				{
				case 0:
					stringBuffer += GetColorCode('o', false) + "Class: " + GetColorCode('r', false) + "Dead! " + GetColorCode('w', false);
					break;
				case 1:
					stringBuffer += GetColorCode('o', false) + "Mercenary State: " + GetColorCode('r', false) + "Suspended! " + GetColorCode('w', false);
					break;
				case 5:
					stringBuffer += GetColorCode('o', false) + "Mercenary State: " + GetColorCode('g', false) + "Alive! " + GetColorCode('w', false);

					{
						DWORD mercStance = pMercInfo->ActiveStance;
						if (PSPAWNINFO myMerc = (PSPAWNINFO)GetSpawnByID(pMercInfo->MercSpawnId)) {
							switch (myMerc->GetClass()) {
							case Cleric:
								stringBuffer += GetColorCode('o', false) + "Class: " + GetColorCode('g', false) + "Cleric " + GetColorCode('w', false);
								{
									switch (mercStance) {
									case 0:
										stringBuffer += GetColorCode('o', false) + "Stance: " + GetColorCode('g', false) + "Balanced " + GetColorCode('w', false);
										//status Balanced
										break;
									case 1:
										stringBuffer += GetColorCode('o', false) + "Stance: " + GetColorCode('g', false) + "Efficient" + GetColorCode('w', false);
										//status Efficient
										break;
									case 2:
										stringBuffer += GetColorCode('o', false) + "Stance: " + GetColorCode('g', false) + "Reactive " + GetColorCode('w', false);
										//status Reactive
										break;
									case 3:
										stringBuffer += GetColorCode('o', false) + "Stance: " + GetColorCode('g', false) + "Passive " + GetColorCode('w', false);
										//status Passive
										break;
									default:
										stringBuffer += GetColorCode('o', false) + "Stance: " + GetColorCode('g', false) + "Unknown " + GetColorCode('w', false);
										break;
									}
								}
								break;
							case Warrior:
								stringBuffer += GetColorCode('o', false) + "Class: " + GetColorCode('g', false) + "Warrior " + GetColorCode('w', false);
								{
									switch (mercStance) {
									case 0:
										stringBuffer += GetColorCode('o', false) + "Stance: " + GetColorCode('g', false) + "Aggressive " + GetColorCode('w', false);
										//status Aggressive
										break;
									case 1:
										stringBuffer += GetColorCode('o', false) + "Stance: " + GetColorCode('g', false) + "Assist " + GetColorCode('w', false);
										//status Assist
										break;
									case 2:
										stringBuffer += GetColorCode('o', false) + "Stance: " + GetColorCode('g', false) + "Passive " + GetColorCode('w', false);
										//status Passive
										break;
									default:
										stringBuffer += GetColorCode('o', false) + "Stance: " + GetColorCode('g', false) + "Unknown " + GetColorCode('w', false);
										break;
									}
								}
								break;
							case Wizard:
								stringBuffer += GetColorCode('o', false) + "Class: " + GetColorCode('g', false) + "Wizard " + GetColorCode('w', false);
								{
									switch (mercStance) {
									case 0:
										stringBuffer += GetColorCode('o', false) + "Stance: " + GetColorCode('g', false) + "Passive " + GetColorCode('w', false);
										//status Passive
										break;
									case 1:
										stringBuffer += GetColorCode('o', false) + "Stance: " + GetColorCode('g', false) + "Balanced " + GetColorCode('w', false);
										//status Balanced
										break;
									case 2:
										stringBuffer += GetColorCode('o', false) + "Stance: " + GetColorCode('g', false) + "Burn " + GetColorCode('w', false);
										//status Burn
										break;
									case 3:
										stringBuffer += GetColorCode('o', false) + "Stance: " + GetColorCode('g', false) + "Burn AE " + GetColorCode('w', false);
										//status Burn AE
										break;
									default:
										stringBuffer += GetColorCode('o', false) + "Stance: " + GetColorCode('g', false) + "Unknown " + GetColorCode('w', false);
										break;
									}
								}
								break;
							case Rogue:
								stringBuffer += GetColorCode('o', false) + "Class: " + GetColorCode('g', false) + "Rogue " + GetColorCode('w', false);
								{
									switch (mercStance) {
									case 0:
										stringBuffer += GetColorCode('o', false) + "Stance: " + GetColorCode('g', false) + "Passive " + GetColorCode('w', false);
										//status Passive
										break;
									case 1:
										stringBuffer += GetColorCode('o', false) + "Stance: " + GetColorCode('g', false) + "Balanced " + GetColorCode('w', false);
										//status Balanced
										break;
									case 2:
										stringBuffer += GetColorCode('o', false) + "Stance: " + GetColorCode('g', false) + "Burn " + GetColorCode('w', false);
										//status Burn
										break;
									default:
										stringBuffer += GetColorCode('o', false) + "Stance: " + GetColorCode('g', false) + "Unknown " + GetColorCode('w', false);
										break;
									}
								}
								break;
							default:
								stringBuffer += GetColorCode('g', false) + "Unknown Class " + GetColorCode('w', false);
								break;
							}
						}
					}
					break;
				default:
					stringBuffer += GetColorCode('g', false) + "Unknown" + GetColorCode('w', false);
					break;
				}
			}
		}
		else {
			stringBuffer += GetColorCode('r', false) + "It does not appear we have a merc." + GetColorCode('w', false);
		}
		strcat_s(buffer, stringBuffer.c_str());
		EzCommand(buffer);
		return;
	}

	if (!_stricmp(Arg, "campfire")) {
		if (pLocalPlayer && ((PSPAWNINFO)pLocalPlayer)->Campfire) {
			std::string cfStatus;
			std::string cfInfo;
			std::string cfTimeRemainHMS;
			std::string cfZoneLongName;
			if (unsigned long cfTimeRemain = ((PSPAWNINFO)pLocalPlayer)->CampfireTimestamp - GetFastTime()) {
				unsigned long Hrs = ((cfTimeRemain / 60) / 60);
				std::string sHrs = std::to_string(Hrs);
				unsigned long Mins = ((cfTimeRemain / 60) - (Hrs * 60));
				std::string sMins = std::to_string(Mins);
				unsigned long Secs = ((cfTimeRemain)-((Mins + (Hrs * 60)) * 60));
				std::string sSecs = std::to_string(Secs);
				cfTimeRemainHMS += sHrs + ":" + sMins + ":" + sSecs;
				if (unsigned long ZoneID = (((PSPAWNINFO)pLocalPlayer)->CampfireZoneID & 0x7FFF)) {
					if (ZoneID < MAX_ZONES && pWorldData) {
						if (ZONELIST* pZoneID = ((WORLDDATA*)pWorldData)->ZoneArray[ZoneID]) {
							cfZoneLongName += pZoneID->LongName;
						}
						else {
							stringBuffer += GetColorCode('r', false) + "I don't appear to have a campfire" + GetColorCode('w', false);
						}
					}
					else {
						if (ZoneID > MAX_ZONES) {
							stringBuffer += GetColorCode('r', false) + "ZoneID is bad?!" + GetColorCode('w', false);
						}
						if (!pWorldData) {
							stringBuffer += GetColorCode('r', false) + "There was no pWorldData, are you in game?!" + GetColorCode('w', false);
						}
					}
				}
				else {
					return;
				}
				cfStatus += GetColorCode('g', false) + "Active " + GetColorCode('g', false);
			}
			stringBuffer += LabeledText("Campfire", cfStatus) + " " + LabeledText("Time Left", cfTimeRemainHMS) + " " + LabeledText("Zone", cfZoneLongName);
			strcat_s(buffer, stringBuffer.c_str());
			EzCommand(buffer);
		}
		else {
			stringBuffer += GetColorCode('r', false) + " " + "We do not appear to have a campfire in a usable location!" + GetColorCode('w', false);
			strcat_s(buffer, stringBuffer.c_str());
			EzCommand(buffer);
		}
		return;
	}

	if (!_stricmp(Arg, "fellow") || !_stricmp(Arg, "fellowship")) { // We only have this WriteChatf and not reporting to eqbc/dannet
		std::string ClassNameStr;
		if (FELLOWSHIPINFO* pFellowship = (FELLOWSHIPINFO*)&pCharInfo->pSpawn->Fellowship) {
			if (pFellowship->Members > 0) {
				WriteChatf("FS MoTD: \ag%s\aw", pFellowship->MotD);
				WriteChatf("FS Leader is: \ag%s\aw , We have: \ay%lu\aw members", pFellowship->FellowshipMember[0].Name, pFellowship->Members);
				if (unsigned long NumMembers = pFellowship->Members) {
					for (unsigned int i = 0; i < NumMembers; i++) {
						if (FELLOWSHIPMEMBER* thisMember = &pFellowship->FellowshipMember[i]) {
							std::string ClassDescString;
							ClassDescString += GetClassDesc(thisMember->Class);
							WriteChatf("\ag%s\aw - \ay%lu\aw - \ap%s\aw ", thisMember->Name, thisMember->Level, ClassDescString.c_str());
						}
					}
				}
			}
			else {
				WriteChatf("\arIt does not appear we are in a fellowship.\aw");
			}
		}
		return;
	}

	if (!_stricmp(Arg, "bagspace")) {
		stringBuffer += LabeledText("Bagspace", GetFreeInventory(4));
		strcat_s(buffer, stringBuffer.c_str());
		EzCommand(buffer);
		return;
	}

	if (!_stricmp(Arg, "xp")) {
		SPAWNINFO* pMe = (SPAWNINFO*)pLocalPlayer;
		stringBuffer += LabeledText("Level", (int)pMe->Level) + " " +
			LabeledText("XP", floor((pCharInfo->Exp *.001) * 100.0) / 100.0) + " " +
			LabeledText("Banked AA", pCharInfo2->AAPoints) + " " +
			LabeledText("AAXP", pCharInfo->AAExp * 0.001);
		strcat_s(buffer, stringBuffer.c_str());
		EzCommand(buffer);
		return;
	}

	if (!_stricmp(Arg, "aaxp")) {
		stringBuffer += LabeledText("Spent AA", pCharInfo2->AAPointsSpent) + " " +
			LabeledText("AAXP", pCharInfo->AAExp * 0.001) + " " +
			LabeledText("Banked AA", pCharInfo2->AAPoints);
		strcat_s(buffer, stringBuffer.c_str());
		EzCommand(buffer);
		return;
	}

	if (!_stricmp(Arg, "parcel")) {
		switch (GetCharInfo()->ParcelStatus) {
		case ePS_HasParcels:
			stringBuffer += GetColorCode('g', false) + "We have parcels!" + GetColorCode('x', false);
			break;
		case ePS_NoParcels:
			stringBuffer += GetColorCode('t', false) + "We do not have any parcels!" + GetColorCode('x', false);
			break;
		case ePS_OverParcelsLimit:
			stringBuffer += GetColorCode('r', false) + "We have a PARCEL OVERLOAD!" + GetColorCode('x', false);
			break;
		default:
			break;
		}
		strcat_s(buffer, stringBuffer.c_str());
		EzCommand(buffer);
		return;
	}

	if (!_stricmp(Arg, "invis")) {
		if (IHaveSpa(12) || IHaveSpa(314)) {
			stringBuffer += GetColorCode('g', false) + "INVIS" + GetColorCode('x', false) + "::";
		}
		else {
			stringBuffer += GetColorCode('r', false) + "INVIS" + GetColorCode('x', false) + "::";
		}
		if (IHaveSpa(28) || IHaveSpa(315)) {
			stringBuffer += GetColorCode('g', false) + "IVU";
		}
		else {
			stringBuffer += GetColorCode('r', false) + "IVU";
		}
		strcat_s(buffer, stringBuffer.c_str());
		EzCommand(buffer);
		return;
	}

	if (!strlen(szLine)) {
#if !defined(ROF2EMU) // Subscription doesn't exist in EMU'
		if (GetSubscriptionLevel() == SUB_SILVER) {
			stringBuffer += GetColorCode('o', false) + "Sub: " + GetColorCode('r', false) + "Silver ";
		}
		else if (GetSubscriptionLevel() == SUB_BRONZE) {
			stringBuffer += GetColorCode('o', false) + "Sub: " + GetColorCode('r', false) + "Bronze ";
		}
#endif
		if (bShowPlugin) {
			switch (GetCharInfo2()->Class) {
			case Berserker:
				if (bShowBerserker) {
					if (FindPlugin("MQ2BerZerker")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Cleric:
				if (bShowCleric) {
					if (FindPlugin("MQ2Cleric")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Monk:
				if (bShowMonk) {
					if (FindPlugin("MQ2Monk")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Beastlord:
				if (bShowBeastlord) {
					if (FindPlugin("MQ2Bst")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Rogue:
				if (bShowRogue) {
					if (FindPlugin("MQ2Rogue")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Warrior:
				if (bShowWarrior) {
					if (FindPlugin("MQ2War")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Shadowknight:
				if (bShowShadowknight) {
					if (FindPlugin("MQ2Eskay")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			default:
				break;
			}

			if (classPlugin) {
				stringBuffer += GetColorCode('o', false) + "Class Plugin: " + GetColorCode('g', false) + "Loaded! " + GetColorCode('w', false);
			}
			else if (notLoaded) { // Only outputs if there is a CWTN classPlugin available for that class, and it wasn't loaded.
				stringBuffer += GetColorCode('o', false) + "Class Plugin: " + GetColorCode('r', false) + "Not Loaded! " + GetColorCode('w', false);
			}
		}

		//Am I running a macro.
		if (gMacroStack && strlen(gszMacroName)) { // FIX ME :: Likely need help in fixing this section.
			char temp[MAX_STRING] = "";
			std::string green = GetColorCode('g', false);
			std::string white = GetColorCode('w', false);
			std::string orange = GetColorCode('o', false);
			strcat_s(temp, orange.c_str());
			strcat_s(temp, "Macro: ");
			strcat_s(temp, green.c_str());
			strcat_s(temp, " ");
			strcat_s(temp, gszMacroName);
			strcat_s(temp, orange.c_str());
			strcat_s(temp, " ");
			if (IsDefined("Role")) {
				strcat_s(temp, " Role: ");
				strcat_s(temp, green.c_str());
				char theRole[64] = "${Role} ";//Get the value of the Role variable
				ParseMacroData(theRole, 64);
				strcat_s(temp, theRole);
				strcat_s(temp, white.c_str());
				stringBuffer += (const char*)temp;
			}
			// IHC Mercs does not use "Roles" everyone is Assist - this will return who they are assisting
			else if (IsDefined("assistname")) {
				strcat_s(temp, " Assisting: ");
				strcat_s(temp, green.c_str());
				char theRole[64] = "${assistname} ";
				ParseMacroData(theRole, 64);
				strcat_s(temp, theRole);
				strcat_s(temp, white.c_str());
				stringBuffer += (const char*)temp;
			}
			else {
				stringBuffer += GetColorCode('o', false) + "Macro: " + GetColorCode('g', false) + gszMacroName + " " + GetColorCode('w', false);
			}

			if (PMACROBLOCK pBlock = GetCurrentMacroBlock()) {
				if (pBlock->Paused) {
					stringBuffer += GetColorCode('r', false) + "***PAUSED*** " + GetColorCode('w', false);
				}
			}
		}
		else {
			if (!classPlugin) {
				stringBuffer += GetColorCode('o', false) + "Macro: " + GetColorCode('r', false) + "FALSE! " + GetColorCode('w', false);
			}
		}

		if (pMercInfo) {
			if (pMercInfo->HaveMerc == 1) {
				switch (pMercInfo->MercState)
				{
				case 0:
					//stringBuffer += GetColorCode('o', false) + "Mercenary State:" + GetColorCode('g', false) + " DEAD " + GetColorCode('w', false);
					break;
				case 1:
					//stringBuffer += GetColorCode('o', false) + "Mercenary State:" + GetColorCode('g', false) + " SUSPENDED " + GetColorCode('w', false);
					break;
				case 5:
					stringBuffer += GetColorCode('o', false) + "Mercenary State:" + GetColorCode('g', false) + " ALIVE! " + GetColorCode('w', false);
					break;
				default:
					break;
				}
			}
		}
			// Am I Invis?
		if (pCharInfo->pSpawn->HideMode) {
			stringBuffer += GetColorCode('o', false) + "Hidden:" + GetColorCode('w', false) + " ";
			if (IHaveSpa(12) || IHaveSpa(314)) {
				stringBuffer += GetColorCode('g', false) + "INVIS" + GetColorCode('w', false) + " ";
			}
			if (IHaveSpa(28) || IHaveSpa(315)) {
				stringBuffer += GetColorCode('g', false) + "IVU" + GetColorCode('w', false);
			}
		}
		strcat_s(buffer, stringBuffer.c_str());
		EzCommand(buffer);
	}
	else {
		WriteChatf("\ao[MQ2Status] \ap%s\ar is not a valid option. \ag /status help \aw for available options", Arg);
	}
}

bool FindPlugin(const char* szPluginName)
{
	if (!strlen(szPluginName)) return false;
	MQPLUGIN* pPlugin = pPlugins;
	while (pPlugin) {
		if (!_stricmp(szPluginName, pPlugin->szFilename)) {
			return true;
		}
		pPlugin = pPlugin->pNext;
	}
	return false;
}

bool IsDefined(char* szLine)
{
	return (FindMQ2DataVariable(szLine) != 0);
}

bool HaveAlias(const std::string& aliasName)
{
	char szTemp[MAX_STRING] = { 0 };
	char mq2dir[128] = "";
	sprintf_s(mq2dir, 128, "%s\\MacroQuest2.ini", gszINIPath);
	GetPrivateProfileString("Aliases", aliasName.c_str(), "None", szTemp, MAX_STRING, mq2dir);

	if (!_stricmp(szTemp, "None")) {
		return false;
	}
	return true;
}

void ReverseString(char* szLine)
{
	std::reverse(szLine, szLine + strlen(szLine));
}

void PutCommas(char* szLine)
{
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
	strcpy_s(szLine, MAX_STRING, temp);
	ReverseString(szLine);
}

inline float PercentHealth(SPAWNINFO* pSpawn)
{
	return ((float)pSpawn->HPCurrent / (float)pSpawn->HPMax) * 100.0f;
}

inline float PercentEndurance(SPAWNINFO* pSpawn)
{
	return ((float)pSpawn->GetCurrentEndurance() / (float)pSpawn->GetMaxEndurance()) * 100.0f;
}

inline float PercentMana(SPAWNINFO* pSpawn)
{
	if (pSpawn->GetMaxMana() <= 0) return 100.0f;
	return ((float)pSpawn->GetCurrentMana() / (float)pSpawn->GetMaxMana()) * 100.0f;
}

int GetSubscriptionLevel()
{
	if (EQADDR_SUBSCRIPTIONTYPE) {
		if (DWORD dwsubtype = *(DWORD*)EQADDR_SUBSCRIPTIONTYPE) {
			BYTE subtype = *(BYTE*)dwsubtype;
			return subtype;
		}
	}
	return 0;
}

bool IHaveSpa(int spa)
{
	for (int i = 0; i < NUM_LONG_BUFFS; i++) {
		PSPELL pBuff = GetSpellByID(GetCharInfo2()->Buff[i].SpellID);
		if (!pBuff)
			continue;
		if (IsSPAEffect(pBuff, spa))
			return true;
	}
	for (int i = 0; i < NUM_SHORT_BUFFS; i++) {
		PSPELL pBuff = GetSpellByID(GetCharInfo2()->Buff[i].SpellID);
		if (!pBuff)
			continue;
		if (IsSPAEffect(pBuff, spa))
			return true;
	}
	return false;
}

PLUGIN_API void InitializePlugin()
{
	if (HaveAlias("/status")) {
		WriteChatf("\ar[\a-tMQ2Status\ar]\ao:: \arIt appears you already have an Alias for \ap/status\ar please type \"\ay/alias /status delete\ar\" then reload this plugin.");
			EzCommand("/timed 10 /plugin MQ2Status Unload");
	}
	else {
		AddCommand("/status", StatusCmd);
	}
	DoINIThings();
}

PLUGIN_API void ShutdownPlugin()
{
	RemoveCommand("/status");
}

bool VerifyINI(char* Section, char* Key, char* Default) {
	char temp[MAX_STRING] = { 0 };
	if (GetPrivateProfileString(Section, Key, nullptr, temp, MAX_STRING, INIFileName) == 0) {
		WritePrivateProfileString(Section, Key, Default, INIFileName);
		// Because there was nothing in the ini, we're going to return the default value
		return atob(Default);
	}
	// Because there was a value in the ini, we're going to atob and return it
	return atob(temp);
}

void DoINIThings() {
	// We are going to Check the ini status, and if there is no ini
	// We are going to write these defaults
	bShowPlugin =       VerifyINI("ShowPlugin", "Plugin", "on");
	bShowWarrior =      VerifyINI("ShowPlugin", "Warrior", "on");
	bShowCleric =       VerifyINI("ShowPlugin", "Cleric", "on");
	bShowPaladin =      VerifyINI("ShowPlugin", "Paladin", "on");
	bShowRanger =       VerifyINI("ShowPlugin", "Ranger", "on");
	bShowShadowknight = VerifyINI("ShowPlugin", "Shadowknight", "on");
	bShowDruid =        VerifyINI("ShowPlugin", "Druid", "on");
	bShowMonk =         VerifyINI("ShowPlugin", "Monk", "on");
	bShowBard =         VerifyINI("ShowPlugin", "Bard", "on");
	bShowRogue =        VerifyINI("ShowPlugin", "Rogue", "on");
	bShowShaman =       VerifyINI("ShowPlugin", "Shaman", "on");
	bShowNecromancer =  VerifyINI("ShowPlugin", "Necromancer", "on");
	bShowWizard =       VerifyINI("ShowPlugin", "Wizard", "on");
	bShowMage =         VerifyINI("ShowPlugin", "Magician", "on");
	bShowEnchanter =    VerifyINI("ShowPlugin", "Enchanter", "on");
	bShowBeastlord =    VerifyINI("ShowPlugin", "Beastlord", "on");
	bShowBerserker =    VerifyINI("ShowPlugin", "Berserker", "on");
}

bool atob(char* pChar)
{
	if (!_stricmp(pChar, "true") || strtol(pChar, nullptr, 10) != 0 || !_stricmp(pChar, "on")) {
		return true;
	}
	return false;
}

void ParseBoolArg(const char* Arg, const char* Arg2, char* Arg3, bool* theOption, char* INIsection)
{
	// Arg3 must be a char* because we are using it with IsNumber which needs a char* and will not accept const char*
	if (!strlen(Arg3)) {
		WriteChatf("\at%s is currently: \ap%s", Arg2, *theOption ? "\agOn" : "\arOff");
		WriteChatf("\ayTo Change this, type /status %s %s [true, false, 0, 1, on, off].", Arg, Arg2);
		return;
	}
	if (IsNumber(Arg3) || !_stricmp(Arg3, "true") || !_stricmp(Arg3, "false") || !_stricmp(Arg3, "on") || !_stricmp(Arg3, "off")) {
		*theOption = atob(Arg3);
		WritePrivateProfileString(INIsection, Arg2, *theOption ? "on" : "off", INIFileName);
		WriteChatf("\at%s is now: \ap%s", Arg2, *theOption ? "\agOn" : "\arOff");
	}
	else {
		WriteChatf("\ap%s \aris not a valid option for \ag%s %s\aw. \arValid options are: \aytrue, false, 0, 1, on, off.", Arg3, Arg, Arg2);
	}
	return;
}

std::string ConnectedToReportOutput()
{
	if (HMODULE hMod = GetModuleHandle("mq2eqbc")) {
		if (auto isConnected = (unsigned short(*)())GetProcAddress(hMod, "isConnected")) {
			if (isConnected()) {
				bConnectedToEQBC = true;
				return "/bc ";
			}
		}
	}
	if (auto hMod = GetModuleHandle("mq2dannet")) {
		bConnectedToDannet = true;
		return "/dgtell all ";
	}
	if (!bConnectedToEQBC && !bConnectedToDannet) {
		WriteChatf("MQ2Status only works if mq2eqbc or mq2dannet is loaded and connected");
		WriteChatf("If you would like to use EQBC, please run /plugin mq2eqbc load and then /bccmd connect");
		WriteChatf("Or if you would like to use Dannet, /plugin mq2dannet load");
	}
	return {};
}

template <typename T>
std::string LabeledText(const std::string& Label, T Value)
{
	return GetColorCode('o', false) + Label + ": " + GetColorCode('g', false) + to_string_with_precision(Value, 2) + GetColorCode('x', false);
}

std::string GetColorCode(char Color, bool Dark)
{
	if (Dark) {
		char ColorU = toupper(Color);
		return bConnectedToDannet ? std::string("\a-") + Color : std::string("[+") + ColorU + "+]";
	}
	else
		return bConnectedToDannet ? std::string("\a") + Color : std::string("[+") + Color + "+]";


}