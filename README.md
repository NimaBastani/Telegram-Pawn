# Telegram-Pawn
Made Telegram bots using pawn language


## Sample

Simple code which sends every text message it receives :
```cpp
#include "telegramPawn.inc"

main() { }
public OnInit()
{
	//ClearConsole();
	print("Initing...");
	return 1;
}
public OnTextSend(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], text[])
{
	printf("%s : %s", chatId, text);
	new message[2048];
	format(message, sizeof(message), "You said : %s", text);
	SendMessage(fromId, message);
	return 1;
}
```

Simple command :
```cpp
#include "telegramPawn.inc"

main() { }
public OnInit()
{
	//ClearConsole();
	print("Initing...");
	return 1;
}
CMD:hi(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], params[])
{
	new string[512];
	format(string, sizeof(string), "Hello there ! : %s", params);
	SendMessage(chatId, string);
	SendPhoto(chatId, "https://telegram.org/img/t_logo.png?1", "Hmmmm");
  return 1;
}
```


## Functions and callbacks
| Callback | Description | Usage |
| --- | --- | --- |
| OnInit | This callback is triggered when the script starts.  | OnInit() |
| OnExit | This callback is triggered when the script ends.  | OnExit() |
| OnCommand | This callback is triggered when send a command  | OnCommand(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], text[]) |
| OnTextSend | This callback is triggered when send a (text) message  | OnTextSend(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], text[]) |
| OnAudioSend | This callback is triggered when send a (audio)  | OnAudioSend(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], duration, fileSize, fileId[], caption[])) |
| OnVideoSend | This callback is triggered when send a (video)  | OnVideoSend(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], duration, fileSize, fileId[], caption[])) |
| OnStickerSend | This callback is triggered when send a (sticker)  | OnStickerSend(chatId[], fromId[], messageId[], forwardFromMessageId[], forwardFromId[], isAnimated, height, width, setName[], fileId[]) |


| --- | Type | Description | 
| --- | --- | --- |
| chatId | string  | Id of the channel/group/user | 
| fromId | string  | Id of the user who send the message  | 
| messageId | string  | Id of the message  | 
| forwardFromMessageId | string  | If the message is not forwarded it will be "-1", id of the forwarded message  | 
| forwardFromId | string  | If the message is not forwarded it will be "-1", id of the forwarded chat  | 
| fileId | string  | Id of the file  | 
| caption | string  | caption of the message (Image/Video/Audio)  | 
| setName | string  | Name of the sticker set  | 
| duration | int  | duration of the video/audio  | 
| height | int  | height of the video/sticker/video  | 
| width | int  | width of the video/sticker/video  | 
| fileSize | int  | size of the file  | 
| isAnimated | bool  | is the sticker animated  | 

## Licence
[The MIT License](https://github.com/NimaBastani/Telegram-Pawn/blob/main/LICENSE).
