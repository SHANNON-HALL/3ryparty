// ASLocalizer.cpp
// Copyright (c) 2018 by Jim Pattee <jimp03@email.com>.
// This code is licensed under the MIT License.
// License.md describes the conditions under which this software may be distributed.
//
// File encoding for this file is UTF-8 WITHOUT a byte order mark (BOM).
//    �����ܧڧ�     ���ģ����壩    �ձ��Z     ???
//
// Windows:
// Add the required "Language" to the system.
// The settings do NOT need to be changed to the added language.
// Change the "Region" settings.
// Change both the "Format" and the "Current Language..." settings.
// A restart is required if the codepage has changed.
//		Windows problems:
//		Hindi    - no available locale, language pack removed
//		Japanese - language pack install error
//		Ukranian - displays a ? instead of i
//
// Linux:
// Change the LANG environment variable: LANG=fr_FR.UTF-8.
// setlocale() will use the LANG environment variable on Linux.
//
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *   To add a new language to this source module:
 *
 *   Add a new translation class to ASLocalizer.h.
 *   Update the WinLangCode array in ASLocalizer.cpp.
 *   Add the language code to setTranslationClass() in ASLocalizer.cpp.
 *   Add the English-Translation pair to the constructor in ASLocalizer.cpp.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

//----------------------------------------------------------------------------
// headers
//----------------------------------------------------------------------------

#include "ASLocalizer.h"

#ifdef _WIN32
	#include <windows.h>
#endif

#ifdef __VMS
	#define __USE_STD_IOSTREAM 1
	#include <assert>
#else
	#include <cassert>
#endif

#include <cstdio>
#include <iostream>
#include <clocale>		// needed by some compilers
#include <cstdlib>
#include <typeinfo>

#ifdef _MSC_VER
	#pragma warning(disable: 4996)  // secure version deprecation warnings
#endif

#ifdef __BORLANDC__
	#pragma warn -8104	    // Local Static with constructor dangerous for multi-threaded apps
#endif

#ifdef __INTEL_COMPILER
	#pragma warning(disable:  383)  // value copied to temporary, reference to temporary used
	#pragma warning(disable:  981)  // operands are evaluated in unspecified order
#endif

#ifdef __clang__
	#pragma clang diagnostic ignored "-Wdeprecated-declarations"  // wcstombs
#endif

