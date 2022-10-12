ScriptName po3_almcmscript extends ski_configbase

;-- Properties --------------------------------------
import PO3_SKSEFunctions
import PO3_Afterlife

Actor property PlayerRef auto

GlobalVariable[] property uniqueGlobals auto
GlobalVariable[] property countGlobals auto
GlobalVariable[] property limitGlobals auto

Location[] property soulLocations auto

;-- Variables ---------------------------------------
Int kSOVNGARDE = 0
Int kSOUL_CAIRN = 1

Bool[] uniquesToggle

String[] sovngarde_List
String[] soulCairn_List

Int defValue = 128
Int defInterval = 1
Int defMinValue = 0
Int defMaxValue = 256

int[] property requiredVersionPE auto

;-- Events ---------------------------------------
Int function GetVersion()

	return 2
	
endFunction

Event OnConfigInit()

	Pages = new String[2]
	Pages[0] = "Sovngarde"
	Pages[1] = "Soul Cairn"
	
	ModName = "Afterlife"
	
	uniquesToggle = new Bool[2]
	uniquesToggle[0] = uniqueGlobals[0].GetValue() as Bool
	uniquesToggle[1] = uniqueGlobals[1].GetValue() as Bool

	Int[] currentVersionPE = GetPapyrusExtenderVersion()
	if currentVersionPE.Length == 0
			Debug.Messagebox("[Afterlife] Papyrus Extender is not installed! Required Version : " + requiredVersionPE + " or higher.")
	else
		if currentVersionPE[0] > requiredVersionPE[0]
			return
		endif	
		if currentVersionPE[0] < requiredVersionPE[0] || currentVersionPE[1] < requiredVersionPE[1] && currentVersionPE[0] <= requiredVersionPE[0]
			Debug.Messagebox("[Afterlife] Papyrus Extender is out of date! Current Version : " + currentVersionPE + ". Required Version : " + requiredVersionPE + " or higher.")
		endif
	endif
	
endEvent

Event OnGameReload()

	parent.OnGameReload()
	OnConfigInit()
	
endEvent

Event OnPageReset(String page)

	If page == "Sovngarde"
		Init_PageReset(kSOVNGARDE)
		
	elseif page == "Soul Cairn"
		Init_PageReset(kSOUL_CAIRN)
		
	endIf
	
endEvent

;-- Sovngarde ---------------------------------------

State Sovngarde_uniquesOnly

	Event OnHighlightST()

		SetInfoText("If checked, only unique NPCs will be sent to Sovngarde\nDefault Value = True")
		
	endEvent

	Event OnSelectST()

		if countGlobals[0].GetValue() > 0 && !uniquesToggle[0]
			if !showmessage("Selecting this option will clear all non-unique actors currently in Sovngarde. Are you sure?" , true, "$Accept", "$Cancel")
				return
			endIf
		endIf	
		uniquesToggle[0] = !uniquesToggle[0]
		SetToggleOptionValueST(uniquesToggle[0], false)
		
		uniqueGlobals[0].SetValue((!uniqueGlobals[0].GetValue() as Bool) as Float)
		
		if (uniquesToggle[0])
			countGlobals[0].Mod(-ClearAllGenericSouls(0))
			ForcePageReset()
		endIf
		
	endEvent
	
endState

State Sovngarde_npcLimit

	Event OnHighlightST()

		SetInfoText("Limits the number of added NPCs to a maximum of " + (limitGlobals[0].GetValue() as Int))
		
	endEvent

	Event OnSliderAcceptST(Float value)

		limitGlobals[0].SetValue(value)
		SetSliderOptionValueST(limitGlobals[0].GetValue(), "{0}", false)
		
	endEvent
	
	Event OnSliderOpenST()

		SetSliderDialogStartValue(limitGlobals[0].GetValue())
		SetSliderDialogDefaultValue(defValue)
		SetSliderDialogRange(defMinValue, defMaxValue)
		SetSliderDialogInterval(defInterval)		
	
	endEvent
	
endState

State Sovngarde_npcList

	Event OnMenuOpenST()

		SetMenuDialogOptions(sovngarde_List)
		SetMenuDialogStartIndex(0)
		SetMenuDialogDefaultIndex(0)
		
	endEvent
	
endState

State Sovngarde_clearNPCList

	Event OnHighlightST()

		SetInfoText("Clears" + (countGlobals[0].GetValue() as Int) + " NPCs currently in Sovngarde.")
		
	endEvent

	Event OnSelectST()

		if showmessage("Are you sure?", true, "$Accept", "$Cancel")
			ClearList(0)
			SetOptionFlagsST(OPTION_FLAG_DISABLED, false)
		endIf
		
	endEvent
	
endState

;-- SoulCairn ---------------------------------------

