#include "LifecycleModule.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "Misc/Guid.h"
#include "JsonObjectConverter.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "GameFramework/GameModeBase.h"
#include "FProcessInfo.h"
#include "IPixelStreamingModule.h"
#include "PixelStreamerDelegates.h"
#include "HAL/IConsoleManager.h"
#include "PixelStreamerInputComponent.h"
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"


DEFINE_LOG_CATEGORY(PureWebPlatform)


void FLifecycleModule::StartupModule()
{
	UE_LOG(PureWebPlatform, Log, TEXT("PureWeb Platform Plugin Version: 0.1"));
}

void FLifecycleModule::ShutdownModule()
{
}

bool FLifecycleModule::IsInitialized() const
{
	return true;
}

void UPureWebPlatformRequestHandler::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (EHttpResponseCodes::IsOk(Response->GetResponseCode()) && bWasSuccessful)
	{
		UE_LOG(PureWebPlatform, Log, TEXT("Platform scheduler connected"));
		return;
	}
	else
	{
		UE_LOG(PureWebPlatform, Error, TEXT("Unable to connect to Platform scheduler: %s"), *Response->GetContentAsString());
	}
}

UPureWebPlatformRequestHandler::UPureWebPlatformRequestHandler(const FObjectInitializer& ObjectInitializer) : UActorComponent(ObjectInitializer)
{
	FGameModeEvents::GameModePostLoginEvent.AddUObject(this, &UPureWebPlatformRequestHandler::OnGameModePostLogin);
	FGameModeEvents::GameModeLogoutEvent.AddUObject(this, &UPureWebPlatformRequestHandler::OnGameModeLogout);


}
void UPureWebPlatformRequestHandler::OnGameModePostLogin(AGameModeBase* GameMode, APlayerController* NewPlayer)
{
	InputComponent = (UPixelStreamerInputComponent*)NewPlayer->GetComponentByClass(UPixelStreamerInputComponent::StaticClass());

	// May be null if PS was unable to initialize.
	if (InputComponent != nullptr)
	{
		HandleAppServerMessageDelegate.BindUFunction(this, "HandleAppServerMessage");
		InputComponent->OnInputEvent.Add(HandleAppServerMessageDelegate);
	}
	
}

void UPureWebPlatformRequestHandler::OnGameModeLogout(AGameModeBase* GameMode, AController* Exiting)
{
	if (InputComponent != nullptr)
	{
		InputComponent->OnInputEvent.Remove(HandleAppServerMessageDelegate);
	}
	
}


void UPureWebPlatformRequestHandler::HandleProcessInfoRequest()
{
	FProcessInfo processInfo;
	FMemory::Memzero(processInfo);

	processInfo.ProcessID = FPlatformProcess::GetCurrentProcessId();
	processInfo.AppName = FPlatformMisc::GetEnvironmentVariable(TEXT("PUREWEB_APPNAME"));
	processInfo.AppID = FPlatformMisc::GetEnvironmentVariable(TEXT("PUREWEB_APPID"));
    processInfo.SchedulerID = FPlatformMisc::GetEnvironmentVariable(TEXT("PUREWEB_SCHEDULERID"));
	processInfo.LaunchReqURL = FPlatformMisc::GetEnvironmentVariable(TEXT("PUREWEB_LAUNCHREQURL"));
	processInfo.ProjectToken = FPlatformMisc::GetEnvironmentVariable(TEXT("PUREWEB_PROJECTTOKEN"));
	processInfo.AgentToken = FPlatformMisc::GetEnvironmentVariable(TEXT("PUREWEB_AGENTTOKEN"));
	processInfo.ServiceManagerID = FPlatformMisc::GetEnvironmentVariable(TEXT("PUREWEB_SERVICEMGRID"));

	if (processInfo.AppID.IsEmpty())
		processInfo.AppID = FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens).ToLower();

	if (processInfo.AppName.IsEmpty())
		processInfo.AppName = FString(TEXT("pluginbuilder"));

	FString msg;
	FJsonObjectConverter::UStructToJsonObjectString(processInfo, msg);

	InputComponent->SendPixelStreamingResponse(msg);


}

void UPureWebPlatformRequestHandler::HandleShutdownRequest()
{
	UE_LOG(PureWebPlatform, Log, TEXT("HandleShutdownRequest"));
	FPlatformMisc::RequestExit(true);
}

void UPureWebPlatformRequestHandler::HandleOnAgentReady()
{
	UE_LOG(PureWebPlatform, Log, TEXT("HandleOnAgentReady"));
}

void UPureWebPlatformRequestHandler::HandleWindowResize(TSharedPtr<FJsonObject> jsonMsg)
{
	int height = 0;
	int width = 0;
	if (!jsonMsg->TryGetNumberField(TEXT("width"), width) || !jsonMsg->TryGetNumberField(TEXT("height"), height))
	{
		UE_LOG(PureWebPlatform, Error, TEXT("Cannot find width or height in request json\n"));
		return;
	}

	UE_LOG(PureWebPlatform, Log, TEXT("Window resized to %dx%d\n"), width, height);
	GEngine->GameViewport->ViewportFrame->ResizeFrame(width, height, EWindowMode::Fullscreen);
}

void UPureWebPlatformRequestHandler::HandleAppServerMessage(const FString& Descriptor)
{
	UE_LOG(PureWebPlatform, Log, TEXT("HandleAppServerRequest %s"), *Descriptor);
	
	TSharedPtr<FJsonObject> JsonMsg;
	auto JsonReader = TJsonReaderFactory<TCHAR>::Create(Descriptor);
	if (!FJsonSerializer::Deserialize(JsonReader, JsonMsg))
	{
		UE_LOG(PureWebPlatform, Error, TEXT("Failed to parse request\n%s"), *Descriptor);
		return;
	}

	FString type;
	if (!JsonMsg->TryGetStringField(TEXT("type"), type))
	{
		UE_LOG(PureWebPlatform, Error, TEXT("Cannot find type in request json\n%s"), *Descriptor);
	}

	if (type == RequestProcessInfo) 
	{
		HandleProcessInfoRequest();
	}
	else if (type == RequestShutdown)
	{
		HandleShutdownRequest();
	}
	else if (type == OnAgentReady)
	{
		HandleOnAgentReady();
	}
	else if (type == UIInteraction)
	{
		int action;
		if (!JsonMsg->TryGetNumberField(TEXT("action"), action))
		{
			UE_LOG(PureWebPlatform, Error, TEXT("Cannot find action in request json\n%s"), *Descriptor);
		}
		
		if (action == OnWindowResize)
		{
			HandleWindowResize(JsonMsg);
		}
	}
	else if (type == BrowserMessage)
	{
		UE_LOG(PureWebPlatform, Log, TEXT("Browser message received\n%s"), *Descriptor);
	}
	else
	{
		UE_LOG(PureWebPlatform, Error, TEXT("Unknown request type\n%s"), *Descriptor);
	}
}



IMPLEMENT_MODULE(FLifecycleModule, LifecyclePlugin)

