Translations for ufo2000		2004-11-20
========================

Since v0.4.1.517, ufo2000 supports translating the game-messages
into foreign languages.

Currently, the following languages are available:
* EN - English (builtin), no translation-files required
* ES - Spanish
* DE - German
* RU - Russian
A french translation is in the works.

To select one of the available languages, 
change in the file ufo2000.ini the line
   language = en
to match any of the above country-codes.

Language-specific files:
* Readme-XX.txt - General information
* .\translations\tips-XX.txt   - Tips-of-the-day (optional)
* .\translations\ufo2000-XX.po - all the messages used by the game

gettext-tools are used to create the po-files,
see http://www.gnu.org/software/gettext/

If you want to translate ufo2000 to a new language,
please contact the ufo2000 development team, see
http://ufo2000.sourceforge.net

See also:
http://ufo2000.lxnt.info/pmwiki/index.php/Main/TipsOfTheDay


Notes + Tools:
==============
* http://www.crimsoneditor.com -  Free editor with UTF8-support 
* http://poedit.sourceforge.net - Editor for po-files

For developers:
* 'libiconv' and 'gettext' packages (for installing into mingw directory):
  http://sourceforge.net/project/showfiles.php?group_id=2435

Convert textfile "ufo2000-de.txt" with translations from latin-1 
("windows-western-europe") encoding to .po-file with utf8-encoding:
  iconv -f ISO-8859-1 -t UTF-8 ufo2000-es.txt > ufo2000-es.po

Update language-file for ES=spanish after changes to source:
  make lng-es
(than edit file ./translations/ufo2000-es.po)

