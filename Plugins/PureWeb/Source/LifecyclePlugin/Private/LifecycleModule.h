#pragma once

#include "CoreMinimal.h"
#include "GenericPlatform/GenericApplicationMessageHandler.h"
#include "Features/IModularFeatures.h"
#include "Features/IModularFeature.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "Runtime/Online/HTTP/Public/HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "UObject/Class.h"
#include "HAL/Event.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/IConsoleManager.h"
#include "PixelStreamerInputComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Dom/JsonObject.h"
#include "Templates/SharedPointer.h"


#include "LifecycleModule.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(PureWebPlatform, Log, All);

const FString RequestProcessInfo = "RequestProcessInfo";
const FString RequestShutdown = "RequestShutdown";
const FString OnAgentReady = "OnAgentReady";
const FString UIInteraction = "50";
const FString BrowserMessage = "BrowserMessage";

// TODO: PWPLAT-655. More work is required around messages, types, etc.
const int OnWindowResize = 1;

UCLASS()
class UPureWebPlatformRequestHandler : public UActorComponent
{
	GENERATED_BODY()
public:
	UPureWebPlatformRequestHandler(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION()
	void HandleAppServerMessage(const FString& Descriptor);

	void HandleProcessInfoRequest();
	void HandleOnAgentReady();
	void HandleShutdownRequest();
	void HandleWindowResize(TSharedPtr<FJsonObject> jsonMsg);

	void OnGameModePostLogin(AGameModeBase* GameMode, APlayerController* NewPlayer);
	void OnGameModeLogout(AGameModeBase* GameMode, AController* Exiting);

	// Scheduler communication
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
private:
	UPixelStreamerInputComponent* InputComponent;
	FScriptDelegate HandleAppServerMessageDelegate;
};

class FLifecycleModule : public IModuleInterface, public IModularFeature
{
public:
	void StartupModule();
	void ShutdownModule();
	bool IsInitialized() const;

};




