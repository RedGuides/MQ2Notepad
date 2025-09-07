---
tags:
  - command
---

# /notepad

## Syntax

<!--cmd-syntax-start-->
```eqcommand
/notepad <filename> | dir <c:\path\to\macros\folder>
```
<!--cmd-syntax-end-->

## Description

<!--cmd-desc-start-->
Opens a file for editing, or sets the directory in which to find files
<!--cmd-desc-end-->

## Options

| Option | Description |
|--------|-------------|
| `(no option)` | displays help text |
| `dir <c:\path\to\macros\folder>` | Sets the folder to look for file |
| `<filename>` | Attempts to open the file |

## Examples

- open a character's kissassist.ini file with a variable for the name.
<code>/notepad kissassist_${Me}.ini</code>