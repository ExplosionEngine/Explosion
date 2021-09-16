//
// Created by John Kindem on 2021/9/16.
//

#ifndef EXPLOSION_MIRROR_H
#define EXPLOSION_MIRROR_H

#include <type_traits>
#include <functional>
#include <unordered_map>
#include <vector>
#include <variant>

namespace Explosion::Mirror {
    class Any;
    class Ref;
}

namespace Explosion::Mirror::Internal {
    enum class StorageCategory {
        SMALL,
        BIG
    };

    inline constexpr size_t MAX_SMALL_STORAGE_SIZE = sizeof(void*) * 4;

    template <typename T>
    inline constexpr StorageCategory GetStorageCategory()
    {
        return sizeof(T) <= MAX_SMALL_STORAGE_SIZE && std::is_trivially_copyable_v<T> ?
            StorageCategory::SMALL :
            StorageCategory::BIG;
    }

    struct BigStorageRtti {
        std::function<void*(void*)> copyFunc;
        std::function<void*(void*)> moveFunc;
        std::function<void(void*)> destroyFunc;
    };

    template <typename T>
    struct BigStorageRttiImpl {
        static void* Copy(void* src)
        {
            return new T(*static_cast<T*>(src));
        }

        static void* Move(void* src)
        {
            return new T(std::move(*static_cast<T*>(src)));
        }

        static void Destroy(void* src)
        {
            delete (static_cast<T*>(src));
        }
    };

    template <typename T>
    static BigStorageRtti bigStorageRtti {
        &BigStorageRttiImpl<T>::Copy,
        &BigStorageRttiImpl<T>::Move,
        &BigStorageRttiImpl<T>::Destroy
    };

    struct SmallStorage {
        uint8_t memory[MAX_SMALL_STORAGE_SIZE];
    };

    struct BigStorage {
        void* memory;
        BigStorageRtti* rtti;
    };

    using Storage = std::variant<SmallStorage, BigStorage>;
}

namespace Explosion::Mirror::Internal {
    struct TypeInfo {
        size_t id;
        std::string name;
    };

    struct VariableInfo {
        const TypeInfo* type;
        std::function<Any(Ref)> getter;
        std::function<void(Ref, Ref)> setter;
    };

    struct FunctionInfo {
        const TypeInfo* type;
        const TypeInfo* retType;
        std::vector<const TypeInfo*> argTypes;
        std::function<Any(Ref, std::vector<Ref>)> invoker;
    };

    template <typename T>
    const TypeInfo* FetchTypeInfo()
    {
        static TypeInfo info {
            typeid(T).hash_code(),
            typeid(T).name()
        };
        return &info;
    }
}

namespace Explosion::Mirror::Internal {
    template <typename... Args, size_t ... I>
    void ForwardArgsAsRefVectorInternal(
        std::vector<Ref>& refs, const std::tuple<Args...>& tuple, std::index_sequence<I...>)
    {
        std::initializer_list<int> { (refs[I] = Ref(&std::get<I>(tuple)), 0)... };
    }

    template <typename... Args>
    std::vector<Ref> ForwardArgsAsRefVector(const std::tuple<Args...>& args)
    {
        std::vector<Ref> refs(sizeof...(Args));
        ForwardArgsAsRefVectorInternal(refs, args, std::make_index_sequence<sizeof...(Args)> {});
        return refs;
    }

    template <typename... Args>
    std::vector<const TypeInfo*> FetchArgTypeInfos(const std::tuple<Args...>&)
    {
        std::vector<const TypeInfo*> typeInfos;
        std::initializer_list<int> { (typeInfos.emplace_back(FetchTypeInfo<Args>()), 0)... };
        return typeInfos;
    }

    template <typename... Args, size_t... I>
    void FillTupleWithRefVector(std::tuple<Args...>& tuple, const std::vector<Ref>& refs, std::index_sequence<I...>)
    {
        std::initializer_list<int> { (std::get<I>(tuple) = *static_cast<Args*>(refs[I].Value()), 0)... };
    }

    template <typename... Args>
    std::tuple<Args...> ForwardRefVectorAsTuple(const std::vector<Ref>& refs)
    {
        auto tuple = std::tuple<Args...> {};
        FillTupleWithRefVector(tuple, refs, std::make_index_sequence<sizeof...(Args)> {});
        return tuple;
    }

