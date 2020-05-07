// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketObject.h"



USocketObject::USocketObject(const FObjectInitializer& ObjectInitializer)
{

}





void USocketObject::BeginDestroy()
{
	Super::BeginDestroy();
	Close();
}

bool USocketObject::Create(const FString& IP, bool bServer, int32 Port, int32 ReceiveSize, int32 SendSize)
{
	this->SendDataSize = SendSize;
	this->RecDataDize = ReceiveSize;

	if (bServer)
	{
		FIPv4Address ServerAddr;
		if (!FIPv4Address::Parse(IP, ServerAddr))
		{
			UE_LOG(LogTemp, Error, TEXT("Server Ip %s is illegal"), *IP);
		}
		Socket = FTcpSocketBuilder(TEXT("Socket Listener"))
			.AsReusable()
			.AsBlocking()
			.BoundToAddress(ServerAddr)
			.BoundToPort(Port)
			.Listening(8)
			.WithReceiveBufferSize(SendDataSize)
			.WithSendBufferSize(RecDataDize);
	}
	else
	{
		Socket = FTcpSocketBuilder(TEXT("Client Socket"))
			.AsReusable()
			.AsBlocking()
			.WithReceiveBufferSize(ReceiveSize)
			.WithSendBufferSize(SendSize);
	}
	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("Create  Socket Error!"));
		return false;
	}

	if (bServer)
	{
		GetWorld()->GetTimerManager().SetTimer(ConnectCheckHandler, this, &USocketObject::ConnectTickCheck, 1, true);
	}
	else
	{
		return ConnectServer(IP, Port);
	}
	return true;
}

void USocketObject::SendData(FString Message)
{
	for (auto SocketThread : RecThreads)
	{
		SocketThread->SendData(Message);
	}
}

void USocketObject::Close()
{
	if (Socket)
	{
		for (auto RecThreald : RecThreads)
		{
			RecThreald->Stop();
		}
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
		Socket = nullptr;
	}
	if (RecSocket)
	{
		RecSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(RecSocket);
		RecSocket = nullptr;
	}
}

void USocketObject::ConnectTickCheck()
{
	bool bPending = false;
	if (Socket->HasPendingConnection(bPending) && bPending)
	{
		TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		RecSocket = Socket->Accept(*RemoteAddress, TEXT("Receive Socket"));
		USocketRSThread* RSThread = NewObject<USocketRSThread>();
		RecThreads.Add(RSThread);
		RSThread->ReceiveSocketDataDelegate = ReceiveSocketDataDelegate;
		RSThread->LostConnectionDelegate.AddDynamic(this, &USocketObject::OnDisConnected);
		RSThread->StartThread(RecSocket, SendDataSize, RecDataDize);
		ConnectReceiveDelegate.Broadcast(RemoteAddress->ToString(false), RemoteAddress->GetPort());
	}
}

void USocketObject::OnDisConnected(USocketRSThread* pThread)
{
	UE_LOG(LogTemp, Warning, TEXT("Client lost"));
	RecThreads.Remove(pThread);
}

bool USocketObject::ConnectServer(FString ServerIP, int32 Port)
{
	FIPv4Endpoint ServerEndpoint;
	FIPv4Endpoint::Parse(ServerIP, ServerEndpoint);
	TSharedPtr<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool Success = true;
	addr->SetIp(*ServerIP, Success);
	if (!Success)
	{
		return false;
	}
	addr->SetPort(Port);

	if (Socket->Connect(*addr))
	{
		USocketRSThread* RSThread = NewObject<USocketRSThread>();
		RecThreads.Add(RSThread);
		RSThread->ReceiveSocketDataDelegate = ReceiveSocketDataDelegate;
		RSThread->LostConnectionDelegate.AddDynamic(this, &USocketObject::OnDisConnected);
		RSThread->StartThread(Socket, SendDataSize, RecDataDize);
		UE_LOG(LogTemp, Warning, TEXT("Client Connect Success"));
		return true;
	}
	else
	{
		ESocketErrors LastErr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLastErrorCode();

		UE_LOG(LogTemp, Warning, TEXT("Connect failed with error code (%d) error (%s)"), LastErr, ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetSocketError(LastErr));
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
	}
	return false;
}

