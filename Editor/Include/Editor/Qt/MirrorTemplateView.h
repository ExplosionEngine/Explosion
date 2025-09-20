//
// Created by Kindem on 2025/8/26.
//

#pragma once

#include <QList>
#include <QMap>

#include <Mirror/Mirror.h>

namespace Editor {
    struct QListMetaViewRtti {
        static constexpr Mirror::TemplateViewId id = Common::HashUtils::StrCrc32("Editor::QListMetaView");

        using GetElementTypeFunc = const Mirror::TypeInfo*();
        using GetSizeFunc = size_t(const Mirror::Any&);
        using ReserveFunc = void(const Mirror::Any&, size_t);
        using ResizeFunc = void(const Mirror::Any&, size_t);
        using ClearFunc = void(const Mirror::Any&);
        using GetElementFunc = Mirror::Any(const Mirror::Any&, size_t);
        using GetConstElementFunc = Mirror::Any(const Mirror::Any&, size_t);
        using EmplaceBackFunc = Mirror::Any(const Mirror::Any&, const Mirror::Argument&);
        using EmplaceDefaultBackFunc = Mirror::Any(const Mirror::Any&);
        using RemoveFunc = void(const Mirror::Any&, size_t);

        template <typename T> static const Mirror::TypeInfo* GetElementType();
        template <typename T> static size_t GetSize(const Mirror::Any& inRef);
        template <typename T> static void Reserve(const Mirror::Any& inRef, size_t inSize);
        template <typename T> static void Resize(const Mirror::Any& inRef, size_t inSize);
        template <typename T> static void Clear(const Mirror::Any& inRef);
        template <typename T> static Mirror::Any GetElement(const Mirror::Any& inRef, size_t inIndex);
        template <typename T> static Mirror::Any GetConstElement(const Mirror::Any& inRef, size_t inIndex);
        template <typename T> static Mirror::Any EmplaceBack(const Mirror::Any& inRef, const Mirror::Argument& inTempObj);
        template <typename T> static Mirror::Any EmplaceDefaultBack(const Mirror::Any& inRef);
        template <typename T> static void Remove(const Mirror::Any& inRef, size_t inIndex);

        GetElementTypeFunc* getElementType;
        GetSizeFunc* getSize;
        ReserveFunc* reserve;
        ResizeFunc* resize;
        ClearFunc* clear;
        GetElementFunc* getElement;
        GetConstElementFunc* getConstElement;
        EmplaceBackFunc* emplaceBack;
        EmplaceDefaultBackFunc* emplaceDefaultBack;
        RemoveFunc* remove;
    };

    template <typename T>
    static constexpr QListMetaViewRtti qListMetaViewRttiImpl = {
        &QListMetaViewRtti::GetElementType<T>,
        &QListMetaViewRtti::GetSize<T>,
        &QListMetaViewRtti::Reserve<T>,
        &QListMetaViewRtti::Resize<T>,
        &QListMetaViewRtti::Clear<T>,
        &QListMetaViewRtti::GetElement<T>,
        &QListMetaViewRtti::GetConstElement<T>,
        &QListMetaViewRtti::EmplaceBack<T>,
        &QListMetaViewRtti::EmplaceDefaultBack<T>,
        &QListMetaViewRtti::Remove<T>
    };

    class QListMetaView {
    public:
        static constexpr Mirror::TemplateViewId id = QListMetaViewRtti::id;

        explicit QListMetaView(const Mirror::Any& inRef);
        NonCopyable(QListMetaView)
        NonMovable(QListMetaView)

        const Mirror::TypeInfo* ElementType() const;
        size_t Size() const;
        void Reserve(size_t inSize) const;
        void Resize(size_t inSize) const;
        void Clear() const;
        Mirror::Any At(size_t inIndex) const;
        Mirror::Any ConstAt(size_t inIndex) const;
        Mirror::Any EmplaceBack(const Mirror::Argument& inTempObj) const;
        Mirror::Any EmplaceDefaultBack() const;
        void Remove(size_t inIndex) const;

    private:
        Mirror::Any ref;
        const QListMetaViewRtti* rtti;
    };

