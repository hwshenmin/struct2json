

目前类型只支持  int/unsigned int/string/vector/自定义

1.可使用jce的定义文件作为输入
例:

module Test
{
    struct A
    {
        0 require int a;
        1 require string b;
        2 optional unsigned int c;
        4 optional vector<int> d;
    };
    struct B
    {
        0 optional A a;
        1 optional vector<A> aa;
    };
};

./structjson xxx.jce


2.可使用另一种格式


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

./structjson xxx.xxx
