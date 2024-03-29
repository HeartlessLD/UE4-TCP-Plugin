﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketRSThread.h"
#include "SocketSubsystem.h"
#include <string>

void USocketRSThread::BeginDestroy()
{
	Stop();
	Super::BeginDestroy();
}

uint32 USocketRSThread::Run()
{
	while (!bThreadStop && ConnectSocket)
	{
		uint32 Size;
		bool LostConnect = false;
		ConnectSocket->HasPendingConnection(LostConnect);
		ConnectSocket->Wait(ESocketWaitConditions::WaitForReadOrWrite, FTimespan(0, 0, 5));
		if (LostConnect && !ConnectSocket->HasPendingData(Size))
		{
			ReceiveData.Init(0, 100);
			int32 Temp;
			if (!ConnectSocket->Recv(ReceiveData.GetData(), 0, Temp))
			{
				UE_LOG(LogTemp, Warning, TEXT(" Connect lost "));
				Stop();
				LostConnectionDelegate.Broadcast(this);
				continue;
			}

		}


		if (ConnectSocket && ConnectSocket->HasPendingData(Size))
		{
			int32 minSize = FMath::Min(Size, MaxRecDataSize);
			ReceiveData.Init(0, minSize + 1);
			int32 Readed;
			if (!ConnectSocket->Recv(ReceiveData.GetData(), minSize, Readed))
			{
				UE_LOG(LogTemp, Warning, TEXT(" Connect lost "));
				LostConnectionDelegate.Broadcast(this);
				continue;
			}
			FString ReceivedStr = FString(UTF8_TO_TCHAR(ReceiveData.GetData()));
			UE_LOG(LogTemp, Warning, TEXT("receive message = %s  size = %d"), *ReceivedStr, minSize);
			if (ReceiveSocketDataDelegate.IsBound())
			{
				ReceiveSocketDataDelegate.Broadcast(ReceivedStr);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT(" thread ReceiveSocketDataDelegate num is 0 "));
			}

		}
		ReceiveData.Empty();
	}
	return 0;
}

void USocketRSThread::StartThread(FSocket* Socket, uint32 SizeSend, uint32 SizeRec)
{
	this->ConnectSocket = Socket;
	this->MaxSendDataSize = SizeSend;
	this->MaxRecDataSize = SizeRec;
	FRunnableThread::Create(this, TEXT("Receive Threald"));
}

void USocketRSThread::Stop()
{
	bThreadStop = true;
	ConnectSocket = nullptr;
}

void USocketRSThread::SendData(FString Message)
{

	std::string strSend(TCHAR_TO_UTF8(*Message));
	SendDataBuffer.Init(0, strSend.size() + 1);
	memcpy(SendDataBuffer.GetData(), strSend.data(), strSend.size());
	int32 sent = 0;
	if (SendDataBuffer.Num() >= (int32)MaxSendDataSize)
	{
		UE_LOG(LogTemp, Error, TEXT("Send Data Size is Larger than Max Size for set"));
	}
	else
	{
		if (ConnectSocket && ConnectSocket->Send(SendDataBuffer.GetData(), SendDataBuffer.Num(), sent))
		{
			UE_LOG(LogTemp, Warning, TEXT("___Send Succeed! msg = %s messageSize = %d sended = %d"), *Message, SendDataBuffer.Num(), sent);

		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("___Send Failed!"));
		}
	}


}
