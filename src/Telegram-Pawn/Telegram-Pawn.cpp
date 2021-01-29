// Telegram-Pawn.cpp : This file contains the 'main' function. Program execution begins and ends there.
//D:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\PlatformSDK\Include
#include <iostream>
#include <cstdarg>
#include "INIReader/INIReader.h"
#include <tgbot/tgbot.h>
#include "obfuscator.hpp"
//#include <stdint.h>
//#include <cstddef>
#define HAVE_STDINT_H
#include <inttypes.h>
#include <stdio.h>
#include "amx/amx.h"

bool LoadScript(char* pFileName);
extern "C" int amx_CoreInit(AMX * amx);
extern "C" int amx_CoreCleanup(AMX * amx);
extern "C" int amx_FloatInit(AMX * amx);
extern "C" int amx_FloatCleanup(AMX * amx);
extern "C" int amx_StringInit(AMX * amx);
extern "C" int amx_StringCleanup(AMX * amx);
extern "C" int amx_FileInit(AMX * amx);
extern "C" int amx_FileCleanup(AMX * amx);
extern "C" int amx_TimeInit(AMX * amx);
extern "C" int amx_TimeCleanup(AMX * amx);
extern "C" int amx_DGramInit(AMX * amx);
extern "C" int amx_DGramCleanup(AMX * amx);
int AMXAPI aux_LoadProgram(AMX* amx, char* filename);
int AMXAPI aux_FreeProgram(AMX* amx);
char* AMXAPI aux_StrError(int errnum);
void logprintf(char* format, ...);
void AMXPrintError(AMX* amx, int error);
char* format_amxstring(AMX* amx, cell* params, int parm, int& len);
int set_amxstring(AMX* amx, cell amx_addr, const char* source, int max);
char amxFile[256];
AMX m_amx;
std::string token;
std::string script;
int64_t S64(const char* s) {
	int64_t i;
	char c;
	int scanned = sscanf(s, "%" SCNd64 "%c", &i, &c);
	if (scanned == 1) return i;
	if (scanned > 1) {
		// TBD about extra data found
		return i;
	}
	// TBD failed to scan;  
	return 0;
}
#define CHECK_PARAMS(n) { if (params[0] != (n * sizeof(cell))) { logprintf((char*)"[%s] : Bad parameter count (count is %d, should be %d): ", amxFile, params[0] / sizeof(cell), n); return 0; } }
int longPollThread(TgBot::Bot bot)
{
	TgBot::TgLongPoll longPoll(bot);
	try {
		logprintf((char*)"Bot username: %s", bot.getApi().getMe()->username.c_str());
		bot.getApi().deleteWebhook();
		//bot.getApi().leaveChat(-1001181793654);
		while (true) {
			longPoll.start();
		}
	}
	catch (TgBot::TgException& e) {
		bot.getApi().getUpdates(longPoll.WtfIsLastOne());
		bot.getApi().getUpdates(longPoll.WtfIsLastOne() + 1);
		logprintf((char*)"Error: %s", e.what());
		logprintf((char*)"[%s] : AMX Exiting", amxFile);
		int tmp;
		if (!amx_FindPublic(&m_amx, "OnExit", &tmp))
			amx_Exec(&m_amx, (cell*)&tmp, tmp);
		aux_FreeProgram(&m_amx);
		return 0;
	}
}
void sendvideo(AMX* amx, cell* params)
{
	try
	{
		char* szId;
		amx_StrParam(amx, params[1], szId);
		char* szURL;
		char* szCaption;
		amx_StrParam(amx, params[2], szURL);
		amx_StrParam(amx, params[7], szCaption);
		TgBot::Bot bot = TgBot::Bot(token);
		bot.getApi().sendVideo(S64(szId), szURL, params[3], params[4], params[5], params[6], "", szCaption, params[8]);
	}
	catch (TgBot::TgException& e)
	{
		logprintf((char*)"Ops, Error : %s", e.what());
	}
}
void sendmessage(AMX* amx, cell* params)
{
	try
	{
		char* szId;
		amx_StrParam(amx, params[1], szId);
		char* szText;
		amx_StrParam(amx, params[2], szText);
		TgBot::Bot bot = TgBot::Bot(token);
		bot.getApi().sendMessage(S64(szId), szText, (bool)params[3], params[4]);
	}
	catch (TgBot::TgException& e)
	{
		logprintf((char*)"Ops, Error : %s", e.what());
	}
}
void sendphoto(AMX* amx, cell* params)
{
	try
	{
		char* szId;
		amx_StrParam(amx, params[1], szId);
		char* szURL;
		char* szCaption;
		amx_StrParam(amx, params[2], szURL);
		amx_StrParam(amx, params[3], szCaption);

		TgBot::Bot bot = TgBot::Bot(token);
		bot.getApi().sendPhoto(S64(szId), szURL, szCaption, params[4]);
	}
	catch (TgBot::TgException& e)
	{
		logprintf((char*)"Ops, Error : %s", e.what());
	}
}


