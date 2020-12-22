// Copyright pafuhana1213.

#pragma once

#include "SimpleVideoCaptureSubsystem.h"
#include "PlatformFeatures.h"

DEFINE_LOG_CATEGORY(SimpleVideoCaptureSubsystem);

#define USE_SIMPLE_VIDEO_CAPTURE_IN_EDITOR 0

USimpleVideoCaptureSubsystem::USimpleVideoCaptureSubsystem()
	: UGameInstanceSubsystem()
{

}

void USimpleVideoCaptureSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if !USE_SIMPLE_VIDEO_CAPTURE_IN_EDITOR
	if (GetWorld()->WorldType == EWorldType::Game)
#endif
	{
		SetEnableVideoCapture(true);
	}
}

void USimpleVideoCaptureSubsystem::Deinitialize()
{
#if !USE_SIMPLE_VIDEO_CAPTURE_IN_EDITOR
	if (GetWorld()->WorldType == EWorldType::Game)
#endif
	{
		SetEnableVideoCapture(false);
	}

}

bool USimpleVideoCaptureSubsystem::StartVideoCapture(FString FileName, EFileSuffixType FileSuffixType, int RecordingLengthSeconds /*= 30*/)
{

#if !USE_SIMPLE_VIDEO_CAPTURE_IN_EDITOR
	if (GetWorld()->WorldType != EWorldType::Game)
	{
		UE_LOG(SimpleVideoCaptureSubsystem, Warning, TEXT("SimpleVideoCaptureSubsystem can be used only in Standalone or Package if USE_SIMPLE_VIDEO_CAPTURE_IN_EDITOR is Disabled"));
		return false;
	}
#endif

	if (VideoRecordingSystem && VideoRecordingSystem->IsEnabled())
	{
		FVideoRecordingParameters Param;
		Param.RecordingLengthSeconds = RecordingLengthSeconds;

		switch (FileSuffixType)
		{
		case EFileSuffixType::FST_IntSuffix:
		{
			if (IFileManager::Get().FileExists(*(FPaths::VideoCaptureDir() + FileName + TEXT(".mp4"))))
			{
				FString UniqueFileName = FileName;
				int IntSuffix = 0;
				while (IFileManager::Get().FileExists(*(FPaths::VideoCaptureDir() + UniqueFileName + TEXT(".mp4"))))
				{
					UniqueFileName = FileName + FString::FromInt(IntSuffix);
					IntSuffix++;
				}
				FileName = UniqueFileName;
			}
		}
		break;
		case EFileSuffixType::FST_TimeStamp:
		{
			FileName += TEXT("-") + FDateTime::Now().ToString();

		}
		break;
		default:
			break;
		}


		if (!VideoRecordingSystem->NewRecording(*FileName, Param))
		{
			UE_LOG(SimpleVideoCaptureSubsystem, Warning, TEXT("VideoRecordingSystem failed NewRecording"));
			return false;
		}
		VideoRecordingSystem->StartRecording();

		UE_LOG(SimpleVideoCaptureSubsystem, Log, TEXT("Start VideoCapture"));
		return true;

	}

	UE_LOG(SimpleVideoCaptureSubsystem, Warning, TEXT("VideoRecordingSystem is NOT Enabled"));
	return false;
}

bool USimpleVideoCaptureSubsystem::PauseVideoCapture()
{
#if !USE_SIMPLE_VIDEO_CAPTURE_IN_EDITOR
	if (GetWorld()->WorldType != EWorldType::Game)
	{
		UE_LOG(SimpleVideoCaptureSubsystem, Warning, TEXT("SimpleVideoCaptureSubsystem can be used only in Standalone or Package if USE_SIMPLE_VIDEO_CAPTURE_IN_EDITOR is Disabled"));
		return false;
	}
#endif

	if (VideoRecordingSystem && VideoRecordingSystem->IsEnabled())
	{
		UE_LOG(SimpleVideoCaptureSubsystem, Log, TEXT("Pause VideoCapture"));
		VideoRecordingSystem->PauseRecording();

		return true;
	}

	UE_LOG(SimpleVideoCaptureSubsystem, Warning, TEXT("VideoRecordingSystem is NOT Enabled"));
	return false;
}

bool USimpleVideoCaptureSubsystem::SaveVideoCapture(FText Title, FText Comment)
{
#if !USE_SIMPLE_VIDEO_CAPTURE_IN_EDITOR
	if (GetWorld()->WorldType != EWorldType::Game)
	{
		UE_LOG(SimpleVideoCaptureSubsystem, Warning, TEXT("SimpleVideoCaptureSubsystem can be used only in Standalone or Package if USE_SIMPLE_VIDEO_CAPTURE_IN_EDITOR is Disabled"));
		return false;
	}
#endif

	if (VideoRecordingSystem && VideoRecordingSystem->IsEnabled())
	{
		UE_LOG(SimpleVideoCaptureSubsystem, Log, TEXT("Save VideoCapture ( %s, %s )"), *Title.ToString(), *Comment.ToString());
		VideoRecordingSystem->FinalizeRecording(true, Title, Comment, true);

		return true;
	}

	UE_LOG(SimpleVideoCaptureSubsystem, Warning, TEXT("VideoRecordingSystem is NOT Enabled"));
	return false;
}

bool USimpleVideoCaptureSubsystem::SetEnableVideoCapture(bool bEnableVideoCapture)
{
#if !USE_SIMPLE_VIDEO_CAPTURE_IN_EDITOR
	if (GetWorld()->WorldType != EWorldType::Game)
	{
		UE_LOG(SimpleVideoCaptureSubsystem, Warning, TEXT("SimpleVideoCaptureSubsystem can be used only in Standalone or Package if USE_SIMPLE_VIDEO_CAPTURE_IN_EDITOR is Disabled"));
		return false;
	}
#endif

	IPlatformFeaturesModule& PlatFormFeaturesModule = IPlatformFeaturesModule::Get();
	VideoRecordingSystem = PlatFormFeaturesModule.GetVideoRecordingSystem();

	if (!VideoRecordingSystem)
	{
		UE_LOG(SimpleVideoCaptureSubsystem, Warning, TEXT("VideoRecordingSystem is NONE. Probably an unsupported platform."));
		return false;
	}

	if (!bEnableVideoCapture)
	{
		PauseVideoCapture();
	}
	VideoRecordingSystem->EnableRecording(bEnableVideoCapture);

	return true;
}


bool USimpleVideoCaptureSubsystem::IsEnabledVideoCapture()
{
	return VideoRecordingSystem && VideoRecordingSystem->IsEnabled();
}

bool USimpleVideoCaptureSubsystem::IsVideoCapturing()
{
	return VideoRecordingSystem ? VideoRecordingSystem->GetRecordingState() == EVideoRecordingState::Recording : false;
}

