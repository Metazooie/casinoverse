#pragma once

#include <functional>
#include <vector>
#include<string>

#include "../SharedLibraryExport.h"

namespace PureWeb
{
	namespace Collaboration
	{
		class EXPORT Sidecar
		{
		private:
			std::shared_ptr<void> sidecarClient;
		public:
			enum class MessageType { ALL, PRIVATE, BROADCAST };
			enum class PresenceType { ALL, ARRIVAL, DEPARTURE };
			enum class StorageType { ALL, REMOVE, WRITE};

			using OnConnectionHandler = std::function<void(const char* environmentId, const char* agentId)>;
			using OnMessageReceivedHandler = std::function<void(const char* agentId, const char* payload, 
				const char* type, const char* timestamp, const char* messageId)>;
			using OnPresenceHandler = std::function<void(const char* agentId, PresenceType type)>;
			using OnStoragehandler = std::function<void(const char* agentId, const char* key, const char* value, StorageType type)>;

			Sidecar(const char* sidecarAddress, const char* token, const char* certificateFilePath = nullptr);
			Sidecar(const char* sidecarAddress);

			bool sendBroadcastMessage(char* messageStr, char* type);
			bool connect(OnConnectionHandler handler);
			bool subscribeToMessages(OnMessageReceivedHandler handler, MessageType type = MessageType::ALL);
			bool sendDirectMessage(char* remoteAgentId, char* messageStr, char* type);

			std::vector<std::string> getAllActiveAgents();
			bool subscribeToPresence(OnPresenceHandler handler, PresenceType type = PresenceType::ALL);

			bool store(char* keyName, char* value, char* agentId = "");
			bool storePrivateScoped(char* keyName, char* value);
			const char* retrieveKey(char* key, char* agentId = "");
			const char* retrievePrivateScopedKey(char* key);
			std::vector<std::string> retrieveAllKeys(char* agentId);
			bool removeKey(char* key, char* agentId = "");
			bool removePrivateScopedKey(char* key);
			bool subscribeToStorage(OnStoragehandler handler, char* key, char* agentId, StorageType type = StorageType::ALL);
			bool isConnected();
			std::string getAgentId();
			std::string getEnvironmentId();
		};
	}
}