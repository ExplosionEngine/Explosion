//
// Created by johnk on 2022/6/7.
//

struct S0 {
    int a;
    float b;
    double c;
};

struct S1 {
public:
    double c;

protected:
    float b;

private:
    int a;
};

struct S2 {
public:
    int GetA() { return a; }
    float* GetPointerB(int t) { return &b; }

private:
    int a;
    float b;
};