    template <typename T, typename R, typename... Args, size_t... I>
    R InvokeFuncInternal(T&& func, const std::tuple<Args...>& args, std::index_sequence<I...>)
    {
        return func(std::get<I>(args)...);
    }

    template <typename T, typename R, typename... Args>
    R InvokeFunc(T&& func, const std::tuple<Args...>& args)
    {
        return InvokeFuncInternal<T, R>(std::forward<T>(func), args, std::make_index_sequence<sizeof...(Args)> {});
    }

    template <typename C, auto T, typename R, typename... Args, size_t... I>
    R InvokeMemberFuncInternal(C&& instance, const std::tuple<Args...>& args, std::index_sequence<I...>)
    {
        return instance.*T(std::get<I>(args)...);
    }

    template <typename C, auto T, typename R, typename... Args>
    R InvokeMemberFunc(C&& instance, const std::tuple<Args...>& args)
    {
        return InvokeMemberFuncInternal<C, T, R>(std::forward<C>(instance), args, std::make_index_sequence<sizeof...(Args)> {});
    }

    template <typename T>
    struct MemberPointerTraits {};

    template <typename S, typename V>
    struct MemberPointerTraits<V S::*> {
        using ValueType = V;
    };

    template <typename S, typename R, typename... Args>
    struct MemberPointerTraits<R S::*(Args...)> {
        using RetType = R;
        using ArgsTupleType = std::tuple<Args...>;
    };
}

namespace Explosion::Mirror {
    class BadAnyCastException : public std::exception {
    public:
        [[nodiscard]] const char* what() const noexcept override
        {
            return "bad any cast";
        }
    };

    class NoSuchDefinitionException : public std::exception {
    public:
        [[nodiscard]] const char * what() const noexcept override
        {
            return "no such definition";
        }
    };

    class Any {
    public:
        Any() : storageCategory(Internal::StorageCategory::SMALL), typeId(0) {}

        template <typename T>
        Any(T&& value)
        {
            Construct(std::forward<T>(value));
        }

        template <typename T>
        Any& operator=(T&& value)
        {
            Construct(std::forward<T>(value));
            return *this;
        }

        Any(Any& any) : storageCategory(any.storageCategory), typeId(any.typeId)
        {
            if (storageCategory == Internal::StorageCategory::SMALL) {
                storage = Internal::SmallStorage {};
                auto& sThis = std::get<Internal::SmallStorage>(storage);
                auto& sAny = std::get<Internal::SmallStorage>(any.storage);
                memcpy(sThis.memory, sAny.memory, Internal::MAX_SMALL_STORAGE_SIZE);
            } else {
                storage = Internal::BigStorage {};
                auto& sThis = std::get<Internal::BigStorage>(storage);
                auto& sAny = std::get<Internal::BigStorage>(any.storage);
                sThis.memory = sAny.rtti->copyFunc(sAny.memory);
                sThis.rtti = sAny.rtti;
            }
        }

        Any(Any&& any)  noexcept : storageCategory(any.storageCategory), typeId(any.typeId)
        {
            if (storageCategory == Internal::StorageCategory::SMALL) {
                storage = Internal::SmallStorage {};
                auto& sThis = std::get<Internal::SmallStorage>(storage);
                auto& sAny = std::get<Internal::SmallStorage>(any.storage);
                std::swap(sThis.memory, sAny.memory);
            } else {
                storage = Internal::BigStorage {};
                auto& sThis = std::get<Internal::BigStorage>(storage);
                auto& sAny = std::get<Internal::BigStorage>(any.storage);
                sThis.memory = sAny.rtti->moveFunc(sAny.memory);
                sThis.rtti = sAny.rtti;
            }
        }

        ~Any()
        {
            if (storageCategory == Internal::StorageCategory::SMALL) {
                auto& s = std::get<Internal::SmallStorage>(storage);
                memset(s.memory, 0, Internal::MAX_SMALL_STORAGE_SIZE);
            } else {
                auto& s = std::get<Internal::BigStorage>(storage);
                s.rtti->destroyFunc(s.memory);
            }
        }

        template <typename T>
        T CastTo()
        {
            return *CastToPointer<T>();
        }

        template <typename T>
        T* CastToPointer()
        {
            if (Internal::FetchTypeInfo<T>()->id != typeId) {
                throw BadAnyCastException {};
            }
            return static_cast<T*>(RawValue());
        }

        void* RawValue()
        {
            return storageCategory == Internal::StorageCategory::SMALL ?
                static_cast<void*>(std::get<Internal::SmallStorage>(storage).memory) :
                std::get<Internal::BigStorage>(storage).memory;
        }