namespace astyle {

#ifndef ASTYLE_LIB

//----------------------------------------------------------------------------
// ASLocalizer class methods.
//----------------------------------------------------------------------------

ASLocalizer::ASLocalizer()
// Set the locale information.
{
	// set language default values to english (ascii)
	// this will be used if a locale or a language cannot be found
	m_localeName = "UNKNOWN";
	m_langID = "en";
	m_lcid = 0;
	m_subLangID.clear();
	m_translation = nullptr;

	// Not all compilers support the C++ function locale::global(locale(""));
	char* localeName = setlocale(LC_ALL, "");
	if (localeName == nullptr)		// use the english (ascii) defaults
	{
		fprintf(stderr, "\n%s\n\n", "Cannot set native locale, reverting to English");
		setTranslationClass();
		return;
	}
	// set the class variables
#ifdef _WIN32
	size_t lcid = GetUserDefaultLCID();
	setLanguageFromLCID(lcid);
#else
	setLanguageFromName(localeName);
#endif
}

ASLocalizer::~ASLocalizer()
// Delete dynamically allocated memory.
{
	delete m_translation;
}

#ifdef _WIN32

struct WinLangCode
{
	size_t winLang;
	char canonicalLang[3];
};

static WinLangCode wlc[] =
// primary language identifier http://msdn.microsoft.com/en-us/library/aa912554.aspx
// sublanguage identifier http://msdn.microsoft.com/en-us/library/aa913256.aspx
// language ID http://msdn.microsoft.com/en-us/library/ee797784%28v=cs.20%29.aspx
{
	{ LANG_BULGARIAN,  "bg" },		//	bg-BG	1251
	{ LANG_CHINESE,    "zh" },		//	zh-CHS, zh-CHT
	{ LANG_DUTCH,      "nl" },		//	nl-NL	1252
	{ LANG_ENGLISH,    "en" },		//	en-US	1252
	{ LANG_ESTONIAN,   "et" },		//	et-EE
	{ LANG_FINNISH,    "fi" },		//	fi-FI	1252
	{ LANG_FRENCH,     "fr" },		//	fr-FR	1252
	{ LANG_GERMAN,     "de" },		//	de-DE	1252
	{ LANG_GREEK,      "el" },		//	el-GR	1253
	{ LANG_HINDI,      "hi" },		//	hi-IN
	{ LANG_HUNGARIAN,  "hu" },		//	hu-HU	1250
	{ LANG_ITALIAN,    "it" },		//	it-IT	1252
	{ LANG_JAPANESE,   "ja" },		//	ja-JP
	{ LANG_KOREAN,     "ko" },		//	ko-KR
	{ LANG_NORWEGIAN,  "nn" },		//	nn-NO	1252
	{ LANG_POLISH,     "pl" },		//	pl-PL	1250
	{ LANG_PORTUGUESE, "pt" },		//	pt-PT	1252
	{ LANG_ROMANIAN,   "ro" },		//	ro-RO	1250
	{ LANG_RUSSIAN,    "ru" },		//	ru-RU	1251
	{ LANG_SPANISH,    "es" },		//	es-ES	1252
	{ LANG_SWEDISH,    "sv" },		//	sv-SE	1252
	{ LANG_UKRAINIAN,  "uk" },		//	uk-UA	1251
};

void ASLocalizer::setLanguageFromLCID(size_t lcid)
// Windows get the language to use from the user locale.
// NOTE: GetUserDefaultLocaleName() gets nearly the same name as Linux.
//       But it needs Windows Vista or higher.
//       Same with LCIDToLocaleName().
{
	m_lcid = lcid;
	m_langID = "en";	// default to english

	size_t lang = PRIMARYLANGID(LANGIDFROMLCID(m_lcid));
	size_t sublang = SUBLANGID(LANGIDFROMLCID(m_lcid));
	// find language in the wlc table
	size_t count = sizeof(wlc) / sizeof(wlc[0]);
	for (size_t i = 0; i < count; i++)
	{
		if (wlc[i].winLang == lang)
		{
			m_langID = wlc[i].canonicalLang;
			break;
		}
	}
	if (m_langID == "zh")
	{
		if (sublang == SUBLANG_CHINESE_SIMPLIFIED || sublang == SUBLANG_CHINESE_SINGAPORE)
			m_subLangID = "CHS";
		else
			m_subLangID = "CHT";	// default
	}
	setTranslationClass();
}

#endif	// _WIN32

string ASLocalizer::getLanguageID() const
// Returns the language ID in m_langID.
{
	return m_langID;
}

const Translation* ASLocalizer::getTranslationClass() const
// Returns the name of the translation class in m_translation.  Used for testing.
{
	assert(m_translation);
	return m_translation;
}

void ASLocalizer::setLanguageFromName(const char* langID)
// Linux set the language to use from the langID.
//
// the language string has the following form
//
//      lang[_LANG][.encoding][@modifier]
//
// (see environ(5) in the Open Unix specification)
//
// where lang is the primary language, LANG is a sublang/territory,
// encoding is the charset to use and modifier "allows the user to select
// a specific instance of localization data within a single category"
//
// for example, the following strings are valid:
//      fr
//      fr_FR
//      de_DE.iso88591
//      de_DE@euro
//      de_DE.iso88591@euro
{
	// the constants describing the format of lang_LANG locale string
	m_lcid = 0;
	string langStr = langID;
	m_langID = langStr.substr(0, 2);

	// need the sublang for chinese
	if (m_langID == "zh" && langStr[2] == '_')
	{
		string subLang = langStr.substr(3, 2);
		if (subLang == "CN" || subLang == "SG")
			m_subLangID = "CHS";
		else
			m_subLangID = "CHT";	// default
	}
	setTranslationClass();
}

const char* ASLocalizer::settext(const char* textIn) const
// Call the settext class and return the value.
{
	assert(m_translation);
	const string stringIn = textIn;
	return m_translation->translate(stringIn).c_str();
}

void ASLocalizer::setTranslationClass()
// Return the required translation class.
// Sets the class variable m_translation from the value of m_langID.
// Get the language ID at http://msdn.microsoft.com/en-us/library/ee797784%28v=cs.20%29.aspx
{
	assert(m_langID.length());
	// delete previously set (--ascii option)
	if (m_translation != nullptr)
	{
		delete m_translation;
		m_translation = nullptr;
	}
	if (m_langID == "bg")
		m_translation = new Bulgarian;
	else if (m_langID == "zh" && m_subLangID == "CHS")
		m_translation = new ChineseSimplified;
	else if (m_langID == "zh" && m_subLangID == "CHT")
		m_translation = new ChineseTraditional;
	else if (m_langID == "nl")
		m_translation = new Dutch;
	else if (m_langID == "en")
		m_translation = new English;
	else if (m_langID == "et")
		m_translation = new Estonian;
	else if (m_langID == "fi")
		m_translation = new Finnish;
	else if (m_langID == "fr")
		m_translation = new French;
	else if (m_langID == "de")
		m_translation = new German;
	else if (m_langID == "el")
		m_translation = new Greek;
	else if (m_langID == "hi")
		m_translation = new Hindi;
	else if (m_langID == "hu")
		m_translation = new Hungarian;
	else if (m_langID == "it")
		m_translation = new Italian;
	else if (m_langID == "ja")
		m_translation = new Japanese;
	else if (m_langID == "ko")
		m_translation = new Korean;
	else if (m_langID == "nn")
		m_translation = new Norwegian;
	else if (m_langID == "pl")
		m_translation = new Polish;
	else if (m_langID == "pt")
		m_translation = new Portuguese;
	else if (m_langID == "ro")
		m_translation = new Romanian;
	else if (m_langID == "ru")
		m_translation = new Russian;
	else if (m_langID == "es")
		m_translation = new Spanish;
	else if (m_langID == "sv")
		m_translation = new Swedish;
	else if (m_langID == "uk")
		m_translation = new Ukrainian;
	else	// default
		m_translation = new English;
}

//----------------------------------------------------------------------------
// Translation base class methods.
//----------------------------------------------------------------------------

void Translation::addPair(const string& english, const wstring& translated)
// Add a string pair to the translation vector.
{
	pair<string, wstring> entry(english, translated);
	m_translation.emplace_back(entry);
}

string Translation::convertToMultiByte(const wstring& wideStr) const
// Convert wchar_t to a multibyte string using the currently assigned locale.
// Return an empty string if an error occurs.
{
	static bool msgDisplayed = false;
	// get length of the output excluding the nullptr and validate the parameters
	size_t mbLen = wcstombs(nullptr, wideStr.c_str(), 0);
	if (mbLen == string::npos)
	{
		if (!msgDisplayed)
		{
			fprintf(stderr, "\n%s\n\n", "Cannot convert to multi-byte string, reverting to English");
			msgDisplayed = true;
		}
		return "";
	}
	// convert the characters
	char* mbStr = new (nothrow) char[mbLen + 1];
	if (mbStr == nullptr)
	{
		if (!msgDisplayed)
		{
			fprintf(stderr, "\n%s\n\n", "Bad memory alloc for multi-byte string, reverting to English");
			msgDisplayed = true;
		}
		return "";
	}
	wcstombs(mbStr, wideStr.c_str(), mbLen + 1);
	// return the string
	string mbTranslation = mbStr;
	delete[] mbStr;
	return mbTranslation;
}

string Translation::getTranslationString(size_t i) const
// Return the translation ascii value. Used for testing.
{
	if (i >= m_translation.size())
		return string();
	return m_translation[i].first;
}

size_t Translation::getTranslationVectorSize() const
// Return the translation vector size.  Used for testing.
{
	return m_translation.size();
}

bool Translation::getWideTranslation(const string& stringIn, wstring& wideOut) const
// Get the wide translation string. Used for testing.
{
	for (size_t i = 0; i < m_translation.size(); i++)
	{
		if (m_translation[i].first == stringIn)
		{
			wideOut = m_translation[i].second;
			return true;
		}
	}
	// not found
	wideOut = L"";
	return false;
}

string& Translation::translate(const string& stringIn) const
// Translate a string.
// Return a mutable string so the method can have a "const" designation.
// This allows "settext" to be called from a "const" method.
{
	m_mbTranslation.clear();
	for (size_t i = 0; i < m_translation.size(); i++)
	{
		if (m_translation[i].first == stringIn)
		{
			m_mbTranslation = convertToMultiByte(m_translation[i].second);
			break;
		}
	}
	// not found, return english
	if (m_mbTranslation.empty())
		m_mbTranslation = stringIn;
	return m_mbTranslation;
}

//----------------------------------------------------------------------------
// Translation class methods.
// These classes have only a constructor which builds the language vector.
//----------------------------------------------------------------------------

Bulgarian::Bulgarian()	// �ҧ�ݧԧѧ��ܧ�
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"�����ާѧ�ڧ�ѧ�  %s\n");		// should align with unchanged
	addPair("Unchanged  %s\n", L"���֧���ާ֧ߧ֧�  %s\n");		// should align with formatted
	addPair("Directory  %s\n", L"�էڧ�֧ܧ���ڧ�  %s\n");
	addPair("Default option file  %s\n", L"���ѧۧ� �� ����ڧ� ��� ���է�ѧ٧ҧڧ�ѧߧ�  %s\n");
	addPair("Project option file  %s\n", L"���ѧۧ� �� ����ڧ� �٧� ����֧ܧ��  %s\n");
	addPair("Exclude  %s\n", L"���٧ܧݧ��ӧѧ�  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"���٧ܧݧ��ӧѧߧ� (�ߧ֧��ѧӧߧڧާ�)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s ����ާѧ�ڧ�ѧ�   %s h�֧���ާ֧ߧ֧�   ");
	addPair(" seconds   ", L" ��֧ܧ�ߧէ�   ");
	addPair("%d min %d sec   ", L"%d �ާڧ� %d ��֧�   ");
	addPair("%s lines\n", L"%s �ݧڧߧڧ�\n");
	addPair("Opening HTML documentation %s\n", L"����ܧ�ڧӧѧߧ� HTML �է�ܧ�ާ֧ߧ�ѧ�ڧ� %s\n");
	addPair("Invalid default options:", L"���֧ӧѧݧڧէߧ� ����ڧ� ��� ���է�ѧ٧ҧڧ�ѧߧ�:");
	addPair("Invalid project options:", L"���֧ӧѧݧڧէߧ� ����ڧ� �٧� ����֧ܧ��:");
	addPair("Invalid command line options:", L"���֧ӧѧݧڧէߧ� ����ڧ� �٧� �ܧ�ާѧߧէߧڧ� ��֧�:");
	addPair("For help on options type 'astyle -h'", L"���� ���ާ�� ���ߧ��ߧ� �ӧ�٧ާ�اߧ���ڧ�� ��ڧ� 'astyle -h'");
	addPair("Cannot open default option file", L"���� �ާ�ا� �է� ��� ���ӧ��� ��ѧۧݧ�� �� ����ڧ� ��� ���է�ѧ٧ҧڧ�ѧߧ�");
	addPair("Cannot open project option file", L"���� �ާ�ا� �է� ��� ���ӧ��� ��ѧۧݧ� �� ����ڧ� �٧� ����֧ܧ��");
	addPair("Cannot open directory", L"���� �ާ�ا� �է� ��� ���ӧ��� �էڧ�֧ܧ���ڧ�");
	addPair("Cannot open HTML file %s\n", L"���� �ާ�ا� �է� ��� ���ӧ��� HTML ��ѧۧ� %s\n");
	addPair("Command execute failure", L"Command �ڧ٧��ݧߧ� �ߧ֧է���ѧ���ߧ���");
	addPair("Command is not installed", L"Command �ߧ� �� �ڧߧ��ѧݧڧ�ѧ�");
	addPair("Missing filename in %s\n", L"���ڧ��ӧ� �ڧާ֧�� �ߧ� ��ѧۧݧ� �� %s\n");
	addPair("Recursive option with no wildcard", L"���֧ܧ���ڧӧߧ� ����ڧ�, �ҧ֧� �ާѧ�ܧ�");
	addPair("Did you intend quote the filename", L"���ߧѧ֧�� �ݧ� �ߧѧާ֧�֧ߧڧ� �է� ��ڧ�ڧ�ѧ� �ڧާ֧�� �ߧ� ��ѧۧݧ�");
	addPair("No file to process %s\n", L"���� ��ѧۧ� �٧� ��ҧ�ѧҧ��ܧ� %s\n");
	addPair("Did you intend to use --recursive", L"���ߧѧ֧�� �ݧ� �ӧ�٧ߧѧާ֧��ӧѧ�� �է� �ڧ٧��ݧ٧ӧѧ�� --recursive");
	addPair("Cannot process UTF-32 encoding", L"���� �ާ�ا� �է� ��� UTF-32 �ܧ�էڧ�ѧߧ�");
	addPair("Artistic Style has terminated\n", L"Artistic Style �� ���֧ܧ�ѧ�֧ߧ�\n");
}

ChineseSimplified::ChineseSimplified()	// ���ģ����壩
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"��ʽ��  %s\n");		// should align with unchanged
	addPair("Unchanged  %s\n", L"δ�ı�  %s\n");		// should align with formatted
	addPair("Directory  %s\n", L"Ŀ¼  %s\n");
	addPair("Default option file  %s\n", L"Ĭ��ѡ���ļ�  %s\n");
	addPair("Project option file  %s\n", L"��Ŀѡ���ļ�  %s\n");
	addPair("Exclude  %s\n", L"�ų�  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"�ų�����ƥ���  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s ��ʽ��   %s δ�ı�   ");
	addPair(" seconds   ", L" ��   ");
	addPair("%d min %d sec   ", L"%d �� %d ��   ");
	addPair("%s lines\n", L"%s ��\n");
	addPair("Opening HTML documentation %s\n", L"��HTML�ĵ� %s\n");
	addPair("Invalid default options:", L"Ĭ��ѡ����Ч:");
	addPair("Invalid project options:", L"��Ŀѡ����Ч:");
	addPair("Invalid command line options:", L"��Ч��������ѡ��:");
	addPair("For help on options type 'astyle -h'", L"���� 'astyle -h' �Ի���й������еİ���");
	addPair("Cannot open default option file", L"�޷���Ĭ��ѡ���ļ�");
	addPair("Cannot open project option file", L"�޷�����Ŀѡ���ļ�");
	addPair("Cannot open directory", L"�޷���Ŀ¼");
	addPair("Cannot open HTML file %s\n", L"�޷���HTML�ļ� %s\n");
	addPair("Command execute failure", L"ִ������ʧ��");
	addPair("Command is not installed", L"δ��װ����");
	addPair("Missing filename in %s\n", L"��%sȱ���ļ���\n");
	addPair("Recursive option with no wildcard", L"�ݹ�ѡ��û��ͨ���");
	addPair("Did you intend quote the filename", L"����������ļ���");
	addPair("No file to process %s\n", L"û���ļ��ɴ��� %s\n");
	addPair("Did you intend to use --recursive", L"�����ʹ�� --recursive");
	addPair("Cannot process UTF-32 encoding", L"���ܴ���UTF-32����");
	addPair("Artistic Style has terminated\n", L"Artistic Style �Ѿ���ֹ����\n");
}

