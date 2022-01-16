#include "serialization.h"

namespace Afterlife
{
	Base::Base() :
		_souls(),
		_lock()
	{
	}

	bool Base::Register(const RE::TESNPC* a_npc)
	{
		const auto soulData = SoulData{
			a_npc->GetFormID(),
			a_npc->IsUnique()
		};

		Locker locker(_lock);
		if (soulData.second && std::ranges::find_if(_souls, [soulData](const auto& soul) { return soul.first == soulData; }) != _souls.end()) {
			return false;
		}

		const auto addedCount = _souls[soulData] + 1;
		if (_souls[soulData] > 100 - addedCount) {
			return false;
		}

		++_souls[soulData];

		return true;
	}

	bool Base::Register(const RE::TESNPC* a_npc, std::uint16_t a_count)
	{
		const auto soulData = SoulData{
			a_npc->GetFormID(),
			a_npc->IsUnique()
		};

		Locker locker(_lock);
		if (soulData.second && std::ranges::find_if(_souls, [soulData](const auto& soul) { return soul.first == soulData; }) != _souls.end()) {
			return false;
		}

		if (a_count > 0 && _souls[soulData] > (100 - a_count)) {
			return false;
		}

		_souls[soulData] += a_count;

		return true;
	}

	bool Base::Unregister(const RE::TESNPC* a_npc)
	{
		const auto soulData = SoulData{
			a_npc->GetFormID(),
			a_npc->IsUnique()
		};

		Locker locker(_lock);

		if (auto it = _souls.find(soulData); it != _souls.end()) {
			if (soulData.second || it->second <= 1) {
				_souls.erase(it);
			} else {
				it->second--;
			}
			return true;
		}

		return false;
	}

	void Base::Clear()
	{
		Locker locker(_lock);
		_souls.clear();
	}

	bool Base::Save(SKSE::SerializationInterface* a_intfc, std::uint32_t a_type, std::uint32_t a_version)
	{
		if (!a_intfc->OpenRecord(a_type, a_version)) {
			logger::error("Failed to open serialization record!"sv);
			return false;
		} else {
			return Save(a_intfc);
		}
	}

	bool Base::Save(SKSE::SerializationInterface* a_intfc)
	{
		assert(a_intfc);
		Locker locker(_lock);

		const std::size_t numSouls = _souls.size();
		if (!a_intfc->WriteRecordData(numSouls)) {
			logger::error("Failed to save number of souls ({})", numSouls);
			return false;
		}

		std::uint16_t soulCount = 0;

		for (const auto& [soulData, count] : _souls) {
			if (count > 100) {
				continue;
			}

			const auto& [formID, unique] = soulData;
			if (!a_intfc->WriteRecordData(formID)) {
				logger::error("Failed to save soul formID ({:X})", formID);
				return false;
			}
			if (!a_intfc->WriteRecordData(unique)) {
				logger::error("Failed to save soul unique state ({})", unique);
				return false;
			}
			if (!a_intfc->WriteRecordData(count)) {
				logger::error("Failed to save soul count ({})", count);
				return false;
			}

			soulCount += count;
		}

		logger::info("{} - {} actors saved", GetType(), soulCount);

		return true;
	}

	bool Base::Load(SKSE::SerializationInterface* a_intfc)
	{
		assert(a_intfc);
		std::size_t size;
		a_intfc->ReadRecordData(size);

		Locker locker(_lock);
		_souls.clear();

		RE::FormID formID;
		bool isUnique;
		std::uint16_t count = 0;

		std::uint16_t soulCount = 0;

		for (std::size_t i = 0; i < size; i++) {
			a_intfc->ReadRecordData(formID);
			if (!a_intfc->ResolveFormID(formID, formID)) {
				logger::error("Failed to resolve formID {}"sv, formID);
				continue;
			}
			a_intfc->ReadRecordData(isUnique);
			SoulData soulData = { formID, isUnique };
			if (soulData.second && _souls.count(soulData) > 1) {
				continue;
			}
			a_intfc->ReadRecordData(count);
			if (count > 100) {
				continue;
			}
			_souls[soulData] += count;

			soulCount += count;
		}

		logger::info("{} - {} actors loaded", GetType(), soulCount);

		return true;
	}