    private:
        template <typename T>
        void Construct(T&& value)
        {
            using NoneRefType = std::remove_reference_t<T>;
            storageCategory = Internal::GetStorageCategory<NoneRefType>();
            typeId = Internal::FetchTypeInfo<NoneRefType>()->id;
            if (storageCategory == Internal::StorageCategory::SMALL) {
                storage = Internal::SmallStorage {};
                auto& s = std::get<Internal::SmallStorage>(storage);
                memcpy(s.memory, &value, sizeof(NoneRefType));
            } else {
                storage = Internal::BigStorage {};
                auto& s = std::get<Internal::BigStorage>(storage);
                s.memory = new NoneRefType(std::forward<T>(value));
                s.rtti = &Internal::bigStorageRtti<NoneRefType>;
            }
        }

        Internal::StorageCategory storageCategory;
        Internal::Storage storage;
        size_t typeId;
    };

    class Ref {
    public:
        Ref() : instance(nullptr) {}
        explicit Ref(void* instance) : instance(instance) {}
        explicit Ref(Any& any) : instance(any.RawValue()) {}
        Ref(Ref& ref) = default;
        Ref(Ref&& ref)  noexcept : instance(ref.instance) {}
        ~Ref() = default;
        Ref& operator=(const Ref& ref) = default;

        [[nodiscard]] void* Value() const
        {
            return instance;
        }

    private:
        void* instance;
    };
}

namespace Explosion::Mirror {
    class Type {
    public:
        explicit Type(const Internal::TypeInfo* info) : info(info) {}
        ~Type() = default;

    private:
        const Internal::TypeInfo* info;
    };

    class Definition {
    public:
        Definition() = default;
        ~Definition() = default;

        virtual Type GetType() = 0;
    };

    class Variable : public Definition {
    public:
        explicit Variable(const Internal::VariableInfo* info) : Definition(), info(info) {}
        Variable(Variable&& variable) noexcept : info(variable.info) {}
        Variable(const Variable&) = default;
        Variable& operator=(const Variable&) = default;
        ~Variable() = default;

        Type GetType() override
        {
            return Type(info->type);
        }

        Any Get(Ref instance)
        {
            return info->getter(instance);
        }

        void Set(Ref instance, Ref value)
        {
            info->setter(instance, value);
        }

    private:
        const Internal::VariableInfo* info;
    };

    class Function : public Definition {
    public:
        explicit Function(const Internal::FunctionInfo* info) : Definition(), info(info) {}
        Function(Function&& function) noexcept : info(function.info) {}
        Function(const Function&) = default;
        Function& operator=(const Function&) = default;
        ~Function() = default;

        Type GetType() override
        {
            return Type(info->type);
        }

        template <typename... Args>
        Any Invoke(Ref instance, Args&&... args)
        {
            return info->invoker(instance, Internal::ForwardArgsAsRefVector(std::forward_as_tuple(args...)));
        }

    private:
        const Internal::FunctionInfo* info;
    };
}

namespace Explosion::Mirror {
    class GlobalFactory {
    public:
        GlobalFactory() = default;
        GlobalFactory(const GlobalFactory&) = delete;
        GlobalFactory& operator=(const GlobalFactory&) = delete;
        ~GlobalFactory() = default;

        static GlobalFactory& Singleton()
        {
            static GlobalFactory instance;
            return instance;
        }

        template <typename Value>
        GlobalFactory& DefineVariable(const std::string& name, Value* address)
        {
            variables[name] = std::make_unique<Internal::VariableInfo>(Internal::VariableInfo {
                Internal::FetchTypeInfo<Value>(),
                [address](Ref instance) -> Any { return Any(*address); },
                [address](Ref instance, Ref value) -> void { *address = *static_cast<Value*>(value.Value()); }
            });
            return *this;
        }

        template <typename Ret, typename... Args>
        GlobalFactory& DefineFunction(const std::string& name, Ret(*func)(Args...))
        {
            functions[name] = std::make_unique<Internal::FunctionInfo>(Internal::FunctionInfo {
                Internal::FetchTypeInfo<Ret(*)(Args...)>(),
                Internal::FetchTypeInfo<Ret>(),
                Internal::FetchArgTypeInfos(std::tuple<Args...> {}),
                [func](Ref instance, std::vector<Ref> args) -> Any {
                    auto argsTuple = Internal::ForwardRefVectorAsTuple<Args...>(args);
                    return Any(Internal::InvokeFunc<std::function<Ret(Args...)>, Ret, Args...>(func, argsTuple));
                }
            });
            return *this;
        }

