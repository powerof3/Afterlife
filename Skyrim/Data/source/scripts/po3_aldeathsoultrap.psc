ScriptName PO3_ALDeathSoulTrap extends ActiveMagicEffect

import po3_SKSEFunctions
import po3_Events_AME

;-- Properties --------------------------------------
ReferenceAlias Property PlayerAlias Auto
GlobalVariable Property AL_SoulTrapUniqueOnly Auto

;-- Variables ---------------------------------------
Actor victim

;-- Event ---------------------------------------
Event OnEffectStart(Actor akTarget, Actor akCaster)

	victim = akTarget
	
endEvent

Event OnDeath(Actor akKiller)
	
	if victim && IsSoulTrapped(victim)
		bool onlyUniques = AL_SoulTrapUniqueOnly.GetValue() as bool
		
		actorbase base = victim.GetActorBase()		
		if !base || onlyUniques && !base.IsUnique()
			return 
		endIf
		
		if (PlayerAlias as po3_alsoulmanager).AddSoul(base, 1)		
			RegisterForActorResurrected(self)
		endif
		
	endIf
	
endEvent

Event OnActorResurrected(Actor akTarget, bool abResetInventory)

	if akTarget == victim
	
		actorbase base = victim.GetActorBase()	
		(PlayerAlias as po3_alsoulmanager).RemoveSoul(base, 1)	
		
		UnregisterForActorResurrected(self)	
		
	endif

endEvent