	Base::SoulMap Base::GetStoredSouls_Impl()
	{
		SoulMap soulMap;

		Locker locker(_lock);
		for (auto& [soulData, count] : _souls) {
			const auto& [formID, isUnique] = soulData;
			if (auto npc = RE::TESForm::LookupByID<RE::TESNPC>(formID); npc) {
				if (isUnique && count > 1) {
					count = 1;
				}
			    soulMap[npc] += count;
			}
		}

		return soulMap;
	}

	std::vector<RE::TESNPC*> Base::GetStoredSouls()
	{
		std::vector<RE::TESNPC*> vec;

		auto soulMap = GetStoredSouls_Impl();
		for (auto& [npc, count] : soulMap) {
			for (std::uint16_t i = 0; i < count; i++) {
				vec.push_back(npc);
			}
		}

		return vec;
	}

	std::uint32_t Base::ClearGenericSouls()
	{
		Locker locker(_lock);
		const auto result = std::erase_if(_souls, [](const auto& data) {
			auto const& [soulData, count] = data;
			return soulData.second == false;
		});
		return static_cast<std::uint32_t>(result);
	}
}

namespace Serialization
{
	std::string DecodeTypeCode(std::uint32_t a_typeCode)
	{
		constexpr std::size_t SIZE = sizeof(std::uint32_t);

		std::string sig;
		sig.resize(SIZE);
		char* iter = reinterpret_cast<char*>(&a_typeCode);
		for (std::size_t i = 0, j = SIZE - 2; i < SIZE - 1; ++i, --j) {
			sig[j] = iter[i];
		}
		return sig;
	}

	void SaveCallback(SKSE::SerializationInterface* a_intfc)
	{
		const auto sovn = Afterlife::Sovngarde::GetSingleton();
		if (!sovn->Save(a_intfc, kSovngarde, kSerializationVersion)) {
			logger::critical("Failed to save Sovngarde regs!"sv);
		}

		const auto soul = Afterlife::SoulCairn::GetSingleton();
		if (!soul->Save(a_intfc, kSoulCairn, kSerializationVersion)) {
			logger::critical("Failed to save SoulCairn regs!"sv);
		}

		logger::info("Finished saving data"sv);
	}

	void LoadCallback(SKSE::SerializationInterface* a_intfc)
	{
		std::uint32_t type;
		std::uint32_t version;
		std::uint32_t length;
		while (a_intfc->GetNextRecordInfo(type, version, length)) {
			if (version != kSerializationVersion) {
				logger::critical("Loaded data is out of date! Read ({}), expected ({}) for type code ({})", version, kSerializationVersion, DecodeTypeCode(type));
				continue;
			}
			switch (type) {
			case kSovngarde:
				{
					auto sovn = Afterlife::Sovngarde::GetSingleton();
					if (!sovn->Load(a_intfc)) {
						logger::critical("Failed to load Sovngarde regs!"sv);
					}
				}
				break;
			case kSoulCairn:
				{
					auto soul = Afterlife::SoulCairn::GetSingleton();
					if (!soul->Load(a_intfc)) {
						logger::critical("Failed to load SoulCairn regs!"sv);
					}
				}
				break;
			default:
				logger::critical("Unrecognized record type ({})!", DecodeTypeCode(type));
				break;
			}
		}
	}

	void RevertCallback(SKSE::SerializationInterface*)
	{
		const auto sovn = Afterlife::Sovngarde::GetSingleton();
		sovn->Clear();

		const auto soul = Afterlife::SoulCairn::GetSingleton();
		soul->Clear();

		logger::info("Reverting...");
	}
}
