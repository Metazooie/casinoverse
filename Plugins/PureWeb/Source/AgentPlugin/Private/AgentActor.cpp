#include "AgentActor.h"

DEFINE_LOG_CATEGORY(AgentPlugin);

const FString AAgentActor::DEFAULT_SIDECAR_PORT = "50051";
const FString AAgentActor::DEFAULT_SIDECAR_ADDRESS = "localhost";
const FString AAgentActor::ENV_SIDECAR_PORT = "SidecarPort=";
const FString AAgentActor::ENV_PIXEL_STREAMING_IP = "PixelStreamingIP=";

//Definition for agent actor member function
typedef  APState (AAgentActor::* AAgentActorFn)(FString key, FString vlaue);  // Please do this!

/*
* Implementation of the FNonAbondableTask to be used to run agent actor
* member function on a separate thread
*/
class MultithreadedTask : public FNonAbandonableTask {
public:
	AAgentActor* agentActor;
    FString key;
    FString value;
    AAgentActorFn fn;
	MultithreadedTask(AAgentActor* agentActor, AAgentActorFn fn, FString key, FString value) { 
        this->agentActor = agentActor;
        this->key = key;
        this->value = value;
        this->fn = fn;
    }


	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(ExampleAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

	void DoWork() {
        std::invoke(this->fn, this->agentActor, key, value);
	}
};

// Sets default values
AAgentActor::AAgentActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAgentActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAgentActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

APState AAgentActor::CreateAgent(AAgentActor* agentActor)
{
    FString pixelStreamingIP;
    FString sidecarPort;

    FParse::Value(FCommandLine::Get(), *AAgentActor::ENV_PIXEL_STREAMING_IP, pixelStreamingIP, false);
    FParse::Value(FCommandLine::Get(), *AAgentActor::ENV_SIDECAR_PORT, sidecarPort, false);

    return CreateAgentByURL("", "", pixelStreamingIP, sidecarPort, agentActor);
}

APState AAgentActor::CreateAgentByURL(FString token, FString certificate, FString serverURL, FString serverPort, AAgentActor* agentActor)
{
    FString address;

    if (!serverURL.IsEmpty())
    {
        address.Append(serverURL);
    }
    else
    {
        address.Append(AAgentActor::DEFAULT_SIDECAR_ADDRESS);
    }

    address.Append(":");

    if (!serverPort.IsEmpty())
    {
        address.Append(serverPort);
    } 
    else 
    {
        address.Append(AAgentActor::DEFAULT_SIDECAR_PORT);
    }
    
	if (token.IsEmpty()) 
	{
		sidecar = new PureWeb::Collaboration::Sidecar(TCHAR_TO_ANSI(*address));
	}
	else
	{
		sidecar = new PureWeb::Collaboration::Sidecar(TCHAR_TO_ANSI(*address), TCHAR_TO_ANSI(*token), TCHAR_TO_ANSI(*certificate));
    }

    auto waitForConnection = [this](AAgentActor* agentActor) {
        static AAgentActor* aa = agentActor;
        PureWeb::Collaboration::Sidecar::OnConnectionHandler onConnection = [&](const char* environmentId, const char* agentId)
        {
            if (aa != nullptr)
            {
                aa->agentConnectionDelegate.Broadcast(ConstructConnectionObject(environmentId, agentId));
            }
        };
        
        sidecar->connect(onConnection);
    };

    
    std::thread connectionThread(waitForConnection, agentActor);
    connectionThread.detach();
    
    return APState::OK;
}


APState AAgentActor::SubscribeToAllMessages(AAgentActor* agentActor)
{
    if (!this->isConnected())
    {
        return APState::ERROR_;
    }

    auto waitForMessage = [this](AAgentActor* agentActor) {
        static AAgentActor* aa = agentActor;
        PureWeb::Collaboration::Sidecar::OnMessageReceivedHandler onMessage = [&](const char* agentId, const char* payload,
            const char* type, const char* timestamp, const char* messageId)
        {
            aa->broadcastMessageDelegate.Broadcast(ConstructMessageObject(agentId, payload, type, timestamp, messageId));
        };
        sidecar->subscribeToMessages(onMessage, PureWeb::Collaboration::Sidecar::MessageType::ALL);
    };

    std::thread messagingThread(waitForMessage, agentActor);
    messagingThread.detach();

    return APState::OK;
}

APState AAgentActor::SendMessage(FString message, FString type)
{
    if (!this->isConnected())
    {
        return APState::ERROR_;
    }

    APState ret = sidecar->sendBroadcastMessage(TCHAR_TO_ANSI(*message), TCHAR_TO_ANSI(*type)) ? APState::OK : APState::ERROR_;
    return ret;
}

APState AAgentActor::SendDirectMessage(FString remoteAgentId, FString message, FString type)
{
    if (!this->isConnected())
    {
        return APState::ERROR_;
    }

    APState ret = sidecar->sendDirectMessage(TCHAR_TO_ANSI(*remoteAgentId), TCHAR_TO_ANSI(*message), TCHAR_TO_ANSI(*type)) ? APState::OK : APState::ERROR_;
    return ret;
}

APState AAgentActor::SubscribeToBroadcastMessages(AAgentActor* agentActor)
{
    if (!this->isConnected())
    {
        return APState::ERROR_;
    }

    auto waitForMessage = [this](AAgentActor* agentActor) {
        static AAgentActor* aa = agentActor;
        PureWeb::Collaboration::Sidecar::OnMessageReceivedHandler onMessage = [&](const char* agentId, const char* payload,
            const char* type, const char* timestamp, const char* messageId)
        {
            aa->broadcastMessageDelegate.Broadcast(ConstructMessageObject(agentId, payload, type, timestamp, messageId));

        };
        sidecar->subscribeToMessages(onMessage, PureWeb::Collaboration::Sidecar::MessageType::BROADCAST);       
    };

    std::thread messagingThread(waitForMessage, agentActor);
    messagingThread.detach();

    return APState::OK;
}

APState AAgentActor::SubscribeToPrivateMessages(AAgentActor* agentActor)
{
    if (!this->isConnected())
    {
        return APState::ERROR_;
    }

    auto waitForMessage = [this](AAgentActor* agentActor) {
        static AAgentActor* aa = agentActor;
        PureWeb::Collaboration::Sidecar::OnMessageReceivedHandler onMessage = [&](const char* agentId, const char* payload,
            const char* type, const char* timestamp, const char* messageId)
        {
            aa->broadcastMessageDelegate.Broadcast(ConstructMessageObject(agentId, payload, type, timestamp, messageId));
        };
        sidecar->subscribeToMessages(onMessage, PureWeb::Collaboration::Sidecar::MessageType::PRIVATE);
    };

    std::thread messagingThread(waitForMessage, agentActor);
    messagingThread.detach();

    return APState::OK;
}

APState AAgentActor::SubscribeToArrivals(AAgentActor* agentActor)
{
    if (!this->isConnected())
    {
        return APState::ERROR_;
    }

    auto waitForArrival = [this](AAgentActor* agentActor) {
        static AAgentActor* aa = agentActor;
        PureWeb::Collaboration::Sidecar::OnPresenceHandler onArrival = [&](const char* agentId, PureWeb::Collaboration::Sidecar::PresenceType type)
        {
            aa->arrivalDelegate.Broadcast(ConstructPresenceObject(agentId, type));
        };
        sidecar->subscribeToPresence(onArrival, PureWeb::Collaboration::Sidecar::PresenceType::ARRIVAL);
    };

    std::thread presenceThread(waitForArrival, agentActor);
    presenceThread.detach();

    return APState::OK;
}

APState AAgentActor::SubscribeToDepartures(AAgentActor* agentActor)
{
    if (!this->isConnected())
    {
        return APState::ERROR_;
    }

    auto waitForDeparture = [this](AAgentActor* agentActor) {
        static AAgentActor* aa = agentActor;
        PureWeb::Collaboration::Sidecar::OnPresenceHandler onDeparture = [&](const char* agentId, PureWeb::Collaboration::Sidecar::PresenceType type)
        {
            aa->arrivalDelegate.Broadcast(ConstructPresenceObject(agentId, type));
        };
        sidecar->subscribeToPresence(onDeparture, PureWeb::Collaboration::Sidecar::PresenceType::DEPARTURE);
    };

    std::thread presenceThread(waitForDeparture, agentActor);
    presenceThread.detach();

    return APState::OK;
}

APState AAgentActor::SubscribeToAllPresenceEvents(AAgentActor* agentActor)
{
    if (!this->isConnected())
    {
        return APState::ERROR_;
    }

    auto waitForPresenceChange = [this](AAgentActor* agentActor) {
        static AAgentActor* aa = agentActor;
        PureWeb::Collaboration::Sidecar::OnPresenceHandler onPresence = [&](const char* agentId, PureWeb::Collaboration::Sidecar::PresenceType type)
        {
            aa->arrivalDelegate.Broadcast(ConstructPresenceObject(agentId, type));
        };
        sidecar->subscribeToPresence(onPresence, PureWeb::Collaboration::Sidecar::PresenceType::ALL);
    };

    std::thread presenceThread(waitForPresenceChange, agentActor);
    presenceThread.detach();

    return APState::OK;
}

APState AAgentActor::Store(FString key, FString value)
{
    if (!this->isConnected())
    {
        return APState::ERROR_;
    }

    APState ret = sidecar->store(TCHAR_TO_ANSI(*key), TCHAR_TO_ANSI(*value), "") ? APState::OK : APState::ERROR_;
    return ret;
}

APState AAgentActor::StoreAgentScoped(FString key, FString value, FString agentId)
{
    if (!this->isConnected())
    {
        return APState::ERROR_;
    }

    APState ret = sidecar->store(TCHAR_TO_ANSI(*key), TCHAR_TO_ANSI(*value), TCHAR_TO_ANSI(*agentId)) ? APState::OK : APState::ERROR_;
    return ret;
}

APState AAgentActor::StorePrivateScopedAsync(FString key, FString value)
{
    if (!this->isConnected())
    {
        return APState::ERROR_;
    }

    APState ret = sidecar->storePrivateScoped(TCHAR_TO_ANSI(*key), TCHAR_TO_ANSI(*value)) ? APState::OK : APState::ERROR_;
    return ret;
}

FString AAgentActor::Retrieve(FString key) 
{
    std::string data = this->sidecar == nullptr ? "" : this->sidecar->retrieveKey(TCHAR_TO_ANSI(*key), "");
    return FString(data.c_str());
}

FString AAgentActor::RetrieveAgentScoped(FString key, FString agentId)
{
    std::string data = this->sidecar == nullptr ? "" : this->sidecar->retrieveKey(TCHAR_TO_ANSI(*key), TCHAR_TO_ANSI(*agentId));
    return FString(data.c_str());
}

FString AAgentActor::RetrievePrivateScoped(FString key)
{
    std::string data = this->sidecar == nullptr ? "" : this->sidecar->retrievePrivateScopedKey(TCHAR_TO_ANSI(*key));
    return FString(data.c_str());
}

APState AAgentActor::Remove(FString key) 
{
    if (!this->isConnected())
    {
        return APState::ERROR_;
    }

    if (this->sidecar->removeKey(TCHAR_TO_ANSI(*key), ""))
    {
        return APState::OK;
    }
    else
    {
        return APState::ERROR_;
    }
}

APState AAgentActor::RemoveAgentScoped(FString key, FString agentId)
{
    if (!this->isConnected())
    {
        return APState::ERROR_;
    }

    if (this->sidecar->removeKey(TCHAR_TO_ANSI(*key), TCHAR_TO_ANSI(*agentId)))
    {
        return APState::OK;
    }
    else
    {
        return APState::ERROR_;
    }
}

APState AAgentActor::RemovePrivateScoped(FString key)
{
    if (!this->isConnected())
    {
        return APState::ERROR_;
    }

    if (this->sidecar->retrievePrivateScopedKey(TCHAR_TO_ANSI(*key)))
    {
        return APState::OK;
    }
    else
    {
        return APState::ERROR_;
    }
}

APState AAgentActor::SubscribeToKey(AAgentActor* agentActor, FString key, FString agentId)
{
    if (!this->isConnected())
    {
        return APState::ERROR_;
    }

    auto waitForStorageChange = [this](AAgentActor* agentActor, FString key, FString agentId) {
        static AAgentActor* aa = agentActor;
        PureWeb::Collaboration::Sidecar::OnStoragehandler onStorage = [&](const char* agentId, const char* key, const char* value, PureWeb::Collaboration::Sidecar::StorageType type)
        {
 
            aa->keyChangeDelegate.Broadcast(ConstructStorageObject(agentId, key, value, type));
        };
        sidecar->subscribeToStorage(onStorage, TCHAR_TO_ANSI(*key), TCHAR_TO_ANSI(*agentId), PureWeb::Collaboration::Sidecar::StorageType::ALL);
    };

    std::thread storageThread(waitForStorageChange, agentActor, key, agentId);
    storageThread.detach();

    return APState::OK;
}

APState AAgentActor::SubscribeToKeyWrite(AAgentActor* agentActor, FString key, FString agentId)
{
    if (!this->isConnected())
    {
        return APState::ERROR_;
    }

    auto waitForStorageChange = [this](AAgentActor* agentActor, FString key, FString agentId) {
        static AAgentActor* aa = agentActor;
        PureWeb::Collaboration::Sidecar::OnStoragehandler onStorage = [&](const char* agentId, const char* key, const char* value, PureWeb::Collaboration::Sidecar::StorageType type)
        {

            aa->keyChangeDelegate.Broadcast(ConstructStorageObject(agentId, key, value, type));
        };
        sidecar->subscribeToStorage(onStorage, TCHAR_TO_ANSI(*key), TCHAR_TO_ANSI(*agentId), PureWeb::Collaboration::Sidecar::StorageType::WRITE);
    };

    std::thread storageThread(waitForStorageChange, agentActor, key, agentId);
    storageThread.detach();

    return APState::OK;
}

APState AAgentActor::StorePrivateScoped(FString key, FString value)
{
    (new FAutoDeleteAsyncTask<MultithreadedTask>(this, &AAgentActor::StorePrivateScopedAsync, key, value))->StartBackgroundTask();
    return APState::OK;
}

APState AAgentActor::SubscribeToKeyRemove(AAgentActor* agentActor, FString key, FString agentId)
{
    if (!this->isConnected())
    {
        return APState::ERROR_;
    }

    auto waitForStorageChange = [this](AAgentActor* agentActor, FString key, FString agentId) {
        static AAgentActor* aa = agentActor;
        PureWeb::Collaboration::Sidecar::OnStoragehandler onStorage = [&](const char* agentId, const char* key, const char* value, PureWeb::Collaboration::Sidecar::StorageType type)
        {
            aa->keyChangeDelegate.Broadcast(ConstructStorageObject(agentId, key, value, type));
        };
        sidecar->subscribeToStorage(onStorage, TCHAR_TO_ANSI(*key), TCHAR_TO_ANSI(*agentId), PureWeb::Collaboration::Sidecar::StorageType::REMOVE);
    };

    std::thread storageThread(waitForStorageChange, agentActor, key, agentId);
    storageThread.detach();

    return APState::OK;
}

FAgentPlatformJsonObject AAgentActor::ConstructMessageObject(const char* agentId, const char* payload,
    const char* type, const char* timestamp, const char* messageId)
{
    FAgentPlatformJsonObject o = UAgentPlatformBlueprintLibrary::JsonAddValue("AgentId", agentId, FAgentPlatformJsonObject());
    o = UAgentPlatformBlueprintLibrary::JsonAddValue("payload", payload, o);
    o = UAgentPlatformBlueprintLibrary::JsonAddValue("type", type, o);
    o = UAgentPlatformBlueprintLibrary::JsonAddValue("timestamp", timestamp, o);
    o = UAgentPlatformBlueprintLibrary::JsonAddValue("messageId", messageId, o);
    return o;
}

FAgentPlatformJsonObject AAgentActor::ConstructStorageObject(const char* agentId, const char* key, const char* value,
    PureWeb::Collaboration::Sidecar::StorageType type) 
{
    FAgentPlatformJsonObject o = UAgentPlatformBlueprintLibrary::JsonAddValue("AgentId", agentId, FAgentPlatformJsonObject());
    UAgentPlatformBlueprintLibrary::JsonAddValue("key", key, o);
    UAgentPlatformBlueprintLibrary::JsonAddValue("type", type == PureWeb::Collaboration::Sidecar::StorageType::REMOVE ?
        "removed" : "updated", o);
    UAgentPlatformBlueprintLibrary::JsonAddValue("value", value, o);
    return o;
}

FAgentPlatformJsonObject AAgentActor::ConstructPresenceObject(const char* agentId, PureWeb::Collaboration::Sidecar::PresenceType type) 
{
    FAgentPlatformJsonObject o = UAgentPlatformBlueprintLibrary::JsonAddValue("AgentId", agentId, FAgentPlatformJsonObject());
    UAgentPlatformBlueprintLibrary::JsonAddValue("Event", type == PureWeb::Collaboration::Sidecar::PresenceType::ARRIVAL ?
        "arrival" : type == PureWeb::Collaboration::Sidecar::PresenceType::DEPARTURE ? "departure" : "all", o);
    return o;
}

FAgentPlatformJsonObject AAgentActor::ConstructConnectionObject(const char* environmentId, const char* agentId)
{
    FAgentPlatformJsonObject o = UAgentPlatformBlueprintLibrary::JsonAddValue("AgentId", agentId, FAgentPlatformJsonObject());
    UAgentPlatformBlueprintLibrary::JsonAddValue("EnvironmentId", environmentId, o);
    return o;
}

bool AAgentActor::isConnected()
{
    return this->sidecar != nullptr && this->sidecar->isConnected();
}