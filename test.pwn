#include "telegramPawn.inc"
#include "matheval.inc"
main()
{

}
new whenStart;
public OnInit()
{
	SetOwner(571070896);
	whenStart = GetTickCount();
	return 1;
}

public OnExit()
{
	return 1;
}
public OnVideoSend(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], duration, fileSize, fileId[], caption[])
{
	new token[64], string[1024];
	GetToken(token, sizeof(token));
	format(string, sizeof(string), "https://api.telegram.org/bot%s/getFile?file_id=%s", token, fileId);
	print(string);
//	SendVideo(chatId, fileId, .supportsStreaming=1);
	return 1;
}

CMD:start(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[])
{
	new string[1024];
	format(string, sizeof(string), "Welcome to Pawn-Telegram testing robot\nChat ID : %s\nYour ID : %s\nMy commands :\n/id\n/myid\n/typing\n/link\n/uploading\n/uptime\n/calculate [what to calculate]\n/video [URL]\n/photo [URL]\n/forward\n/audio [URL]", chatId, fromId);
	SendMessage(chatId, string, .replyToMessageId = strval(messageId));
	return 1;
}

CMD:id(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[])
{
	new string[32];
	format(string, sizeof(string), "ID : %s", chatId);
	SendMessage(chatId, string, .replyToMessageId = strval(messageId));
	return 1;
}

CMD:myid(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[])
{
	new string[32];
	format(string, sizeof(string), "ID : %s", fromId);
	SendMessage(chatId, string, .replyToMessageId = strval(messageId));
	return 1;
}

CMD:calculate(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[])
{
	if(strlen(params)>21)
	{
		return SendMessage(chatId, "Sorry, I can't calculate that.", .replyToMessageId=strval(messageId));
	}
	SendMessage(chatId, "Calculating...");
	new string[24];
	format(string, sizeof(string), "%f", MathEval(params));
	if(!strcmp(string, "./,),(-*,(.((((((", true))
	{
		return SendMessage(chatId, "Sorry, I can't calculate that.", .replyToMessageId=strval(messageId));
	}
	SendMessage(chatId, string, .replyToMessageId = strval(messageId));
	return 1;
}

CMD:video(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[])
{
	if(strcmp(params, "http", true, 4) != 0)
	{
		return SendMessage(chatId, "Wrong URL.", .replyToMessageId=strval(messageId));
	}
	SendVideo(chatId, params, .caption="Bia", .replyToMessageId=strval(messageId));
	return 1;
}

CMD:image(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[])
{
	if(strcmp(params, "http", true, 4) != 0)
	{
		return SendMessage(chatId, "Wrong URL.", .replyToMessageId=strval(messageId));
	}
	SendPhoto(chatId, params, "Bieh", strval(messageId));
	return 1;
}
CMD:audio(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[])
{
	if(strcmp(params, "http", true, 4) != 0)
	{
		return SendMessage(chatId, "Wrong URL.", .replyToMessageId=strval(messageId));
	}
	//printf("Link : %s", params);
	SendAudio(chatId, params, "Bieh", .replyToMessageId=strval(messageId));
	return 1;
}

CMD:typing(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[])
{
	SendChatAction(chatId, "typing");
	return 1;
}
CMD:uploading(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[])
{
	SendChatAction(chatId, "upload_document");
	return 1;
}
CMD:forward(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[])
{
	ForwardMessage(chatId, chatId, messageId);
	return 1;
}

CMD:link(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[])
{
	new link[64] = "Error";
	ExportChatInviteLink(chatId, link, sizeof(link));
	SendMessage(chatId, link, .replyToMessageId=strval(messageId));
	return 1;
}

CMD:ban(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[], replyToMessageId[], replyToUserId[])
{
	if(strcmp(fromId, "571070896", true) != 0) return 1;
	KickChatMember(chatId, replyToUserId);
	new string[64];
	format(string, sizeof(string), "Banned %s", replyToUserId);
	SendMessage(chatId, string, .replyToMessageId=strval(replyToMessageId));
	return 1;
}
CMD:unban(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[], replyToMessageId[], replyToUserId[])
{
	if(strcmp(fromId, "571070896", true) != 0) return 1;
	UnbanChatMember(chatId, replyToUserId);
	new string[64];
	format(string, sizeof(string), "Unbanned %s", replyToUserId);
	SendMessage(chatId, string, .replyToMessageId=strval(replyToMessageId));
	return 1;
}

CMD:mute(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[], replyToMessageId[], replyToUserId[])
{
	if(strcmp(fromId, "571070896", true) != 0) return 1;
	//RestrictChatMember(chatId, replyToUserId, r_canSendMessages, 0);
	new string[64];
	format(string, sizeof(string), "Muted %s", replyToUserId);
	SendMessage(chatId, string, .replyToMessageId=strval(replyToMessageId));
	return 1;
}
CMD:unmute(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[], replyToMessageId[], replyToUserId[])
{
	if(strcmp(fromId, "571070896", true) != 0) return 1;
	//RestrictChatMember(chatId, replyToUserId, r_canSendMessages, 1);
	new string[64];
	format(string, sizeof(string), "Unmuted %s", replyToUserId);
	SendMessage(chatId, string, .replyToMessageId=strval(replyToMessageId));
	return 1;
}
CMD:uptime(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[], replyToMessageId[], replyToUserId[])
{
	new string[128];
	format(string, sizeof(string), "Bot uptime : %s", ConvertSeconds((GetTickCount()-whenStart) / 1000));
	SendMessage(chatId, string, .replyToMessageId=strval(replyToMessageId));
	return 1;
}
stock ConvertSeconds(n) 
{ 
    new day = n / (24 * 3600); 
    n = n % (24 * 3600); 
    new hour = n / 3600; 
    n %= 3600; 
    new minutes = n / 60 ; 
    n %= 60; 
    new seconds = n; 
    new time[64];
    if(day)
    	format(time, sizeof(time), "%02d:%02d:%02d:%02d", day, hour, minutes, seconds);
    else if(hour)
    	format(time, sizeof(time), "%02d:%02d:%02d", hour, minutes, seconds);
    else if(minutes)
    	format(time, sizeof(time), "%02d:%02d", minutes, seconds);
    else if(seconds)
    	format(time, sizeof(time), "%02d", seconds);
    return time;
}