/*
//MQ2Status by Chatwiththisname and Sic
//	/status help - returns help information for the / commands
*/
#include <mq/Plugin.h>
#include <mq/imgui/ImGuiUtils.h>

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
bool HaveAlias(const std::string& aliasName);
bool IHaveSpa(int spa);
bool IsDefined(char* szLine);
bool VerifyINI(char* Section, char* Key, char* Default);
inline float PercentHealth(SPAWNINFO* pSpawn);
inline float PercentEndurance(SPAWNINFO* pSpawn);
inline float PercentMana(SPAWNINFO* pSpawn);
void DoINIThings();
void ParseBoolArg(const char* Arg, const char* Arg2, char* Arg3, bool* theOption, char* INIsection);
void PutCommas(char* szLine);
void ReverseString(char* szLine);
void StatusCmd(SPAWNINFO* pChar, char* szLine);
std::string GetSpellUpgradeType(int level);

template <typename T>
std::string LabeledText(const std::string& Label, T Value);
std::string stringBuffer;
std::string GetColorCode(char Color, bool Dark);

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
	std::ostringstream out;
	out.precision(n);
	out << std::fixed << a_value;
	return out.str();
}

// TODO: FIXME
// There currently isn't a way to check what lua stuff is running in c++ land
// unlike plugins that use the g_pluginMap global
// MQ2Lua uses a s_infoMap which is local to MQ2Lua
static char* LuaScriptStatus(const char* scriptname) {
	static char luaScript[64] = "";
	if (IsPluginLoaded("Lua")) {
		// format is "${Lua.Script[scriptname].Status}"
		sprintf_s(luaScript, "${Lua.Script[");
		strcat_s(luaScript, scriptname);
		strcat_s(luaScript, "].Status}");
		ParseMacroData(luaScript, 64);
	}
	/* return values as defined in LuaThread.h
		"STARTING";
		"RUNNING";
		"PAUSED";
		"EXITED";
		"UNKNOWN";
	*/
	return luaScript;
}

// TODO:: FIXME
// ideally would actually be able to pull in-script TLO's
// so we can see if it is paused
// which is not the same as the paused lua status in some circumstances
bool IsLuaScriptRunning(const char* scriptname) {
	return ci_equals(LuaScriptStatus(scriptname), "RUNNING");
}

// TODO remove once an appropriate function is in main/core.
int ItemCountByType(int type) {
	PcProfile* pcProfile = GetPcProfile();
	int count = 0;
	for (int i = InvSlot_FirstWornItem; i < InvSlot_Max; i++) {
		// check top level slots
		if (ItemClient* pItem = pcProfile->GetInventorySlot(i)) {
			if (pItem->GetItemClass() == type) {
				count += pItem->GetItemCount();
			}

			if (!pItem->IsContainer())
				continue;

			// check inside the container
			for (ItemClient* pPackItem : pItem->GetHeldItems()) {
				if (pPackItem && pPackItem->GetItemClass() == type) {
					count += pPackItem->GetItemCount();
				}
			}
		}
	}
	return count;
};

enum eItemCountStatusType {
	Item,
	ItemAll,
	ItemBank
};

std::string ItemCountStatusByID(const char* Arg, const int type)
{
	std::string output;
	const char* findItemIDname = GetNextArg(Arg, 2);
	int iItemID = GetIntFromString(findItemIDname, 0);
	ItemClient* findItemName = FindItemByID(iItemID);

	if (findItemName) {
		findItemIDname = findItemName->GetName();
	}

	if (iItemID) {
		switch (type) {
			case eItemCountStatusType::Item:
				output = LabeledText(findItemIDname, FindItemCountByID(iItemID));
				break;
			case eItemCountStatusType::ItemAll:
				output = LabeledText(findItemIDname, FindItemCountByID(iItemID) + FindBankItemCountByID(iItemID));
				break;
			case eItemCountStatusType::ItemBank:
				output = LabeledText(findItemIDname, FindBankItemCountByID(iItemID));
				break;
			default:
				break;
		}
	}
	else {
		WriteChatf("\ao[MQ2Status] \arPlease provide a valid Item ID to search for.\aw");
		WriteChatf("\ao[MQ2Status] \arExample: \ay\"/status itembank id 10037\"\aw.");
	}

	return output;
}

const std::map<std::string, int>  mEquipInvSlotName {
	{ "Ammo",         eInventorySlot::InvSlot_Ammo },
	{ "Arms",         eInventorySlot::InvSlot_Arms },
	{ "Back",         eInventorySlot::InvSlot_Back },
	{ "Charm",        eInventorySlot::InvSlot_Charm },
	{ "Chest",        eInventorySlot::InvSlot_Chest },
	{ "Face",         eInventorySlot::InvSlot_Face },
	{ "Feet",         eInventorySlot::InvSlot_Feet },
	{ "Hands",        eInventorySlot::InvSlot_Hands },
	{ "Head",         eInventorySlot::InvSlot_Head },
	{ "Left Ear",     eInventorySlot::InvSlot_LeftEar },
	{ "Left Finger",  eInventorySlot::InvSlot_LeftFingers },
	{ "Left Ring",    eInventorySlot::InvSlot_LeftFingers },
	{ "Left Wrist",   eInventorySlot::InvSlot_LeftWrist },
	{ "Legs",         eInventorySlot::InvSlot_Legs },
	{ "Neck",         eInventorySlot::InvSlot_Neck },
	{ "Powersource",  eInventorySlot::InvSlot_PowerSource },
	{ "Primary",      eInventorySlot::InvSlot_Primary },
	{ "Mainhand",     eInventorySlot::InvSlot_Primary },
	{ "Range",        eInventorySlot::InvSlot_Range },
	{ "Right Ear",    eInventorySlot::InvSlot_RightEar },
	{ "Right Finger", eInventorySlot::InvSlot_RightFingers },
	{ "Right Ring",   eInventorySlot::InvSlot_RightFingers },
	{ "Right Wrist",  eInventorySlot::InvSlot_RightWrist },
	{ "Secondary",    eInventorySlot::InvSlot_Secondary },
	{ "Offhand",      eInventorySlot::InvSlot_Secondary },
	{ "Shoulders",    eInventorySlot::InvSlot_Shoulders },
	{ "Waist",        eInventorySlot::InvSlot_Waist },
};

