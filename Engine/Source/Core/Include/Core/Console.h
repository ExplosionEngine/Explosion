//
// Created by johnk on 2025/1/16.
//

#pragma once

#include <unordered_map>

#include <Common/Concepts.h>
#include <Common/String.h>
#include <Common/Utility.h>
#include <Common/Math/Common.h>
#include <Core/Thread.h>
#include <Core/Api.h>

namespace Core {
    enum class CSFlagBits : uint8_t {
        configOverridable = 0x1,
        max = 0x2
    };
    using CSFlags = Common::Flags<CSFlagBits>;
    DECLARE_FLAG_BITS_OP(CSFlags, CSFlagBits)

    template <typename T>
    concept ConsoleSettingBasicType = Common::CppArithmetic<T> || Common::CppStdString<T>;

    class CORE_API ConsoleSetting {
    public:
        NonCopyable(ConsoleSetting)
        NonMovable(ConsoleSetting)

        virtual ~ConsoleSetting();

        const std::string& Name() const;
        const std::string& Description() const;
        CSFlags Flags() const;

        // any-thread
        virtual int8_t GetI8() const = 0;
        virtual uint8_t GetU8() const = 0;
        virtual int16_t GetI16() const = 0;
        virtual uint16_t GetU16() const = 0;
        virtual int32_t GetI32() const = 0;
        virtual uint32_t GetU32() const = 0;
        virtual int64_t GetI64() const = 0;
        virtual uint64_t GetU64() const = 0;
        virtual bool GetBool() const = 0;
        virtual float GetFloat() const = 0;
        virtual double GetDouble() const = 0;
        virtual std::string GetString() const = 0;

        // game-thread
        virtual void SetI8(int8_t inValue) = 0;
        virtual void SetU8(uint8_t inValue) = 0;
        virtual void SetI16(int16_t inValue) = 0;
        virtual void SetU16(uint16_t inValue) = 0;
        virtual void SetI32(int32_t inValue) = 0;
        virtual void SetU32(uint32_t inValue) = 0;
        virtual void SetI64(int64_t inValue) = 0;
        virtual void SetU64(uint64_t inValue) = 0;
        virtual void SetBool(bool inValue) = 0;
        virtual void SetFloat(float inValue) = 0;
        virtual void SetDouble(double inValue) = 0;
        virtual void SetString(const std::string& inValue) = 0;

    protected:
        ConsoleSetting(const std::string& inName, const std::string& inDescription, const CSFlags& inFlags);

        virtual void PerformRenderThreadCopy() = 0;

    private:
        friend class Console;

        std::string name;
        std::string description;
        CSFlags flags;
    };

    template <ConsoleSettingBasicType T>
    class ConsoleSettingValue final : public ConsoleSetting {
    public:
        NonCopyable(ConsoleSettingValue)
        NonMovable(ConsoleSettingValue)

        ConsoleSettingValue(const std::string& inName, const std::string& inDescription, const T& inDefaultValue, const CSFlags& inFlags = CSFlags::null);
        ~ConsoleSettingValue() override;

        // any-thread
        const T& Get() const;
        // game-thread
        const T& GetGT() const;
        // render-thread
        const T& GetRT() const;
        // game-thread
        void Set(const T& inValue);

        // any-thread
        int8_t GetI8() const override;
        uint8_t GetU8() const override;
        int16_t GetI16() const override;
        uint16_t GetU16() const override;
        int32_t GetI32() const override;
        uint32_t GetU32() const override;
        int64_t GetI64() const override;
        uint64_t GetU64() const override;
        bool GetBool() const override;
        float GetFloat() const override;
        double GetDouble() const override;
        std::string GetString() const override;

        // game-thread
        void SetI8(int8_t inValue) override;
        void SetU8(uint8_t inValue) override;
        void SetI16(int16_t inValue) override;
        void SetU16(uint16_t inValue) override;
        void SetI32(int32_t inValue) override;
        void SetU32(uint32_t inValue) override;
        void SetI64(int64_t inValue) override;
        void SetU64(uint64_t inValue) override;
        void SetBool(bool inValue) override;
        void SetFloat(float inValue) override;
        void SetDouble(double inValue) override;
        void SetString(const std::string& inValue) override;

