#ifndef JCEJSON_TRANSLATE_H

#define JCEJSON_TRANSLATE_H

#include <string>
#include <vector>
#include <sstream>

struct Context;
class Member
{
public:
    Member():m_iType(0){}
    int Member2Json(Context &ctx);
    int Json2Member(Context &ctx);
public:
    std::string m_sName;
    std::string m_sTypeName;//m_iType是TYPE_CUSTOM时，保存custom类型名字
    int m_iType;
    int m_iVecType;//m_iType是TYPE_VECTOR时有效，此变量不能等于TYPE_VECTOR
};
class Module;
class Struct
{
public:
    int Struct2JsonDom(Context &ctx);
    int Struct2JsonValue(Context &ctx);
    int JsonValue2Struct(Context &ctx);
    int JsonDom2Struct(Context &ctx);
    int Struct2Json(Context &ctx);
    int Json2Struct(Context &ctx);
public:
    std::string m_sName;
    std::vector<Member> m_vMember;
};
class Module
{
public:
    int Module2Json(Context &ctx);
    int Json2Module(Context &ctx);
    int GenerateCode(Context &ctx);
public:
    std::string m_sName;
    std::vector<Struct> m_vStruct;
};

struct Context
{
    Context():pmember(0),pstruct(0),pmodule(0){}
    Member *pmember;
    Struct *pstruct;
    Module *pmodule;
    std::stringstream cpp;
    std::stringstream header;
};


#endif /* end of include guard: JCEJSON_TRANSLATE_H */