ChineseTraditional::ChineseTraditional()	// ���ģ����w��
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"��ʽ��  %s\n");		// should align with unchanged
	addPair("Unchanged  %s\n", L"δ��׃  %s\n");		// should align with formatted
	addPair("Directory  %s\n", L"Ŀ�  %s\n");
	addPair("Default option file  %s\n", L"Ĭ�J�x��ļ�  %s\n");
	addPair("Project option file  %s\n", L"�Ŀ�x��ļ�  %s\n");
	addPair("Exclude  %s\n", L"�ų�  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"�ų����oƥ��헣�  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s ��ʽ��   %s δ��׃   ");
	addPair(" seconds   ", L" ��   ");
	addPair("%d min %d sec   ", L"%d �� %d ��   ");
	addPair("%s lines\n", L"%s ��\n");
	addPair("Opening HTML documentation %s\n", L"���_HTML�ęn %s\n");
	addPair("Invalid default options:", L"Ĭ�J�x헟oЧ:");
	addPair("Invalid project options:", L"�Ŀ�x헟oЧ:");
	addPair("Invalid command line options:", L"�oЧ���������x�:");
	addPair("For help on options type 'astyle -h'", L"ݔ��'astyle -h'�ԫ@�����P�����еĎ���:");
	addPair("Cannot open default option file", L"�o�����_Ĭ�J�x��ļ�");
	addPair("Cannot open project option file", L"�o�����_�Ŀ�x��ļ�");
	addPair("Cannot open directory", L"�o�����_Ŀ�");
	addPair("Cannot open HTML file %s\n", L"�o�����_HTML�ļ� %s\n");
	addPair("Command execute failure", L"��������ʧ��");
	addPair("Command is not installed", L"δ���b����");
	addPair("Missing filename in %s\n", L"��%sȱ���ļ���\n");
	addPair("Recursive option with no wildcard", L"�f�w�x헛]��ͨ���");
	addPair("Did you intend quote the filename", L"����������ļ���");
	addPair("No file to process %s\n", L"�]���ļ���̎�� %s\n");
	addPair("Did you intend to use --recursive", L"�����ʹ�� --recursive");
	addPair("Cannot process UTF-32 encoding", L"����̎��UTF-32���a");
	addPair("Artistic Style has terminated\n", L"Artistic Style �ѽ��Kֹ�\��\n");
}

Dutch::Dutch()	// Nederlandse
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"Geformatteerd  %s\n");	// should align with unchanged
	addPair("Unchanged  %s\n", L"Onveranderd    %s\n");	// should align with formatted
	addPair("Directory  %s\n", L"Directory  %s\n");
	addPair("Default option file  %s\n", L"Standaard optie bestand  %s\n");
	addPair("Project option file  %s\n", L"Project optie bestand  %s\n");
	addPair("Exclude  %s\n", L"Uitsluiten  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"Uitgesloten (onge?venaarde)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s geformatteerd   %s onveranderd   ");
	addPair(" seconds   ", L" seconden   ");
	addPair("%d min %d sec   ", L"%d min %d sec   ");
	addPair("%s lines\n", L"%s lijnen\n");
	addPair("Opening HTML documentation %s\n", L"Het openen van HTML-documentatie %s\n");
	addPair("Invalid default options:", L"Ongeldige standaardopties:");
	addPair("Invalid project options:", L"Ongeldige projectopties:");
	addPair("Invalid command line options:", L"Ongeldige command line opties:");
	addPair("For help on options type 'astyle -h'", L"Voor hulp bij 'astyle-h' opties het type");
	addPair("Cannot open default option file", L"Kan het standaardoptiesbestand niet openen");
	addPair("Cannot open project option file", L"Kan het project optie bestand niet openen");
	addPair("Cannot open directory", L"Kan niet open directory");
	addPair("Cannot open HTML file %s\n", L"Kan HTML-bestand niet openen %s\n");
	addPair("Command execute failure", L"Voeren commando falen");
	addPair("Command is not installed", L"Command is niet ge?nstalleerd");
	addPair("Missing filename in %s\n", L"Ontbrekende bestandsnaam in %s\n");
	addPair("Recursive option with no wildcard", L"Recursieve optie met geen wildcard");
	addPair("Did you intend quote the filename", L"Heeft u van plan citaat van de bestandsnaam");
	addPair("No file to process %s\n", L"Geen bestand te verwerken %s\n");
	addPair("Did you intend to use --recursive", L"Hebt u van plan bent te gebruiken --recursive");
	addPair("Cannot process UTF-32 encoding", L"Kan niet verwerken UTF-32 codering");
	addPair("Artistic Style has terminated\n", L"Artistic Style heeft be?indigd\n");
}

English::English()
// this class is NOT translated
{}

Estonian::Estonian()	// Eesti
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"Formaadis  %s\n");		// should align with unchanged
	addPair("Unchanged  %s\n", L"Muutumatu  %s\n");		// should align with formatted
	addPair("Directory  %s\n", L"Kataloog  %s\n");
	addPair("Default option file  %s\n", L"Vaikefunktsioonifail  %s\n");
	addPair("Project option file  %s\n", L"Projekti valiku fail  %s\n");
	addPair("Exclude  %s\n", L"V?lista  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"V?lista (tasakaalustamata)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s formaadis   %s muutumatu   ");
	addPair(" seconds   ", L" sekundit   ");
	addPair("%d min %d sec   ", L"%d min %d sek   ");
	addPair("%s lines\n", L"%s read\n");
	addPair("Opening HTML documentation %s\n", L"Avamine HTML dokumentatsioon %s\n");
	addPair("Invalid default options:", L"Vaikevalikud on sobimatud:");
	addPair("Invalid project options:", L"Projekti valikud on sobimatud:");
	addPair("Invalid command line options:", L"Vale k?sureav?tmetega:");
	addPair("For help on options type 'astyle -h'", L"Abiks v?imaluste t����p 'astyle -h'");
	addPair("Cannot open default option file", L"Vaikimisi valitud faili ei saa avada");
	addPair("Cannot open project option file", L"Projektivaliku faili ei saa avada");
	addPair("Cannot open directory", L"Ei saa avada kataloogi");
	addPair("Cannot open HTML file %s\n", L"Ei saa avada HTML-faili %s\n");
	addPair("Command execute failure", L"K?sk t?ita rike");
	addPair("Command is not installed", L"K?sk ei ole paigaldatud");
	addPair("Missing filename in %s\n", L"Kadunud failinimi %s\n");
	addPair("Recursive option with no wildcard", L"Rekursiivne v?imalus ilma metam?rgi");
	addPair("Did you intend quote the filename", L"Kas te kavatsete tsiteerida failinimi");
	addPair("No file to process %s\n", L"No faili t??tlema %s\n");
	addPair("Did you intend to use --recursive", L"Kas te kavatsete kasutada --recursive");
	addPair("Cannot process UTF-32 encoding", L"Ei saa t??delda UTF-32 kodeeringus");
	addPair("Artistic Style has terminated\n", L"Artistic Style on l?petatud\n");
}

Finnish::Finnish()	// Suomeksi
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"Muotoiltu  %s\n");	// should align with unchanged
	addPair("Unchanged  %s\n", L"Ennallaan  %s\n");	// should align with formatted
	addPair("Directory  %s\n", L"Directory  %s\n");
	addPair("Default option file  %s\n", L"Oletusasetustiedosto  %s\n");
	addPair("Project option file  %s\n", L"Projektin valintatiedosto  %s\n");
	addPair("Exclude  %s\n", L"Sulkea  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"Sulkea (verraton)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s muotoiltu   %s ennallaan   ");
	addPair(" seconds   ", L" sekuntia   ");
	addPair("%d min %d sec   ", L"%d min %d sek   ");
	addPair("%s lines\n", L"%s linjat\n");
	addPair("Opening HTML documentation %s\n", L"Avaaminen HTML asiakirjat %s\n");
	addPair("Invalid default options:", L"Virheelliset oletusasetukset:");
	addPair("Invalid project options:", L"Virheelliset hankevalinnat:");
	addPair("Invalid command line options:", L"Virheellinen komentorivin:");
	addPair("For help on options type 'astyle -h'", L"Apua vaihtoehdoista tyyppi 'astyle -h'");
	addPair("Cannot open default option file", L"Et voi avata oletusasetustiedostoa");
	addPair("Cannot open project option file", L"Projektin asetustiedostoa ei voi avata");
	addPair("Cannot open directory", L"Ei Open Directory");
	addPair("Cannot open HTML file %s\n", L"Ei voi avata HTML-tiedoston %s\n");
	addPair("Command execute failure", L"Suorita komento vika");
	addPair("Command is not installed", L"Komento ei ole asennettu");
	addPair("Missing filename in %s\n", L"Puuttuvat tiedostonimi %s\n");
	addPair("Recursive option with no wildcard", L"Rekursiivinen vaihtoehto ilman wildcard");
	addPair("Did you intend quote the filename", L"Oletko aio lainata tiedostonimi");
	addPair("No file to process %s\n", L"Ei tiedostoa k?sitell? %s\n");
	addPair("Did you intend to use --recursive", L"Oliko aiot k?ytt?? --recursive");
	addPair("Cannot process UTF-32 encoding", L"Ei voi k?sitell? UTF-32 koodausta");
	addPair("Artistic Style has terminated\n", L"Artistic Style on p??ttynyt\n");
}