    protected:
        void PerformRenderThreadCopy() override;

    private:
        // 0: game/gameWorker
        // 1: render/renderWorker
        T value[2];
        bool dirty;
    };

    class CORE_API Console {
    public:
        static Console& Get();
        ~Console();

        bool HasSetting(const std::string& inName) const;
        ConsoleSetting* FindSetting(const std::string& inName) const;
        ConsoleSetting& GetSetting(const std::string& inName) const;
        template <typename T> ConsoleSettingValue<T>* FindSettingValue(const std::string& inName) const;
        template <typename T> ConsoleSettingValue<T>& GetSettingValue(const std::string& inName) const;
        void OverrideSettingsByConfig() const;
        void PerformRenderThreadSettingsCopy() const;

    private:
        friend class ConsoleSetting;

        Console();

        void RegisterConsoleSetting(ConsoleSetting& inSetting);
        void UnregisterConsoleSetting(ConsoleSetting& inSetting);

        std::unordered_map<std::string, ConsoleSetting*> settings;
    };
}

namespace Core {
    template <ConsoleSettingBasicType T>
    ConsoleSettingValue<T>::ConsoleSettingValue(const std::string& inName, const std::string& inDescription, const T& inDefaultValue, const CSFlags& inFlags)
        : ConsoleSetting(inName, inDescription, inFlags)
        , dirty(false)
    {
        value[0] = inDefaultValue;
        value[1] = inDefaultValue;
    }

    template <ConsoleSettingBasicType T>
    ConsoleSettingValue<T>::~ConsoleSettingValue() = default;

    template <ConsoleSettingBasicType T>
    const T& ConsoleSettingValue<T>::Get() const
    {
        if (ThreadContext::IsGameOrWorkerThread()) {
            return value[0];
        }
        if (ThreadContext::IsRenderOrWorkerThread()) {
            return value[1];
        }
        return QuickFail(), value[0];
    }

    template <ConsoleSettingBasicType T>
    const T& ConsoleSettingValue<T>::GetGT() const
    {
        Assert(ThreadContext::IsGameOrWorkerThread());
        return value[0];
    }

    template <ConsoleSettingBasicType T>
    const T& ConsoleSettingValue<T>::GetRT() const
    {
        Assert(ThreadContext::IsRenderOrWorkerThread());
        return value[1];
    }

    template <ConsoleSettingBasicType T>
    void ConsoleSettingValue<T>::Set(const T& inValue)
    {
        value[0] = inValue;
        dirty = true;
    }

    template <ConsoleSettingBasicType T>
    void ConsoleSettingValue<T>::PerformRenderThreadCopy()
    {
        if (!dirty) {
            return;
        }
        value[1] = value[0];
        dirty = false;
    }

