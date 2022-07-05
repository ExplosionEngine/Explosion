//
// Created by johnk on 2022/5/25.
//

#pragma once

#ifdef META_TOOL
#define Meta(...) __attribute__((annotate(#__VA_ARGS__)))
#else
#define Meta(...)
#endif

// Basic
#define Class
#define Struct
#define Enum
#define Function
#define Property

// Runtime
#define Transient

// Script
#define ScriptVisible
#define ScriptReadOnly
#define ScriptReadWrite

// Engine
#define EnginePrivate

// Editor
#define EditorUI
#define DefaultValue
#define MinValue
#define MaxValue
#define Alias

// Shaders
#define ShaderMacro
#define MinValue
#define MaxValue
#define BufferType
