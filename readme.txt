

目前类型只支持  int/unsigned int/string/vector/自定义

1.使用一种格式


namespace Test
{
    struct A
    {
        int a;
        string b;
        unsigned int c;
        vector<int> d;
    };
    struct B
    {
        A a;
        vector<A> aa;
    };
};

注意要用;结尾

./struct2json xxx.xxx