French::French()	// Fran?aise
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"Format��    %s\n");	// should align with unchanged
	addPair("Unchanged  %s\n", L"Inchang��e  %s\n");	// should align with formatted
	addPair("Directory  %s\n", L"R��pertoire  %s\n");
	addPair("Default option file  %s\n", L"Fichier d'option par d��faut  %s\n");
	addPair("Project option file  %s\n", L"Fichier d'option de projet  %s\n");
	addPair("Exclude  %s\n", L"Exclure  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"Exclure (non appari��s)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s format��   %s inchang��e   ");
	addPair(" seconds   ", L" seconde   ");
	addPair("%d min %d sec   ", L"%d min %d sec   ");
	addPair("%s lines\n", L"%s lignes\n");
	addPair("Opening HTML documentation %s\n", L"Ouverture documentation HTML %s\n");
	addPair("Invalid default options:", L"Options par d��faut invalides:");
	addPair("Invalid project options:", L"Options de projet non valides:");
	addPair("Invalid command line options:", L"Blancs options ligne de commande:");
	addPair("For help on options type 'astyle -h'", L"Pour de l'aide sur les options tapez 'astyle -h'");
	addPair("Cannot open default option file", L"Impossible d'ouvrir le fichier d'option par d��faut");
	addPair("Cannot open project option file", L"Impossible d'ouvrir le fichier d'option de projet");
	addPair("Cannot open directory", L"Impossible d'ouvrir le r��pertoire");
	addPair("Cannot open HTML file %s\n", L"Impossible d'ouvrir le fichier HTML %s\n");
	addPair("Command execute failure", L"Ex��cuter ��chec de la commande");
	addPair("Command is not installed", L"Commande n'est pas install��");
	addPair("Missing filename in %s\n", L"Nom de fichier manquant dans %s\n");
	addPair("Recursive option with no wildcard", L"Option r��cursive sans joker");
	addPair("Did you intend quote the filename", L"Avez-vous l'intention de citer le nom de fichier");
	addPair("No file to process %s\n", L"Aucun fichier �� traiter %s\n");
	addPair("Did you intend to use --recursive", L"Avez-vous l'intention d'utiliser --recursive");
	addPair("Cannot process UTF-32 encoding", L"Impossible de traiter codage UTF-32");
	addPair("Artistic Style has terminated\n", L"Artistic Style a mis fin\n");
}

German::German()	// Deutsch
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"Formatiert   %s\n");	// should align with unchanged
	addPair("Unchanged  %s\n", L"Unver?ndert  %s\n");	// should align with formatted
	addPair("Directory  %s\n", L"Verzeichnis  %s\n");
	addPair("Default option file  %s\n", L"Standard-Optionsdatei  %s\n");
	addPair("Project option file  %s\n", L"Projektoptionsdatei  %s\n");
	addPair("Exclude  %s\n", L"Ausschlie?en  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"Ausschlie?en (unerreichte)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s formatiert   %s unver?ndert   ");
	addPair(" seconds   ", L" sekunden   ");
	addPair("%d min %d sec   ", L"%d min %d sek   ");
	addPair("%s lines\n", L"%s linien\n");
	addPair("Opening HTML documentation %s\n", L"?ffnen HTML-Dokumentation %s\n");
	addPair("Invalid default options:", L"Ung��ltige Standardoptionen:");
	addPair("Invalid project options:", L"Ung��ltige Projektoptionen:");
	addPair("Invalid command line options:", L"Ung��ltige Kommandozeilen-Optionen:");
	addPair("For help on options type 'astyle -h'", L"F��r Hilfe zu den Optionen geben Sie 'astyle -h'");
	addPair("Cannot open default option file", L"Die Standardoptionsdatei kann nicht ge?ffnet werden");
	addPair("Cannot open project option file", L"Die Projektoptionsdatei kann nicht ge?ffnet werden");
	addPair("Cannot open directory", L"Kann nicht ge?ffnet werden Verzeichnis");
	addPair("Cannot open HTML file %s\n", L"Kann nicht ?ffnen HTML-Datei %s\n");
	addPair("Command execute failure", L"Execute Befehl Scheitern");
	addPair("Command is not installed", L"Befehl ist nicht installiert");
	addPair("Missing filename in %s\n", L"Missing in %s Dateiname\n");
	addPair("Recursive option with no wildcard", L"Rekursive Option ohne Wildcard");
	addPair("Did you intend quote the filename", L"Haben Sie die Absicht Inhalte der Dateiname");
	addPair("No file to process %s\n", L"Keine Datei zu verarbeiten %s\n");
	addPair("Did you intend to use --recursive", L"Haben Sie verwenden m?chten --recursive");
	addPair("Cannot process UTF-32 encoding", L"Nicht verarbeiten kann UTF-32 Codierung");
	addPair("Artistic Style has terminated\n", L"Artistic Style ist beendet\n");
}

Greek::Greek()	// �Ŧ˦˦Ǧͦɦ�?
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"���ɦ��̦ϦѦզئ�?�ͦ�  %s\n");	// should align with unchanged
	addPair("Unchanged  %s\n", L"���̦Ŧ�?�¦˦ǦӦ�?   %s\n");	// should align with formatted
	addPair("Directory  %s\n", L"������?�˦Ϧæ�?  %s\n");
	addPair("Default option file  %s\n", L"���ѦϦŦЦɦ˦Ŧæ�?�ͦ� ���Ѧ֦�?�� �ŦЦɦ˦Ϧ�?��  %s\n");
	addPair("Project option file  %s\n", L"���Ѧ֦�?�� �ŦЦɦ˦Ϧ�?? ?�ѦæϦ�  %s\n");
	addPair("Exclude  %s\n", L"���ЦϦʦ˦�?��  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"Ausschlie?en (unerreichte)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s �Ҧ֦Ǧ̦��ӦϦЦϦɦǦ�?�ͦ�   %s ���̦Ŧ�?�¦˦ǦӦ�?   ");
	addPair(" seconds   ", L" �ĦŦԦӦŦ�?�˦ŦЦӦ�   ");
	addPair("%d min %d sec   ", L"%d �˦Ŧ� %d �ĦŦ�   ");
	addPair("%s lines\n", L"%s �æѦ��̦�??\n");
	addPair("Opening HTML documentation %s\n", L"���æʦ�?�ͦɦ� ?�ææѦ��զ� HTML %s\n");
	addPair("Invalid default options:", L"���� ?�æʦԦѦ�? �ŦЦɦ˦Ϧ�?? �ЦѦϦŦЦɦ˦Ϧ�??:");
	addPair("Invalid project options:", L"���� ?�æʦԦѦ�? �ŦЦɦ˦Ϧ�?? ?�ѦæϦ�:");
	addPair("Invalid command line options:", L"���� ?�æʦԦѦ� �ŦЦɦ˦Ϧ�?? �æѦ��̦�?? �ŦͦӦϦ�?��:");
	addPair("For help on options type 'astyle -h'", L"���ɦ� �¦�?�ȦŦɦ� �Ҧ֦ŦӦɦ�? �̦� �Ӧ� ��?�Ħ�? �ŦЦɦ˦Ϧ�?? 'astyle -h'");
	addPair("Cannot open default option file", L"���Ŧ� ��?�ͦ��� �ĦԦͦ���? �ͦ� ���ͦ�?�ΦŦ� �Ӧ� �ЦѦϦŦЦɦ˦Ŧæ�?�ͦ� ���Ѧ֦�?�� �ŦЦɦ˦Ϧ�?��");
	addPair("Cannot open project option file", L"���Ŧ� ��?�ͦ��� �ĦԦͦ���? �ͦ� ���ͦ�?�ΦŦ� �Ӧ� ���Ѧ֦�?�� �ŦЦɦ˦Ϧ�?? ?�ѦæϦ�");
	addPair("Cannot open directory", L"���Ŧ� �̦ЦϦѦ�?�Ӧ� �ͦ� ���ͦ�?�ΦŦӦ� �ӦϦ� �ʦ���?�˦Ϧæ�");
	addPair("Cannot open HTML file %s\n", L"���Ŧ� �̦ЦϦѦ�?�Ӧ� �ͦ� ���ͦ�?�ΦŦӦ� �Ӧ� ���Ѧ֦�?�� HTML %s\n");
	addPair("Command execute failure", L"���ͦӦϦ�? �ͦ� �ŦʦӦŦ�?�ҦŦ� �ӦǦ� ���ЦϦӦԦ�?��");
	addPair("Command is not installed", L"�� �ŦͦӦϦ�? �ĦŦ� ?�֦Ŧ� �Ŧæʦ��Ӧ��ҦӦ��Ȧ�?");
	addPair("Missing filename in %s\n", L"����?�ЦŦ� �Ӧ� ?�ͦϦ̦� ���Ѧ֦�?�Ϧ� �Ҧ� %s\n");
	addPair("Recursive option with no wildcard", L"���ͦ��ĦѦϦ̦ɦ�?? �ŦЦɦ˦Ϧ�? �֦ئ�?? �̦Ц��˦��ͦ�?��");
	addPair("Did you intend quote the filename", L"��?�Ц�? �ҦʦϦЦ�?�ŦӦ� �ͦ� ���ͦ���?�ѦŦӦ� �Ӧ� ?�ͦϦ̦� �ӦϦ� ���Ѧ֦�?�Ϧ�");
	addPair("No file to process %s\n", L"���Ŧ� �Ԧ�?�Ѧ֦Ŧ� ���Ѧ֦�?�� �æɦ� �ӦǦ� �ŦЦŦΦŦѦæ���?�� %s\n");
	addPair("Did you intend to use --recursive", L"��?�Ц�? �ҦʦϦЦ�?�ŦӦ� �ͦ� �֦ѦǦҦɦ̦ϦЦϦ�?�ҦŦӦ� --recursive");
	addPair("Cannot process UTF-32 encoding", L"�ĦŦ� �̦ЦϦѦ�? �ͦ� �ŦЦŦΦŦѦæ��ҦӦ�? UTF-32 �ʦئĦɦʦϦЦ�?�ǦҦ�");
	addPair("Artistic Style has terminated\n", L"Artistic Style ?�֦Ŧ� ��?�ΦŦ�\n");
}

