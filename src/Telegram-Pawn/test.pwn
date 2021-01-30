#include "telegramPawn.inc"
#include "matheval.inc"
main()
{

}

public OnInit()
{
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
	//SendVideo(const id[], const url[], .supportsStreaming=1)
	return 1;
}

CMD:start(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[])
{
	new string[1024];
	format(string, sizeof(string), "Welcome to Pawn-Telegram testing robot\nChat ID : %s\nYour ID : %s\nMy commands :\n/id\n/myid\n/typing\n/calculate [what to calculate]\n/video [URL]\n/photo [URL]\n/forward\n/audio [URL]", chatId, fromId);
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

CMD:forward(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[])
{
	ForwardMessage(chatId, chatId, messageId);
	return 1;
}