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
        virtual int8_t GetI8() const;
        virtual uint8_t GetU8() const;
        virtual int16_t GetI16() const;
        virtual uint16_t GetU16() const;
        virtual int32_t GetI32() const;
        virtual uint32_t GetU32() const;
        virtual int64_t GetI64() const;
        virtual uint64_t GetU64() const;
        virtual bool GetBool() const;
        virtual float GetFloat() const;
        virtual double GetDouble() const;
        virtual std::string GetString() const;

        // game-thread
        virtual void SetI8(int8_t inValue);
        virtual void SetU8(uint8_t inValue);
        virtual void SetI16(int16_t inValue);
        virtual void SetU16(uint16_t inValue);
        virtual void SetI32(int32_t inValue);
        virtual void SetU32(uint32_t inValue);
        virtual void SetI64(int64_t inValue);
        virtual void SetU64(uint64_t inValue);
        virtual void SetBool(bool inValue);
        virtual void SetFloat(float inValue);
        virtual void SetDouble(double inValue);
        virtual void SetString(const std::string& inValue);

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

    private:
        // 0: game/gameWorker
        // 1: render/renderWorker
        T value[2];
    };
}