Hindi::Hindi()	// ??????
// build the translation vector in the Translation base class
{
	// NOTE: Scintilla based editors (CodeBlocks) cannot always edit Hindi.
	//       Use Visual Studio instead.
	addPair("Formatted  %s\n", L"???????? ????  %s\n");	// should align with unchanged
	addPair("Unchanged  %s\n", L"??????????     %s\n");	// should align with formatted
	addPair("Directory  %s\n", L"??????????  %s\n");
	addPair("Default option file  %s\n", L"???????? ?????? ?????  %s\n");
	addPair("Project option file  %s\n", L"????????? ?????? ?????  %s\n");
	addPair("Exclude  %s\n", L"???????  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"???????? (??????)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s ???????? ????   %s ??????????   ");
	addPair(" seconds   ", L" ?????   ");
	addPair("%d min %d sec   ", L"%d ???? %d ?????   ");
	addPair("%s lines\n", L"%s ??????\n");
	addPair("Opening HTML documentation %s\n", L"???????? ??????? ????? %s\n");
	addPair("Invalid default options:", L"?????? ???????? ??????:");
	addPair("Invalid project options:", L"?????? ????????? ??????:");
	addPair("Invalid command line options:", L"????? ???? ?????? ????:");
	addPair("For help on options type 'astyle -h'", L"???????? ?? ??? ?? ??? ?????? 'astyle -h'");
	addPair("Cannot open default option file", L"???????? ?????? ????? ???? ??? ????");
	addPair("Cannot open project option file", L"???????? ?????? ????? ???? ??? ????");
	addPair("Cannot open directory", L"?????????? ???? ??? ????");
	addPair("Cannot open HTML file %s\n", L"HTML ????? ???? ??? ???? %s\n");
	addPair("Command execute failure", L"???? ?????? ?????????");
	addPair("Command is not installed", L"???? ??????? ???? ??");
	addPair("Missing filename in %s\n", L"????? ??? ???????? %s\n");
	addPair("Recursive option with no wildcard", L"??? ??????????? ??? ?????????? ??????");
	addPair("Did you intend quote the filename", L"???? ?? ???? ???????? ?? ?????");
	addPair("No file to process %s\n", L"??? ????? %s ????????? ?? ???\n");
	addPair("Did you intend to use --recursive", L"???? ?? ????? ???? ????? ??? --recursive");
	addPair("Cannot process UTF-32 encoding", L"UTF-32 ???????? ????????? ???? ?? ????");
	addPair("Artistic Style has terminated\n", L"Artistic Style ?????? ???? ??\n");
}

Hungarian::Hungarian()	// Magyar
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"Form��zott    %s\n");	// should align with unchanged
	addPair("Unchanged  %s\n", L"V��ltozatlan  %s\n");	// should align with formatted
	addPair("Directory  %s\n", L"C��mjegyz��k  %s\n");
	addPair("Default option file  %s\n", L"Alap��rtelmezett be��ll��t��si f��jl  %s\n");
	addPair("Project option file  %s\n", L"Projekt opci��f��jl  %s\n");
	addPair("Exclude  %s\n", L"Kiz��r  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"Escludere (senza pari)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s form��zott   %s v��ltozatlan   ");
	addPair(" seconds   ", L" m��sodperc   ");
	addPair("%d min %d sec   ", L"%d jeg %d m��s   ");
	addPair("%s lines\n", L"%s vonalak\n");
	addPair("Opening HTML documentation %s\n", L"Nyit�� HTML dokument��ci�� %s\n");
	addPair("Invalid default options:", L"��rv��nytelen alap��rtelmezett be��ll��t��sok:");
	addPair("Invalid project options:", L"��rv��nytelen projektbe��ll��t��sok:");
	addPair("Invalid command line options:", L"��rv��nytelen parancssori opci��k:");
	addPair("For help on options type 'astyle -h'", L"Ha seg��ts��gre van lehet?s��g t��pus 'astyle-h'");
	addPair("Cannot open default option file", L"Nem lehet megnyitni az alap��rtelmezett be��ll��t��si f��jlt");
	addPair("Cannot open project option file", L"Nem lehet megnyitni a projekt opci�� f��jlt");
	addPair("Cannot open directory", L"Nem lehet megnyitni k?nyvt��r");
	addPair("Cannot open HTML file %s\n", L"Nem lehet megnyitni a HTML f��jlt %s\n");
	addPair("Command execute failure", L"Command v��gre hiba");
	addPair("Command is not installed", L"Parancs nincs telep��tve");
	addPair("Missing filename in %s\n", L"Hi��nyz�� f��jln��v %s\n");
	addPair("Recursive option with no wildcard", L"Rekurz��v kapcsol��t nem wildcard");
	addPair("Did you intend quote the filename", L"Esetleg k��v��nja id��zni a f��jln��v");
	addPair("No file to process %s\n", L"Nincs f��jl feldolgoz��sa %s\n");
	addPair("Did you intend to use --recursive", L"Esetleg a haszn��lni k��v��nt --recursive");
	addPair("Cannot process UTF-32 encoding", L"Nem tudja feldolgozni UTF-32 k��dol��ssal");
	addPair("Artistic Style has terminated\n", L"Artistic Style megsz?nt\n");
}

Italian::Italian()	// Italiano
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"Formattata  %s\n");	// should align with unchanged
	addPair("Unchanged  %s\n", L"Immutato    %s\n");	// should align with formatted
	addPair("Directory  %s\n", L"Elenco  %s\n");
	addPair("Default option file  %s\n", L"File di opzione predefinito  %s\n");
	addPair("Project option file  %s\n", L"File di opzione del progetto  %s\n");
	addPair("Exclude  %s\n", L"Escludere  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"Escludere (senza pari)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s ormattata   %s immutato   ");
	addPair(" seconds   ", L" secondo   ");
	addPair("%d min %d sec   ", L"%d min %d seg   ");
	addPair("%s lines\n", L"%s linee\n");
	addPair("Opening HTML documentation %s\n", L"Apertura di documenti HTML %s\n");
	addPair("Invalid default options:", L"Opzioni di default non valide:");
	addPair("Invalid project options:", L"Opzioni di progetto non valide:");
	addPair("Invalid command line options:", L"Opzioni della riga di comando non valido:");
	addPair("For help on options type 'astyle -h'", L"Per informazioni sulle opzioni di tipo 'astyle-h'");
	addPair("Cannot open default option file", L"Impossibile aprire il file di opzione predefinito");
	addPair("Cannot open project option file", L"Impossibile aprire il file di opzione del progetto");
	addPair("Cannot open directory", L"Impossibile aprire la directory");
	addPair("Cannot open HTML file %s\n", L"Impossibile aprire il file HTML %s\n");
	addPair("Command execute failure", L"Esegui fallimento comando");
	addPair("Command is not installed", L"Il comando non �� installato");
	addPair("Missing filename in %s\n", L"Nome del file mancante in %s\n");
	addPair("Recursive option with no wildcard", L"Opzione ricorsiva senza jolly");
	addPair("Did you intend quote the filename", L"Avete intenzione citare il nome del file");
	addPair("No file to process %s\n", L"Nessun file al processo %s\n");
	addPair("Did you intend to use --recursive", L"Hai intenzione di utilizzare --recursive");
	addPair("Cannot process UTF-32 encoding", L"Non �� possibile processo di codifica UTF-32");
	addPair("Artistic Style has terminated\n", L"Artistic Style ha terminato\n");
}

