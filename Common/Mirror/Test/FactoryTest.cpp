//
// Created by John Kindem on 2021/9/14
//

#include <cstring>

#include <gtest/gtest.h>

#include <Mirror/Mirror.h>
using namespace Explosion::Mirror;

int v0 = 1;
float v1 = 2.f;
double v2 = 3.0;

int F0(int a, int b)
{
    return a + b;
}

float F1(float a, float b)
{
    return a - b;
}

double F2(double a, double b)
{
    return a * b;
}

int F3()
{
    return v0;
}

void F4()
{
    throw std::exception("F4");
}

struct S0 {
    int v0;
    float v1;
    double v2;
};

class C0 {
public:
    void SetV0(int value)
    {
        v0 = value;
    }

    [[nodiscard]] int GetV0() const
    {
        return v0;
    }

    void SetV1(float value)
    {
        v1 = value;
    }

    [[nodiscard]] float GetV1() const
    {
        return v1;
    }

    void SetV2(double value)
    {
        v2 = value;
    }

    [[nodiscard]] double GetV2() const
    {
        return v2;
    }

private:
    int v0;
    float v1;
    double v2;
};

class MirrorFactoryTest : public testing::Test {
protected:
    void SetUp() override
    {
        GlobalFactory::Singleton()
            .DefineVariable("v0", &v0)
            .DefineVariable("v1", &v1)
            .DefineVariable("v2", &v2)
            .DefineFunction("F0", F0)
            .DefineFunction("F1", F1)
            .DefineFunction("F2", F2)
            .DefineFunction("F3", F3)
            .DefineFunction("F4", F4);

        ClassFactory<S0>::Singleton()
            .DefineVariable<&S0::v0>("v0")
            .DefineVariable<&S0::v1>("v1")
            .DefineVariable<&S0::v2>("v2");

        ClassFactory<C0>::Singleton()
            .DefineFunction<&C0::SetV0>("SetV0")
            .DefineFunction<&C0::SetV1>("SetV1")
            .DefineFunction<&C0::SetV2>("SetV2")
            .DefineFunction<&C0::GetV0>("GetV0")
            .DefineFunction<&C0::GetV1>("GetV1")
            .DefineFunction<&C0::GetV2>("GetV2");
    }
};

TEST_F(MirrorFactoryTest, GlobalVariableTest)
{
    auto& factory = GlobalFactory::Singleton();

    auto vd0 = factory.GetVariable("v0");
    auto value0 = vd0.Get(Ref {});
    ASSERT_EQ(value0.CastTo<int>(), 1);

    auto vd1 = factory.GetVariable("v1");
    auto value1 = vd1.Get(Ref {});
    ASSERT_EQ(std::abs(value1.CastTo<float>() - 2.f) < 0.001f, true);

    auto vd2 = factory.GetVariable("v2");
    auto value2 = vd2.Get(Ref {});
    ASSERT_EQ(std::abs(value2.CastTo<double>() - 3.0) < 0.001, true);
}

TEST_F(MirrorFactoryTest, GlobalFunctionTest)
{
    auto& factory = GlobalFactory::Singleton();

    auto fd0 = factory.GetFunction("F0");
    int al0 = 1;
    int ar0 = 2;
    auto value0 = fd0.Invoke(Ref {}, al0, ar0);
    ASSERT_EQ(value0.CastTo<int>(), 3);

    auto fd1 = factory.GetFunction("F1");
    float al1 = 10.f;
    float ar1 = 3.f;
    auto value1 = fd1.Invoke(Ref {}, al1, ar1);
    ASSERT_EQ(std::abs(value1.CastTo<float>() - 7.f) < 0.001f, true);

    auto fd2 = factory.GetFunction("F2");
    double al2 = 5.0;
    double ar2 = 6.0;
    auto value2 = fd2.Invoke(Ref {}, al2, ar2);
    ASSERT_EQ(std::abs(value2.CastTo<double>() - 30.0) < 0.001, true);

    auto fd3 = factory.GetFunction("F3");
    auto value3 = fd3.Invoke(Ref {});
    ASSERT_EQ(value3.CastTo<int>(), 1);

    bool exceptionCaught = false;
    try {
        auto fd4 = factory.GetFunction("F4");
        fd4.Invoke(Ref{});
    } catch (std::exception& e) {
        exceptionCaught = strcmp(e.what(), "F4") == 0;
    }
    ASSERT_EQ(exceptionCaught, true);
}

TEST_F(MirrorFactoryTest, ClassVariableTest)
{
    auto& factory = ClassFactory<S0>::Singleton();

    Any s = S0 { 1, 2.f, 3.0 };

    auto vd0 = factory.GetVariable("v0");
    auto value0 = vd0.Get(Ref(s));
    ASSERT_EQ(value0.CastTo<int>(), 1);

    auto vd1 = factory.GetVariable("v1");
    auto value1 = vd1.Get(Ref(s));
    ASSERT_EQ(std::abs(value1.CastTo<float>() - 2.f) < 0.001f, true);

    auto vd2 = factory.GetVariable("v2");
    auto value2 = vd2.Get(Ref(s));
    ASSERT_EQ(std::abs(value2.CastTo<double>() - 3.0) < 0.001, true);
}

TEST_F(MirrorFactoryTest, ClassFunctionTest)
{
    auto& factory = ClassFactory<C0>::Singleton();

    Any c = C0();
    auto* cp = c.CastToPointer<C0>();

    auto setV0Fd = factory.GetFunction("SetV0");
    setV0Fd.Invoke(Ref(c), 1);
    ASSERT_EQ(cp->GetV0(), 1);

    auto setV1Fd = factory.GetFunction("SetV1");
    setV1Fd.Invoke(Ref(c), 2.f);
    ASSERT_EQ(std::abs(cp->GetV1() - 2.f) < 0.001f, true);

    auto setV2Fd = factory.GetFunction("SetV2");
    setV2Fd.Invoke(Ref(c), 3.0);
    ASSERT_EQ(std::abs(cp->GetV2() - 3.0) < 0.001, true);

    auto getV0Fd = factory.GetFunction("GetV0");
    cp->SetV0(4);
    auto value0 = getV0Fd.Invoke(Ref(c));
    ASSERT_EQ(value0.CastTo<int>(), 4);

    auto getV1Fd = factory.GetFunction("GetV1");
    cp->SetV1(5.f);
    auto value1 = getV1Fd.Invoke(Ref(c));
    ASSERT_EQ(std::abs(value1.CastTo<float>() - 5.f) < 0.001f, true);

    auto getV2Fd = factory.GetFunction("GetV2");
    cp->SetV2(6.0);
    auto value2 = getV2Fd.Invoke(Ref(c));
    ASSERT_EQ(std::abs(value2.CastTo<double>() - 6.0) < 0.001, true);
}