void StatusCmd(SPAWNINFO* pChar, char* szLine)
{
	std::string outputcmd = ConnectedToReportOutput();
	std::string stringBuffer;
	if (!bConnectedToDannet && !bConnectedToEQBC) return;

	bool classPlugin = false; // Only true if there is a class plugin for this class, and the plugin was loaded.
	bool notLoaded = false; // Would only be true if one of the classes in the switch has a plugin, but it's not loaded

	// Get our Parameters
	char Arg[MAX_STRING] = { 0 };
	char Arg2[MAX_STRING] = { 0 };
	char Arg3[MAX_STRING] = { 0 };
	char NextArg[MAX_STRING] = { 0 };
	// TODO:: refactor parseboolarg to not need arg, arg2, and arg3 passed
	GetArg(Arg, szLine, 1);
	GetArg(Arg2, szLine, 2);
	GetArg(Arg3, szLine, 3);
	strcpy_s(NextArg, GetNextArg(szLine));
	PcClient* pCharInfo = GetCharInfo();
	PcProfile* pCharInfo2 = GetPcProfile();
	if (Arg[0] != '\0') {
		if (!_stricmp(Arg, "aa")) {
			stringBuffer += LabeledText("Available AA Points", pCharInfo2->AAPoints);
		}
		else if (!_stricmp(Arg, "achieve") || !_stricmp(Arg, "achievement")) {
			AchievementManager& achievemanager = AchievementManager::Instance();

			int id = 0;
			// if there is no second argument, we're going to give them the total completed achievement score
			if (NextArg[0] == '\0') {
				stringBuffer += LabeledText("Score", achievemanager.completedAchievementScore);
				stringBuffer += GetColorCode('g', false) + " Points";
			}
			else {
				const Achievement* Achieve = nullptr;
				if (IsNumber(NextArg)) {
					id = atoi(NextArg);
					Achieve = GetAchievementById(id);
				}
				else {
					Achieve = GetAchievementByName(NextArg);
					if (Achieve) {
						id = Achieve->id;
					}
				}
				// get the achievement by the ID number just collected
				const eqlib::Achievement* AchieveByID = GetAchievementById(id);

				if (AchieveByID != nullptr) {
					// is this achievement complete?
					const bool bComplete = IsAchievementComplete(AchieveByID);
					stringBuffer += LabeledText("Achieve", AchieveByID->name.c_str());
					stringBuffer += LabeledText(" Status", (bComplete ? " Completed" : GetColorCode('r', false) + " Incomplete"));

					// if the achievement is not complete, output what we're missing from it.
					if (!bComplete) {
						// Get incomplete components
						// we need to get achievementindex by ID
						int achievementIndex = achievemanager.GetAchievementIndexById(id);
						// we need to get the individual achievement/component info
						const SingleAchievementAndComponentsInfo* info = achievemanager.GetAchievementClientInfoByIndex(achievementIndex);

						if (info) {
							// we want to go through these achievements and check their completion status
							for (int i = 0; i < Achieve->componentsByType[AchievementComponentCompletion].GetCount(); i++) {
								const AchievementComponent& component = Achieve->componentsByType[AchievementComponentCompletion][i];
								// if it is not set, then we are missing it.
								if (!info->IsComponentComplete(AchievementComponentCompletion, i)) {
									stringBuffer += LabeledText(" Missing", component.description.c_str());
								}
							}
						}
					}
				}
				else {
					WriteChatf("\ao[MQ2Status] \arPlease provide a valid Achievement by name or ID to search for.");
					WriteChatf("\ao[MQ2Status] \arExample: \agNorrathian Explorer\ar or \ag100000050\ar.");
					WriteChatf("\ao[MQ2Status] \arKeep in mind achievements, by name will report the first achievement it finds with that name.");
				}
			}
		}
		else if (!_stricmp(Arg, "aaxp")) {
			stringBuffer += LabeledText("Assigned AA", pCharInfo2->AAPointsAssigned[0]);
			stringBuffer += LabeledText(" Spent AA", pCharInfo2->AAPointsSpent);
			stringBuffer += LabeledText(" AAXP", pCharInfo->AAExp * 0.001);
			stringBuffer += LabeledText(" Banked AA", pCharInfo2->AAPoints);
		}
		else if (!_stricmp(Arg, "bagspace")) {
			stringBuffer += LabeledText("Bagspace", GetFreeInventory(1));
		}
		else if (!_stricmp(Arg, "campfire")) {
			if (pLocalPlayer && pLocalPlayer->Campfire) {
				std::string cfStatus;
				std::string cfInfo;
				std::string cfTimeRemainHMS;
				std::string cfZoneLongName;
				if (int cfTimeRemain = pLocalPlayer->CampfireTimestamp - GetFastTime()) {
					int Hrs = ((cfTimeRemain / 60) / 60);
					std::string sHrs = std::to_string(Hrs);
					int Mins = ((cfTimeRemain / 60) - (Hrs * 60));
					std::string sMins = std::to_string(Mins);
					int Secs = ((cfTimeRemain)-((Mins + (Hrs * 60)) * 60));
					std::string sSecs = std::to_string(Secs);
					cfTimeRemainHMS += sHrs + ":" + sMins + ":" + sSecs;
					if (int ZoneID = pWorldData->GetZoneBaseId(pLocalPlayer->CampfireZoneID)) {
						if (ZoneID < MAX_ZONES && pWorldData) {
							if (EQZoneInfo* pZoneID = ((EQWorldData*)pWorldData)->ZoneArray[ZoneID]) {
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
			}
			else {
				stringBuffer += GetColorCode('r', false) + " " + "We do not appear to have a campfire in a usable location!" + GetColorCode('w', false);
			}
		}
		else if (!_stricmp(Arg, "collected") || !_stricmp(Arg, "collection")) {
			if (NextArg[0] == '\0') {
				WriteChatf("\ao[MQ2Status] \arPlease provide a valid \agCollection Item\ag to search for.\aw");
				WriteChatf("\ao[MQ2Status] \arExamples: \agKromzek Bracer\ar, \agLucky Clover\ar, \agAir-Infused Opal\ar, etc.\aw");
			}
			else {
				// start an achievement manager
				AchievementManager& achievemanager = AchievementManager::Instance();
				// We need this bool so we can output if there was invalid component search
				bool bFoundComponent = false;
				// we need to check all the categories for "Collections"
				// most of them are just ##09 for collections, but there are at least 2 that are not, so we should check everything
				for (const AchievementCategory& achievecat : achievemanager.categories) {
					// if we found a category with collections in the name
					if (string_equals(achievecat.name, "Collections")) {
						// We need to check the children of this category
						for (int i = 0; i < achievecat.GetAchievementCount(); ++i) {
							int id = achievecat.GetAchievementId(i);
							// we need to get achievementindex by ID
							int achievementIndex = achievemanager.GetAchievementIndexById(id);
							// we need to get the actual achievement by the index we just got
							const Achievement* Achieve = achievemanager.GetAchievementByIndex(achievementIndex);
							// we need to get the individual achievement/component info
							const SingleAchievementAndComponentsInfo* info = achievemanager.GetAchievementClientInfoByIndex(achievementIndex);

							if (info) {
								// we want to go through these achievements and check their completion status
								for (int i = 0; i < Achieve->componentsByType[AchievementComponentCompletion].GetCount(); i++) {
									const AchievementComponent& component = Achieve->componentsByType[AchievementComponentCompletion][i];
									if (ci_equals(NextArg, component.description)) {
										bFoundComponent = true;
										// if it is not set, then we are missing it.
										if (!info->IsComponentComplete(AchievementComponentCompletion, i)) {
											stringBuffer += LabeledText(component.description.c_str(), "\arMissing\ax");
										}
										else {
											stringBuffer += LabeledText(component.description.c_str(), "Collected");
										}
									}
								}
							}
						}
					}
				}

				// if we didn't find what we're checking for in any achievement, output it was invalid
				if (!bFoundComponent) {
					WriteChatf("\ao[MQ2Status] \arNo such collection item: \ay%s\ax.", NextArg);
					WriteChatf("\ao[MQ2Status] \arPlease provide a valid \agCollection Item\ar to search for.");
					WriteChatf("\ao[MQ2Status] \arExamples: \agKromzek Bracer\ar, \agLucky Clover\ar, \agAir-Infused Opal\ar, etc.");
				}
			}
		}
		else if (!_stricmp(Arg, "currency")) {
			if (NextArg[0] == '\0') { // if an Argument after currency wasn't made, we need to ask for one
				WriteChatf("\ao[MQ2Status] \arPlease provide a valid Currency Name to search for.\aw");
			}
			else { // We need to lowercase and be able to do a "find" in case someone puts an "s" on a currency
				std::string tempArg = NextArg; // convert our arg to string for transform
				std::transform(tempArg.begin(), tempArg.end(), tempArg.begin(), tolower); // lowercase
#if !defined (ROF2EMU)
				if (tempArg.find("loyalty") == '\0')
					stringBuffer += LabeledText("Loyalty Tokens", pCharInfo->LoyaltyRewardBalance); // Using LoyaltyRewardBalance instead of AltCurrency since we can access directly
				else if (tempArg.find("dbc") == '\0' || tempArg.find("daybreak") == '\0') { // DayBreakCurrency
					if (CSidlScreenWnd* MarketWnd = (CSidlScreenWnd*)FindMQ2Window("MarketPlaceWnd")) {
						if (CXWnd* Funds = MarketWnd->GetChildItem("MKPW_AvailableFundsUpper")) {
							if (Funds) {
								stringBuffer += LabeledText("Daybreak Cash", Funds->GetWindowText().c_str());
							}
						}
					}
					else
						stringBuffer += LabeledText("Daybreak Cash", "Unable to access");
				}
				else {
#endif !defined (ROF2EMU)
					int altCurrency = GetCurrencyIDByName(NextArg);
					if (altCurrency != -1)
						stringBuffer += LabeledText(tempArg, pPlayerPointManager->GetAltCurrency(altCurrency));
					else {
						stringBuffer += LabeledText(tempArg, "Is not a valid currency");
					}
#if !defined (ROF2EMU)
				}
#endif !defined (ROF2EMU)
			}
		}
		else if (!_stricmp(Arg, "evolve") || !_stricmp(Arg, "evolving")) {
			if (NextArg[0] == '\0') {
				WriteChatf("\ao[MQ2Status] \arPlease provide a valid Item to check your evolving status on\aw");
				WriteChatf("\ao[MQ2Status] \arExamples: Threadbare Weighted Tabard, Djarn's Tarnished Amethyst Ring, Wrathful Harasser's Earring of Rallos Zek, etc.\aw");
			}
			else {
				if (ItemClient* myItem = FindItemByName(NextArg)) {
					if (IsEvolvingItem(myItem)) {
						stringBuffer += LabeledText("Item", myItem->GetName());
						stringBuffer += LabeledText(" Level", myItem->pEvolutionData->EvolvingCurrentLevel);
						stringBuffer += LabeledText(" Max", myItem->pEvolutionData->EvolvingMaxLevel);
						stringBuffer += LabeledText(" Pct", myItem->pEvolutionData->EvolvingExpPct);
					}
					else {
						WriteChatf("\ao[MQ2Status] \ag%s \aris not an evolving item. Please provide a valid Item to check your evolving status on\aw", myItem->GetName());
					}
				}
				else {
					WriteChatf("\ao[MQ2Status] \arPlease provide a valid Item to check your evolving status on\aw");
					WriteChatf("\ao[MQ2Status] \arExamples: Threadbare Weighted Tabard, Djarn's Tarnished Amethyst Ring, Wrathful Harasser's Earring of Rallos Zek, etc.\aw");
				}
			}
		}
		else if (!_stricmp(Arg, "fellow") || !_stricmp(Arg, "fellowship")) { // We only have this WriteChatf and not reporting to eqbc/dannet
			SFellowship& fellowship = pLocalPlayer->Fellowship;
			if (fellowship.Members > 0) {
				WriteChatf("FS MoTD: \ag%s\aw", fellowship.MotD);
				WriteChatf("FS Leader is: \ag%s\aw , We have: \ay%lu\aw members", fellowship.FellowshipMember[0].Name, fellowship.Members);
				for (int i = 0; i < fellowship.Members; i++) {
					SFellowshipMember& thisMember = fellowship.FellowshipMember[i];
					WriteChatf("\ag%s\aw - \ay%lu\aw - \ap%s\aw ", thisMember.Name, thisMember.Level, GetClassDesc(thisMember.Class));
				}
			} else {
				WriteChatf("\arIt does not appear we are in a fellowship.\aw");
			}
		}
		else if (!_stricmp(Arg, "gear")) {
			bool bFound = false;
			size_t size = strlen(NextArg);
			if (size) {
				for (const auto& x : mEquipInvSlotName) {
					if (ci_equals(x.first, NextArg)) {
						// did we find a valid gear slot?
						bFound = true;
						if (ItemClient* item = FindItemBySlot(x.second)) {
							// itemtagsize is 512 in FormatItemLink, so we need this buffer that size as well
							char buffer[512] = {};
							FormatItemLink(buffer, 512, item);
							stringBuffer += LabeledText(x.first, buffer);
						}
						else {
							stringBuffer += LabeledText(x.first, " is empty.");
						}
					}
				}
			}

			// this bool check also covers if !strlen as found would be false
			if (!bFound) {
				if (size)
					WriteChatf("\ao[MQ2Status] \ag%s\ar is invalid.", NextArg);
				WriteChatf("\ao[MQ2Status] \arPlease provide a valid gear slot.");
				WriteChatf("\ao[MQ2Status] \arExamples: primary, secondary, left ring, etc.\aw");
			}
		}
		else if (!_stricmp(Arg, "hunger") || !_stricmp(Arg, "thirst")) {
			stringBuffer += LabeledText("Hunger", pCharInfo2->hungerlevel);
			stringBuffer += LabeledText(" Food", ItemCountByType(ItemClass_Food));
			stringBuffer += LabeledText(" Thirst", pCharInfo2->thirstlevel);
			stringBuffer += LabeledText(" Drink", ItemCountByType(ItemClass_Drink));
		}
		else if (!_stricmp(Arg, "help")) {
			WriteChatf("Welcome to MQ2Status");
			WriteChatf("By \aoChatWithThisName\aw & \agSic\aw Exclusively for \arRedGuides\aw.");
			WriteChatf("\agValid Status options are:\aw");
			WriteChatf("\ao/status will output to eqbc/dannet: If we have a CWTN Class Plugin loaded, if we have a macro, if our macro is kiss - it will say what our role is, if we are paused, if we are hidden, and if we have a merc that is alive.");
			WriteChatf("\ao/status \agaa\aw: Reports how many \"banked\" AA points you have.");
			WriteChatf("\ao/status \agaaxp\aw: Reports our Spent AA, our AAXP %%, and our Banked AA.");
			WriteChatf("\ao/status \agachievement\aw: Reports the status of an achievement by name, or id.");
			WriteChatf("\ao/status \agbagspace\aw: Reports how many open bagspaces you have.");
			WriteChatf("\ao/status \agcampfire\aw: Reports campfire information including Active, Duration, and Zone.");
			WriteChatf("\ao/status \agcollection\aw: Reports if we have collected an item by name.");
			WriteChatf("\ao/status \agcurrency\aw: Reports how many of an alt currency you have.");
			WriteChatf("\ao/status \agevolve / evolving\aw: Reports the evolution status of an item by name.");
			WriteChatf("\ao/status \agfellowship\aw: This returns to your mq2window (does not eqbc/dannet) information on your fellowship");
			WriteChatf("\ao/status \aghunger / thirst\aw: Reports your hunger, how many food items you have, your thirst, and how many drink items you have.");
			WriteChatf("\ao/status \aggtribute\aw: or \agguildtribute\aw: Displays if your current Guild Tribute Status is On or Off and the current Guild Favor.");
			WriteChatf("\ao/status \aginvis\aw: Reports our Invis and IVU status, so we can check we are \"Double Invis\".");
			WriteChatf("\ao/status \agitem\aw \ayitem name\aw: reports how many \ayitem name\aw you have in your inventory.");
			WriteChatf("\ao/status \agitembank\aw \ayitem name\aw: reports how many \ayitem name\aw you have in your bank.");
			WriteChatf("\ao/status \agitemall\aw \ayitem name\aw: reports how many \ayitem name\aw you have in your bank + inventory combined.");
			WriteChatf("\ao/status \agitem \ap/\ag itemall \ap/\ag itembank \awsearching can also use \ayID #\aw. \arExample: \ay\"/status itembank id 10037\"");
			WriteChatf("\ao/status \agkrono\aw: Reports how many krono we have.");
			WriteChatf("\ao/status \aglogin\aw: Reports your login account name.");
			WriteChatf("\ao/status \agmerc\aw: Reports mercenary information including class, and role.");
			WriteChatf("\ao/status \agmacro\aw: Reports what macro you currently have running.");
			WriteChatf("\ao/status \agmoney \aw or \agplat\aw: Reports how much plat you have.");
			WriteChatf("\ao/status \agparcel\aw: Reports our \"Parcel\" status.");
			WriteChatf("\ao/status \agquest\aw or \agtask\aw \ayQuest name\aw: Reports if you have a quest/task matching \ayQuest name\aw.");
			WriteChatf("\ao/status \agqueststep\aw or \agtaskstep\aw \ayQuest name\aw: Reports what step you are on if you have a quest/task matching \ayQuest name\aw.");
			WriteChatf("\ao/status \agshow\aw: Allows toggling on/off of the CWTN Class Plugins to be visible during /status.");
			WriteChatf("\ao/status \agskill\aw \ayskill name\aw: reports out your current skill value for \ay skill name\aw.");
			WriteChatf("\ao/status \agstat\aw \ayoption\aw: reports the following options: Hdex, HStr, HSta, HInt, HAgi, HWis, HCha, HPS, Mana, Endurance, Luck, Weight.");
			WriteChatf("\ao/status \agspell \ayoption\aw: takes a spell by name, or level, and reports the spells you have that match. Useful to see what ranks of spells you have.");
#if !defined(ROF2EMU)
			WriteChatf("\ao/status \agsub\aw: Reports our subscription level, and if we are All Access, how many days are left.");
#endif
			WriteChatf("\ao/status \agtribute\aw: Displays if your current Tribute Status is On or Off and the current Favor");
			WriteChatf("\ao/status \agxp\aw: Reports our level, Current XP %%, Banked AA, and our AAXP %%.");
			WriteChatf("\ao/status \agzone\aw: Reports what zone we are in.");
		}
		// We don't appear to have a way to access the information directly so I'm accessing the tribute window
		else if (!_stricmp(Arg, "gtribute") || !_stricmp(Arg, "guildtribute")) {
			if (CXWnd* TributeBenefitWnd = FindMQ2Window("TributeBenefitWnd")) {
				if (CLabelWnd* GuildTributeStatus = (CLabelWnd*)TributeBenefitWnd->GetChildItem("TBWG_ActivateButton")) {
					std::string strGuildTributeStatus = GuildTributeStatus->GetWindowText().c_str();
					if (!strGuildTributeStatus.empty()) {
						stringBuffer += LabeledText("Guild Tribute Status", !ci_equals(strGuildTributeStatus, "Deactivate") ? "On" : "Off");
					}
				}

				if (CLabelWnd* GuildTributePoints = (CLabelWnd*)TributeBenefitWnd->GetChildItem("TBWG_GuildPoolLabel")) {
					std::string strGuildTributePoints = GuildTributePoints->GetWindowText().c_str();
					if (!strGuildTributePoints.empty()) {
						stringBuffer += LabeledText(" Guild Favor", strGuildTributePoints);
					}

				}
			}
		}
		else if (!_stricmp(Arg, "invis")) {
			if (IHaveSpa(eEQSPA::SPA_INVISIBILITY) || IHaveSpa(eEQSPA::SPA_IMPROVED_INVIS)) {
				stringBuffer += GetColorCode('g', false) + "INVIS" + GetColorCode('x', false) + "::";
			}
			else {
				stringBuffer += GetColorCode('r', false) + "INVIS" + GetColorCode('x', false) + "::";
			}

			if (IHaveSpa(eEQSPA::SPA_INVIS_VS_UNDEAD) || IHaveSpa(eEQSPA::SPA_IMPROVED_INVIS_UNDEAD)) {
				stringBuffer += GetColorCode('g', false) + "IVU";
			}
			else {
				stringBuffer += GetColorCode('r', false) + "IVU";
			}
		}
		else if (!_stricmp(Arg, "item")) {
			if (NextArg[0] == '\0') {
				WriteChatf("\ao[MQ2Status] \arPlease provide a valid Item to search for\aw");
				WriteChatf("\ao[MQ2Status] \arExamples: Bone Chips, Diamond, Blue Diamond, etc.\aw");
				WriteChatf("\ao[MQ2Status] \arOr ID using the id tag. Example: \ay\"/status item id 10037\"\aw.");
			}
			else if (ci_equals(NextArg, "id")) {
				stringBuffer += ItemCountStatusByID(szLine, eItemCountStatusType::Item);
			}
			else {
				stringBuffer += LabeledText(NextArg, FindItemCountByName(NextArg));
			}
		}
		else if (!_stricmp(Arg, "itemall")) {
			if (NextArg[0] == '\0') {
				WriteChatf("\ao[MQ2Status] \arPlease provide a valid Item to search for\aw");
				WriteChatf("\ao[MQ2Status] \arExamples: Bone Chips, Diamond, Blue Diamond, etc.\aw");
				WriteChatf("\ao[MQ2Status] \arOr ID using the id tag. Example: \ay\"/status itemall id 10037\"\aw.");
			}
			else if (ci_equals(NextArg, "id")) {
				stringBuffer += ItemCountStatusByID(szLine, eItemCountStatusType::ItemAll);
			}
			else {
				stringBuffer += LabeledText(NextArg, FindItemCountByName(NextArg) + FindBankItemCountByName(NextArg, 0)); // FindBankItemCountByName requires bExact
			}
		}
		else if (!_stricmp(Arg, "itembank")) {
			if (NextArg[0] == '\0') {
				WriteChatf("\ao[MQ2Status] \arPlease provide a valid Item to search for\aw");
				WriteChatf("\ao[MQ2Status] \arExamples: Bone Chips, Diamond, Blue Diamond, etc.\aw");
				WriteChatf("\ao[MQ2Status] \arOr ID using the id tag. Example: \ay\"/status itembank id 10037\"\aw.");
			}
			else if (ci_equals(NextArg, "id")) {
				stringBuffer += ItemCountStatusByID(szLine, eItemCountStatusType::ItemBank);
			}
			else {
				stringBuffer += LabeledText(NextArg, FindBankItemCountByName(NextArg, 0)); // FindBankItemCountByName requires bExact
			}
		}
		else if (!_stricmp(Arg, "krono")) {
			stringBuffer += LabeledText("Krono", pCharInfo->Krono);
		}
		else if (!_stricmp(Arg, "login")) {
			stringBuffer += LabeledText("Login Name", GetLoginName());
		}
		else if (!_stricmp(Arg, "macro")) {

			if (gMacroStack && strlen(gszMacroName)) {
				stringBuffer += LabeledText(" Macro", gszMacroName);
			}
			else {
				stringBuffer += LabeledText(" Macro", "NULL");
			}
		}
		else if (!_stricmp(Arg, "merc")) {
			if (pMercManager && pMercManager->HasMercenary()) {
				switch (pMercManager->GetMercenaryState())
				{
					case MercenaryState_Dead:
						stringBuffer += GetColorCode('o', false) + "Class: " + GetColorCode('r', false) + "Dead! " + GetColorCode('w', false);
						break;
					case MercenaryState_Suspended:
						stringBuffer += GetColorCode('o', false) + "Mercenary State: " + GetColorCode('r', false) + "Suspended! " + GetColorCode('w', false);
						break;
					case MercenaryState_Active:
						stringBuffer += GetColorCode('o', false) + "Mercenary State: " + GetColorCode('g', false) + "Alive! " + GetColorCode('w', false);

						{
							int mercStance = pMercManager->currentMercenary.GetCurrentStanceId();
							if (PlayerClient* myMerc = GetSpawnByID(pMercManager->mercenarySpawnId)) {
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
			else {
				stringBuffer += GetColorCode('r', false) + "It does not appear we have a merc." + GetColorCode('w', false);
			}
		}
		else if (!_stricmp(Arg, "money") || !_stricmp(Arg, "plat")) {
			char szmyPlat[MAX_STRING] = "";
			_ltoa_s(pCharInfo2->Plat, szmyPlat, 10);
			PutCommas(szmyPlat);
			stringBuffer += LabeledText("Plat", szmyPlat);
		}
		else if (!_stricmp(Arg, "parcel")) {
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
		}
		else if (!_stricmp(Arg, "powersource")) {
			if (auto powersource = GetPcProfile()->GetInventorySlot(InvSlot_PowerSource)) {
				stringBuffer += LabeledText(powersource->GetName(), (powersource->Power * 100) / powersource->GetItemDefinition()->MaxPower);
				stringBuffer += GetColorCode('o', true) + "%";
			}
			else {
				stringBuffer += GetColorCode('r', false) + "No Powersource Equipped.";
			}
		}
		else if (!_stricmp(Arg, "quest") || !_stricmp(Arg, "task")) {
			if (NextArg[0] == '\0') { // if an Argument after quest/task wasn't made, we need to ask for one
				WriteChatf("\ao[MQ2Status] \arPlease provide a valid Quest/Task Name to search for.\aw");
			}
			else {
				stringBuffer += GetColorCode('o', false);
				stringBuffer += "Quest/Task \"";
				stringBuffer += GetColorCode('t', true);
				stringBuffer += NextArg;
				stringBuffer += GetColorCode('o', false);
				stringBuffer += "\": ";
				char tempTask[MAX_STRING] = "";
				sprintf_s(tempTask, "${Task[%s]}", NextArg);
				ParseMacroData(tempTask, MAX_STRING);
				if (_stricmp(tempTask, "NULL")) {
					stringBuffer += GetColorCode('g', false);
				}
				else {
					stringBuffer += GetColorCode('r', false);
				}
				stringBuffer += tempTask;
			}
		}
		else if (!_stricmp(Arg, "queststep") || !_stricmp(Arg, "taskstep")) {
			if (NextArg[0] == '\0') { // if an Argument after quest/task wasn't made, we need to ask for one
				WriteChatf("\ao[MQ2Status] \arPlease provide a valid Quest/Task Name to search for.\aw");
			}
			else {
				char buffer[MAX_STRING] = {};
				bool bFoundMatch = false;

				for (int i = 0; i < MAX_SHARED_TASK_ENTRIES; ++i) {
					const auto& task = pTaskManager->SharedTaskEntries[i];
					auto taskStatus = pTaskManager->GetTaskStatus(pLocalPC, i, task.TaskSystem);
					if (ci_find_substr(task.TaskTitle, NextArg) != -1) {
						for (int j = 0; j < MAX_TASK_ELEMENTS; ++j) {
							int iCurrCount = taskStatus->CurrentCounts[j];
							int iReqCount = task.Elements[j].RequiredCount;
							if (iCurrCount < iReqCount && !task.Elements[j].bOptional) {
								pTaskManager->GetElementDescription(&task.Elements[j], buffer);
								if (buffer[0]) {
									stringBuffer += LabeledText(task.TaskTitle, buffer);
									stringBuffer += LabeledText(" Have", iCurrCount);
									stringBuffer += LabeledText(" Need", iReqCount);
									bFoundMatch = true;
									// We only want to report the first step we find.
									//bBreak = true;
									break;
								}
							}
						}
					}
				}

				if (!bFoundMatch) {
					for (int i = 0; i < MAX_QUEST_ENTRIES; ++i) {
						const auto& task = pTaskManager->QuestEntries[i];
						auto taskStatus = pTaskManager->GetTaskStatus(pLocalPC, i, task.TaskSystem);
						if (ci_find_substr(task.TaskTitle, NextArg) != -1) {
							for (int j = 0; j < MAX_TASK_ELEMENTS; ++j) {
								if (taskStatus->CurrentCounts[j] < task.Elements[j].RequiredCount && !task.Elements[j].bOptional) {
									pTaskManager->GetElementDescription(&task.Elements[j], buffer);
									if (buffer[0]) {
										int iCurrCount = taskStatus->CurrentCounts[j];
										int iReqCount = task.Elements[j].RequiredCount;
										stringBuffer += LabeledText(task.TaskTitle, buffer);
										stringBuffer += LabeledText(" Curr", iCurrCount);
										stringBuffer += LabeledText(" Rec", iReqCount);
										bFoundMatch = true;
										// We only want to report the first step we find.
										break;
									}
								}
							}
						}
					}
				}

				if (!bFoundMatch)
					stringBuffer += LabeledText(NextArg, "NULL");
			}
		}
		else if (!_stricmp(Arg, "show")) {
			if (!_stricmp(NextArg, "plugin")) {
				ParseBoolArg(Arg, NextArg, Arg3, &bShowPlugin, "ShowPlugin");
			}
			else if (!_stricmp(NextArg, "warrior")) {
				ParseBoolArg(Arg, NextArg, Arg3, &bShowWarrior, "ShowPlugin");
			}
			else if (!_stricmp(NextArg, "cleric")) {
				ParseBoolArg(Arg, NextArg, Arg3, &bShowCleric, "ShowPlugin");
			}
			else if (!_stricmp(NextArg, "paladin")) {
				ParseBoolArg(Arg, NextArg, Arg3, &bShowPaladin, "ShowPlugin");
			}
			else if (!_stricmp(NextArg, "ranger")) {
				ParseBoolArg(Arg, NextArg, Arg3, &bShowRanger, "ShowPlugin");
			}
			else if (!_stricmp(NextArg, "shadowknight")) {
				ParseBoolArg(Arg, NextArg, Arg3, &bShowShadowknight, "ShowPlugin");
			}
			else if (!_stricmp(NextArg, "druid")) {
				ParseBoolArg(Arg, NextArg, Arg3, &bShowDruid, "ShowPlugin");
			}
			else if (!_stricmp(NextArg, "monk")) {
				ParseBoolArg(Arg, NextArg, Arg3, &bShowMonk, "ShowPlugin");
			}
			else if (!_stricmp(NextArg, "bard")) {
				ParseBoolArg(Arg, NextArg, Arg3, &bShowBard, "ShowPlugin");
			}
			else if (!_stricmp(NextArg, "rogue")) {
				ParseBoolArg(Arg, NextArg, Arg3, &bShowRogue, "ShowPlugin");
			}
			else if (!_stricmp(NextArg, "shaman")) {
				ParseBoolArg(Arg, NextArg, Arg3, &bShowShaman, "ShowPlugin");
			}
			else if (!_stricmp(NextArg, "necromancer")) {
				ParseBoolArg(Arg, NextArg, Arg3, &bShowNecromancer, "ShowPlugin");
			}
			else if (!_stricmp(NextArg, "wizard")) {
				ParseBoolArg(Arg, NextArg, Arg3, &bShowWizard, "ShowPlugin");
			}
			else if (!_stricmp(NextArg, "magician")) {
				ParseBoolArg(Arg, NextArg, Arg3, &bShowMage, "ShowPlugin");
			}
			else if (!_stricmp(NextArg, "enchanter")) {
				ParseBoolArg(Arg, NextArg, Arg3, &bShowEnchanter, "ShowPlugin");
			}
			else if (!_stricmp(NextArg, "beastlord")) {
				ParseBoolArg(Arg, NextArg, Arg3, &bShowBeastlord, "ShowPlugin");
			}
			else if (!_stricmp(NextArg, "berserker")) {
				ParseBoolArg(Arg, NextArg, Arg3, &bShowBerserker, "ShowPlugin");
			}
			else {
				WriteChatf("\ao[MQ2Status] \arPlease provide a valid \agShow\aw option.\aw");
				WriteChatf("\ao[MQ2Status] \ayImportant!\aw \agShow Plugin Off\aw will hide ALL the plugins.");
				WriteChatf("\ao[MQ2Status] \ayImportant!\aw To display an individual plugin, you will need \agShow Plugin On\aw as well as the individual class plugin set to on.");
				WriteChatf("\ao[MQ2Status] \arExamples: \agShow\ay Plugin, \agShow\ay Warrior, \agShow\ay Cleric, \agShow\ay Paladin, \agShow\ay Ranger, \agShow\ay Shadowknight, \agShow\ay Druid, \agShow\ay Monk, \agShow\ay Bard\aw");
				WriteChatf("\ao[MQ2Status] \arExamples: \agShow\ay Rogue, \agShow\ay Shaman, \agShow\ay Necromancer, \agShow\ay Wizard, \agShow\ay Magician, \agShow\ay Enchanter, \agShow\ay Beastlord, \agShow\ay Berserker\aw");
			}
		}
		else if (!_stricmp(Arg, "skill")) {
			bool bFoundMatch = false;
			if (NextArg[0] == '\0') {
				WriteChatf("\ao[MQ2Status] \arPlease provide a valid skill to search for.\aw");
				WriteChatf("\ao[MQ2Status] \arExamples: Baking, Fishing, Jewelry Making, etc.\aw");
			}
			else {
				for (int iSkillNum = 0; iSkillNum < NUM_SKILLS; iSkillNum++) {
					if (!_stricmp(NextArg, szSkills[iSkillNum])) {
						if (pCharInfo2->Skill[iSkillNum] >= 0) {
							stringBuffer += LabeledText(NextArg, GetAdjustedSkill(iSkillNum));
							bFoundMatch = true;
							break;
						}
					}
				}
			}

			if (!bFoundMatch) {
				WriteChatf("\ao[MQ2Status] \ay%s \arwas not found to be a valid skill.", NextArg);
				return;
			}

		}
		else if (!_stricmp(Arg, "spell")) {
			int iArg = GetIntFromString(NextArg, 0);
			int myclass = pLocalPlayer->GetClass();
			if (NextArg[0] == '\0' || iArg > pCharInfo->GetLevel()) {
				WriteChatf("\ao[MQ2Status] \arPlease provide a valid spell by name or level to search for.\aw");
			}
			else if (!IsNumber(NextArg)) {
				bool bFound = false;

				// spells
				for (int i = 0; i < NUM_BOOK_SLOTS; i++) {
					if (pCharInfo2->SpellBook[i] != -1) {
						if (EQ_Spell* thisSpell = GetSpellByID(pCharInfo2->SpellBook[i])) {
							if (ci_starts_with(thisSpell->Name, NextArg)) {
								bFound = true;
								if (thisSpell->ClassLevel[myclass] > 70)
									stringBuffer += fmt::format(" {}({}) ", GetColorCode('r', false), GetSpellUpgradeType(thisSpell->ClassLevel[myclass]));
								stringBuffer += LabeledText(thisSpell->Name, static_cast<int>(thisSpell->ClassLevel[myclass]));
								// unfortunately we CAN'T break early here, since there are spells with the same damn name.
							}
						}
					}
				}

				// discs
				for (int j = 0; j < NUM_COMBAT_ABILITIES; j++) {
					if (pCombatSkillsSelectWnd->ShouldDisplayThisSkill(j)) {
						if (EQ_Spell* thisSpell = GetSpellByID(pPCData->GetCombatAbility(j))) {
							if (ci_starts_with(thisSpell->Name, NextArg)) {
								bFound = true;
								if (thisSpell->ClassLevel[myclass] > 70)
									stringBuffer += fmt::format(" {}({}) ", GetColorCode('r', false), GetSpellUpgradeType(thisSpell->ClassLevel[myclass]));
								stringBuffer += LabeledText(thisSpell->Name, static_cast<int>(thisSpell->ClassLevel[myclass]));
								// unfortunately we CAN'T break early here, since there are spells with the same damn name.
							}
						}
					}
				}

				if (!bFound) {
					WriteChatf("\ao[MQ2Status] \arWe did not find a matching spell/disc for %s.\aw", NextArg);
					return;
				}
			}
			else {
				stringBuffer += LabeledText("Level", iArg);
				int count = 0;

				// spells
				bool bCheckSpellUpgradeType = true;
				for (int i = 0; i < NUM_BOOK_SLOTS; i++) {
					if (pCharInfo2->SpellBook[i] != -1) {
						if (EQ_Spell* thisSpell = GetSpellByID(pCharInfo2->SpellBook[i])) {
							if (thisSpell->ClassLevel[myclass] == iArg) {
								// Rank II/III spells start at lvl 71
								if (bCheckSpellUpgradeType && thisSpell->ClassLevel[myclass] > 70) {
									stringBuffer += fmt::format(" {}({}) ", GetColorCode('r', false), GetSpellUpgradeType(thisSpell->ClassLevel[myclass]));
									// we only want to putput that the (Upgrade Type) once
									bCheckSpellUpgradeType = false;
								}

								// we want to alternate green/orange for readability, using count will always alternate
								// we can't use the for loop "i" here due to if spells are out of "order" in the spellbook
								stringBuffer += fmt::format("{} {} ", GetColorCode('y', false), "--");
								if (count % 2 == 0) {
									stringBuffer += GetColorCode('g', false);
								}
								else {
									stringBuffer += GetColorCode('o', false);
								}
								// Cool Spell -- Cooler Spell Rk. III -- Suck Spell Rk. II
								stringBuffer += thisSpell->Name;
								count++;
							}
						}
					}
				}

				// discs
				bCheckSpellUpgradeType = true;
				for (int j = 0; j < NUM_COMBAT_ABILITIES; j++) {
					if (pCombatSkillsSelectWnd->ShouldDisplayThisSkill(j)) {
						if (EQ_Spell* thisSpell = GetSpellByID(pPCData->GetCombatAbility(j))) {
							if (thisSpell->ClassLevel[myclass] == iArg) {
								// Rank II/III spells start at lvl 71
								if (bCheckSpellUpgradeType && thisSpell->ClassLevel[myclass] > 70) {
									stringBuffer += fmt::format(" {}({}) ", GetColorCode('r', false), GetSpellUpgradeType(thisSpell->ClassLevel[myclass]));
									// we only want to putput that the (Upgrade Type) once
									bCheckSpellUpgradeType = false;
								}

								// we want to alternate green/orange for readability, using count will always alternate
								// we can't use the for loop "i" here due to if spells are out of "order" in the spellbook
								stringBuffer += fmt::format("{} {} ", GetColorCode('y', false) , "--");
								if (count % 2 == 0) {
									stringBuffer += GetColorCode('g', false);
								}
								else {
									stringBuffer += GetColorCode('o', false);
								}
								// Cool Spell -- Cooler Spell Rk. III -- Suck Spell Rk. II
								stringBuffer += thisSpell->Name;
								count++;
							}
						}
					}
				}
			}
		}
		else if (!_stricmp(Arg, "stat")) {
			if (Arg2[0] == '\0') {
				WriteChatf("\ao[MQ2Status] \arPlease provide a valid MQ2Status stat\aw");
				WriteChatf("\ao[MQ2Status] \aoThese are currently: \aghstr, hsta, hint, hwis, hagi, hdex, hcha, hps, mana, endurance, and weight.\aw");
			}
			else {
				if (ci_equals(Arg2, "crit")) {
					stringBuffer += LabeledText("Dot Crit", pLocalPC->TotalEffect(SPA_DOTCRIT));
					stringBuffer += LabeledText(" Spell Crit", pLocalPC->TotalEffect(SPA_SPELL_CRIT_CHANCE));
				}
				else if (!_stricmp(Arg2, "hstr")) {
					stringBuffer += LabeledText("HSTR", pCharInfo->HeroicSTRBonus);
				}
				else if (!_stricmp(Arg2, "hsta")) {
					stringBuffer += LabeledText("HSTA", pCharInfo->HeroicSTABonus);
				}
				else if (!_stricmp(Arg2, "hint")) {
					stringBuffer += LabeledText("HINT", pCharInfo->HeroicINTBonus);
				}
				else if (!_stricmp(Arg2, "hwis")) {
					stringBuffer += LabeledText("HWIS", pCharInfo->HeroicWISBonus);
				}
				else if (!_stricmp(Arg2, "hagi")) {
					stringBuffer += LabeledText("HAGI", pCharInfo->HeroicAGIBonus);
				}
				else if (!_stricmp(Arg2, "hdex")) {
					stringBuffer += LabeledText("HDEX", pCharInfo->HeroicDEXBonus);
				}
				else if (!_stricmp(Arg2, "hcha")) {
					stringBuffer += LabeledText("HCHA", pCharInfo->HeroicCHABonus);
				}
				else if (!_stricmp(Arg2, "hps")) {
					stringBuffer += LabeledText("Current HPs", pLocalPlayer->HPCurrent);
					stringBuffer += LabeledText(" Max HPs", pLocalPlayer->HPMax);
					stringBuffer += LabeledText(" Health Pct", PercentHealth(pLocalPlayer));
				}
#if HAS_LUCK_STAT
				else if (!_stricmp(Arg2, "luck")) {
					stringBuffer += LabeledText("Luck", pCharInfo->LCK);
				}
#endif
				else if (!_stricmp(Arg2, "mana")) {
					stringBuffer += LabeledText("Current Mana", pLocalPlayer->GetCurrentMana());
					stringBuffer += LabeledText(" Max Mana", pLocalPlayer->GetMaxMana());
					stringBuffer += LabeledText(" Mana Pct", PercentMana(pLocalPlayer));
				}
				else if (!_stricmp(Arg2, "endurance")) {
					stringBuffer += LabeledText("Current Endurance", pLocalPlayer->GetCurrentEndurance());
					stringBuffer += LabeledText(" Max Endurance", pLocalPlayer->GetMaxEndurance());
					stringBuffer += LabeledText(" Endurance Pct", PercentEndurance(pLocalPlayer));
				}
				else if (!_stricmp(Arg2, "weight")) {
					stringBuffer += LabeledText("Current Weight", pCharInfo->CurrWeight);
					stringBuffer += LabeledText(" Max Weight", pCharInfo->STR);
					stringBuffer += LabeledText(" Remaining", (long)(pCharInfo->STR - pCharInfo->CurrWeight));
				}
				else {
					WriteChatf("\ao[MQ2Status] \arThat was not a valid stat, \agplease use hstr, hsta, hint, hwis, hagi, hdex, hcha, hps, mana, endurance, or weight for this option!\aw");
				}
			}
		}
#if !defined(ROF2EMU) // Subscription doesn't exist in EMU'
		else if (!_stricmp(Arg, "sub") || !_stricmp(Arg, "subscription")) {
			switch (GetMembershipLevel())
			{
				case MembershipLevel::LifetimeAllAccess:
					stringBuffer += GetColorCode('o', false) + "Sub: " + GetColorCode('r', false) + "Lifetime.";
					break;
				case MembershipLevel::AllAccess:
					stringBuffer += GetColorCode('o', false) + "Sub: " + GetColorCode('g', false) + "All Access " + LabeledText("Days Remaining", pCharInfo->SubscriptionDays);
					break;
				case MembershipLevel::Silver:
					stringBuffer += GetColorCode('o', false) + "Sub: " + GetColorCode('r', false) + "Silver.";
					break;
				case MembershipLevel::Free:
					stringBuffer += GetColorCode('o', false) + "Sub: " + GetColorCode('r', false) + "Free.";
					break;
				default:
					break;
			}
		}
#endif
		else if (!_stricmp(Arg, "tribute")) {
			stringBuffer += LabeledText("Tribute Status", *pTributeActive ? "On" : "Off");
			char prettyFavor[21] = { 0 };
			sprintf_s(prettyFavor, "%lld", pCharInfo->CurrFavor);
			PrettifyNumber(prettyFavor, sizeof(prettyFavor));
			stringBuffer += LabeledText(" Favor", prettyFavor);
		}
		else if (!_stricmp(Arg, "xp")) {
			stringBuffer += LabeledText("Level", static_cast<int>(pLocalPlayer->Level));
			stringBuffer += LabeledText(" XP", pCharInfo->Exp / 1000.0f);
			stringBuffer += LabeledText(" Banked AA", pCharInfo2->AAPoints);
			stringBuffer += LabeledText(" AAXP", pCharInfo->AAExp * 0.001);
		}
		else if (!_stricmp(Arg, "zone")) {
			stringBuffer += LabeledText("Zone", GetFullZone(pCharInfo->zoneId));
		}
		else {
			WriteChatf("\ao[MQ2Status] \ap%s\ar is not a valid option. \ag /status help \aw for available options", Arg);
		}

		if (!stringBuffer.empty()) {
			stringBuffer = outputcmd + stringBuffer;
			EzCommand(&stringBuffer[0]);
		}
	}
	else {
#if !defined(ROF2EMU) // Subscription doesn't exist in EMU'
		if (GetMembershipLevel() == MembershipLevel::Silver) {
			stringBuffer += GetColorCode('o', false) + "Sub: " + GetColorCode('r', false) + "Silver ";
		}
		else if (GetMembershipLevel() == MembershipLevel::Free) {
			stringBuffer += GetColorCode('o', false) + "Sub: " + GetColorCode('r', false) + "Free ";
		}
#endif
		if (bShowPlugin) {
			switch (GetPcProfile()->Class) {
				case Bard:
					if (bShowBard) {
						if (IsPluginLoaded("MQ2Bard")) {
							classPlugin = true;
						}
						else {
							notLoaded = true;
						}
					}
					break;
			case Berserker:
				if (bShowBerserker) {
					if (IsPluginLoaded("MQ2BerZerker")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Cleric:
				if (bShowCleric) {
					if (IsPluginLoaded("MQ2Cleric")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Monk:
				if (bShowMonk) {
					if (IsPluginLoaded("MQ2Monk")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Beastlord:
				if (bShowBeastlord) {
					if (IsPluginLoaded("MQ2Bst")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Rogue:
				if (bShowRogue) {
					if (IsPluginLoaded("MQ2Rogue")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Warrior:
				if (bShowWarrior) {
					if (IsPluginLoaded("MQ2War")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Shadowknight:
				if (bShowShadowknight) {
					if (IsPluginLoaded("MQ2Eskay")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Shaman:
				if (bShowShaman) {
					if (IsPluginLoaded("MQ2Shaman")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Enchanter:
				if (bShowEnchanter) {
					if (IsPluginLoaded("MQ2Enchanter")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Necromancer:
				if (bShowNecromancer) {
					if (IsPluginLoaded("MQ2Necro")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Paladin:
				if (bShowPaladin) {
					if (IsPluginLoaded("MQ2Paladin")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Mage:
				if (bShowMage) {
					if (IsPluginLoaded("MQ2Mage")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Druid:
				if (bShowDruid) {
					if (IsPluginLoaded("MQ2Druid")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Ranger:
				if (bShowRanger) {
					if (IsPluginLoaded("MQ2Ranger")) {
						classPlugin = true;
					}
					else {
						notLoaded = true;
					}
				}
				break;
			case Wizard:
				if (bShowWizard) {
					if (IsPluginLoaded("MQ2Wizard")) {
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

				if (FindMQ2DataType("CWTN")) {
					stringBuffer += GetColorCode('o', false);
					// Possible way to do this without Parsing Macro Data?
					constexpr int MAX_CWTN_OUTPUT_BUFFER = 24;
					char theMode[MAX_CWTN_OUTPUT_BUFFER] = "${CWTN.Mode}"; // What mode are we in?
					char cwtnPaused[MAX_CWTN_OUTPUT_BUFFER] = "${CWTN.Paused}"; // is the plugin paused

					stringBuffer += "Mode: ";
					stringBuffer += GetColorCode('g', false);
					ParseMacroData(theMode, MAX_CWTN_OUTPUT_BUFFER);
					stringBuffer += theMode;
					stringBuffer += " ";

					ParseMacroData(cwtnPaused, MAX_CWTN_OUTPUT_BUFFER);
					if (!strcmp(cwtnPaused, "TRUE")) {
						stringBuffer += GetColorCode('r', false);
						stringBuffer += " ***PAUSED*** ";
					}
				}
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
			std::string red = GetColorCode('r', false);
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

			if (MQMacroBlockPtr pBlock = GetCurrentMacroBlock()) {
				if (pBlock->Paused) {
					stringBuffer += GetColorCode('r', false) + "***PAUSED*** " + GetColorCode('w', false);
				}

				// KissAssist has a "BackOff" that uses ${DPSPaused}
				// Users would like to know when this is on doing a /status
				if (IsDefined("DPSPaused")) {
					char dpsPaused[64] = "${DPSPaused}";
					ParseMacroData(dpsPaused, 64);
					if (atoi(dpsPaused) > 0) {
						stringBuffer += GetColorCode('r', false);
						stringBuffer += " ***BACKEDOFF!*** ";
						stringBuffer += GetColorCode('w', false);
					}
				}
			}
		}
		else {
			if (!classPlugin) {
				if (IsLuaScriptRunning("RGMercs")) {
					stringBuffer += GetColorCode('o', false) + "Lua: RGMercs " + GetColorCode('w', false);
				}
				stringBuffer += GetColorCode('o', false) + "Macro: " + GetColorCode('r', false) + "FALSE! " + GetColorCode('w', false);
			}
		}

		if (pMercManager) {
			if (pMercManager->HasMercenary()) {
				switch (pMercManager->currentMercenary.suspendedState)
				{
				case MercenaryState_Dead:
					//stringBuffer += GetColorCode('o', false) + "Mercenary State:" + GetColorCode('g', false) + " DEAD " + GetColorCode('w', false);
					break;
				case MercenaryState_Suspended:
					//stringBuffer += GetColorCode('o', false) + "Mercenary State:" + GetColorCode('g', false) + " SUSPENDED " + GetColorCode('w', false);
					break;
				case MercenaryState_Active:
					stringBuffer += GetColorCode('o', false) + " Mercenary State:" + GetColorCode('g', false) + " ALIVE! " + GetColorCode('w', false);
					break;
				default:
					break;
				}
			}
		}
			// Am I Invis?
		if (pCharInfo->pSpawn->HideMode) {
			stringBuffer += GetColorCode('o', false) + " Hidden:" + GetColorCode('w', false);
			if (IHaveSpa(eEQSPA::SPA_INVISIBILITY) || IHaveSpa(eEQSPA::SPA_IMPROVED_INVIS)) {
				stringBuffer += GetColorCode('g', false) + " INVIS" + GetColorCode('w', false);
			}
			if (IHaveSpa(eEQSPA::SPA_INVIS_VS_UNDEAD) || IHaveSpa(eEQSPA::SPA_IMPROVED_INVIS_UNDEAD)) {
				stringBuffer += GetColorCode('g', false) + " IVU" + GetColorCode('w', false);
			}
		}
		if (!stringBuffer.empty()) {
			stringBuffer = outputcmd + stringBuffer;
			EzCommand(&stringBuffer[0]);
		}
	}
}

bool IsDefined(char* szLine)
{
	return (FindMQ2DataVariable(szLine) != 0);
}

bool HaveAlias(const std::string& aliasName) {
	const std::string alias = GetPrivateProfileString("Aliases", aliasName, "None", gPathMQini);
	if (alias == "None") {
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
	//TODO: Verify / Update for EMU
	return ((float)pSpawn->HPCurrent / (float)pSpawn->HPMax) * 100.0f;
}

inline float PercentEndurance(SPAWNINFO* pSpawn)
{
	//TODO: Verify / Update for EMU
	return ((float)pSpawn->GetCurrentEndurance() / (float)pSpawn->GetMaxEndurance()) * 100.0f;
}

inline float PercentMana(SPAWNINFO* pSpawn)
{
	//TODO: Verify / Update for EMU
	if (pSpawn->GetMaxMana() <= 0) return 100.0f;
	return ((float)pSpawn->GetCurrentMana() / (float)pSpawn->GetMaxMana()) * 100.0f;
}

bool IHaveSpa(int spa)
{
	for (int i = 0; i < MAX_TOTAL_BUFFS; i++) {
		PSPELL pBuff = GetSpellByID(GetPcProfile()->GetEffect(i).SpellID);
		if (!pBuff)
			continue;
		if (IsSPAEffect(pBuff, spa))
			return true;
	}

	return false;
}

struct PluginCheckbox {
	const char* name;
	bool* value;
};

static const PluginCheckbox checkboxes[] = {
	{ "Bard", &bShowBard },
	{ "Beastlord", &bShowBeastlord },
	{ "Berserker", &bShowBerserker },
	{ "Cleric", &bShowCleric },
	{ "Druid", &bShowDruid },
	{ "Enchanter", &bShowEnchanter },
	{ "Magician", &bShowMage },
	{ "Monk", &bShowMonk },
	{ "Necro", &bShowNecromancer },
	{ "Paladin", &bShowPaladin },
	{ "Ranger", &bShowRanger },
	{ "Rogue", &bShowRogue },
	{ "Shadowknight", &bShowShadowknight },
	{ "Shaman", &bShowShaman },
	{ "Warrior", &bShowWarrior },
	{ "Wizard", &bShowWizard },
};

void MQ2StatusImGuiSettingsPanel()
{
	if (ImGui::Checkbox("Show Class Plugins", &bShowPlugin))
	{
		WritePrivateProfileBool("ShowPlugin", "Plugin", bShowPlugin, INIFileName);
	}
	ImGui::SameLine();
	mq::imgui::HelpMarker("Sets if you want to display CWTN Class Plugins in your /status results.");

	if (bShowPlugin) {
		ImGui::Indent();
		ImGui::Columns(2);
		for (const PluginCheckbox& cb : checkboxes)
		{
			if (ImGui::Checkbox(cb.name, cb.value))
			{
				WritePrivateProfileBool("ShowPlugin", cb.name, *cb.value, INIFileName);
			}
			ImGui::NextColumn();
		}
		ImGui::Columns(1);
	}
}

PLUGIN_API void InitializePlugin()
{
	if (HaveAlias("/status")) {
		WriteChatf("\ar[\a-tMQ2Status\ar]\ao:: \arIt appears you already have an Alias for \ap/status\ar please type \"\ay/alias /status delete\ar\" then reload this plugin.");
		EzCommand("/timed 10 /plugin MQ2Status Unload");
	}
	else {
		AddCommand("/status", StatusCmd, false, true, true);
	}

	DoINIThings();
	AddSettingsPanel("plugins/Status", MQ2StatusImGuiSettingsPanel);
}

PLUGIN_API void ShutdownPlugin()
{
	RemoveCommand("/status");
	RemoveSettingsPanel("plugins/Status");
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
		WriteChatf("\ao[MQ2Status] \awMQ2Status only works if mq2eqbc or mq2dannet is loaded and connected");
		WriteChatf("\ao[MQ2Status] \awIf you would like to use EQBC, please run /plugin mq2eqbc load and then /bccmd connect");
		WriteChatf("\ao[MQ2Status] \awOr if you would like to use Dannet, /plugin mq2dannet load");
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

std::string GetSpellUpgradeType(int level) {
	int iLastDigit = level % 10;
	switch (iLastDigit) {
		case 0:
		case 5:
			return "Glowing";
		case 1:
		case 6:
			return "Minor";
		case 2:
		case 7:
			return "Lesser";
		case 3:
		case 8:
			return "Median";
		case 4:
		case 9:
			return "Greater";
		default:
			return "";
	}
}