Japanese::Japanese()	// �ձ��Z
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"�ե��`�ޥåȜg�ߤ�  %s\n");		// should align with unchanged
	addPair("Unchanged  %s\n", L"����ޤ���        %s\n");		// should align with formatted
	addPair("Directory  %s\n", L"�ǥ��쥯�ȥ�  %s\n");
	addPair("Default option file  %s\n", L"�ǥե���ȥ��ץ����ե�����  %s\n");
	addPair("Project option file  %s\n", L"�ץ������ȥ��ץ����ե�����  %s\n");
	addPair("Exclude  %s\n", L"���⤹��  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"���⤹�루һ�¤��ޤ���  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s �եե��`�ޥåȜg�ߤ�   %s ����ޤ���   ");
	addPair(" seconds   ", L" ��   ");
	addPair("%d min %d sec   ", L"%d �� %d ��   ");
	addPair("%s lines\n", L"%s �饤��\n");
	addPair("Opening HTML documentation %s\n", L"���`�ץ˥�HTML�ɥ������ %s\n");
	addPair("Invalid default options:", L"�o���ʥǥե���ȥ��ץ����:");
	addPair("Invalid project options:", L"�o���ʥץ������ȥ��ץ����:");
	addPair("Invalid command line options:", L"�o���ʥ��ޥ�ɥ饤�󥪥ץ����");
	addPair("For help on options type 'astyle -h'", L"�����ץ����ηNΥإ�פˤĤ���'astyle- h'���������Ƥ�������");
	addPair("Cannot open default option file", L"�ǥե���ȤΥ��ץ����ե�������_�����Ȥ��Ǥ��ޤ���");
	addPair("Cannot open project option file", L"�ץ������ȥ��ץ����ե�������_�����Ȥ��Ǥ��ޤ���");
	addPair("Cannot open directory", L"�ǥ��쥯�ȥ���_�����Ȥ��Ǥ��ޤ���");
	addPair("Cannot open HTML file %s\n", L"HTML�ե�������_�����Ȥ��Ǥ��ޤ��� %s\n");
	addPair("Command execute failure", L"���ޥ�ɤ�ʧ����g�Ф��ޤ�");
	addPair("Command is not installed", L"���ޥ�ɤ����󥹥ȩ`�뤵��Ƥ��ޤ���");
	addPair("Missing filename in %s\n", L"%s �ǡ��ե�������������ޤ���\n");
	addPair("Recursive option with no wildcard", L"�o�磻��ɥ��`�ɤ�ʹ�ä����َ��Ĥʥ��ץ����");
	addPair("Did you intend quote the filename", L"���ʤ��ϥե������������ä���Ĥ��Ǥ���");
	addPair("No file to process %s\n", L"�������ե�����τI���ʤ��褦�� %s\n");
	addPair("Did you intend to use --recursive", L"���ʤ���--recursiveʹ�ä���Ĥ��Ǥ���");
	addPair("Cannot process UTF-32 encoding", L"UTF - 32���󥳩`�ǥ��󥰤�I��Ǥ��ޤ���");
	addPair("Artistic Style has terminated\n", L"Artistic Style �K�ˤ��ޤ���\n");
}

Korean::Korean()	// ???
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"???    %s\n");		// should align with unchanged
	addPair("Unchanged  %s\n", L"????  %s\n");		// should align with formatted
	addPair("Directory  %s\n", L"????  %s\n");
	addPair("Default option file  %s\n", L"?? ?? ??  %s\n");
	addPair("Project option file  %s\n", L"???? ?? ??  %s\n");
	addPair("Exclude  %s\n", L"???  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"?? (NO ??)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s ???   %s ????   ");
	addPair(" seconds   ", L" ?   ");
	addPair("%d min %d sec   ", L"%d ? %d ?   ");
	addPair("%s lines\n", L"%s ??\n");
	addPair("Opening HTML documentation %s\n", L"HTML ??? ?? %s\n");
	addPair("Invalid default options:", L"??? ?? ??:");
	addPair("Invalid project options:", L"??? ???? ??:");
	addPair("Invalid command line options:", L"??? ??? ?? :");
	addPair("For help on options type 'astyle -h'", L"???? ??? ?? ?? 'astyle - H'? ?????");
	addPair("Cannot open default option file", L"?? ?? ??? ? ? ????.");
	addPair("Cannot open project option file", L"???? ?? ??? ? ? ????.");
	addPair("Cannot open directory", L"????? ?? ?????");
	addPair("Cannot open HTML file %s\n", L"HTML ??? ? ? ???? %s\n");
	addPair("Command execute failure", L"?? ??? ??");
	addPair("Command is not installed", L"??? ???? ?? ????");
	addPair("Missing filename in %s\n", L"%s ?? ??? ?? ??\n");
	addPair("Recursive option with no wildcard", L"??? ???? ?? ??");
	addPair("Did you intend quote the filename", L"??? ?? ??? ????????");
	addPair("No file to process %s\n", L"??? ??? ???? %s\n");
	addPair("Did you intend to use --recursive", L"--recursive ? ????? ????");
	addPair("Cannot process UTF-32 encoding", L"UTF-32 ???? ??? ? ????");
	addPair("Artistic Style has terminated\n", L"Artistic Style? ?????\n");
}

Norwegian::Norwegian()	// Norsk
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"Formatert  %s\n");		// should align with unchanged
	addPair("Unchanged  %s\n", L"Uendret    %s\n");		// should align with formatted
	addPair("Directory  %s\n", L"Katalog  %s\n");
	addPair("Default option file  %s\n", L"Standard alternativfil  %s\n");
	addPair("Project option file  %s\n", L"Prosjekt opsjonsfil  %s\n");
	addPair("Exclude  %s\n", L"Ekskluder  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"Ekskluder (uovertruffen)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s formatert   %s uendret   ");
	addPair(" seconds   ", L" sekunder   ");
	addPair("%d min %d sec   ", L"%d min %d sek?   ");
	addPair("%s lines\n", L"%s linjer\n");
	addPair("Opening HTML documentation %s\n", L"?pning HTML dokumentasjon %s\n");
	addPair("Invalid default options:", L"Ugyldige standardalternativer:");
	addPair("Invalid project options:", L"Ugyldige prosjektalternativer:");
	addPair("Invalid command line options:", L"Kommandolinjevalg Ugyldige:");
	addPair("For help on options type 'astyle -h'", L"For hjelp til alternativer type 'astyle -h'");
	addPair("Cannot open default option file", L"Kan ikke ?pne standardvalgsfilen");
	addPair("Cannot open project option file", L"Kan ikke ?pne prosjektvalgsfilen");
	addPair("Cannot open directory", L"Kan ikke ?pne katalog");
	addPair("Cannot open HTML file %s\n", L"Kan ikke ?pne HTML-fil %s\n");
	addPair("Command execute failure", L"Command utf?re svikt");
	addPair("Command is not installed", L"Command er ikke installert");
	addPair("Missing filename in %s\n", L"Mangler filnavn i %s\n");
	addPair("Recursive option with no wildcard", L"Rekursiv alternativ uten wildcard");
	addPair("Did you intend quote the filename", L"Har du tenkt sitere filnavnet");
	addPair("No file to process %s\n", L"Ingen fil ? behandle %s\n");
	addPair("Did you intend to use --recursive", L"Har du tenkt ? bruke --recursive");
	addPair("Cannot process UTF-32 encoding", L"Kan ikke behandle UTF-32 koding");
	addPair("Artistic Style has terminated\n", L"Artistic Style har avsluttet\n");
}

Polish::Polish()	// Polski
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"Sformatowany  %s\n");	// should align with unchanged
	addPair("Unchanged  %s\n", L"Niezmienione  %s\n");	// should align with formatted
	addPair("Directory  %s\n", L"Katalog  %s\n");
	addPair("Default option file  %s\n", L"Domy?lny plik opcji  %s\n");
	addPair("Project option file  %s\n", L"Plik opcji projektu  %s\n");
	addPair("Exclude  %s\n", L"Wyklucza?  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"Wyklucz (niezr��wnany)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s sformatowany   %s niezmienione   ");
	addPair(" seconds   ", L" sekund   ");
	addPair("%d min %d sec   ", L"%d min %d sek   ");
	addPair("%s lines\n", L"%s linii\n");
	addPair("Opening HTML documentation %s\n", L"Otwarcie dokumentacji HTML %s\n");
	addPair("Invalid default options:", L"Nieprawid?owe opcje domy?lne:");
	addPair("Invalid project options:", L"Nieprawid?owe opcje projektu:");
	addPair("Invalid command line options:", L"Nieprawid?owe opcje wiersza polecenia:");
	addPair("For help on options type 'astyle -h'", L"Aby uzyska? pomoc od rodzaju opcji 'astyle -h'");
	addPair("Cannot open default option file", L"Nie mo?na otworzy? pliku opcji domy?lnych");
	addPair("Cannot open project option file", L"Nie mo?na otworzy? pliku opcji projektu");
	addPair("Cannot open directory", L"Nie mo?na otworzy? katalogu");
	addPair("Cannot open HTML file %s\n", L"Nie mo?na otworzy? pliku HTML %s\n");
	addPair("Command execute failure", L"Wykonaj polecenia niepowodzenia");
	addPair("Command is not installed", L"Polecenie nie jest zainstalowany");
	addPair("Missing filename in %s\n", L"Brakuje pliku w %s\n");
	addPair("Recursive option with no wildcard", L"Rekurencyjne opcja bez symboli");
	addPair("Did you intend quote the filename", L"Czy zamierza Pan poda? nazw? pliku");
	addPair("No file to process %s\n", L"Brak pliku do procesu %s\n");
	addPair("Did you intend to use --recursive", L"Czy masz zamiar u?ywa? --recursive");
	addPair("Cannot process UTF-32 encoding", L"Nie mo?na procesu kodowania UTF-32");
	addPair("Artistic Style has terminated\n", L"Artistic Style zosta? zako��czony\n");
}

