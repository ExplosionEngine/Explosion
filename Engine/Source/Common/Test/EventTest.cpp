//
// Created by johnk on 2024/11/5.
//

#include <Common/Event.h>

static int counter = 0;

static void StaticReceiver(int a, bool b)
{
    counter++;
    ASSERT_EQ(a, 1);
    ASSERT_EQ(b, true);
}

class Receiver {
public:
    Receiver() = default;

    void Receive(int a, bool b) // NOLINT
    {
        counter++;
        ASSERT_EQ(a, 1);
        ASSERT_EQ(b, true);
    }
};

TEST(EventTest, BasicTest)
{
    Receiver receiver;

    Event<int, bool> event;
    ASSERT_EQ(event.BindStatic<&StaticReceiver>(), 0);
    ASSERT_EQ(event.BindMember<&Receiver::Receive>(receiver), 1);
    ASSERT_EQ(event.BindLambda([](int a, bool b) -> void {
        counter++;
        ASSERT_EQ(a, 1);
        ASSERT_EQ(b, true);
    }), 2);

    event.Broadcast(1, true);
    ASSERT_EQ(counter, 3);
}
