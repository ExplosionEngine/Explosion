//
// Created by johnk on 2022/11/20.
//

#pragma once

#define EVar(...) __attribute__((annotate("var;" #__VA_ARGS__)))
#define EFunc(...) __attribute__((annotate("func;" #__VA_ARGS__)))
#define EClass(...) __attribute__((annotate("class;" #__VA_ARGS__)))