static cell AMX_NATIVE_CALL n_print(AMX* amx, cell* params)
{
	CHECK_PARAMS(1);
	char* msg;
	amx_StrParam(amx, params[1], msg);
	logprintf((char*)"[%s] : %s", amxFile, msg);
	return 0;
}
static cell AMX_NATIVE_CALL n_printf(AMX* amx, cell* params)
{
	int len;
	logprintf((char*)"[%s] : %s", amxFile, format_amxstring(amx, params, 1, len));
	return 0;
}
static cell AMX_NATIVE_CALL n_GetToken(AMX* amx, cell* params)
{
	CHECK_PARAMS(2);
	return set_amxstring(amx, params[1], token.c_str(), params[2]);
}
static cell AMX_NATIVE_CALL n_RegisterCommand(AMX* amx, cell* params)
{
	CHECK_PARAMS(1);
	char* szCommand;
	amx_StrParam(amx, params[1], szCommand);
	return set_amxstring(amx, params[1], token.c_str(), params[2]);
}
static cell AMX_NATIVE_CALL n_SendMessage(AMX* amx, cell* params)
{
	std::thread what_thread_is_this(sendmessage, amx, params);
	what_thread_is_this.join();
	return 0;
}

static cell AMX_NATIVE_CALL n_SendPhoto(AMX* amx, cell* params)
{
	std::thread what_thread_is_this(sendphoto, amx, params);
	what_thread_is_this.join();
	return 0;
}
static cell AMX_NATIVE_CALL n_SendVideo(AMX* amx, cell* params)
{
	std::thread what_thread_is_this(sendvideo, amx, params);
	what_thread_is_this.join();
	return 0;
}
static cell AMX_NATIVE_CALL n_format(AMX* amx, cell* params)
{
	int len;
	return set_amxstring(amx, params[1], format_amxstring(amx, params, 3, len), params[2]);
}
cell* get_amxaddr(AMX* amx, cell amx_addr);;
static cell AMX_NATIVE_CALL n_CallLocalFunction(AMX* amx, cell* params)
{
	if (params[0] < 2 * sizeof(cell))
	{
		logprintf((char*)"[%s] : Bad parameter count (%d < 2): ", amxFile, params[0]);
		return 0;
	}
	else if (params[0] > 258 * sizeof(cell))
	{
		logprintf((char*)"Bad parameter count (%d > 258): ", amxFile, params[0]);
		return 0;
	}
	int iLength;
	char* szFuncName; //[32];
	char* szParamList; //[16];
	bool bFound = false;

	amx_StrParam(amx, params[1], szFuncName);
	amx_StrParam(amx, params[2], szParamList);
	if (szParamList == NULL) iLength = 0;
	else iLength = strlen(szParamList);

	int idx, j;
	cell ret = 0;

	if (!amx_FindPublic(amx, szFuncName, &idx))
	{
		cell amx_addr[256];
		j = iLength;
		int numstr, iOff = 3; // Count, func, map
		for (numstr = 0; numstr < 16; numstr++)
		{
			amx_addr[numstr] = NULL;
		}
		numstr = 0;
		while (j)
		{
			j--;
			if (*(szParamList + j) == 'a')
			{
				cell* paddr;
				int numcells = *get_amxaddr(amx, params[j + iOff + 1]);
				if (amx_Allot(amx, numcells, &amx_addr[numstr], &paddr) == AMX_ERR_NONE)
				{
					memcpy(paddr, get_amxaddr(amx, params[j + iOff]), numcells * sizeof(cell));
					amx_Push(amx, amx_addr[numstr]);
					numstr++;
				}
			}
			else if (*(szParamList + j) == 's')
			{
				char* szParamText;

				amx_StrParam(amx, params[j + iOff], szParamText);
				if (szParamText != NULL && strlen(szParamText) > 0)
				{
					amx_PushString(amx, &amx_addr[numstr], NULL, szParamText, 0, 0);
					numstr++;
				}
				else
				{
					*szParamText = 1;
					*(szParamText + 1) = 0;
					amx_PushString(amx, &amx_addr[numstr], NULL, szParamText, 0, 0);
				}
			}
			else
			{
				amx_Push(amx, *get_amxaddr(amx, params[j + iOff]));
			}
		}
		amx_Exec(amx, &ret, idx);
		while (numstr)
		{
			numstr--;
			amx_Release(amx, amx_addr[numstr]);
		}
	}
	return (int)ret;
}
static cell AMX_NATIVE_CALL n_ClearConsole(AMX* amx, cell* params)
{
	CHECK_PARAMS(0);
#if defined  __WIN32__ || defined WIN32 || defined _WIN32
	system("cls");
#else
	system("clear");    //if other
#endif  //finish
	return 0;
}
int32_t botOwner = 0;
static cell AMX_NATIVE_CALL n_SetOwner(AMX* amx, cell* params)
{
	CHECK_PARAMS(1);
	botOwner = params[1];
	return 0;
}
static cell AMX_NATIVE_CALL n_ExecCommand(AMX* amx, cell* params)
{
	CHECK_PARAMS(1);
	char* szCommand;
	amx_StrParam(amx, params[2], szCommand);
	system(szCommand);
	return 0;
}
AMX_NATIVE_INFO tp_Natives[] =
{
	{ "print",				n_print },
	{ "printf",				n_printf },
	{ "format",				n_format },
	{ "CallLocalFunction",	n_CallLocalFunction },
	{ "ExecCommand",		n_ExecCommand },

	{ "GetToken",			n_GetToken },
	{ "SendMessage",		n_SendMessage },
	{ "SendPhoto",			n_SendPhoto },
	{ "SendVideo",			n_SendVideo },
	{ "SetOwner",			n_SetOwner }
};
int amx_TelegramPawnInit(AMX* amx)
{
	return amx_Register(amx, tp_Natives, -1);
}

