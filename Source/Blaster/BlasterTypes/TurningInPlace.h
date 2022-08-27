#pragma once

UENUM(BlueprintType)
enum class ETurningInPlace:uint8
{
	ETIP_Left UMETA(DispalyName ="Turning Left"),
	ETIP_Right UMETA(DispalyName = "Turning Right"),
	ETIP_NotTurning UMETA(DispalyName = "Not Turning"),
	ETIP_MAX UMETA(DispalyName = "DefalutMAX"),
};