    struct QMapMetaViewRtti {
        static constexpr Mirror::TemplateViewId id = Common::HashUtils::StrCrc32("Editor::QMapMetaView");

        using GetKeyTypeFunc = const Mirror::TypeInfo*();
        using GetValueTypeFunc = const Mirror::TypeInfo*();
        using CreateKeyFunc = Mirror::Any();
        using CreateValueFunc = Mirror::Any();
        using GetSizeFunc = size_t(const Mirror::Any&);
        using ReserveFunc = void(const Mirror::Any&, size_t);
        using ClearFunc = void(const Mirror::Any&);
        using GetOrAddFunc = Mirror::Any(const Mirror::Any&, const Mirror::Argument&);
        using ConstAtFunc = Mirror::Any(const Mirror::Any&, const Mirror::Argument&);
        using TraverseFunc = void(const Mirror::Any&, const std::function<void(const Mirror::Any&, const Mirror::Any&)>&);
        using ConstTraverseFunc = void(const Mirror::Any&, const std::function<void(const Mirror::Any&, const Mirror::Any&)>&);
        using ContainsFunc = bool(const Mirror::Any&, const Mirror::Argument&);
        using EmplaceFunc = void(const Mirror::Any&, const Mirror::Argument&, const Mirror::Argument&);
        using EraseFunc = void(const Mirror::Any&, const Mirror::Argument&);

        template <typename K, typename V> static const Mirror::TypeInfo* GetKeyType();
        template <typename K, typename V> static const Mirror::TypeInfo* GetValueType();
        template <typename K, typename V> static Mirror::Any CreateKey();
        template <typename K, typename V> static Mirror::Any CreateValue();
        template <typename K, typename V> static size_t GetSize(const Mirror::Any& inRef);
        template <typename K, typename V> static void Reserve(const Mirror::Any& inRef, size_t inSize);
        template <typename K, typename V> static void Clear(const Mirror::Any& inRef);
        template <typename K, typename V> static Mirror::Any GetOrAdd(const Mirror::Any& inRef, const Mirror::Argument& inKey);
        template <typename K, typename V> static Mirror::Any ConstAt(const Mirror::Any& inRef, const Mirror::Argument& inKey);
        template <typename K, typename V> static void Traverse(const Mirror::Any& inRef, const std::function<void(const Mirror::Any&, const Mirror::Any&)>& inVisitor);
        template <typename K, typename V> static void ConstTraverse(const Mirror::Any& inRef, const std::function<void(const Mirror::Any&, const Mirror::Any&)>& inVisitor);
        template <typename K, typename V> static bool Contains(const Mirror::Any& inRef, const Mirror::Argument& inKey);
        template <typename K, typename V> static void Emplace(const Mirror::Any& inRef, const Mirror::Argument& inKey, const Mirror::Argument& inValue);
        template <typename K, typename V> static void Erase(const Mirror::Any& inRef, const Mirror::Argument& inKey);

        GetKeyTypeFunc* getKeyType;
        GetValueTypeFunc* getValueType;
        CreateKeyFunc* createKey;
        CreateValueFunc* createValue;
        GetSizeFunc* getSize;
        ReserveFunc* reserve;
        ClearFunc* clear;
        GetOrAddFunc* getOrAdd;
        ConstAtFunc* constAt;
        TraverseFunc* traverse;
        ConstTraverseFunc* constTraverse;
        ContainsFunc* contains;
        EmplaceFunc* emplace;
        EraseFunc* erase;
    };

    template <typename K, typename V>
    static constexpr QMapMetaViewRtti qMapMetaViewRttiImpl = {
        &QMapMetaViewRtti::GetKeyType<K, V>,
        &QMapMetaViewRtti::GetValueType<K, V>,
        &QMapMetaViewRtti::CreateKey<K, V>,
        &QMapMetaViewRtti::CreateValue<K, V>,
        &QMapMetaViewRtti::GetSize<K, V>,
        &QMapMetaViewRtti::Reserve<K, V>,
        &QMapMetaViewRtti::Clear<K, V>,
        &QMapMetaViewRtti::GetOrAdd<K, V>,
        &QMapMetaViewRtti::ConstAt<K, V>,
        &QMapMetaViewRtti::Traverse<K, V>,
        &QMapMetaViewRtti::ConstTraverse<K, V>,
        &QMapMetaViewRtti::Contains<K, V>,
        &QMapMetaViewRtti::Emplace<K, V>,
        &QMapMetaViewRtti::Erase<K, V>
    };

