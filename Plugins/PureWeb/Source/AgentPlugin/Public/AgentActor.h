// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include <memory>
#include <thread>
#include "GameFramework/Actor.h"
#include <string>
#include "Collaboration/Sidecar.h"
#include "Engine/GameViewportClient.h"
#include "Engine/GameInstance.h"
#include "AgentPlatformBlueprintLibrary.h"
#include "Async/AsyncWork.h"
#include "AgentActor.generated.h"


UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class APState : uint8
{
	OK UMETA(DisplayName = "Success"),
	ERROR_ UMETA(DisplayName = "Error"),
	CONNECTED UMETA(DisplayName = "Connected"),
	DISCONNECTED UMETA(DisplayName = "Disconnected")
};



DECLARE_LOG_CATEGORY_EXTERN(AgentPlugin, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAgentConnectionDelegate, FAgentPlatformJsonObject, message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBroadcastMessageDelegate, FAgentPlatformJsonObject, message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPrivateMessagedDelegate, FAgentPlatformJsonObject, message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FArrivaldDelegate, FAgentPlatformJsonObject, message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDepartureDelegate, FAgentPlatformJsonObject, message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPresenceEventDelegate, FAgentPlatformJsonObject, message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKeyChangeDelegate, FAgentPlatformJsonObject, message);

UCLASS()
class AGENTPLUGIN_API AAgentActor : public AActor
{
	GENERATED_BODY()
private:
	//Sidecar
	PureWeb::Collaboration::Sidecar* sidecar;
	PureWeb::Collaboration::Sidecar::OnMessageReceivedHandler handler;

	static const FString DEFAULT_SIDECAR_PORT;
	static const FString DEFAULT_SIDECAR_ADDRESS;
	static const FString ENV_PIXEL_STREAMING_IP;
	static const FString ENV_SIDECAR_PORT;

	//Helpers
	FAgentPlatformJsonObject ConstructMessageObject(const char* agentId, const char* payload,
		const char* type, const char* timestamp, const char* messageId);
	FAgentPlatformJsonObject ConstructStorageObject(const char* agentId, const char* key, const char* value,
		PureWeb::Collaboration::Sidecar::StorageType type);
	FAgentPlatformJsonObject ConstructPresenceObject(const char* agentId, PureWeb::Collaboration::Sidecar::PresenceType type);
	FAgentPlatformJsonObject ConstructConnectionObject(const char* environmentId, const char* agentId);
	bool isConnected();
	
	char* CopyCharArray(const std::string input);

	APState StorePrivateScopedAsync(FString key, FString value);

public:	
	// Sets default values for this actor's properties
	AAgentActor();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	UPROPERTY(BlueprintAssignable, Category = "Pureweb Agent")
		FAgentConnectionDelegate agentConnectionDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Pureweb Agent")
		FBroadcastMessageDelegate broadcastMessageDelegate;	
	UPROPERTY(BlueprintAssignable, Category = "Pureweb Agent")
		FPrivateMessagedDelegate privateMessagetDelegate;	
	UPROPERTY(BlueprintAssignable, Category = "Pureweb Agent")
		FArrivaldDelegate arrivalDelegate;	
	UPROPERTY(BlueprintAssignable, Category = "Pureweb Agent")
		FDepartureDelegate departureDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Pureweb Agent")
		FPresenceEventDelegate presenceDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Pureweb Agent")
		FKeyChangeDelegate keyChangeDelegate;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual APState CreateAgent(AAgentActor* agentActor = nullptr);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual APState CreateAgentByURL(FString token, FString certificate, FString serverURL, FString serverPort, AAgentActor* agentActor = nullptr);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual APState SubscribeToAllMessages(AAgentActor* agentActor);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual APState SendMessage(FString message, FString type);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual APState SendDirectMessage(FString remoteAgentId, FString message, FString type);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual APState SubscribeToBroadcastMessages(AAgentActor* agentActor);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual APState SubscribeToPrivateMessages(AAgentActor* agentActor);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual APState SubscribeToArrivals(AAgentActor* agentActor);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual APState SubscribeToDepartures(AAgentActor* agentActor);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual APState SubscribeToAllPresenceEvents(AAgentActor* agentActor);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual APState Store(FString key, FString value);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual APState StoreAgentScoped(FString key, FString value, FString agentId);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual APState StorePrivateScoped(FString key, FString value);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual FString Retrieve(FString key);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual FString RetrieveAgentScoped(FString key, FString agentId);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual FString RetrievePrivateScoped(FString key);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual APState Remove(FString key);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual APState RemoveAgentScoped(FString key, FString agentId);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual APState RemovePrivateScoped(FString key);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual APState SubscribeToKey(AAgentActor* agentActor, FString keyName, FString agentId);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual APState SubscribeToKeyRemove(AAgentActor* agentActor, FString keyName, FString agentId);
	UFUNCTION(BlueprintCallable, Category = "Pureweb Agent")
		virtual APState SubscribeToKeyWrite(AAgentActor* agentActor, FString keyName, FString agentId);

};

