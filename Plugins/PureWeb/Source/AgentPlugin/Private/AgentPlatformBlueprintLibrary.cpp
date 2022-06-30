// Fill out your copyright notice in the Description page of Project Settings.


#include "AgentPlatformBlueprintLibrary.h"

FString UAgentPlatformBlueprintLibrary::GetKeyValue(FString values, FString key)
{
	TArray<FString> args;
	TArray<FString> parms;
	int32 tot = values.ParseIntoArrayWS(args);
	for (int32 i = 0; i < tot; i++) 
	{

		int loc = args[i].Find(key);
		if (loc >= 0) 
		{
			int32 tot2 = args[i].ParseIntoArray(parms, TEXT("="));
			if (tot2 == 2) 
			{
				return parms[1];
			}
		}
	}
	return FString();
}

FString UAgentPlatformBlueprintLibrary::JsonGetValue(FString json, FString keyValue)
{
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(json);
	TSharedPtr<FJsonObject> jsonObject;
	FJsonSerializer::Deserialize(reader, jsonObject);
	TMap<FString, TSharedPtr<FJsonValue>> values = jsonObject.Get()->Values;
	FString retVal =  UAgentPlatformBlueprintLibrary::Find(values, keyValue);
	return retVal;
}

FAgentPlatformJsonObject UAgentPlatformBlueprintLibrary::JsonGetObject(FString json)
{
	FAgentPlatformJsonObject object;
	if (json.IsEmpty()) 
	{
		return object;
	}
	TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(json);
	TSharedPtr<FJsonObject> jsonObject;
	FJsonSerializer::Deserialize(reader, object.Object);

	return object;
}

FAgentPlatformJsonObject UAgentPlatformBlueprintLibrary::JsonAddValue(FString key, FString value, FAgentPlatformJsonObject jsonObject)
{
	FAgentPlatformJsonObject retObject;
	if (jsonObject.Object != nullptr) 
	{
		retObject.Object = jsonObject.Object;
	}
	else 
	{
		retObject.Object = MakeShared<FJsonObject>();
	}
	FJsonValueString jsonValue(value);
	retObject.Object.Get()->SetStringField(key, value);
	return retObject;
}

FAgentPlatformJsonObject UAgentPlatformBlueprintLibrary::JsonAddObject(FString key, FAgentPlatformJsonObject newObject, FAgentPlatformJsonObject jsonObject)
{
	FAgentPlatformJsonObject retObject;
	if (jsonObject.Object != nullptr) 
	{
		retObject.Object = jsonObject.Object;
	}
	else 
	{
		retObject.Object = MakeShared<FJsonObject>();
	}
	retObject.Object.Get()->SetObjectField(key, newObject.Object);
	return retObject;
}

FString UAgentPlatformBlueprintLibrary::JsonToString(FAgentPlatformJsonObject jsonObject)
{
	FString OutputString;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString, 0);
	FJsonSerializer::Serialize(jsonObject.Object.ToSharedRef(), Writer);
	return OutputString;
}

FString UAgentPlatformBlueprintLibrary::Find(TMap<FString, TSharedPtr<FJsonValue>> values, FString keyValue)
{
	TMap<FString, TSharedPtr<FJsonValue>> vals;
	for (auto& Elem : values) 
	{
		FString key = Elem.Key;
		FString valueStr = Elem.Value->AsString();
		if (valueStr.IsEmpty()) 
		{
			vals = Elem.Value->AsObject().Get()->Values;
			valueStr =  UAgentPlatformBlueprintLibrary::Find(vals, keyValue);
			if (!valueStr.IsEmpty()) 
			{
				return valueStr;
			}
		}
		if (key == keyValue) 
		{
			return valueStr;
		}
	}
	return FString();
}

void UAgentPlatformBlueprintLibrary::HandleStreamerInputComponent(const FString& descriptor)
{

	int height = 0;
	int width = 0;

		TSharedPtr<FJsonObject> JsonMsg;
		auto JsonReader = TJsonReaderFactory<TCHAR>::Create(descriptor);
		if (!FJsonSerializer::Deserialize(JsonReader, JsonMsg))
		{
			return;
		}

		FString type;
		if (!JsonMsg->TryGetStringField(TEXT("type"), type))
		{
		}

		if (type == UIInteraction)
		{
			int action;
			if (!JsonMsg->TryGetNumberField(TEXT("action"), action))
			{
			}

			if (action == OnWindowResize)
			{
				if (!JsonMsg->TryGetNumberField(TEXT("width"), width) || !JsonMsg->TryGetNumberField(TEXT("height"), height))
				{
					return;
				}

				GEngine->GameViewport->ViewportFrame->ResizeFrame(width, height, EWindowMode::Fullscreen);
			}
		}
		
}