Portuguese::Portuguese()	// Portugu��s
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"Formatado   %s\n");	// should align with unchanged
	addPair("Unchanged  %s\n", L"Inalterado  %s\n");	// should align with formatted
	addPair("Directory  %s\n", L"Diret��rio  %s\n");
	addPair("Default option file  %s\n", L"Arquivo de op??o padr?o  %s\n");
	addPair("Project option file  %s\n", L"Arquivo de op??o de projeto  %s\n");
	addPair("Exclude  %s\n", L"Excluir  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"Excluir (incompar��vel)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s formatado   %s inalterado   ");
	addPair(" seconds   ", L" segundo   ");
	addPair("%d min %d sec   ", L"%d min %d seg   ");
	addPair("%s lines\n", L"%s linhas\n");
	addPair("Opening HTML documentation %s\n", L"Abrindo a documenta??o HTML %s\n");
	addPair("Invalid default options:", L"Op??es padr?o inv��lidas:");
	addPair("Invalid project options:", L"Op??es de projeto inv��lidas:");
	addPair("Invalid command line options:", L"Op??es de linha de comando inv��lida:");
	addPair("For help on options type 'astyle -h'", L"Para obter ajuda sobre as op??es de tipo 'astyle -h'");
	addPair("Cannot open default option file", L"N?o �� poss��vel abrir o arquivo de op??o padr?o");
	addPair("Cannot open project option file", L"N?o �� poss��vel abrir o arquivo de op??o do projeto");
	addPair("Cannot open directory", L"N?o �� poss��vel abrir diret��rio");
	addPair("Cannot open HTML file %s\n", L"N?o �� poss��vel abrir arquivo HTML %s\n");
	addPair("Command execute failure", L"Executar falha de comando");
	addPair("Command is not installed", L"Comando n?o est�� instalado");
	addPair("Missing filename in %s\n", L"Filename faltando em %s\n");
	addPair("Recursive option with no wildcard", L"Op??o recursiva sem curinga");
	addPair("Did you intend quote the filename", L"Ser�� que voc�� pretende citar o nome do arquivo");
	addPair("No file to process %s\n", L"Nenhum arquivo para processar %s\n");
	addPair("Did you intend to use --recursive", L"Ser�� que voc�� pretende usar --recursive");
	addPair("Cannot process UTF-32 encoding", L"N?o pode processar a codifica??o UTF-32");
	addPair("Artistic Style has terminated\n", L"Artistic Style terminou\n");
}

Romanian::Romanian()	// Roman?
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"Formatat    %s\n");	// should align with unchanged
	addPair("Unchanged  %s\n", L"Neschimbat  %s\n");	// should align with formatted
	addPair("Directory  %s\n", L"Director  %s\n");
	addPair("Default option file  %s\n", L"Fi?ier op?ional implicit  %s\n");
	addPair("Project option file  %s\n", L"Fi?ier op?iune proiect  %s\n");
	addPair("Exclude  %s\n", L"Exclude?i  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"Exclude?i (necompensat?)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s formatat   %s neschimbat   ");
	addPair(" seconds   ", L" secunde   ");
	addPair("%d min %d sec   ", L"%d min %d sec   ");
	addPair("%s lines\n", L"%s linii\n");
	addPair("Opening HTML documentation %s\n", L"Documenta?ie HTML deschidere %s\n");
	addPair("Invalid default options:", L"Op?iuni implicite nevalide:");
	addPair("Invalid project options:", L"Op?iunile de proiect nevalide:");
	addPair("Invalid command line options:", L"Op?iuni de linie de comand? nevalide:");
	addPair("For help on options type 'astyle -h'", L"Pentru ajutor cu privire la tipul de op?iuni 'astyle -h'");
	addPair("Cannot open default option file", L"Nu se poate deschide fi?ierul cu op?iuni implicite");
	addPair("Cannot open project option file", L"Nu se poate deschide fi?ierul cu op?iuni de proiect");
	addPair("Cannot open directory", L"Nu se poate deschide directorul");
	addPair("Cannot open HTML file %s\n", L"Nu se poate deschide fi?ierul HTML %s\n");
	addPair("Command execute failure", L"Comand? executa e?ec");
	addPair("Command is not installed", L"Comanda nu este instalat");
	addPair("Missing filename in %s\n", L"Lips? nume de fi?ier %s\n");
	addPair("Recursive option with no wildcard", L"Op?iunea recursiv cu nici un wildcard");
	addPair("Did you intend quote the filename", L"V-intentionati cita numele de fi?ier");
	addPair("No file to process %s\n", L"Nu exist? un fi?ier pentru a procesa %s\n");
	addPair("Did you intend to use --recursive", L"V-a?i inten?iona?i s? utiliza?i --recursive");
	addPair("Cannot process UTF-32 encoding", L"Nu se poate procesa codificarea UTF-32");
	addPair("Artistic Style has terminated\n", L"Artistic Style a terminat\n");
}

Russian::Russian()	// �����ܧڧ�
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"�����ާѧ�ڧ��ӧѧߧߧ��  %s\n");	// should align with unchanged
	addPair("Unchanged  %s\n", L"�ҧ֧� �ڧ٧ާ֧ߧ֧ߧڧ�    %s\n");	// should align with formatted
	addPair("Directory  %s\n", L"�ܧѧ�ѧݧ��  %s\n");
	addPair("Default option file  %s\n", L"���ѧۧ� �� ����ڧ֧� ��� ��ާ�ݧ�ѧߧڧ�  %s\n");
	addPair("Project option file  %s\n", L"���ѧۧ� ����ڧ� ����֧ܧ��  %s\n");
	addPair("Exclude  %s\n", L"�ڧ�ܧݧ��ѧ��  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"����ܧݧ��ڧ�� (�ߧ֧��֧ӧ٧�ۧէ֧ߧߧ��)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s �����ާѧ�ڧ��ӧѧߧߧ��   %s �ҧ֧� �ڧ٧ާ֧ߧ֧ߧڧ�   ");
	addPair(" seconds   ", L" ��֧ܧ�ߧէ�   ");
	addPair("%d min %d sec   ", L"%d �ާڧ� %d ��֧�   ");
	addPair("%s lines\n", L"%s �ݧڧߧڧ�\n");
	addPair("Opening HTML documentation %s\n", L"����ܧ���ڧ� HTML �է�ܧ�ާ֧ߧ�ѧ�ڧ� %s\n");
	addPair("Invalid default options:", L"���֧է֧ۧ��ӧڧ�֧ݧ�ߧ�� ��ѧ�ѧާ֧��� ��� ��ާ�ݧ�ѧߧڧ�:");
	addPair("Invalid project options:", L"���֧է�����ڧާ�� ��ѧ�ѧާ֧��� ����֧ܧ��:");
	addPair("Invalid command line options:", L"���֧է�����ڧާ�� ��ѧ�ѧާ֧��� �ܧ�ާѧߧէߧ�� �����ܧ�:");
	addPair("For help on options type 'astyle -h'", L"���ݧ� ���ݧ��֧ߧڧ� ����ѧӧܧ� ��� 'astyle -h' ����ڧ� ��ڧ��");
	addPair("Cannot open default option file", L"���� ��էѧ֧��� ���ܧ���� ��ѧۧ� ��ѧ�ѧާ֧���� ��� ��ާ�ݧ�ѧߧڧ�");
	addPair("Cannot open project option file", L"���� ��էѧ֧��� ���ܧ���� ��ѧۧ� ����ڧ� ����֧ܧ��");
	addPair("Cannot open directory", L"���� �ާ�ԧ� ���ܧ���� �ܧѧ�ѧݧ��");
	addPair("Cannot open HTML file %s\n", L"���� ��էѧ֧��� ���ܧ���� ��ѧۧ� HTML %s\n");
	addPair("Command execute failure", L"������ݧߧڧ�� �ܧ�ާѧߧէ� �ߧ֧է���ѧ���ߧ����");
	addPair("Command is not installed", L"���� ����ѧߧ�ӧݧ֧� ����ާѧߧէ�");
	addPair("Missing filename in %s\n", L"���������ӧ�֧� �ڧާ� ��ѧۧݧ� �� %s\n");
	addPair("Recursive option with no wildcard", L"���֧ܧ���ڧӧߧ�� �ӧѧ�ڧѧߧ� �ҧ֧� �ܧѧܧڧ�-�ݧڧҧ� ��ѧҧݧ�ߧ�");
	addPair("Did you intend quote the filename", L"���� �ߧѧާ֧�֧ߧ� ��ڧ�ѧ��� ��ѧۧݧ�");
	addPair("No file to process %s\n", L"���֧� ��ѧۧݧ�� �էݧ� ��ҧ�ѧҧ��ܧ� %s\n");
	addPair("Did you intend to use --recursive", L"���֧�ا֧ݧ� �ӧ� ���ҧڧ�ѧ֧�֧�� �ڧ���ݧ�٧�ӧѧ�� --recursive");
	addPair("Cannot process UTF-32 encoding", L"���� ��էѧ֧��� ��ҧ�ѧҧ��ѧ�� UTF-32 �ܧ�էڧ��ӧܧ�");
	addPair("Artistic Style has terminated\n", L"Artistic Style ���֧ܧ�ѧ�ڧ�\n");
}

