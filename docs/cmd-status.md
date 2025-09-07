---
tags:
  - command
---

# /status

## Syntax

<!--cmd-syntax-start-->
```eqcommand
/status [option] [setting]
```
<!--cmd-syntax-end-->

## Description

<!--cmd-desc-start-->
Reports various status options over eqbc or dannet
<!--cmd-desc-end-->

## Options

| Option | Description |
|--------|-------------|
| `[no option]` | Will output to eqbc/dannet: If we have a CWTN Class Plugin loaded, if we have a macro, if our macro is kiss - it will say what our role is, if we are paused, if we are hidden, and if we have a merc that is alive. |
| `aa` | Reports how many banked AA points you have. |
| `aaxp` | Reports our Spent AA, our AAXP %%, and our Banked AA. |
| `achievement [option]` | Reports if you have completed an achievement by name or id. Keep in mind this will report the 1st match it finds - in the case of names, it would report Hunter of Cobalt Scar from scars of velious and not from tears of veeshan<br>**[no option]** - tells you that you need to provide a name or id.<br> **[name]** - reports your achievement completion status by name.<br> **[id]** - reports your achievement completion status by id. |
| `bagspace` | Reports how many open bag spaces you have. |
| `campfire` | Reports campfire information including Active, Duration, and Zone. |
| `collected [option]` | Reports if you have collected a shiny be *exact name*<br>**[no option]** - Tells you to provide a valid option.<br>**[exact item name]** - reports if you have collected this collection item. |
| `currency [option]` | Reports how many of an alt currency you have.<br>**[no option]** - Tells you to provide a currency name.<br>**[currency name]** - Reports how many of an alt currency you have. |
| `evolving [option]` | Reports the evolving status on an item.<br>**[no option]** - Tells you to provide a valid option.<br>**[partial or whole name of item]** - Reports the evolving status on that item. |
| `fellowship` | Returns to your mq window (does not broadcast report) information on your fellowship. |
| `gtribute` | Displays if your current Guild Tribute Status is On or Off and the current Guild Favor |
| `guildtribute` | Displays if your current Guild Tribute Status is On or Off and the current Guild Favor |
| `invis` | Reports our Invis and IVU status, so we can check we are "Double Invis". |
| `item [option]` | Reports how many of an item you have in your inventory.<br>**[no option]** - Asks for a valid item to search for<br>**[item whole or partial name]** - Returns the quantity of the item in your inventory |
| `itembank [option]` | Reports how many of an item you have in your bank.<br>**[no option]** - Asks for a valid item to search for<br>**[item whole or partial name]** - Returns the quantity of the item in your bank |
| `itemall [option]` | Reports how many of an item you have in your inventory and bank.<br>**[no option]** - Asks for a valid item to search for<br>**[item whole or partial name]** - Returns the quantity of the item in your inventory and bank |
| `krono` | Reports how many krono you have |
| `login` | Reports your account login name. |
| `merc` | Reports mercenary information including class, and role. |
| `macro` | Reports the name of the macro you are currently running. |
| `money` | Reports how much plat you have. |
| `plat` | Reports how much plat you have. |
| `parcel` | Reports your parcel status. |
| `powersource` | Reports our powersource status. |
| `quest [option]` | Reports if you have a quest/task by name.<br>**[no option]** - Tells you to please provide a quest/task name<br>**[partial or whole quest name]** - Reports if you have a quest/task by this name |
| `show [option] [setting]` | Allows toggling on/off of the CWTN Class Plugins to be visible during /status.<br><br>**[no option]** - Tells you to provide a valid option<br>**[plugin]** - Tells you to provide a valid plugin name<br>**[on/off]** - toggles the plugin on/off<br>**[bard]** - toggles visiblity on/off<br>**[beastlord]** - toggles visiblity on/off<br>**[berserker]** - toggles visiblity on/off<br>**[cleric]** - toggles visiblity on/off<br>**[druid]** - toggles visiblity on/off<br>**[enchanter]** - toggles visibility on/off<br>**[magician]** - toggles visiblity on/off<br>**[monk]** - toggles visiblity on/off<br>**[necromancer]** - toggles visiblity on/off<br>**[paladin]** - toggles visiblity on/off<br>**[ranger]** - toggles visiblity on/off<br>**[rogue]** - toggles visiblity on/off<br>**[shadowknight]** - toggles visiblity on/off<br>**[shaman]** - toggles visiblity on/off<br>**[warrior]** - toggles visiblity on/off<br>**[wizard]** - toggles visiblity on/off |
| `skill [option]` | Reports your current skill value.<br>**[no option]** - Asks you to provide a valid skill name<br>**[skill by name]** - reports your current skill value<br><br>e.g. `/status skill jewelry making` |
| `sub` | Reports your subscription level, and if you are gold - how many days you have left on that subscription. |
| `task [option]` | see quest |
| `tribute` | Reports if your current Tribute Status is On or Off and your current favor |
| `xp` | Reports your level, current XP %, Banked AA, and your AAXP %. |
| `zone` | Reports what zone we are in. |

## Examples

This will check how much plat you have on you
: `/status plat`

This will check how many bone chips you have on you
: `/status item bone chips`

This will check how many bone chips you have in your bank
: `/status itembank bone chips`