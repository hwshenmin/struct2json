
#include <iostream>
#include <fstream>


#include "jcejson_translate.h"
#include "jcejson_lib.h"

using namespace std;

int Member::Member2Json(Context &ctx)
{
    string varname = m_sName;

    stringstream &cpp = ctx.cpp;
    switch (m_iType)
    {
        case TYPE_INT:
        case TYPE_UINT:
            {
                cpp << "value.AddMember(\"" << m_sName << "\", info." << m_sName << ", ator);\n";
            }
            break;
        case TYPE_STRING:
            {
                cpp << "Value " << varname << ";\n";
                cpp << varname << ".SetString(" << "info." << m_sName << ".data(), "
                    << "info." << m_sName << ".size(), ator);\n";
                cpp << "value.AddMember(\"" << m_sName << "\", " << varname << ", ator);\n";
            }
            break;
        case TYPE_CUSTOM:
            {
                cpp << "Value " << varname << ";\n";
                cpp << "ret = " << ctx.pmodule->m_sName << "_" << m_sTypeName << "_struct2jsonvalue(info." << m_sName << ", " << varname << ", ator);\n";
                cpp << "if (ret != 0)\n";
                cpp << "    return ret;\n";
                cpp << "value.AddMember(\"" << m_sName << "\", " << varname << ", ator);\n";
            }
            break;
        case TYPE_VECTOR:
            {
                cpp << "Value " << varname << ";\n";
                cpp << varname << ".SetArray();\n";
                if (m_iVecType == TYPE_CUSTOM)
                cpp << "for (vector<" << ctx.pmodule->m_sName << "::" << m_sTypeName << ">::iterator itr = info." << m_sName << ".begin();\n" << "itr != info." << m_sName << ".end(); ++itr)\n";
                else
                cpp << "for (vector<" << m_sTypeName << ">::iterator itr = info." << m_sName << ".begin();\n" << "itr != info." << m_sName << ".end(); ++itr)\n";
                cpp << "{\n";
                cpp << "Value v;\n";
                switch (m_iVecType)
                {
                    case TYPE_INT:
                    case TYPE_UINT:
                        cpp << "v.SetUint(*itr);\n";
                        break;
                    case TYPE_STRING:
                        cpp << "v.SetString(itr->data(), itr->size(), ator);\n";
                        break;
                    case TYPE_CUSTOM:
                        {
                            cpp << "ret = " << ctx.pmodule->m_sName << "_" << m_sTypeName << "_struct2jsonvalue(*itr, v, ator);\n";
                            cpp << "if (ret != 0)\n";
                            cpp << "    return ret;\n";
                        }
                        break;
                    default:
                        cerr << m_sTypeName << ": unknown vector member type: " << m_iVecType << endl;
                }
                cpp << varname << ".PushBack(v, ator);\n";
                cpp << "}\n";
                cpp << "value.AddMember(\"" << m_sName << "\", " << varname << ", ator);\n";
            }
            break;
        default:
            cerr << m_sName << ": unknown member type: " << m_iType << endl;
            return -1;
    }
    return 0;
}
int Member::Json2Member(Context &ctx)
{
    stringstream &cpp = ctx.cpp;
    cpp << "do {\n";
    cpp << "Value::ConstMemberIterator mitr;\n";
    cpp << "mitr = value.FindMember(\"" << m_sName << "\");\n";
    cpp << "if (mitr == value.MemberEnd())\n";
    cpp << "return E_MEMBER_NOT_FOUND;\n";
    //cpp << "    break;\n";
    switch (m_iType)
    {
        case TYPE_INT:
            cpp << "if (!mitr->value.IsInt())\n";
            cpp << "    return E_TYPE_NOT_MATCH;\n";
            cpp << "info." << m_sName << " = mitr->value.GetInt();\n";
            break;
        case TYPE_UINT:
            cpp << "if (!mitr->value.IsUint())\n";
            cpp << "    return E_TYPE_NOT_MATCH;\n";
            cpp << "info." << m_sName << " = mitr->value.GetUint();\n";
            break;
        case TYPE_STRING:
            cpp << "if (!mitr->value.IsString())\n";
            cpp << "    return E_TYPE_NOT_MATCH;\n";
            cpp << "info." << m_sName << " = mitr->value.GetString();\n";
            break;
        case TYPE_CUSTOM:
            {
                cpp << "ret = " << ctx.pmodule->m_sName << "_" << m_sTypeName << "_jsonvalue2struct(mitr->value, info." << m_sName << ");\n";
                cpp << "if (ret != 0)\n";
                cpp << "     return ret;\n";
            }
            break;
        case TYPE_VECTOR:
            {
                cpp << "for (Value::ConstValueIterator itr = mitr->value.Begin(); itr != mitr->value.End(); ++itr)\n";
                cpp << "{\n";
                switch (m_iVecType)
                {
                    case TYPE_INT:
                        cpp << "if (!itr->IsInt())\n";
                        cpp << "return E_TYPE_NOT_MATCH;\n";
                        cpp << "info." << m_sName << ".push_back(itr->GetInt());\n";
                        break;
                    case TYPE_UINT:
                        cpp << "if (!itr->IsUint())\n";
                        cpp << "return E_TYPE_NOT_MATCH;\n";
                        cpp << "info." << m_sName << ".push_back(itr->GetUint());\n";
                        break;
                    case TYPE_STRING:
                        cpp << "if (!itr->IsString())\n";
                        cpp << "return E_TYPE_NOT_MATCH;\n";
                        cpp << "info." << m_sName << ".push_back(itr->GetString());\n";
                        break;
                    case TYPE_CUSTOM:
                        cpp << "if (!itr->IsObject())\n";
                        cpp << "return E_TYPE_NOT_MATCH;\n";
                        cpp << ctx.pmodule->m_sName << "::" << m_sTypeName << " st;\n";
                        cpp << "ret = " << ctx.pmodule->m_sName << "_" << m_sTypeName << "_jsonvalue2struct(*itr, st);\n";
                        cpp << "if (ret != 0)\n";
                        cpp << "return ret;\n";
                        cpp << "info." << m_sName << ".push_back(st);\n";
                        break;
                    default:
                        cerr << m_sTypeName << ": unknown vector member type: " << m_iVecType << endl;
                        break;
                }
                cpp << "}\n";
            }
            break;
        default:
            cerr << m_sName << ": unknown member type: " << m_iType << endl;
            return -1;
    }
    cpp << "} while(0);\n";
    return 0;
}