        Variable GetVariable(const std::string& name)
        {
            auto iter = variables.find(name);
            if (iter == variables.end()) {
                throw NoSuchDefinitionException {};
            }
            return Variable(iter->second.get());
        }

        void ForEachVariable(const std::function<void(Variable)>& iter)
        {
            for (auto& v : variables) {
                iter(Variable(v.second.get()));
            }
        }

        Function GetFunction(const std::string& name)
        {
            auto iter = functions.find(name);
            if (iter == functions.end()) {
                throw NoSuchDefinitionException {};
            }
            return Function(iter->second.get());
        }

        void ForEachFunction(const std::function<void(Function)>& iter)
        {
            for (auto& v : functions) {
                iter(Function(v.second.get()));
            }
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<Internal::VariableInfo>> variables;
        std::unordered_map<std::string, std::unique_ptr<Internal::FunctionInfo>> functions;
    };

    template <typename S>
    class StructFactory {
    public:
        StructFactory() = default;
        StructFactory(const StructFactory<S>&) = delete;
        StructFactory& operator=(const StructFactory<S>&) = delete;
        virtual ~StructFactory() = default;

        static StructFactory<S>& Singleton()
        {
            static StructFactory<S> instance;
            return instance;
        }

        template <auto T>
        StructFactory<S>& DefineVariable(const std::string& name)
        {
            using ValueType = typename Internal::MemberPointerTraits<decltype(T)>::ValueType;

            variables[name] = std::make_unique<Internal::VariableInfo>(Internal::VariableInfo {
                Internal::FetchTypeInfo<ValueType>(),
                [](Ref instance) -> Any {
                    return Any(static_cast<S*>(instance.Value())->*T);
                },
                [](Ref instance, Ref value) -> Any {
                    return Any(static_cast<S*>(instance.Value())->*T = *static_cast<ValueType*>(value.Value()));
                }
            });
            return *this;
        }

        Variable GetVariable(const std::string& name)
        {
            auto iter = variables.find(name);
            if (iter == variables.end()) {
                throw NoSuchDefinitionException {};
            }
            return Variable(iter->second.get());
        }

        void ForEachVariable(const std::function<void(Variable)>& iter)
        {
            for (auto& v : variables) {
                iter(Variable(v.second.get()));
            }
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<Internal::VariableInfo>> variables;
    };

    template <typename C>
    class ClassFactory : public StructFactory<C> {
    public:
        ClassFactory() = default;
        ClassFactory(const ClassFactory<C>&) = delete;
        ClassFactory& operator=(const ClassFactory<C>&) = delete;
        ~ClassFactory() override = default;

        static ClassFactory<C>& Singleton()
        {
            static ClassFactory<C> instance;
            return instance;
        }

        template <auto T>
        ClassFactory<C>& DefineFunction(const std::string& name)
        {
            using RetType = typename Internal::MemberPointerTraits<decltype(T)>::RetType;
            using ArgsTupleType = typename Internal::MemberPointerTraits<decltype(T)>::ArgsTupleType;

            functions[name] = std::make_unique<Internal::FunctionInfo>(Internal::FunctionInfo {
                Internal::FetchTypeInfo<decltype(T)>(),
                Internal::FetchTypeInfo<RetType>(),
                Internal::FetchArgTypeInfos(ArgsTupleType {}),
                [](Ref instance, std::vector<Ref> args) -> Any {
                    ArgsTupleType argsTuple {};
                    Internal::FillTupleWithRefVector(argsTuple, args, std::make_index_sequence<ArgsTupleType::size()> {});
                    return Any(Internal::InvokeMemberFunc<C, T, RetType>(*static_cast<C*>(instance.Value()), argsTuple));
                }
            });
            return *this;
        }

        Function GetFunction(const std::string& name)
        {
            auto iter = functions.find(name);
            if (iter == functions.end()) {
                throw NoSuchDefinitionException {};
            }
            return Function(iter->second.get());
        }

        void ForEachFunction(const std::function<void(Function)>& iter)
        {
            for (auto& v : functions) {
                iter(Function(v.second.get()));
            }
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<Internal::FunctionInfo>> functions;
    };
}

#endif //EXPLOSION_MIRROR_H
