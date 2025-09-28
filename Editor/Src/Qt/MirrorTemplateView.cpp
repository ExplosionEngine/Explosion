//
// Created by Kindem on 2025/8/26.
//

#include <Editor/Qt/MirrorTemplateView.h>

namespace Editor {
    QListMetaView::QListMetaView(const Mirror::Any& inRef)
        : ref(inRef)
    {
        Assert(inRef.IsRef() && ref.CanAsTemplateView<QListMetaView>());
        rtti = static_cast<const QListMetaViewRtti*>(ref.GetTemplateViewRtti());
    }

    const Mirror::TypeInfo* QListMetaView::ElementType() const
    {
        return rtti->getElementType();
    }

    size_t QListMetaView::Size() const
    {
        return rtti->getSize(ref);
    }

    void QListMetaView::Reserve(size_t inSize) const
    {
        rtti->reserve(ref, inSize);
    }

    void QListMetaView::Resize(size_t inSize) const
    {
        rtti->resize(ref, inSize);
    }

    void QListMetaView::Clear() const
    {
        rtti->clear(ref);
    }

    Mirror::Any QListMetaView::At(size_t inIndex) const
    {
        return rtti->getElement(ref, inIndex);
    }

    Mirror::Any QListMetaView::ConstAt(size_t inIndex) const
    {
        return rtti->getConstElement(ref, inIndex);
    }

    Mirror::Any QListMetaView::EmplaceBack(const Mirror::Argument& inTempObj) const
    {
        return rtti->emplaceBack(ref, inTempObj);
    }

    Mirror::Any QListMetaView::EmplaceDefaultBack() const
    {
        return rtti->emplaceDefaultBack(ref);
    }

    void QListMetaView::Remove(size_t inIndex) const
    {
        rtti->remove(ref, inIndex);
    }

    QMapMetaView::QMapMetaView(const Mirror::Any& inRef)
        : ref(inRef)
    {
        Assert(inRef.IsRef() && ref.CanAsTemplateView<QMapMetaView>());
        rtti = static_cast<const QMapMetaViewRtti*>(ref.GetTemplateViewRtti());
    }

    const Mirror::TypeInfo* QMapMetaView::GetKeyType() const
    {
        return rtti->getKeyType();
    }

    const Mirror::TypeInfo* QMapMetaView::GetValueType() const
    {
        return rtti->getValueType();
    }

    Mirror::Any QMapMetaView::CreateKey() const
    {
        return rtti->createKey();
    }

    Mirror::Any QMapMetaView::CreateValue() const
    {
        return rtti->createValue();
    }

    size_t QMapMetaView::GetSize() const
    {
        return rtti->getSize(ref);
    }

    void QMapMetaView::Reserve(size_t inSize) const
    {
        rtti->reserve(ref, inSize);
    }

    void QMapMetaView::Clear() const
    {
        rtti->clear(ref);
    }

    Mirror::Any QMapMetaView::GetOrAdd(const Mirror::Argument& inKey) const
    {
        return rtti->getOrAdd(ref, inKey);
    }

    Mirror::Any QMapMetaView::ConstAt(const Mirror::Argument& inKey) const
    {
        return rtti->constAt(ref, inKey);
    }

    void QMapMetaView::Traverse(const PairTraverser& inTraverser) const
    {
        rtti->traverse(ref, inTraverser);
    }

    void QMapMetaView::ConstTraverse(const PairTraverser& inTraverser) const
    {
        rtti->constTraverse(ref, inTraverser);
    }

    bool QMapMetaView::Contains(const Mirror::Argument& inKey) const
    {
        return rtti->contains(ref, inKey);
    }

    void QMapMetaView::Emplace(const Mirror::Argument& inTempKey, const Mirror::Argument& inTempValue) const
    {
        rtti->emplace(ref, inTempKey, inTempValue);
    }

    void QMapMetaView::Erase(const Mirror::Argument& inKey) const
    {
        rtti->erase(ref, inKey);
    }
} // namespace Editor