int Struct::Struct2JsonDom(Context &ctx)
{
    stringstream &cpp = ctx.cpp;
    stringstream &header = ctx.header;
    //header << "int " << ctx.pmodule->m_sName << "_" << m_sName << "_struct2jsondom(" << ctx.pmodule->m_sName << "::" << m_sName << " &info, " << "rapidjson::Document &dom);\n";
    cpp << "int " << ctx.pmodule->m_sName << "_" << m_sName << "_struct2jsondom(" << ctx.pmodule->m_sName << "::" << m_sName << " &info, " << "Document &dom)\n";
    cpp << "{\n";

    cpp << "dom.SetObject();\n";

    cpp << "Value value;\n";
    cpp << "int ret = " << ctx.pmodule->m_sName << "_" << m_sName << "_struct2jsonvalue(info, value, dom.GetAllocator());\n";
    cpp << "if (ret != 0)\n return ret;\n";
    cpp << "dom.AddMember(\"" << m_sName << "\", value, dom.GetAllocator());\n";

    cpp <<"return 0;\n";
    cpp << "}\n";
    return 0;
}
int Struct::Struct2JsonValue(Context &ctx)
{
    stringstream &cpp = ctx.cpp;
    stringstream &header = ctx.header;
    //header << "int " << ctx.pmodule->m_sName << "_" << m_sName << "_struct2jsonvalue(" << ctx.pmodule->m_sName << "::" << m_sName << " &info, " << "rapidjson::Value &value, rapidjson::Document::AllocatorType &ator);\n";
    cpp << "int " << ctx.pmodule->m_sName << "_" << m_sName << "_struct2jsonvalue(" << ctx.pmodule->m_sName << "::" << m_sName << " &info, " << "Value &value, Document::AllocatorType &ator)\n";
    cpp << "{\n";

    cpp << "value.SetObject();\n";
    cpp << "int ret = 0;\n";

    for (vector<Member>::iterator itr = m_vMember.begin();
            itr != m_vMember.end(); ++itr)
    {
        ctx.pmember = &(*itr);
        int ret = itr->Member2Json(ctx);
        if (ret != 0)
            return ret;
    }

    cpp <<"return ret;\n";
    cpp << "}\n";
    return 0;
}
int Struct::JsonValue2Struct(Context &ctx)
{
    stringstream &cpp = ctx.cpp;
    stringstream &header = ctx.header;
    cpp << "int " << ctx.pmodule->m_sName << "_" << m_sName << "_jsonvalue2struct(" << "const Value &value, " << ctx.pmodule->m_sName << "::" << m_sName << " &info)\n";
    //header << "int " << ctx.pmodule->m_sName << "_" << m_sName << "_jsonvalue2struct(" << "const rapidjson::Value &value, " << ctx.pmodule->m_sName << "::" << m_sName << " &info);\n";
    cpp << "{\n";
    cpp << "if (!value.IsObject())\n";
    cpp << "    return E_NOT_OBJECT;\n";
    cpp << "int ret = 0;\n";
    for (vector<Member>::iterator itr = m_vMember.begin();
            itr != m_vMember.end(); ++itr)
    {
        ctx.pmember = &(*itr);
        int ret = itr->Json2Member(ctx);
        if (ret != 0)
            return ret;
    }
    cpp << "return ret;\n";
    cpp << "}\n";
    return 0;
}
int Struct::JsonDom2Struct(Context &ctx)
{
    stringstream &cpp = ctx.cpp;
    stringstream &header = ctx.header;
    cpp << "int " << ctx.pmodule->m_sName << "_" << m_sName << "_jsondom2struct(" << "const Document &dom, " << ctx.pmodule->m_sName << "::" << m_sName << " &info)\n";
    //header << "int " << ctx.pmodule->m_sName << "_" << m_sName << "_jsondom2struct(" << "const rapidjson::Document &dom, " << ctx.pmodule->m_sName << "::" << m_sName << " &info);\n";
    cpp << "{\n";
    cpp << "Value::ConstMemberIterator itr = dom.FindMember(\"" << m_sName << "\");\n";
    cpp << "if (itr == dom.MemberEnd())\n return E_MEMBER_NOT_FOUND;\n";
    cpp << "return " << ctx.pmodule->m_sName << "_" << m_sName << "_jsonvalue2struct(itr->value, info);\n";
    cpp << "}\n";
    return 0;
}
int Struct::Struct2Json(Context &ctx)
{
    int ret = Struct2JsonValue(ctx);
    if (ret != 0)
        return ret;
    ret = Struct2JsonDom(ctx);
    if (ret != 0)
        return ret;

    stringstream &cpp = ctx.cpp;
    stringstream &header = ctx.header;

    header << "int " << ctx.pmodule->m_sName << "_" << m_sName << "_struct2json(" << ctx.pmodule->m_sName << "::" << m_sName << " &info, " << "std::string &json);\n";
    cpp << "int " << ctx.pmodule->m_sName << "_" << m_sName << "_struct2json(" << ctx.pmodule->m_sName << "::" << m_sName << " &info, " << "std::string &json)\n";
    cpp << "{\n";
    cpp << "Document dom;\n";
    cpp << "int ret = " << ctx.pmodule->m_sName << "_" << m_sName << "_struct2jsondom(info, dom);\n";
    cpp << "if (ret != 0) return ret;\n";
    cpp << "StringBuffer sb;\n";
    cpp << "Writer<StringBuffer> writer(sb);\n";
    cpp << "dom.Accept(writer);\n";
    cpp << "json = sb.GetString();\n";
    cpp << "return 0;\n";
    cpp << "}\n";

    return 0;
}
int Struct::Json2Struct(Context &ctx)
{
    int ret = JsonValue2Struct(ctx);
    if (ret != 0)
        return ret;
    ret = JsonDom2Struct(ctx);
    if (ret != 0)
        return ret;

    stringstream &cpp = ctx.cpp;
    stringstream &header = ctx.header;

    header << "int " << ctx.pmodule->m_sName << "_" << m_sName << "_json2struct(" << "const char *json, " << ctx.pmodule->m_sName << "::" << m_sName << " &info);\n";
    cpp << "int " << ctx.pmodule->m_sName << "_" << m_sName << "_json2struct(" << "const char *json, " << ctx.pmodule->m_sName << "::" << m_sName << " &info)\n";
    cpp << "{\n";
    cpp << "Document dom;\n";
    cpp << "if (dom.Parse(json).HasParseError()) return E_PARSE_FAIL;\n";
    cpp << "return " << ctx.pmodule->m_sName << "_" << m_sName << "_jsondom2struct(dom, info);\n";
    cpp << "}\n";

    return 0;
}

