// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "C:/Program Files/Epic Games/UE_5.3/Engine/Plugins/Runtime/OSC/Source/OSC/Public/OSCServer.h"
#include "C:/Program Files/Epic Games/UE_5.3/Engine/Plugins/Runtime/OSC/Source/OSC/Public/OSCManager.h"
#include "KickClay_PlayerController.h"
#include "OSCReceiver.generated.h"

class UOSCServer;
class UOSCManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHokuyoDataReceived, const FVector&, HokuyoData, const FString&, IPAddress);

UCLASS()
class XR_KICKCLAY_V2_API AOSCReceiver : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOSCReceiver();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float PosX;
	float PosY;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnRightDataOSC(const FOSCAddress& AddressPattern, const FOSCMessage& Message, const FString& IPAddress, int32 Port);

	// UFUNCTION()
	// void OnOSCDataReceived(const FOSCAddress& Address, const TArray<float>& Data);
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "OSC")
	int32 InPort = 7000;
	
	UPROPERTY()
	UOSCServer* OSCServer;

	UPROPERTY()
	UOSCManager* OSCManager;

	UPROPERTY(EditAnywhere, Category = "OSC")
	AKickClay_PlayerController* KickClay_PlayerController;


	UPROPERTY(BlueprintAssignable, Category = "OSC")
	FOnHokuyoDataReceived OnHokuyoDataReceived;
	
	UFUNCTION(BlueprintCallable, Category = "OSC")
	TArray<float> GetHokuyoInputXY();
};







//-----------------------------

// Fill out your copyright notice in the Description page of Project Settings.


#include "OSCReceiver.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


// Sets default values
AOSCReceiver::AOSCReceiver()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AOSCReceiver::BeginPlay()
{
	Super::BeginPlay();
	OSCManager = NewObject<UOSCManager>();

	if (OSCManager)
	{
		OSCServer = OSCManager->CreateOSCServer(FString("0.0.0.0"), InPort, false, true, FString("OSCReceiverServer"), this);
		if (OSCServer)
		{
			FOSCAddress Address("/Front/Data");
			FOSCDispatchMessageEventBP Delegate;
			Delegate.BindUFunction(this, FName("OnRightDataOSC"));
			OSCServer->BindEventToOnOSCAddressPatternMatchesPath(Address, Delegate);
			UE_LOG(LogTemp, Display, TEXT("OSC ServerDone!"));  //연결 확인

		}
		UE_LOG(LogTemp, Display, TEXT("OSC Done!"));
		DrawDebugSphere(GetWorld(), GetActorLocation(), 20, 20, FColor::Red, true);
	}

	KickClay_PlayerController = Cast<AKickClay_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (!KickClay_PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fail to find AKICKCLAY_PlayerController in OSCReceiver!"));
	}

}

void AOSCReceiver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOSCReceiver::OnRightDataOSC(const FOSCAddress& AddressPattern, const FOSCMessage& Message, const FString& IPAddress, int32 Port)
{
	TArray<float> XYs;
	OSCManager->GetAllFloats(Message, XYs);

	if (XYs.Num() >= 2 && KickClay_PlayerController)
	{
		PosX = XYs[0];
		PosY = XYs[1];
		FVector SensorData(PosX, PosY, 0.0f);

		OnHokuyoDataReceived.Broadcast(SensorData, IPAddress);
		// KickClay_PlayerController->SetHokuyoInputXY(SensorData);
		UE_LOG(LogTemp, Warning, TEXT("Right OSC Data PosX: %f, PosY:%f"), PosX,PosY);


		//여기 추가해서 넣어봄 10.20.02:08
		// FInputActionValue Value(1.0f);
		// KickClay_PlayerController->OnHokuyoInputOngoing(Value);
		// KickClay_PlayerController->OnHokuyoInput(Value);
		// Kick
	}
}

TArray<float> AOSCReceiver::GetHokuyoInputXY()
{
	TArray<float> HokuyoXY;
	HokuyoXY.Add(PosX);
	HokuyoXY.Add(PosY);
	return HokuyoXY;
}
