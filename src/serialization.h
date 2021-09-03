#pragma once

namespace Afterlife
{
	class Base
	{
	public:
		using SoulData = std::pair<RE::FormID, bool>;
	    using BaseSoulMap = std::map<SoulData, std::uint16_t>;
		using SoulMap = std::map<RE::TESNPC*, std::uint16_t>;

		Base();
		Base(Base&) = delete;
		Base(Base&&) = delete;
		virtual ~Base() = default;

		Base& operator=(const Base&) = delete;
		Base& operator=(Base&&) = delete;

		virtual const char* GetType() = 0;

		bool Register(const RE::TESNPC* a_npc);
		bool Register(const RE::TESNPC* a_npc, std::uint16_t a_count);

		void Clear();
		bool Save(SKSE::SerializationInterface* a_intfc, std::uint32_t a_type, std::uint32_t a_version);
		bool Save(SKSE::SerializationInterface* a_intfc);
		bool Load(SKSE::SerializationInterface* a_intfc);

	    std::vector<RE::TESNPC*> GetStoredSouls();
		std::uint32_t ClearGenericSouls();

	protected:
		using Lock = std::recursive_mutex;
		using Locker = std::lock_guard<Lock>;

		BaseSoulMap _souls;
		mutable Lock _lock;

		SoulMap GetStoredSouls_Impl();
	};

	class Sovngarde final : public Base
	{
	public:
		static Sovngarde* GetSingleton()
		{
			static Sovngarde singleton;
			return &singleton;
		}

		const char* GetType() override
		{
			return "Sovngarde";
		}

	protected:
		Sovngarde() = default;
		Sovngarde(const Sovngarde&) = delete;
		Sovngarde(Sovngarde&&) = delete;
		~Sovngarde() override = default;

		Sovngarde& operator=(const Sovngarde&) = delete;
		Sovngarde& operator=(Sovngarde&&) = delete;
	};

	class SoulCairn final : public Base
	{
	public:
		static SoulCairn* GetSingleton()
		{
			static SoulCairn singleton;
			return &singleton;
		}

		const char* GetType() override
		{
			return "Soul Cairn";
		}

	protected:
		SoulCairn() = default;
		SoulCairn(const SoulCairn&) = delete;
		~SoulCairn() override = default;

		SoulCairn& operator=(const SoulCairn&) = delete;
		SoulCairn& operator=(SoulCairn&&) = delete;
	};
}

namespace Serialization
{
	constexpr std::uint32_t kSerializationVersion = 2;
	constexpr std::uint32_t kAfterlife = 'AFTL';
	constexpr std::uint32_t kSovngarde = 'SOVN';
	constexpr std::uint32_t kSoulCairn = 'SOUL';

	std::string DecodeTypeCode(std::uint32_t a_typeCode);

	void SaveCallback(SKSE::SerializationInterface* a_intfc);
	void LoadCallback(SKSE::SerializationInterface* a_intfc);
	void RevertCallback(SKSE::SerializationInterface* a_intfc);
}
