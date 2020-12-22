// Copyright pafuhana1213.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "VideoRecordingSystem.h"

#include "SimpleVideoCaptureSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(SimpleVideoCaptureSubsystem, Log, Log);


UENUM(BlueprintType)
enum class EFileSuffixType : uint8
{
	FST_IntSuffix UMETA(DisplayName = "Int Suffix"),
	FST_TimeStamp UMETA(DisplayName = "TimeStamp"),
	FST_None UMETA(DisplayName = "None"),
};

UCLASS(config = EditorPerProjectUserSettings)
class SIMPLEVIDEOCAPTURE_API USimpleVideoCaptureSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	IVideoRecordingSystem* VideoRecordingSystem;

	USimpleVideoCaptureSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "SimpleVideoCapture")
	bool StartVideoCapture(FString FileName, EFileSuffixType FileSuffixType = EFileSuffixType::FST_IntSuffix, int RecordingLengthSeconds = 30);

	UFUNCTION(BlueprintCallable, Category = "SimpleVideoCapture")
	bool PauseVideoCapture();

	UFUNCTION(BlueprintCallable, Category = "SimpleVideoCapture")
	bool SaveVideoCapture(FText Title = FText::GetEmpty(), FText Comment = FText::GetEmpty());

	UFUNCTION(BlueprintCallable, Category = "SimpleVideoCapture")
	bool SetEnableVideoCapture( bool bEnableVideoCapture );

	UFUNCTION(BlueprintCallable, Category = "SimpleVideoCapture")
	bool IsEnabledVideoCapture();

	UFUNCTION(BlueprintCallable, Category = "SimpleVideoCapture")
	bool IsVideoCapturing();

private:

	//UFUNCTION(BlueprintCallable)
	//	bool SetResolution(uint32 ResolutionX, uint32 ResolutionY, );
};