---
tags:
  - plugin
resource_link: "https://www.redguides.com/community/resources/mq2status.1256/"
support_link: "https://www.redguides.com/community/threads/mq2status.70045/"
repository: "https://github.com/RedGuides/MQ2Status"
config: "mq2status.ini"
authors: "Sic, ChatWithThisName, Knightly, brainiac"
tagline: "Status report for your toons"
quick_start: "https://www.youtube.com/watch?v=OwPO15__wGY"
---

# MQ2Status

<!--desc-start-->
Allows a quick way to get the "status" of various things from your character. This is particularly useful when you are running multiple toons and want to ask them all to report the information for you to see.
<!--desc-end-->

## Commands

<a href="cmd-status/">
{% 
  include-markdown "projects/mq2status/cmd-status.md" 
  start="<!--cmd-syntax-start-->" 
  end="<!--cmd-syntax-end-->" 
%}
</a>
:    {% include-markdown "projects/mq2status/cmd-status.md" 
        start="<!--cmd-desc-start-->" 
        end="<!--cmd-desc-end-->" 
        trailing-newlines=false 
     %} {{ readMore('projects/mq2status/cmd-status.md') }}

## Settings

These options allow changes to what is reported when you do the simple: `/status`

```ini
[ShowPlugin]
Plugin=on
Warrior=on
Cleric=on
Paladin=on
Ranger=on
Shadowknight=on
Druid=on
Monk=on
Bard=on
Rogue=on
Shaman=on
Necromancer=on
Wizard=on
Magician=on
Enchanter=on
Beastlord=on
Berserker=on
```

## Video

- [MQ2Status Original Video](https://www.youtube.com/watch?v=OwPO15__wGY)

## Examples

You can use eqbc or dannet to have your boxed characters respond with the "status" of something.

`/bcga //status item bone chips`
:   Would have everyone in your group including yourself report how many "bone chips" they have on their person.

`/bcga //status plat`
:   Would have everyone in your group including yourself report how much plat they had.