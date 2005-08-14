==============================================================================
== Translations for UFO2000
==============================================================================

UFO2000 supports translating the game-messages into foreign languages.

Currently, the following translations are available:
* English (builtin), no translation-files required
* Spanish
* German
* Russian
* Belarusian

To select one of the available languages for use in the game, you can 
use options menu, Alternatively you can change the line 'language = eng'
to match any of the above language codes in ufo2000.ini

We are using 3-letter iso639-2 language codes, for more details check:
http://www.loc.gov/standards/iso639-2/frelangn_ascii.html

==============================================================================
== Creating a new translation
==============================================================================

If you want to translate ufo2000 to a new language, you just need to
go to 'translations' subdirectory and copy ufo2000.pot into a ufo2000-XXX.po 
where XXX is your language code (check a link above for a list of language 
codes). After that, search for the following pairs in this file:

msgid "some message in English"
msgstr ""

You need to add translations to 'msgstr' variables. Not all the message 
need to be translated, if you want the game to keep using English variant 
for some message, just keep msgstr variable as an empty string.

A good start would be to search for "connect to internet server" string
in a po-file, add a translation there, save the file and start the game.
A new translation should be detected by the game automatically and it
will become available in the options dialog. Select your language, return
to the main menu and you should see the results of your work in the
top menu item :)

IMPORTANT: the game uses unicode internally, so the translation file should
be in UTF-8 encoding. That's not a problem as a standard windows notepad
supports UTF-8 quite well. There are even better editors, the links to them 
can be found in "Notes + Tools" section at the end of this file. Anyway, if 
you got stuck and the game displays your translated messages in a wrong 
encoding or does not show some characters correctly, feel free to contact us.

For any questions about internationalization, post a message in
the internationalization thread on our forum:
http://ufo2000.sourceforge.net

==============================================================================
== Updating existing translation
==============================================================================

As the game gets improved and new versions are released, it can display some 
new messages. Old po-files do not have translation for such messages, so they 
need to be updated. The old files will still correctly load and work with the 
game, but all the untranslated messages will be shown in English. 

Fortunately gettext tools, which we are using for internationalization can 
greatly help to update old translations. It is possible to automatically 
update po-files so that all the existing translations are kept and new 
messages are added. Actually some translation files which come with the
game are not up to date and may need to be updated.

In order to update translation, just open po-file in a text editor and search 
for msgstr "" lines (empty translations), new translations can be added there.

Also there could be some special 'fuzzy' marks in the po-file. Let's suppose
some old version of the game had the following translation record:

msgid "some old message in English"
msgstr "some old translation"

After a new version got released, the original English message could be changed.
When we update po-file by gettext tools, we can get the following:

#, fuzzy
msgid "some new message in English"
msgstr "some old translation"

Gettext can see here that the original message was changes, but it inserts an
old translation and marks this message with a 'fuzzy' marker. You need
to update translation and remove the fuzzy marker:

msgid "some new message in English"
msgstr "some new translation"

After there are no untranslated or fuzzy messages left, the translation is up 
to date again. Submit it to the developers and it will be included into the 
next distributive of the game.

==============================================================================
== Notes + Tools
==============================================================================

* http://www.crimsoneditor.com -  Free editor with UTF8-support 
* http://poedit.sourceforge.net - Specialized editor for po-files

For developers: gettext-tools are used to create the po-file templates
http://gnuwin32.sourceforge.net/packages/gettext.htm
