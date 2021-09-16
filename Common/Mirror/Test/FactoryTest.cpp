//
// Created by John Kindem on 2021/9/14
//

#include <gtest/gtest.h>

#include <Mirror/Mirror.h>
using namespace Explosion::Mirror;

int v0 = 1;
float v1 = 2.f;
double v2 = 3.0;

int f0(int a, int b)
{
    return a + b;
}

float f1(float a, float b)
{
    return a - b;
}

double f2(double a, double b)
{
    return a * b;
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

    int GetV0() const
    {
        return v0;
    }

    void SetV1(float value)
    {
        v1 = value;
    }

    float GetV1() const
    {
        return v1;
    }

    void SetV2(double value)
    {
        v2 = value;
    }

    double GetV2() const
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
            .DefineFunction("f0", f0)
            .DefineFunction("f1", f1)
            .DefineFunction("f2", f2);

        StructFactory<S0>::Singleton()
            .DefineVariable<&S0::v0>("v0")
            .DefineVariable<&S0::v1>("v1")
            .DefineVariable<&S0::v2>("v2");
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

    auto fd0 = factory.GetFunction("f0");
    int al0 = 1;
    int ar0 = 2;
    auto value0 = fd0.Invoke(Ref {}, al0, ar0);
    ASSERT_EQ(value0.CastTo<int>(), 3);

    auto fd1 = factory.GetFunction("f1");
    float al1 = 10.f;
    float ar1 = 3.f;
    auto value1 = fd1.Invoke(Ref {}, al1, ar1);
    ASSERT_EQ(std::abs(value1.CastTo<float>() - 7.f) < 0.001f, true);

    auto fd2 = factory.GetFunction("f2");
    double al2 = 5.0;
    double ar2 = 6.0;
    auto value2 = fd2.Invoke(Ref {}, al2, ar2);
    ASSERT_EQ(std::abs(value2.CastTo<double>() - 30.0) < 0.001, true);
}

TEST_F(MirrorFactoryTest, StructVariableTest)
{
    auto& factory = StructFactory<S0>::Singleton();

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
