/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#pragma once
#include "Engine.h"
#include "Runtime/Core/Public/Containers/ContainersFwd.h"
#include "RookEnums.generated.h"

UENUM( Category = "Rook Enums" )
enum class EEAX : uint8 
{
		None, GenericEffect, PaddedCell, Room, Bathroom, Livingroom, StoneRoom, Auditorium,
		ConcertHall, Cave, Arena, Hangar, CarpetedHallway, Hallway, StoneCorridor, Alley,
		Forest, City, Mountains, Quarry, Plain, ParkingLot, SewerPipe, Underwater, Drugged,
		Dizzy, Psychotic, DustyRoom, Chapel, SmallWaterRoom, CastleSmallRoom, CastleMediumRoom,
		CastleLargeRoom, CastleShortPassage, CastleLongPassage, CastleHall, CastleCupBoard,
		CastleCourtyard, CastleAlcove, FactorySmallRoom, FactoryMediumRoom, FactoryLargeRoom,
		FactoryShortPassage, FactoryLongPassage, FactoryHall, FactoryCupBoard, FactoryCourtyard,
		FactoryAlcove, IcePalaceSmallRoom, IcePalaceMediumRoom, IcePalaceLargeRoom,
		IcePalaceShortPassage, IcePalaceLongPassage, IcePalaceHall, IcePalaceCupBoard,
		IcePalaceCourtyard, IcePalaceAlcove, SpaceStationSmallRoom, SpaceStationMediumRoom,
		SpaceStationLargeRoom, SpaceStationShortPassage, SpaceStationLongPassage,
		SpaceStationHall, SpaceStationCupBoard, SpaceStationAlcove, WoodenSmallRoom,
		WoodenMediumRoom, WoodenLargeRoom, WoodenShortPassage, WoodenLongPassage, WoodenHall,
		WoodenCupBoard, WoodenCourtyard, WoodenAlcove, SportEmptyStadium, SportFullStadium,
		SportStadiumTannoySpeaker, SportSquashCourt, SportSmallSwimmingPool,
		SportLargeSwimmingPool, SportGymnasium, DomeTomb, DomeSaintPauls, PipeSmall, PipeLarge,
		PipeLongThin, PipeResonant, OutdoorsBackyard, OutdoorsRollingPlains, OutdoorsDeepCanyon,
		OutdoorsCreek, OutdoorsValley, MoodHeaven, MoodHell, MoodMemory, DrivingCommentator,
		DrivingPitGarage, DrivingInCarRacer, DrivingInCarSports, DrivingInCarLuxury,
		DrivingFullGrandstand, DrivingEmptyGrandstand, DrivingTunnel,
		CityStreet, CitySubway, CityMuseum, CityLibrary, CityUnderpass, CityAbandoned,
		MAX UMETA(Hidden)
};

UENUM( Category = "Rook Enums" )
enum class EAudioBus : uint8 
{
		Ambience,
		EFX,
		EFX3D,
		Foley,
		Foley3D,
		HUD,
		Music,		
		SFX,
		SFX3D,
		Voice,
		Voice3D,
		MAX UMETA(Hidden)
};

UENUM(Category = "Rook Enums")
enum class EAudioType : uint8 
{
	is2D,
	is3D,
	MAX UMETA(Hidden)
};

UENUM( Category = "Rook Enums" )
enum class EPlayback : uint8 
{
		Single,
		Loop,
		Random,
		SingleRandom,
		Sequence,
		SingleSequence,
		MAX UMETA(Hidden)
};

UENUM(Category = "Rook Enums")
enum class EAudioState : uint8 
{
	None,
	Playing,
	Stopped,
	Paused,
	WasPlaying,	
	MAX UMETA(Hidden)
};

UENUM(Category = "Rook Enums")
enum class EFreezRotationState : uint8 
{
	NoFreez,
	Freez,
	MAX UMETA(Hidden)
};

UENUM(Category = "Rook Enums")
enum class EEnableState : uint8 
{
	Enable,
	Disable,
	MAX UMETA(Hidden)
};

UENUM(Category = "Rook Enums")
enum class EListenerType : uint8 
{
	FollowCamera,
	FollowActor,
	FollowMeshSocket,
	MAX UMETA(Hidden)
};