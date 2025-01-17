//
// Created by johnk on 2025/1/16.
//

#pragma once

#include <Common/Concepts.h>
#include <Core/Thread.h>

namespace Core {
    template <typename T>
    concept ConsoleSettingBasicType = Common::CppIntegral<T> || Common::CppStdString<T>;

    class ConsoleSetting {
    public:
        virtual ~ConsoleSetting();

        // any-thread
        virtual int8_t ValueI8() const;
        virtual uint8_t ValueU8() const;
        virtual int16_t ValueI16() const;
        virtual uint16_t ValueU16() const;
        virtual int32_t ValueI32() const;
        virtual uint32_t ValueU32() const;
        virtual int64_t ValueI64() const;
        virtual uint64_t ValueU64() const;
        virtual bool ValueBool() const;
        virtual float ValueFloat() const;
        virtual double ValueDouble() const;
        virtual std::string ValueString() const;

        // TODO setter

    protected:
        ConsoleSetting(const std::string& inName, const std::string& inDescription);

    private:
        std::string name;
        std::string description;
    };

    template <ConsoleSettingBasicType T>
    class ConsoleSettingValue final : public ConsoleSetting {
    public:
        ConsoleSettingValue(const std::string& inName, const std::string& inDescription, const T& inDefaultValue);
        ~ConsoleSettingValue() override;

        // any-thread
        const T& Value() const;
        // game-thread
        const T& ValueGT() const;
        // render-thread
        const T& ValueRT() const;

        int8_t ValueI8() const override;
        uint8_t ValueU8() const override;
        int16_t ValueI16() const override;
        uint16_t ValueU16() const override;
        int32_t ValueI32() const override;
        uint32_t ValueU32() const override;
        int64_t ValueI64() const override;
        uint64_t ValueU64() const override;
        bool ValueBool() const override;
        float ValueFloat() const override;
        double ValueDouble() const override;
        std::string ValueString() const override;

        // TODO setter

    private:
        // 0: game/gameWorker
        // 1: render/renderWorker
        T value[2];
    };

    enum class CSRThreadScope : uint8_t {
        // for game/gameWorker
        game,
        // for render/renderWorker
        render,
        max
    };

    template <ConsoleSettingBasicType T>
    class ConsoleSettingRef final : public ConsoleSetting {
    public:
        ConsoleSettingRef(const std::string& inName, const std::string& inDescription, T& inRef, CSRThreadScope inScope);
        ~ConsoleSettingRef() override;

        // TODO

    private:
        CSRThreadScope scope;
        // scope == CSRThreadScope::game: same as valueGT
        // scope == CSRThreadScope::render: render thread ref
        T& ref;
        T valueGT;
    };
}