    class QMapMetaView {
    public:
        using PairTraverser = std::function<void(const Mirror::Any&, const Mirror::Any&)>;
        static constexpr Mirror::TemplateViewId id = QMapMetaViewRtti::id;

        explicit QMapMetaView(const Mirror::Any& inRef);
        NonCopyable(QMapMetaView)
        NonMovable(QMapMetaView)

        const Mirror::TypeInfo* GetKeyType() const;
        const Mirror::TypeInfo* GetValueType() const;
        Mirror::Any CreateKey() const;
        Mirror::Any CreateValue() const;
        size_t GetSize() const;
        void Reserve(size_t inSize) const;
        void Clear() const;
        Mirror::Any GetOrAdd(const Mirror::Argument& inKey) const;
        Mirror::Any ConstAt(const Mirror::Argument& inKey) const;
        void Traverse(const PairTraverser& inTraverser) const;
        void ConstTraverse(const PairTraverser& inTraverser) const;
        bool Contains(const Mirror::Argument& inKey) const;
        void Emplace(const Mirror::Argument& inTempKey, const Mirror::Argument& inTempValue) const;
        void Erase(const Mirror::Argument& inKey) const;

    private:
        Mirror::Any ref;
        const QMapMetaViewRtti* rtti;
    };
} // namespace Editor

namespace Mirror {
    template <typename T>
    struct TemplateViewRttiGetter<QList<T>> {
        static constexpr TemplateViewId Id();
        static const void* Get();
    };

    template <typename K, typename V>
    struct TemplateViewRttiGetter<QMap<K, V>> {
        static constexpr TemplateViewId Id();
        static const void* Get();
    };
}

namespace Editor {
    template <typename T>
    const Mirror::TypeInfo* QListMetaViewRtti::GetElementType()
    {
        return Mirror::GetTypeInfo<T>();
    }

    template <typename T>
    size_t QListMetaViewRtti::GetSize(const Mirror::Any& inRef)
    {
        return inRef.As<const QList<T>&>().size();
    }

    template <typename T>
    void QListMetaViewRtti::Reserve(const Mirror::Any& inRef, size_t inSize)
    {
        inRef.As<QList<T>&>().reserve(inSize);
    }

    template <typename T>
    void QListMetaViewRtti::Resize(const Mirror::Any& inRef, size_t inSize)
    {
        inRef.As<QList<T>&>().resize(inSize);
    }

    template <typename T>
    void QListMetaViewRtti::Clear(const Mirror::Any& inRef)
    {
        inRef.As<QList<T>&>().clear();
    }

    template <typename T>
    Mirror::Any QListMetaViewRtti::GetElement(const Mirror::Any& inRef, size_t inIndex)
    {
        return std::ref(inRef.As<QList<T>&>()[inIndex]);
    }

    template <typename T>
    Mirror::Any QListMetaViewRtti::GetConstElement(const Mirror::Any& inRef, size_t inIndex)
    {
        return std::ref(inRef.As<const QList<T>&>()[inIndex]);
    }

    template <typename T>
    Mirror::Any QListMetaViewRtti::EmplaceBack(const Mirror::Any& inRef, const Mirror::Argument& inTempObj)
    {
        return std::ref(inRef.As<QList<T>&>().emplaceBack(std::move(inTempObj.As<T&>())));
    }

    template <typename T>
    Mirror::Any QListMetaViewRtti::EmplaceDefaultBack(const Mirror::Any& inRef)
    {
        return std::ref(inRef.As<QList<T>&>().emplaceBack());
    }

    template <typename T>
    void QListMetaViewRtti::Remove(const Mirror::Any& inRef, size_t inIndex)
    {
        inRef.As<QList<T>&>().remove(inIndex);
    }