char* Gets(char* str, int num);

void console_Thread()
{
	while (true)
	{
		char gets[1024];
		Gets(gets, sizeof(gets));
		if (strcmp(gets, "exit") == 0)
		{
			TgBot::Bot bot = TgBot::Bot(token);
			bot.getApi().getUpdates().clear();
			logprintf((char*)"[%s] : AMX Exiting", amxFile);
			int tmp;
			if (!amx_FindPublic(&m_amx, "OnExit", &tmp))
				amx_Exec(&m_amx, (cell*)&tmp, tmp);
			aux_FreeProgram(&m_amx);
			break;
		}
		if (strcmp(gets, "clear") == 0)
		{
#if defined  __WIN32__ || defined WIN32 || defined _WIN32
			system("cls");
#else
			system("clear");    //if other
#endif  //finish
		}
		if (strcmp(gets, "amx") == 0) logprintf((char*)"amx = %s", script.c_str());
		if (strcmp(gets, "token") == 0) logprintf((char*)"token = %s", token.c_str());
		if (strcmp(gets, "restart") == 0)
		{
			TgBot::Bot bot = TgBot::Bot(token);
			bot.getApi().getUpdates().clear();
			logprintf((char*)"[%s] : Restarting", amxFile);
			int tmp;
			if (!amx_FindPublic(&m_amx, "OnExit", &tmp))
				amx_Exec(&m_amx, (cell*)&tmp, tmp);
			aux_FreeProgram(&m_amx);
			LoadScript((char*)script.c_str());
		}
	}
	exit(0);
}
void LoadLogFile();
int main()
{
	logprintf((char*)OBFUSCATE(" "));
	LoadLogFile();
	logprintf((char*)OBFUSCATE(" "));
	logprintf((char*)OBFUSCATE("--------------------------------------------------------------------"));
	logprintf((char*)OBFUSCATE("[Telegram-Pawn] : Telegram-Pawn by Nima Bastani, Thanks for using !"));
	logprintf((char*)OBFUSCATE("--------------------------------------------------------------------"));
	//std::thread console_t(console_Thread);
#if defined  __WIN32__ || defined WIN32 || defined _WIN32
	system("Color F");
#endif
	logprintf((char*)OBFUSCATE("[Telegram-Pawn] : Loading configuration"));
    INIReader reader("config.cfg");
	token = reader.Get("bot", "token", "null");
	script = reader.Get("script", "amx", "test");
	script = script + ".amx";
	logprintf((char*)OBFUSCATE("[Telegram-Pawn] : Script : %s"), script.c_str());
	TgBot::Bot bot = TgBot::Bot(token);
	LoadScript((char*)script.c_str());
	
	bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
		if (!message->text.empty())
		{
			if (StringTools::startsWith(message->text, "/"))
			{
				
			}
			else
			{
				int idx;
				cell ret = 1;
				int orig_strlen = strlen((char*)message->text.c_str()) + 1;
				if (!amx_FindPublic(&m_amx, "OnTextSend", &idx))
				{
					cell amx_addr, * phys_addr;
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)message->text.c_str(), 0, 0);
					if (message->forwardFrom)
					{
						amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->forwardFrom->id).c_str(), 0, 0);
						amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->forwardFromMessageId).c_str(), 0, 0);
					}
					else
					{
						amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(-1).c_str(), 0, 0);
						amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(-1).c_str(), 0, 0);
					}
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->messageId).c_str(), 0, 0);
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->from->id).c_str(), 0, 0);
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->chat->id).c_str(), 0, 0);
					amx_Exec(&m_amx, &ret, idx);
					//amx_GetString((char*)message->text.c_str(), phys_addr, 0, orig_strlen);
					amx_Release(&m_amx, amx_addr);
				}
			}
		}
		if (message->audio)
		{
			int idx;
			cell ret = 1;
			int orig_strlen = strlen((char*)message->text.c_str()) + 1;
			if (!amx_FindPublic(&m_amx, "OnAudioSend", &idx))
			{
				cell amx_addr, * phys_addr;
				amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)message->caption.c_str(), 0, 0);
				amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)message->audio->fileId.c_str(), 0, 0);
				amx_Push(&m_amx, (cell)message->audio->fileSize);
				amx_Push(&m_amx, (cell)message->audio->duration);
				if (message->forwardFrom)
				{
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->forwardFrom->id).c_str(), 0, 0);
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->forwardFromMessageId).c_str(), 0, 0);
				}
				else
				{
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(-1).c_str(), 0, 0);
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(-1).c_str(), 0, 0);
				}
				amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->messageId).c_str(), 0, 0);
				amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->from->id).c_str(), 0, 0);
				amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->chat->id).c_str(), 0, 0);
				amx_Exec(&m_amx, &ret, idx);
				amx_Release(&m_amx, amx_addr);
			}
		}
		if (message->video)
		{
			int idx;
			cell ret = 1;
			int orig_strlen = strlen((char*)message->text.c_str()) + 1;
			if (!amx_FindPublic(&m_amx, "OnVideoSend", &idx))
			{
				cell amx_addr, * phys_addr;
				amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)message->caption.c_str(), 0, 0);
				amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)message->video->fileId.c_str(), 0, 0);
				amx_Push(&m_amx, (cell)message->video->fileSize);
				amx_Push(&m_amx, (cell)message->video->duration);
				if (message->forwardFrom)
				{
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->forwardFrom->id).c_str(), 0, 0);
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->forwardFromMessageId).c_str(), 0, 0);
				}
				else
				{
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(-1).c_str(), 0, 0);
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(-1).c_str(), 0, 0);
				}
				amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->messageId).c_str(), 0, 0);
				amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->from->id).c_str(), 0, 0);
				amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->chat->id).c_str(), 0, 0);
				amx_Exec(&m_amx, &ret, idx);
				amx_Release(&m_amx, amx_addr);
			}
		}
		if (message->sticker)
		{
			int idx;
			cell ret = 1;
			int orig_strlen = strlen((char*)message->text.c_str()) + 1;
			if (!amx_FindPublic(&m_amx, "OnStickerSend", &idx))
			{
				cell amx_addr, * phys_addr;
				amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)message->sticker->fileId.c_str(), 0, 0);
				amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)message->sticker->setName.c_str(), 0, 0);
				amx_Push(&m_amx, (cell)message->sticker->width);
				amx_Push(&m_amx, (cell)message->sticker->height);
				amx_Push(&m_amx, (cell)message->sticker->isAnimated);
				if (message->forwardFrom)
				{
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->forwardFrom->id).c_str(), 0, 0);
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->forwardFromMessageId).c_str(), 0, 0);
				}
				else
				{
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(-1).c_str(), 0, 0);
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(-1).c_str(), 0, 0);
				}
				amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->messageId).c_str(), 0, 0);
				amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->from->id).c_str(), 0, 0);
				amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->chat->id).c_str(), 0, 0);
				amx_Exec(&m_amx, &ret, idx);
				amx_Release(&m_amx, amx_addr);
			}
		}
	});
	bot.getEvents().onUnknownCommand([&bot](TgBot::Message::Ptr message) {
		int idx;
		cell ret = 1;	// DEFAULT TO 1!
		if (message->text.length() < 120)
		{
			int orig_strlen = strlen((char*)message->text.c_str()) + 1;
			if (!amx_FindPublic(&m_amx, "OnCommand", &idx))
			{
				cell amx_addr, * phys_addr;
				amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)message->text.c_str(), 0, 0);
				if (message->forwardFrom)
				{
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->forwardFrom->id).c_str(), 0, 0);
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->forwardFromMessageId).c_str(), 0, 0);
				}
				else
				{
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(-1).c_str(), 0, 0);
					amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(-1).c_str(), 0, 0);
				}
				amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->messageId).c_str(), 0, 0);
				amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->from->id).c_str(), 0, 0);
				amx_PushString(&m_amx, &amx_addr, &phys_addr, (char*)std::to_string(message->chat->id).c_str(), 0, 0);
				amx_Exec(&m_amx, &ret, idx);
				amx_GetString((char*)message->text.c_str(), phys_addr, 0, orig_strlen);
				amx_Release(&m_amx, amx_addr);
			}
		}
	});
	signal(SIGINT, [](int s) {
		printf("SIGINT got\n");
		exit(0);
		});

	std::thread what_thread_is_this(longPollThread, bot);
	what_thread_is_this.join();
	return 0;
}
bool LoadScript(char* pFileName)
{
	logprintf((char*)"[%s] : Loading %s", pFileName, pFileName);
	FILE* f = fopen(pFileName, "rb");
	if (!f) return false;
	fclose(f);
	strcpy(amxFile, pFileName);
	logprintf((char*)"[%s] : Loading program", pFileName);
	int err = aux_LoadProgram(&m_amx, pFileName);
	if (err != AMX_ERR_NONE)
	{
		logprintf((char*)"[%s] : Failed to load script.", pFileName);
		AMXPrintError(&m_amx, err);
		return false;
	}
	logprintf((char*)"[%s] : AMX Initing", pFileName);
	amx_CoreInit(&m_amx);
	amx_FloatInit(&m_amx);
	amx_StringInit(&m_amx);
	amx_FileInit(&m_amx);
	amx_TimeInit(&m_amx);
	amx_TelegramPawnInit(&m_amx);
	int tmp;
	if (!amx_FindPublic(&m_amx, "OnInit", &tmp))
		amx_Exec(&m_amx, (cell*)&tmp, tmp);
	cell ret = 0;
	err = amx_Exec(&m_amx, &ret, AMX_EXEC_MAIN);
	if (err != AMX_ERR_NONE)
	{
		AMXPrintError(&m_amx, err);
	}
	logprintf((char*)"[%s] : Script %s loaded", pFileName, pFileName);
	return true;
}
FILE* pLogFile;
void LoadLogFile()
{
	int reload = 0;
	if (pLogFile)
	{
		fclose(pLogFile);
		reload = 1;
	}
	pLogFile = fopen("telegram_pawn.log", "a");
	if (pLogFile)
	{
		if (reload) logprintf((char*)"Reloaded log file.");
		else logprintf((char*)"Loaded log file.");
	}
	else {
		logprintf((char*)"Failed to load log file.");
	}
}
void logprintf(char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	char buffer[512];
	vsprintf(buffer, format, ap);
	va_end(ap);
	if (pLogFile) {
		const struct tm* tm;
		time_t now;
		now = time(NULL);
		tm = localtime(&now);
		char* s;
		s = new char[256];
		strftime(s, 256, "[%H:%M:%S]", tm);
		fprintf(pLogFile, "%s %s\n", s, buffer);
		delete[] s;
		fflush(pLogFile);
	}
	puts(buffer);
}
char* Gets(char* str, int num)
{
	fgets(str, num, stdin);
	if (str[0] == '\n' || str[0] == '\r')
		str[0] = 0;

	size_t len = strlen(str);
	if (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r'))
		str[len - 1] = 0;
	if (len > 1 && (str[len - 2] == '\n' || str[len - 2] == '\r'))
		str[len - 2] = 0;

	return str;
}