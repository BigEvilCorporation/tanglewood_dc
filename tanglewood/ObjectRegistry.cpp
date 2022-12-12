#include "framework/ObjectFactory.h"

#include "tanglewood/Boulder.h"
#include "tanglewood/Djakk.h"
#include "tanglewood/Firefly.h"
#include "tanglewood/Flue.h"
#include "tanglewood/Fuzzl.h"
#include "tanglewood/Hogg.h"
#include "tanglewood/Mushroom.h"
#include "tanglewood/Nest.h"
#include "tanglewood/Player.h"
#include "tanglewood/Stone.h"
#include "tanglewood/TriggerBox.h"
#include "tanglewood/DeathBox.h"
#include "tanglewood/Checkpoint.h"
#include "tanglewood/Scirus.h"

namespace ObjectFactory
{
	const std::vector<ObjectRegistryEntry> objectRegistry = 
	{
		//                Name              SpriteActor     Class
		{ REGISTER_OBJECT("Nest",			"",				Nest) },
		{ REGISTER_OBJECT("Nymn",			"Nymn",			Player) },
		{ REGISTER_OBJECT("Echo",			"Echo",			Player) },
		{ REGISTER_OBJECT("Boulder",		"Boulder",		Boulder) },
		{ REGISTER_OBJECT("Monster",		"Djakk",		Djakk) },
		{ REGISTER_OBJECT("Firefly",		"Firefly",		Firefly) },
		{ REGISTER_OBJECT("Flue",			"",				Flue) },
		{ REGISTER_OBJECT("Fuzzl",			"Fuzzl",		Fuzzl) },
		{ REGISTER_OBJECT("Hogg",			"Hogg",			Hogg) },
		{ REGISTER_OBJECT("BouncePlant",	"mushroom",		Mushroom) },
		{ REGISTER_OBJECT("Stone",			"Stone",		Stone) },
		{ REGISTER_OBJECT("TriggerBox",		"",				TriggerBox) },
		{ REGISTER_OBJECT("Deathbox",		"",				DeathBox) },
		{ REGISTER_OBJECT("Checkpoint",		"Checkpnt",		Checkpoint) },
		{ REGISTER_OBJECT("Scirus",			"Scirus",		Scirus) },
	};
}
