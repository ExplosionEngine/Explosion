//
// Created by johnk on 2022/11/20.
//

#pragma once

#define EVAR(...) __attribute__((annotate("var;" #__VA_ARGS__)))
#define EFUNC(...) __attribute__((annotate("func;" #__VA_ARGS__)))
#define ECLASS(...) __attribute__((annotate("class;" #__VA_ARGS__)))
