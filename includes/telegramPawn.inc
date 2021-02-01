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

// ----------------------- Natives -----------------------
native print(const string[]);
native printf(const format[], {Float,_}:...);
native format(output[], len, const format[], {Float,_}:...);
native CallLocalFunction(const function[], const format[], {Float,_}:...);
native GetTickCount();
native ExecCommand(const cmd[]);

native GetToken(const token[], len);
native SendMessage(const id[], const text[], disableWebPreview=0, replyToMessageId=0);
native SendPhoto(const id[], const url[], const caption[]="", replyToMessageId=0);
native SendVideo(const id[], const url[], supportsStreaming=1, duration=0, width=0, height=0, const caption[]="", replyToMessageId=0);
native SendAudio(const id[], const url[], const caption[]="", duration=0, const performer[]=" ", const title[]=" ", replyToMessageId=0);
native RestrictChatMember(const chatId[], const userId[], permission, toggle);
native KickChatMember(const chatId[], const userId[]);
native UnbanChatMember(const chatId[], const userId[]);
native ExportChatInviteLink(const chatId[], const link[], len);

native ForwardMessage(const id[], const fromChatId[], const messageId[], disableNotification=0);
//Type of action to broadcast. Choose one, depending on what the user is about to receive: typing for text messages, upload_photo for photos, record_video or upload_video for videos, record_voice or upload_voice for voice notes, upload_document for general files, find_location for location data, record_video_note or upload_video_note for video notes.
native SendChatAction(const id[], const action[]);


native SetOwner(id);
native ClearConsole();
// ----------------------- Callbacks -----------------------
forward OnCommand(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], text[], replyToMessageId[], replyToUserId[]);
forward OnTextSend(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], text[], replyToMessageId[], replyToUserId[]);
forward OnAudioSend(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], duration, fileSize, fileId[], caption[], replyToMessageId[], replyToUserId[]);
forward OnVideoSend(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], duration, fileSize, fileId[], caption[], replyToMessageId[], replyToUserId[]);
forward OnStickerSend(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], isAnimated, height, width, setName[], fileId[], replyToMessageId[], replyToUserId[]);
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

#define r_canAddWebPagePreviews 0
#define r_canChangeInfo 1
#define r_canInviteUsers 2
#define r_canPinMessages 3
#define r_canSendMediaMessages 4
#define r_canSendMessages 5
#define r_canSendOtherMessages 6
#define r_canSendPolls 7

// ----------------------- Operations -----------------------
public OnCommand(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], text[], replyToMessageId[], replyToUserId[])
{
    new
        pos,
        funcname[512];
    while (text[++pos] > ' ')
	{
		funcname[pos-1] = tolower(text[pos]);
	}
	format(funcname, sizeof(funcname), "cmd_%s", funcname);
	printf("%s : %s : %s", chatId, text, funcname);
    while (text[pos] == ' ') pos++;
    //(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[])
	if (!text[pos])
	{
		return CallLocalFunction(funcname, "ssssssss", chatId, fromId, messageId, forwardFromMessageId, forwardFromId, "\1", replyToMessageId, replyToUserId);
	}
	return CallLocalFunction(funcname, "ssssssss", chatId, fromId, messageId, forwardFromMessageId, forwardFromId, text[pos], replyToMessageId, replyToUserId);
}
