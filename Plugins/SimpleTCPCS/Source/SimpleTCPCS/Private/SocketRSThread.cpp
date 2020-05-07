// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketRSThread.h"
#include "SocketSubsystem.h"

uint32 USocketRSThread::Run()
{
	while (!bThreadStop && ConnectSocket)
	{
		uint32 Size;
		bool LostConnect = false;
		ConnectSocket->HasPendingConnection(LostConnect);
		ConnectSocket->Wait(ESocketWaitConditions::WaitForReadOrWrite, FTimespan(0, 0, 5));
		if (LostConnect)
		{
			UE_LOG(LogTemp, Warning, TEXT(" Connect lost "));
			Stop();
			LostConnectionDelegate.Broadcast(this);
			continue;
		}

		
		if (ConnectSocket && ConnectSocket->HasPendingData(Size))
		{
			ReceiveData.Init(0, FMath::Min(Size, RecDataSize));
			int32 Readed;
			ConnectSocket->Recv(ReceiveData.GetData(), RecDataSize, Readed);
			FString ReceivedString = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ReceiveData.GetData())));
			if (ReceiveSocketDataDelegate.IsBound())
			{
				ReceiveSocketDataDelegate.Broadcast(ReceivedString);
			}
			
		}
	}
	return 0;
}

void USocketRSThread::StartThread(FSocket* Socket, uint32 SizeSend, uint32 SizeRec)
{
	this->ConnectSocket = Socket;
	this->SendDataSize = SizeSend;
	this->RecDataSize = SizeRec;
	FRunnableThread::Create(this, TEXT("Receive Threald"));
}

void USocketRSThread::Stop()
{
	bThreadStop = true;
	
}

void USocketRSThread::SendData(FString Message)
{

	TCHAR* SendMessage = Message.GetCharArray().GetData();
	int32 size = FCString::Strlen(SendMessage) + 1;
	int32 sent = 0;
	if (size >= (int32)SendDataSize)
	{
		UE_LOG(LogTemp, Error, TEXT("Send Data Size is Larger than Max Size for set"));
	}
	else
	{
		if (ConnectSocket && ConnectSocket->Send((uint8*)TCHAR_TO_UTF8(SendMessage), size, sent))
		{
			UE_LOG(LogTemp, Warning, TEXT("___Send Succeed!"));

		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("___Send Failed!"));
		}
	}


}