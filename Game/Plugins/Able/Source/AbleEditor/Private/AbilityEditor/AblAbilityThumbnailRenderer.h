// Copyright (c) 2016 - 2018 Extra Life Studios, LLC. All rights reserved.

#pragma once

#include "Editor/UnrealEd/Classes/ThumbnailRendering/DefaultSizedThumbnailRenderer.h"

#include "AblAbilityThumbnailRenderer.generated.h"

/* Simple class that renders a user-captured thumbnail for our Abilities. */
UCLASS()
class UAblAbilityThumbnailRenderer : public UDefaultSizedThumbnailRenderer
{
	GENERATED_UCLASS_BODY()

	// UThumbnailRenderer interface
	virtual void Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* RenderTarget, FCanvas* Canvas) override;
	// End of UThumbnailRenderer interface
private:
	UPROPERTY()
	class UTexture2D* m_NoImage;
};