State SoulCairn_uniquesOnly

	Event OnHighlightST()

		SetInfoText("If checked, only unique NPCs will be sent to the Soul Cairn\nDefault Value = True")
		
	endEvent

	Event OnSelectST()

		if countGlobals[1].GetValue() > 0 && !uniquesToggle[1]
			if !showmessage("Selecting this option will clear all non-unique actors currently in the Soul Cairn. Are you sure?" , true, "$Accept", "$Cancel")
				return
			endIf
		endIf	
		uniquesToggle[1] = !uniquesToggle[1]
		SetToggleOptionValueST(uniquesToggle[1], false)
		
		uniqueGlobals[1].SetValue((!uniqueGlobals[1].GetValue() as Bool) as Float)
		
		if (uniquesToggle[1])
			countGlobals[1].Mod(-ClearAllGenericSouls(1))
			ForcePageReset()
		endIf
		
	endEvent
	
endState

State SoulCairn_npcLimit

	Event OnHighlightST()

		SetInfoText("Limits the number of added NPCs to a maximum of " + (limitGlobals[1].GetValue() as Int))
		
	endEvent

	Event OnSliderAcceptST(Float value)

		limitGlobals[1].SetValue(value)
		SetSliderOptionValueST(limitGlobals[1].GetValue(), "{0}", false)
		
	endEvent
	
	Event OnSliderOpenST()

		SetSliderDialogStartValue(limitGlobals[1].GetValue())
		SetSliderDialogDefaultValue(defValue)
		SetSliderDialogRange(defMinValue, defMaxValue)
		SetSliderDialogInterval(defInterval)		
	
	endEvent
	
endState

State SoulCairn_npcList

	Event OnMenuOpenST()

		SetMenuDialogOptions(soulCairn_List)
		SetMenuDialogStartIndex(0)
		SetMenuDialogDefaultIndex(0)
		
	endEvent
	
endState

State SoulCairn_clearNPCList

	Event OnHighlightST()

		SetInfoText("Clears " + (countGlobals[1].GetValue() as Int) + " NPCs currently in the Soul Cairn.")
		
	endEvent

	Event OnSelectST()

		if showmessage("Are you sure?", true, "$Accept", "$Cancel")
			ClearList(1)
			SetOptionFlagsST(OPTION_FLAG_DISABLED, false)
		endIf
		
	endEvent
	
endState

;-- MCM function to avoid code duplication ---------------------------------------

function Init_PageReset(Int aiType)

	String asState
	Actorbase[] souls
	if aiType == kSOVNGARDE
		asState = "Sovngarde"
		souls = GetStoredSouls(0)
		if souls.length > 0
			sovngarde_List = GetSortedNPCNames(souls, "(s)")
		else
			sovngarde_List = new String[1]
			sovngarde_List[0] = "NONE"
		endif	
	else
		asState = "SoulCairn"
		souls = GetStoredSouls(1)
		if souls.length > 0
			soulCairn_List = GetSortedNPCNames(souls, "(s)")
		else
			soulCairn_List = new String[1]
			soulCairn_List[0] = "NONE"
		endif
	endif
	if souls.length != countGlobals[aiType].GetValue() as Int
		countGlobals[aiType].SetValue(souls.length as Float)
	endif
	
	SetCursorFillMode(TOP_TO_BOTTOM)
	SetCursorPosition(0)

	String killCountText = "Number of NPCs in " + Pages[aiType] ;doubles as location name
	AddTextOption(killCountText, (countGlobals[aiType].GetValue() as Int))
	
	String uniqueOnlyState = asState + "_uniquesOnly"	
	if !PlayerRef.IsInLocation(soulLocations[aiType])
		AddToggleOptionST(uniqueOnlyState, "Unique NPCs only?", uniquesToggle[aiType])
	else
		AddToggleOptionST(uniqueOnlyState, "Unique NPCs only?", uniquesToggle[aiType], OPTION_FLAG_DISABLED)
	endif
	
	String npcLimitState = asState + "_npcLimit"	
	AddSliderOptionST(npcLimitState, "NPC Limit", limitGlobals[aiType].GetValue(), "{0}")

	AddEmptyOption()
	
	String npcListState = asState + "_npcList"	
	if aiType == kSOVNGARDE
		AddMenuOptionST(npcListState, "NPCs", sovngarde_List[0])
	else
		AddMenuOptionST(npcListState, "NPCs", soulCairn_List[0])
	endif

	String clearListState = asState + "_clearNPCList"	
	if countGlobals[aiType].GetValue() > 0 && !PlayerRef.IsInLocation(soulLocations[aiType])
		AddTextOptionST(clearListState, "Clear NPC list", "")
	else
		AddTextOptionST(clearListState, "Clear NPC list", "", OPTION_FLAG_DISABLED)
	endIf
	
endFunction	

;-- Functions ---------------------------------------
function ClearList(Int aiType)

	countGlobals[aiType].SetValue(0.0)
	ClearAllSouls(aiType)
	
endFunction