    template <typename K, typename V>
    const Mirror::TypeInfo* QMapMetaViewRtti::GetKeyType()
    {
        return Mirror::GetTypeInfo<K>();
    }

    template <typename K, typename V>
    const Mirror::TypeInfo* QMapMetaViewRtti::GetValueType()
    {
        return Mirror::GetTypeInfo<V>();
    }

    template <typename K, typename V>
    Mirror::Any QMapMetaViewRtti::CreateKey()
    {
        return K();
    }

    template <typename K, typename V>
    Mirror::Any QMapMetaViewRtti::CreateValue()
    {
        return V();
    }

    template <typename K, typename V>
    size_t QMapMetaViewRtti::GetSize(const Mirror::Any& inRef)
    {
        return inRef.As<const QMap<K, V>&>().size();
    }

    template <typename K, typename V>
    void QMapMetaViewRtti::Reserve(const Mirror::Any& inRef, size_t inSize)
    {
        inRef.As<QMap<K, V>&>().reserve(inSize);
    }

    template <typename K, typename V>
    void QMapMetaViewRtti::Clear(const Mirror::Any& inRef)
    {
        inRef.As<QMap<K, V>&>().clear();
    }

    template <typename K, typename V>
    Mirror::Any QMapMetaViewRtti::ConstAt(const Mirror::Any& inRef, const Mirror::Argument& inKey)
    {
        return inRef.As<const QMap<K, V>&>()[inKey];
    }

    template <typename K, typename V>
    Mirror::Any QMapMetaViewRtti::GetOrAdd(const Mirror::Any& inRef, const Mirror::Argument& inKey)
    {
        return inRef.As<QMap<K, V>&>()[inKey];
    }

    template <typename K, typename V>
    void QMapMetaViewRtti::Traverse(const Mirror::Any& inRef, const std::function<void(const Mirror::Any&, const Mirror::Any&)>& inVisitor)
    {
        for (QMap<K, V>& map = inRef.As<QMap<K, V>&>();
            const auto& [key, value] : map) {
            inVisitor(std::ref(key), std::ref(value));
        }
    }

    template <typename K, typename V>
    void QMapMetaViewRtti::ConstTraverse(const Mirror::Any& inRef, const std::function<void(const Mirror::Any&, const Mirror::Any&)>& inVisitor)
    {
        for (const QMap<K, V>& map = inRef.As<const QMap<K, V>&>();
            const auto& [key, value] : map) {
            inVisitor(std::ref(key), std::ref(value));
            }
    }

    template <typename K, typename V>
    bool QMapMetaViewRtti::Contains(const Mirror::Any& inRef, const Mirror::Argument& inKey)
    {
        return inRef.As<const QMap<K, V>&>().contains(inKey);
    }

    template <typename K, typename V>
    void QMapMetaViewRtti::Emplace(const Mirror::Any& inRef, const Mirror::Argument& inKey, const Mirror::Argument& inValue)
    {
        inRef.As<QMap<K, V>&>().emplace(inKey, inValue);
    }

    template <typename K, typename V>
    void QMapMetaViewRtti::Erase(const Mirror::Any& inRef, const Mirror::Argument& inKey)
    {
        inRef.As<QMap<K, V>&>().remove(inKey);
    }
} // namespace Editor

namespace Mirror {
    template <typename T>
    constexpr TemplateViewId TemplateViewRttiGetter<QList<T>>::Id()
    {
        return Editor::QListMetaViewRtti::id;
    }

    template <typename T>
    const void* TemplateViewRttiGetter<QList<T>>::Get()
    {
        return &Editor::qListMetaViewRttiImpl<T>;
    }

    template <typename K, typename V>
    constexpr TemplateViewId TemplateViewRttiGetter<QMap<K, V>>::Id()
    {
        return Editor::QMapMetaViewRtti::id;
    }

    template <typename K, typename V>
    const void* TemplateViewRttiGetter<QMap<K, V>>::Get()
    {
        return &Editor::qMapMetaViewRttiImpl<K, V>;
    }
} // namespace Mirror
