---
tags:
  - plugin
resource_link: "https://www.redguides.com/community/resources/mq2notepad.152/"
support_link: "https://www.redguides.com/community/threads/mq2notepad.66859/"
repository: "https://github.com/RedGuides/MQ2Notepad"
config: "MQ2Notepad.ini"
authors: "Cosmic, Maskoi, Sym"
tagline: "This plugin is a very simple in game file editor for small macro or ini files in your macro folder."
---

# MQ2Notepad

<!--desc-start-->
This plugin is a very simple in-game file editor for small macro or INI files found in your macros folder.
<!--desc-end-->

## Commands

<a href="cmd-notepad/">
{% 
  include-markdown "projects/mq2notepad/cmd-notepad.md" 
  start="<!--cmd-syntax-start-->" 
  end="<!--cmd-syntax-end-->" 
%}
</a>
:    {% include-markdown "projects/mq2notepad/cmd-notepad.md" 
        start="<!--cmd-desc-start-->" 
        end="<!--cmd-desc-end-->" 
        trailing-newlines=false 
     %} {{ readMore('projects/mq2notepad/cmd-notepad.md') }}

## Settings

Example MQ2Notepad.ini

```ini
[Settings]
Directory=C:\Users\Redbot\macroquest\macros
ChatTop=50
ChatBottom=437
ChatLeft=257
ChatRight=642
Locked=0
Fades=1
Delay=2000
Duration=500
Alpha=255
FadeToAlpha=255
BGType=1
BGTint.alpha=255
BGTint.red=255
BGTint.green=255
BGTint.blue=255
```

## Troubleshooting

If you get an error about wrong folder,
```bash
/notepad dir c:/full/directory/path
```
If you don't see the notepad window,
```bash
/plugin mq2notepad unload
/plugin mq2notepad load
/loadskin default
```
and then try opening a file with /notepad