Spanish::Spanish()	// Espa?ol
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"Formato     %s\n");	// should align with unchanged
	addPair("Unchanged  %s\n", L"Inalterado  %s\n");	// should align with formatted
	addPair("Directory  %s\n", L"Directorio  %s\n");
	addPair("Default option file  %s\n", L"Archivo de opciones predeterminado  %s\n");
	addPair("Project option file  %s\n", L"Archivo de opciones del proyecto  %s\n");
	addPair("Exclude  %s\n", L"Excluir  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"Excluir (incomparable)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s formato   %s inalterado   ");
	addPair(" seconds   ", L" segundo   ");
	addPair("%d min %d sec   ", L"%d min %d seg   ");
	addPair("%s lines\n", L"%s l��neas\n");
	addPair("Opening HTML documentation %s\n", L"Apertura de documentaci��n HTML %s\n");
	addPair("Invalid default options:", L"Opciones predeterminadas no v��lidas:");
	addPair("Invalid project options:", L"Opciones de proyecto no v��lidas:");
	addPair("Invalid command line options:", L"No v��lido opciones de l��nea de comando:");
	addPair("For help on options type 'astyle -h'", L"Para obtener ayuda sobre las opciones tipo 'astyle -h'");
	addPair("Cannot open default option file", L"No se puede abrir el archivo de opciones predeterminado");
	addPair("Cannot open project option file", L"No se puede abrir el archivo de opciones del proyecto");
	addPair("Cannot open directory", L"No se puede abrir el directorio");
	addPair("Cannot open HTML file %s\n", L"No se puede abrir el archivo HTML %s\n");
	addPair("Command execute failure", L"Ejecutar el fracaso de comandos");
	addPair("Command is not installed", L"El comando no est�� instalado");
	addPair("Missing filename in %s\n", L"Falta nombre del archivo en %s\n");
	addPair("Recursive option with no wildcard", L"Recursiva opci��n sin comod��n");
	addPair("Did you intend quote the filename", L"Se tiene la intenci��n de citar el nombre de archivo");
	addPair("No file to process %s\n", L"No existe el fichero a procesar %s\n");
	addPair("Did you intend to use --recursive", L"Se va a utilizar --recursive");
	addPair("Cannot process UTF-32 encoding", L"No se puede procesar la codificaci��n UTF-32");
	addPair("Artistic Style has terminated\n", L"Artistic Style ha terminado\n");
}

Swedish::Swedish()	// Svenska
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"Formaterade  %s\n");	// should align with unchanged
	addPair("Unchanged  %s\n", L"Of?r?ndrade  %s\n");	// should align with formatted
	addPair("Directory  %s\n", L"Katalog  %s\n");
	addPair("Default option file  %s\n", L"Standardalternativsfil  %s\n");
	addPair("Project option file  %s\n", L"Projektalternativ fil  %s\n");
	addPair("Exclude  %s\n", L"Uteslut  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"Uteslut (o?vertr?ffad)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s formaterade   %s of?r?ndrade   ");
	addPair(" seconds   ", L" sekunder   ");
	addPair("%d min %d sec   ", L"%d min %d sek   ");
	addPair("%s lines\n", L"%s linjer\n");
	addPair("Opening HTML documentation %s\n", L"?ppna HTML-dokumentation %s\n");
	addPair("Invalid default options:", L"Ogiltiga standardalternativ:");
	addPair("Invalid project options:", L"Ogiltiga projektalternativ:");
	addPair("Invalid command line options:", L"Ogiltig kommandoraden alternativ:");
	addPair("For help on options type 'astyle -h'", L"F?r hj?lp om alternativ typ 'astyle -h'");
	addPair("Cannot open default option file", L"Kan inte ?ppna standardalternativsfilen");
	addPair("Cannot open project option file", L"Kan inte ?ppna projektalternativsfilen");
	addPair("Cannot open directory", L"Kan inte ?ppna katalog");
	addPair("Cannot open HTML file %s\n", L"Kan inte ?ppna HTML-filen %s\n");
	addPair("Command execute failure", L"Utf?r kommando misslyckande");
	addPair("Command is not installed", L"Kommandot ?r inte installerat");
	addPair("Missing filename in %s\n", L"Saknade filnamn i %s\n");
	addPair("Recursive option with no wildcard", L"Rekursiva alternativ utan jokertecken");
	addPair("Did you intend quote the filename", L"Visste du t?nker citera filnamnet");
	addPair("No file to process %s\n", L"Ingen fil att bearbeta %s\n");
	addPair("Did you intend to use --recursive", L"Har du f?r avsikt att anv?nda --recursive");
	addPair("Cannot process UTF-32 encoding", L"Kan inte hantera UTF-32 kodning");
	addPair("Artistic Style has terminated\n", L"Artistic Style har upph?rt\n");
}

Ukrainian::Ukrainian()	// ���ܧ��?�ߧ��ܧڧ�
// build the translation vector in the Translation base class
{
	addPair("Formatted  %s\n", L"����ާѧ��ӧѧߧڧ�  %s\n");	// should align with unchanged
	addPair("Unchanged  %s\n", L"�ҧ֧� �٧�?��      %s\n");	// should align with formatted
	addPair("Directory  %s\n", L"���ѧ�ѧݧ��  %s\n");
	addPair("Default option file  %s\n", L"���ѧۧ� ��ѧ�ѧާ֧��� �٧� �٧ѧާ�ӧ��ӧѧߧߧ��  %s\n");
	addPair("Project option file  %s\n", L"���ѧۧ� �ӧѧ�?�ѧߧ�� ����֧ܧ��  %s\n");
	addPair("Exclude  %s\n", L"���ڧܧݧ��ڧ��  %s\n");
	addPair("Exclude (unmatched)  %s\n", L"���ڧܧݧ��ڧ�� (�ߧ֧�֧�֧ӧ֧��֧ߧڧ�)  %s\n");
	addPair(" %s formatted   %s unchanged   ", L" %s ��?�է���ާѧ��ӧѧߧڧ�   %s �ҧ֧� �٧�?��   ");
	addPair(" seconds   ", L" ��֧ܧ�ߧէ�   ");
	addPair("%d min %d sec   ", L"%d ��ӧ� %d cek   ");
	addPair("%s lines\n", L"%s ��?��?��\n");
	addPair("Opening HTML documentation %s\n", L"��?�էܧ�ڧ��� HTML �է�ܧ�ާ֧ߧ�ѧ�?? %s\n");
	addPair("Invalid default options:", L"���֧�?�ۧ��? ��ѧ�ѧާ֧��� �٧� ��ާ�ӧ�ѧߧߧ��:");
	addPair("Invalid project options:", L"���֧�?�ۧ��? ��ѧ�ѧާ֧��� ����֧ܧ��:");
	addPair("Invalid command line options:", L"���֧��ڧ����ڧާ� ��ѧ�ѧާ֧��� �ܧ�ާѧߧէߧ�ԧ� ���էܧ�:");
	addPair("For help on options type 'astyle -h'", L"���ݧ� ����ڧާѧߧߧ� �է��?�էܧ� ��� 'astyle -h' ����?�� ��ڧ��");
	addPair("Cannot open default option file", L"���֧ާ�اݧڧӧ� ��?�էܧ�ڧ�� ��ѧۧ� ��ѧ�ѧާ֧��?�� �٧� �٧ѧާ�ӧ��ӧѧߧߧ��");
	addPair("Cannot open project option file", L"���֧ާ�اݧڧӧ� ��?�էܧ�ڧ�� ��ѧۧ� ��ѧ�ѧާ֧��?�� ����֧ܧ��");
	addPair("Cannot open directory", L"���� �ާ�ا� ��?�էܧ�ڧ�� �ܧѧ�ѧݧ��");
	addPair("Cannot open HTML file %s\n", L"���� �ӧէ�?����� ��?�էܧ�ڧ�� ��ѧۧ� HTML %s\n");
	addPair("Command execute failure", L"���ڧܧ�ߧѧ�� �ܧ�ާѧߧէ� �ߧ֧է���ѧ�ߧ���?");
	addPair("Command is not installed", L"���� �ӧ��ѧߧ�ӧݧ֧ߧڧ� ����ާѧߧէ�");
	addPair("Missing filename in %s\n", L"��?�է���ߧ� �ߧѧ٧ӧ� ��ѧۧݧ� �� %s\n");
	addPair("Recursive option with no wildcard", L"���֧ܧ���ڧӧߧڧ� �ӧѧ�?�ѧߧ� �ҧ֧� �ҧ�է�-��ܧڧ� ��ѧҧݧ�ߧ�");
	addPair("Did you intend quote the filename", L"���� �ާ�?��� �ߧѧ�?�� ��ڧ�ѧ��� ��ѧۧݧ�");
	addPair("No file to process %s\n", L"���֧ާ�? ��ѧۧ�?�� �էݧ� ��ҧ��ҧܧ� %s\n");
	addPair("Did you intend to use --recursive", L"���֧ӧا� �ӧ� �٧ҧڧ��?��֧�� �ӧڧܧ��ڧ���ӧ�ӧѧ�� --recursive");
	addPair("Cannot process UTF-32 encoding", L"���� �ӧէ�?����� ��ҧ��ҧڧ�� UTF-32 �ܧ�է�ӧѧߧ�?");
	addPair("Artistic Style has terminated\n", L"Artistic Style ���ڧ�ڧߧڧ�\n");
}


#endif	// ASTYLE_LIB

}   // end of namespace astyle

