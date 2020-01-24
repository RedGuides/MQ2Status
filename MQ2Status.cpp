// MQ2Status.cpp : Defines the entry point for the DLL application.
//
//MQ2Status by Chatwiththisname and Sic
//Commands: 
//    /status
//	/status help - returns help information for the / commands
//	/status item - Using finditemcount this will return the amount of the first item matched with your string
//		you can do up to /status item bone chips and it would do a ${FindItemCount[bone chips]}
//		/status item bone chips returns how many bone chips you have on your person
//	/status itembank - same as item, but checks your bank
//	/status merc - outputs mercenary information
//	/status aa - shows how many banked aa you have
//	/status campfire - Returns if you have an active campfire, and if so duration and zone
//	/status stat
//		/status stat Hdex returns how much HDex you have
//		/status stat Hstr returns how much HStr you have
//		/status stat HSta returns how much HSta you have
//		/status stat HInt returns how much HInt you have
//		/status stat HAgi returns how much HAgi you have
//		/status stat HWis returns how much HWis you have
//		/status stat HCha returns how much HCha you have
//		/status stat Hps returns current hps / total hps and %
//		/status stat mana returns current mana / total mana and %
//		/status stat money returns how much platinum you have
//	/status fellowship - returns some information on your fellowship (WIP)
//	/status campfire - returns information on your campfire
//	/status bagspace - returns how many free inventory spaces you have
//	/status sub reports your subscription level, and if gold how much time is left
//  /status xp - reports Level - XP % - Banked AA - AAXP %
//	/status aaxp - reports Spent AA - AAXP % - Banked AA
//
//Purpose: 
//	to quickly display your "status" as relevant to if:
//		a class plugin is running
//		a macro is running
//			if the macro is kissassist variation
//				what your role is
//			If the macro is paused
//		if you are hidden
//		if you have a merc alive
//Usage:
//	using the command /status to output to /bc the information in purpose. 
//	can be used with "/bcaa //status" to get all members of eqbc to output their current status (also bcga, bca, bct etc as needed by the user).
//
//To Do:
//	Convert the finditem and findbankitem to not use macro code
//	Clean up redundant code

#include "../MQ2Plugin.h"
#include "StatusFunctions.h"


PreSetup("MQ2Status");
PLUGIN_VERSION(1.8);


// Called once, when the plugin is to initialize
PLUGIN_API VOID InitializePlugin(VOID)
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

// Called once, when the plugin is to shutdown
PLUGIN_API VOID ShutdownPlugin(VOID)
{
	RemoveCommand("/status");
}


// Called once directly after initialization, and then every time the gamestate changes
//PLUGIN_API VOID SetGameState(DWORD GameState) {
//
//	if (GameState == GAMESTATE_INGAME) {
//	}
//}
// THIS IS A BLOCK COMMENT!
//// This is called every time MQ pulses (MainLOOP!)
//PLUGIN_API VOID OnPulse(VOID)
//{
//	//Not sure if this will ever bee needed, so holding onto it. 
//}
//
//// This is called every time WriteChatColor is called by MQ2Main or any plugin,
//// IGNORING FILTERS, IF YOU NEED THEM MAKE SURE TO IMPLEMENT THEM. IF YOU DONT
//// CALL CEverQuest::dsp_chat MAKE SURE TO IMPLEMENT EVENTS HERE (for chat plugins)
//PLUGIN_API DWORD OnWriteChatColor(PCHAR Line, DWORD Color, DWORD Filter)
//{
//	//Wasn't sure if this would ever be needed, so hanging onto it.
//    return 0;
//}
//
//// This is called every time EQ shows a line of chat with CEverQuest::dsp_chat,
//// but after MQ filters and chat events are taken care of.
//PLUGIN_API DWORD OnIncomingChat(PCHAR Line, DWORD Color)
//{
//	//Wasn't sure if this would ever be needed, so hanging onto it.
//    return 0;
//}


void VerifyINI(char* Section, char* Key, char* Default, bool& plugin) {
	char temp[MAX_STRING] = { 0 };
	if (GetPrivateProfileString(Section, Key, nullptr, temp, MAX_STRING, INIFileName) == 0) {
		WritePrivateProfileString(Section, Key, Default, INIFileName);
		plugin = atob(temp);
	}
}

void DoINIThings() {
	VerifyINI("ShowPlugin", "Plugin", "on", bShowPlugin);
	VerifyINI("ShowPlugin", "Warrior", "on", bShowWarrior);
	VerifyINI("ShowPlugin", "Warrior", "on", bShowWarrior);
	VerifyINI("ShowPlugin", "Cleric", "on", bShowCleric);
	VerifyINI("ShowPlugin", "Paladin", "on", bShowPaladin);
	VerifyINI("ShowPlugin", "Ranger", "on", bShowRanger);
	VerifyINI("ShowPlugin", "Shadowknight", "on", bShowShadowknight);
	VerifyINI("ShowPlugin", "Druid", "on", bShowDruid);
	VerifyINI("ShowPlugin", "Monk", "on", bShowMonk);
	VerifyINI("ShowPlugin", "Bard", "on", bShowBard);
	VerifyINI("ShowPlugin", "Rogue", "on", bShowRogue);
	VerifyINI("ShowPlugin", "Shaman", "on", bShowShaman);
	VerifyINI("ShowPlugin", "Necromancer", "on", bShowNecromancer);
	VerifyINI("ShowPlugin", "Wizard", "on", bShowWizard);
	VerifyINI("ShowPlugin", "Magician", "on", bShowMage);
	VerifyINI("ShowPlugin", "Enchanter", "on", bShowEnchanter);
	VerifyINI("ShowPlugin", "Beastlord", "on", bShowBeastlord);
	VerifyINI("ShowPlugin", "Berserker", "on", bShowBerserker);
}

bool atob(char* x) {
	if (!_stricmp(x, "true") || strtol(x, nullptr, 10) != 0 || !_stricmp(x, "on")) {
		return true;
	}
	return false;
}

void ParseBoolArg(PCHAR Arg, PCHAR Arg2, PCHAR Arg3, bool *theOption, char* INIsection) {
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