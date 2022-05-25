//
// Created by johnk on 2022/5/25.
//

#pragma once

#define Meta(...) __attribute__((annotate(#__VA_ARGS__)))

#define Class "Class;"
#define Enum "Enum;"
#define Function "Function;"
#define Property "Property;"

#define Transient "Transient;"

#define ScriptVisible "ScriptVisible;"
#define ScriptReadOnly "ScriptReadOnly;"
#define ScriptReadWrite "ScriptReadWrite;"

#define EditorOnly "EditorOnly;"
#define EditorUI(...) "EditorUI:" #__VA_ARGS__ ";"
#define DefaultValue(...) "DefaultValue:" #__VA_ARGS__ ";"
#define MinValue(...) "MinValue:" #__VA_ARGS__ ";"
#define MaxValue(...) "MaxValue:" #__VA_ARGS__ ";"
#define Alias(...) "Alias:" #__VA_ARGS__ ";"
