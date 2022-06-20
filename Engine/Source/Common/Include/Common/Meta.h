//
// Created by johnk on 2022/5/25.
//

#pragma once

#ifdef META_TOOL
#define Meta(...) __attribute__((annotate(#__VA_ARGS__)))
#else
#define Meta(...)
#endif

#define Class
#define Struct
#define Enum
#define Function
#define Property

#define Transient

#define ScriptVisible
#define ScriptReadOnly
#define ScriptReadWrite

#define EnginePrivate

#define EditorUI(...)
#define DefaultValue(...)
#define MinValue(...)
#define MaxValue(...)
#define Alias(...)
