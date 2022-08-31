// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "UObject/NoExportTypes.h"
#include "SocketRSThread.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReceiveSocketDataDelegate, FString, Data); //Receive Connect Callback
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLostConnectionDelegate, USocketRSThread*, Thread);	//DisConnect Callback
/**
 * 
 */
UCLASS()
class SIMPLETCPCS_API USocketRSThread : public UObject, public FRunnable
{
	GENERATED_BODY()
public:
	virtual void BeginDestroy() override;
	//FRunnable Interface
	virtual bool Init() override { return true; }
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override {}
	//End Interface
	

	void StartThread(FSocket* Socket, uint32 SizeSend, uint32 SizeRec);
	void SendData(FString Data);

public:
	FReceiveSocketDataDelegate ReceiveSocketDataDelegate;
	FLostConnectionDelegate	LostConnectionDelegate;

protected:
	FSocket* ConnectSocket;
	uint32 MaxSendDataSize;
	uint32 MaxRecDataSize;
	TArray<uint8> ReceiveData;
	FRunnableThread* pThread;
	bool bThreadStop;
};
