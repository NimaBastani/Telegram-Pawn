#if defined _telegramPawn
	#endinput
#endif
#define _telegramPawn
#pragma library telegramPawn

#pragma tabsize 4

#include <core>
#include <float>
#include <string>
#include <file>
#include <time>
//D:\Code\openssl\lib
//D:\Code\openssl\lib\VC\static
//D:\Code\11-C++-MyThings\Telegram-Pawn\Telegram-Pawn\
//D:\Code\11-C++-MyThings\Telegram-Pawn\packages\boost.1.72.0.0\lib\native\include
// ----------------------- Natives -----------------------
native print(const string[]);
native printf(const format[], {Float,_}:...);
native format(output[], len, const format[], {Float,_}:...);
native CallLocalFunction(const function[], const format[], {Float,_}:...);
native ExecCommand(const cmd[]);

native GetToken(const token[], len);
native SendMessage(const id[], const text[], disableWebPreview=0, replyToMessageId=0);
native SendPhoto(const id[], const url[], const caption[]="", replyToMessageId=0);
native SendVideo(const id[], const url[], supportsStreaming=1, duration=0, width=0, height=0, const caption[]="", replyToMessageId=0);
native SetOwner(id);
native ClearConsole();
// ----------------------- Callbacks -----------------------
forward OnCommand(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], text[]);
forward OnTextSend(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], text[]);
forward OnAudioSend(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], duration, fileSize, fileId[], caption[]);
forward OnVideoSend(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], duration, fileSize, fileId[], caption[]);
forward OnStickerSend(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], isAnimated, height, width, setName[], fileId[]);
forward OnInit();
forward OnExit();

// ----------------------- Definitions -----------------------
#define INVALID_CHAT_ID (-1) 
#define COMMAND:%1(%2)          \
			forward cmd_%1(%2); \
			public cmd_%1(%2)
#define CMD:%1(%2) \
			COMMAND:%1(%2)
#define command(%1,%2,%3) \
			COMMAND:%1(%2, %3)
#define cmd(%1,%2,%3) \
			COMMAND:%1(%2, %3)

// ----------------------- Operations -----------------------
public OnCommand(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], text[])
{
    new
        pos,
        funcname[512];
    while (text[++pos] > ' ')
	{
		funcname[pos-1] = tolower(text[pos]);
	}
	format(funcname, sizeof(funcname), "cmd_%s", funcname);
	printf("%s : %s : %s : %d", chatId, text, funcname);
    while (text[pos] == ' ') pos++;
    //(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[])
	if (!text[pos])
	{
		return CallLocalFunction(funcname, "ssssss", chatId, fromId, messageId, forwardFromMessageId, forwardFromId, "\1");
	}
	return CallLocalFunction(funcname, "ssssss", chatId, fromId, messageId, forwardFromMessageId, forwardFromId, text[pos]);
}