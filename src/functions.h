#pragma once

#include "serialization.h"

#define BIND(a_method, ...) a_vm->RegisterFunction(#a_method##sv, obj, a_method __VA_OPT__(, ) __VA_ARGS__)

namespace Afterlife
{
	using VM = RE::BSScript::IVirtualMachine;
	using StackID = RE::VMStackID;
	using Severity = RE::BSScript::ErrorLogger::Severity;

	inline std::uint32_t GetPastSouls(RE::StaticFunctionTag*, std::uint32_t, bool a_unique)
	{
		std::uint32_t soulCount = 0;

		if (const auto TES = RE::TES::GetSingleton()) {
			static std::array<std::string, 8> alreadyPresent = {
				"Ulfric Stormcloak",
				"Froki Whetted-Blade",
				"Kodlak Whitemane",
				"Legate Rikke",
				"Galmar Stone-Fist",
				"Gormlaith Golden-Hilt",
				"Felldir the Old",
				"Hakon One-Eye"
			};

			const auto is_worthy = [a_unique](RE::TESNPC* a_npc) {
				if (a_unique && !a_npc->IsUnique()) {
					return false;
				}
				const std::string name = a_npc->GetName();
				if (name.empty()) {
					return false;
				}
				if (const auto race = a_npc->GetRace(); race && race->GetFormID() == 0x00013746) {  //nord race
					if (std::ranges::find(alreadyPresent, name) != alreadyPresent.end()) {
						return false;
					}
					return a_npc->GetConfidenceLevel() > RE::ACTOR_CONFIDENCE::kCowardly;
				}
				return a_npc->GetFormID() == 0x040285C3;  //ebony warrior;
			};

			const auto sovnMap = Sovngarde::GetSingleton();
			for (const auto& deadCount : TES->deadCount) {
				if (deadCount) {
					auto& [npc, count] = *deadCount;
					if (npc && is_worthy(npc) && count < 100 && sovnMap->Register(npc, count)) {
						soulCount += count;
					}
				}
			}
		}

		return soulCount;
	}

	inline std::vector<RE::TESNPC*> GetStoredSouls(VM*, StackID, RE::StaticFunctionTag*, std::uint32_t a_type)
	{
		std::vector<RE::TESNPC*> vec;

		switch (a_type) {
		case 0:
			vec = Sovngarde::GetSingleton()->GetStoredSouls();
			break;
		case 1:
			vec = SoulCairn::GetSingleton()->GetStoredSouls();
			break;
		default:
			break;
		}

		return vec;
	}

	inline bool StoreSoul(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESNPC* a_npc, std::uint32_t a_type)
	{
		if (!a_npc || a_npc->IsDynamicForm()) {  //avoid placeatme'd actors that have dynamic base
			a_vm->TraceStack("NPC is None", a_stackID, Severity::kWarning);
			return false;
		}

		if (const std::string name = a_npc->GetName(); name.empty()) {
			return false;
		}

		switch (a_type) {
		case 0:
			return Sovngarde::GetSingleton()->Register(a_npc);
		case 1:
			return SoulCairn::GetSingleton()->Register(a_npc);
		default:
			return false;
		}
	}

	inline bool ClearSoul(VM* a_vm, StackID a_stackID, RE::StaticFunctionTag*, RE::TESNPC* a_npc, std::uint32_t a_type)
	{
		if (!a_npc || a_npc->IsDynamicForm()) {  //avoid placeatme'd actors that have dynamic base
			a_vm->TraceStack("NPC is None", a_stackID, Severity::kWarning);
			return false;
		}

		if (const std::string name = a_npc->GetName(); name.empty()) {
			return false;
		}

		switch (a_type) {
		case 0:
			return Sovngarde::GetSingleton()->Unregister(a_npc);
		case 1:
			return SoulCairn::GetSingleton()->Unregister(a_npc);
		default:
			return false;
		}
	}

	inline std::uint32_t ClearAllGenericSouls(VM*, StackID, RE::StaticFunctionTag*, std::uint32_t a_type)
	{
		switch (a_type) {
		case 0:
			return Sovngarde::GetSingleton()->ClearGenericSouls();
		case 1:
			return SoulCairn::GetSingleton()->ClearGenericSouls();
		default:
			return 0;
		}
	}

	inline void ClearAllSouls(VM*, StackID, RE::StaticFunctionTag*, std::uint32_t a_type)
	{
		switch (a_type) {
		case 0:
			Sovngarde::GetSingleton()->Clear();
			break;
		case 1:
			SoulCairn::GetSingleton()->Clear();
			break;
		default:
			break;
		}
	}

	inline bool RegisterFuncs(VM* a_vm)
	{
		if (!a_vm) {
			return false;
		}

		constexpr auto obj = "PO3_Afterlife"sv;

		BIND(GetPastSouls);
		BIND(GetStoredSouls);
		BIND(StoreSoul);
		BIND(ClearSoul);
		BIND(ClearAllGenericSouls);
		BIND(ClearAllSouls);

		logger::info("Registered functions"sv);

		return true;
	}
}