int Module::Module2Json(Context &ctx)
{
    stringstream &cpp = ctx.cpp;
    stringstream &header = ctx.header;
    ctx.pmodule = this;
    for (vector<Struct>::iterator itr = m_vStruct.begin();
            itr != m_vStruct.end(); ++itr)
    {
        ctx.pstruct = &(*itr);
        int ret = itr->Struct2Json(ctx);
        if (ret != 0)
            return ret;
#if 0
        cpp << "int " << ctx.pmodule->m_sName << "_" << itr->m_sName << "_jce2json(const char *data, int len, char *outbuf, int *outbuflen)\n";
        header << "int " << ctx.pmodule->m_sName << "_" << itr->m_sName << "_jce2json(const char *data, int len, char *outbuf, int *outbuflen);\n";
        cpp << "{\n";
        cpp << m_sName << "::" << itr->m_sName << " info;\n";
        cpp << "int ret = hydra::JceDecode(data, len, info);\n";
        cpp << "if (ret != 0)\n";
        cpp << "    return ret;\n";
        cpp << "Document dom;\n";
        cpp << "ret = " << ctx.pmodule->m_sName << "_" << itr->m_sName << "_struct2jsondom(info, dom);\n";
        cpp << "if (ret != 0)\n";
        cpp << "    return ret;\n";
        cpp << "StringBuffer sb;\n";
        cpp << "Writer<StringBuffer> writer(sb);\n";
        cpp << "dom.Accept(writer);\n";
        cpp << "const char *json = sb.GetString();\n";
        cpp << "len = strlen(json);\n";
        cpp << "int buflen = *outbuflen;\n";
        cpp << "*outbuflen = len;\n";
        cpp << "if (buflen < len)\n";
        cpp << "    return E_BUF_NOT_ENOUGH;\n";
        cpp << "memcpy(outbuf, json, len);\n";
        cpp << "return 0;\n";
        cpp << "}\n";
#endif
    }
    return 0;
}
int Module::Json2Module(Context &ctx)
{
    stringstream &cpp = ctx.cpp;
    stringstream &header = ctx.header;
    ctx.pmodule = this;
    for (vector<Struct>::iterator itr = m_vStruct.begin();
            itr != m_vStruct.end(); ++itr)
    {
        int ret = itr->Json2Struct(ctx);
        if (ret != 0)
            return ret;
#if 0
        cpp << "int " << ctx.pmodule->m_sName << "_" << itr->m_sName << "_json2jce(const char *json, char *outbuf, int *outbuflen)\n";
        header << "int " << ctx.pmodule->m_sName << "_" << itr->m_sName << "_json2jce(const char *json, char *outbuf, int *outbuflen);\n";
        cpp << "{\n";
        cpp << m_sName << "::" << itr->m_sName << " info;\n";
        cpp << "Document dom;\n";
        cpp << "if (dom.Parse(json).HasParseError())\n";
        cpp << "    return E_PARSE_FAIL;\n";
        cpp << "int ret = " << ctx.pmodule->m_sName << "_" << itr->m_sName << "_jsondom2struct(dom, info);\n";
        cpp << "if (ret != 0)\n";
        cpp << "    return ret;\n";
        cpp << "string bindata;\n";
        cpp << "ret = hydra::JceEncode(info, bindata);\n";
        cpp << "if (ret != 0)\n";
        cpp << "    return ret;\n";
        cpp << "int len = bindata.size();\n";
        cpp << "int buflen = *outbuflen;\n";
        cpp << "*outbuflen = len;\n";
        cpp << "if (buflen < len)\n";
        cpp << "    return E_BUF_NOT_ENOUGH;\n";
        cpp << "memcpy(outbuf, bindata.data(), len);\n";
        cpp << "return 0;\n";
        cpp << "}\n";
#endif
    }
    return 0;
}
int Module::GenerateCode(Context &ctx)
{
    stringstream &cpp = ctx.cpp;
    stringstream &header = ctx.header;

    header << "#include <string>\n";
    header << "#include <vector>\n";
    cpp << "#include \"rapidjson/document.h\"\n";
    cpp << "#include \"rapidjson/stringbuffer.h\"\n";
    cpp << "#include \"rapidjson/writer.h\"\n";
    cpp << "using namespace rapidjson;\n";
    cpp << "static const int E_BUF_NOT_ENOUGH = -22220000;\n";
    cpp << "static const int E_TYPE_NOT_MATCH = -22220001;\n";
    cpp << "static const int E_MEMBER_NOT_FOUND = -22220002;\n";
    cpp << "static const int E_PARSE_FAIL = -22220003;\n";
    cpp << "static const int E_NOT_OBJECT = -22220004;\n";
    int ret = 0;
    ret = Module2Json(ctx);
    if (ret != 0)
        return ret;
    ret = Json2Module(ctx);
    if (ret != 0)
        return ret;

    return 0;
}
