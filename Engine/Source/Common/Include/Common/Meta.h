//
// Created by johnk on 2022/5/25.
//

#pragma once

#define Meta(...) __attribute__((annotate(#__VA_ARGS__)))

#define Class ""
#define Enum ""
#define Function ""
#define Property ""

#define Transient ""

#define ScriptVisible ""
#define ScriptReadOnly ""
#define ScriptReadWrite ""

#define EnginePrivate ""

#define EditorUI(...) ""
#define DefaultValue(...) ""
#define MinValue(...) ""
#define MaxValue(...) ""
#define Alias(...) ""