    template <ConsoleSettingBasicType T>
    int8_t ConsoleSettingValue<T>::GetI8() const
    {
        if constexpr (Common::CppStdString<T>) {
            return Common::ToArithmetic<int8_t>(Get());
        } else if constexpr (Common::CppBool<T>) {
            return Get() ? 1 : 0;
        } else if constexpr (Common::CppArithmetic<T>) {
            return static_cast<int8_t>(Get());
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    uint8_t ConsoleSettingValue<T>::GetU8() const
    {
        if constexpr (Common::CppStdString<T>) {
            return Common::ToArithmetic<uint8_t>(Get());
        } else if constexpr (Common::CppBool<T>) {
            return Get() ? 1 : 0;
        } else if constexpr (Common::CppArithmetic<T>) {
            return static_cast<uint8_t>(Get());
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    int16_t ConsoleSettingValue<T>::GetI16() const
    {
        if constexpr (Common::CppStdString<T>) {
            return Common::ToArithmetic<int16_t>(Get());
        } else if constexpr (Common::CppBool<T>) {
            return Get() ? 1 : 0;
        } else if constexpr (Common::CppArithmetic<T>) {
            return static_cast<int16_t>(Get());
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    uint16_t ConsoleSettingValue<T>::GetU16() const
    {
        if constexpr (Common::CppStdString<T>) {
            return Common::ToArithmetic<uint16_t>(Get());
        } else if constexpr (Common::CppBool<T>) {
            return Get() ? 1 : 0;
        } else if constexpr (Common::CppArithmetic<T>) {
            return static_cast<uint16_t>(Get());
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    int32_t ConsoleSettingValue<T>::GetI32() const
    {
        if constexpr (Common::CppStdString<T>) {
            return Common::ToArithmetic<int32_t>(Get());
        } else if constexpr (Common::CppBool<T>) {
            return Get() ? 1 : 0;
        } else if constexpr (Common::CppArithmetic<T>) {
            return static_cast<int32_t>(Get());
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    uint32_t ConsoleSettingValue<T>::GetU32() const
    {
        if constexpr (Common::CppStdString<T>) {
            return Common::ToArithmetic<uint32_t>(Get());
        } else if constexpr (Common::CppBool<T>) {
            return Get() ? 1 : 0;
        } else if constexpr (Common::CppArithmetic<T>) {
            return static_cast<uint32_t>(Get());
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    int64_t ConsoleSettingValue<T>::GetI64() const
    {
        if constexpr (Common::CppStdString<T>) {
            return Common::ToArithmetic<int64_t>(Get());
        } else if constexpr (Common::CppBool<T>) {
            return Get() ? 1 : 0;
        } else if constexpr (Common::CppArithmetic<T>) {
            return static_cast<int64_t>(Get());
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    uint64_t ConsoleSettingValue<T>::GetU64() const
    {
        if constexpr (Common::CppStdString<T>) {
            return Common::ToArithmetic<uint64_t>(Get());
        } else if constexpr (Common::CppBool<T>) {
            return Get() ? 1 : 0;
        } else if constexpr (Common::CppArithmetic<T>) {
            return static_cast<uint64_t>(Get());
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    bool ConsoleSettingValue<T>::GetBool() const
    {
        if constexpr (Common::CppStdString<T>) {
            return Common::ToArithmetic<bool>(Get());
        } else if constexpr (Common::CppBool<T>) {
            return Get();
        } else if constexpr (Common::CppArithmetic<T>) {
            return Get() != 0;
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    float ConsoleSettingValue<T>::GetFloat() const
    {
        if constexpr (Common::CppStdString<T>) {
            return Common::ToArithmetic<float>(Get());
        } else if constexpr (Common::CppBool<T>) {
            return Get() ? 1.0f : 0.0f;
        } else if constexpr (Common::CppArithmetic<T>) {
            return static_cast<float>(Get());
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    double ConsoleSettingValue<T>::GetDouble() const
    {
        if constexpr (Common::CppStdString<T>) {
            return Common::ToArithmetic<double>(Get());
        } else if constexpr (Common::CppBool<T>) {
            return Get() ? 1.0 : 0.0;
        } else if constexpr (Common::CppArithmetic<T>) {
            return static_cast<double>(Get());
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    std::string ConsoleSettingValue<T>::GetString() const
    {
        if constexpr (Common::CppStdString<T>) {
            return Get();
        } else if constexpr (Common::CppBool<T>) {
            return Get() ? "true" : "false";
        } else if constexpr (Common::CppArithmetic<T>) {
            return Common::ToString<T>(Get());
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    void ConsoleSettingValue<T>::SetI8(int8_t inValue)
    {
        if constexpr (Common::CppStdString<T>) {
            Set(Common::ToString<int8_t>(inValue));
        } else if constexpr (Common::CppBool<T>) {
            Set(inValue != 0);
        } else if constexpr (Common::CppArithmetic<T>) {
            Set(static_cast<T>(inValue));
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    void ConsoleSettingValue<T>::SetU8(uint8_t inValue)
    {
        if constexpr (Common::CppStdString<T>) {
            Set(Common::ToString<uint8_t>(inValue));
        } else if constexpr (Common::CppBool<T>) {
            Set(inValue != 0);
        } else if constexpr (Common::CppArithmetic<T>) {
            Set(static_cast<T>(inValue));
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    void ConsoleSettingValue<T>::SetI16(int16_t inValue)
    {
        if constexpr (Common::CppStdString<T>) {
            Set(Common::ToString<int16_t>(inValue));
        } else if constexpr (Common::CppBool<T>) {
            Set(inValue != 0);
        } else if constexpr (Common::CppArithmetic<T>) {
            Set(static_cast<T>(inValue));
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    void ConsoleSettingValue<T>::SetU16(uint16_t inValue)
    {
        if constexpr (Common::CppStdString<T>) {
            Set(Common::ToString<uint16_t>(inValue));
        } else if constexpr (Common::CppBool<T>) {
            Set(inValue != 0);
        } else if constexpr (Common::CppArithmetic<T>) {
            Set(static_cast<T>(inValue));
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    void ConsoleSettingValue<T>::SetI32(int32_t inValue)
    {
        if constexpr (Common::CppStdString<T>) {
            Set(Common::ToString<int32_t>(inValue));
        } else if constexpr (Common::CppBool<T>) {
            Set(inValue != 0);
        } else if constexpr (Common::CppArithmetic<T>) {
            Set(static_cast<T>(inValue));
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    void ConsoleSettingValue<T>::SetU32(uint32_t inValue)
    {
        if constexpr (Common::CppStdString<T>) {
            Set(Common::ToString<uint32_t>(inValue));
        } else if constexpr (Common::CppBool<T>) {
            Set(inValue != 0);
        } else if constexpr (Common::CppArithmetic<T>) {
            Set(static_cast<T>(inValue));
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    void ConsoleSettingValue<T>::SetI64(int64_t inValue)
    {
        if constexpr (Common::CppStdString<T>) {
            Set(Common::ToString<int64_t>(inValue));
        } else if constexpr (Common::CppBool<T>) {
            Set(inValue != 0);
        } else if constexpr (Common::CppArithmetic<T>) {
            Set(static_cast<T>(inValue));
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    void ConsoleSettingValue<T>::SetU64(uint64_t inValue)
    {
        if constexpr (Common::CppStdString<T>) {
            Set(Common::ToString<uint64_t>(inValue));
        } else if constexpr (Common::CppBool<T>) {
            Set(inValue != 0);
        } else if constexpr (Common::CppArithmetic<T>) {
            Set(static_cast<T>(inValue));
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    void ConsoleSettingValue<T>::SetBool(bool inValue)
    {
        if constexpr (Common::CppStdString<T>) {
            Set(Common::ToString<bool>(inValue));
        } else if constexpr (Common::CppBool<T>) {
            Set(inValue);
        } else if constexpr (Common::CppArithmetic<T>) {
            Set(inValue ? 1 : 0);
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    void ConsoleSettingValue<T>::SetFloat(float inValue)
    {
        if constexpr (Common::CppStdString<T>) {
            Set(Common::ToString<float>(inValue));
        } else if constexpr (Common::CppBool<T>) {
            Set(!Common::CompareNumber(inValue, 0.0f));
        } else if constexpr (Common::CppArithmetic<T>) {
            Set(static_cast<T>(inValue));
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    void ConsoleSettingValue<T>::SetDouble(double inValue)
    {
        if constexpr (Common::CppStdString<T>) {
            Set(Common::ToString<double>(inValue));
        } else if constexpr (Common::CppBool<T>) {
            Set(!Common::CompareNumber(inValue, 0.0));
        } else if constexpr (Common::CppArithmetic<T>) {
            Set(static_cast<T>(inValue));
        } else {
            Unimplement();
        }
    }

    template <ConsoleSettingBasicType T>
    void ConsoleSettingValue<T>::SetString(const std::string& inValue)
    {
        if constexpr (Common::CppStdString<T>) {
            Set(inValue);
        } else if constexpr (Common::CppArithmetic<T>) {
            Set(Common::ToArithmetic<T>(inValue));
        } else {
            Unimplement();
        }
    }

    template <typename T>
    ConsoleSettingValue<T>* Console::FindSettingValue(const std::string& inName) const
    {
        return dynamic_cast<ConsoleSettingValue<T>*>(FindSettingValue<T>(inName));
    }

    template <typename T>
    ConsoleSettingValue<T>& Console::GetSettingValue(const std::string& inName) const
    {
        auto* result = FindSettingValue<T>(inName);
        Assert(result != nullptr);
        return *result;
    }
}
