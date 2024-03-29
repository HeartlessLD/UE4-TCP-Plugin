// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SocketRSThread.h"
#include "Common/TcpSocketBuilder.h"
#include "SocketObject.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FClientConnectDelegate, FString, RemoteIP, int32, RemotePort);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FConnectedResultDelegate, bool, bSuccess);
/**
 * 
 */
UCLASS(BlueprintType)
class SIMPLETCPCS_API USocketObject : public UObject
{
	GENERATED_BODY()

public:
	USocketObject(const FObjectInitializer& ObjectInitializer);
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = Network)
		bool Create(const FString& IP, bool bServer, int32 Port, int32 ReceiveSize = 1024, int32 SendSize = 1024);

	UFUNCTION(BlueprintCallable, Category = Network)
		void SendData(FString Message);

	UFUNCTION(BlueprintCallable, Category = Network)
		void Close();

protected:
	/** Server */
	void ConnectTickCheck();
	UFUNCTION()
		void OnDisConnected(USocketRSThread* pThread);

	/** Client */
	UFUNCTION(BlueprintCallable)
	void ConnectServer(FString IP, int32 Port);
	//重连服务器
	UFUNCTION(BlueprintCallable)
		void ReconnectServer();
protected:
	class FSocket* Socket;
	bool bShutDown;
	bool bConnecting;
	int32 SendDataSize;
	int32 RecDataDize;

	UPROPERTY()
		TArray<class USocketRSThread*> RecThreads;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, Category = Network)
		FReceiveSocketDataDelegate ReceiveSocketDataDelegate;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, Category = Network)
        FConnectedResultDelegate ConnectedResultDelegate;

	/** Server */
	FSocket* RecSocket;
	FString ServerIP;
	int32 ServerPort;
	FTimerHandle ConnectCheckHandler;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, Category = Network)
	FClientConnectDelegate ClientConnectDelegate;
	
};
