#include <iostream>
#include <fstream>
#include "jcejson_lib.h"
#include "jcejson_translate.h"

using namespace std;

int gargc;
const char **gargv;
void translate2class(module_t *m, Module &module)
{
    module.m_sName = m->name->name;
    struct_t *srt = m->structs;
    while (srt)
    {
        Struct st;
        st.m_sName = srt->name->name;
        member_t *mbr = srt->members;
        while (mbr)
        {
            Member member;
            member.m_sName = mbr->name->name;
            member.m_iType = mbr->type->type;
            member.m_iVecType = mbr->type->vectype;
            member.m_sTypeName = mbr->type->name->name;
            mbr = mbr->next;
            st.m_vMember.push_back(member);
        }
        srt = srt->next;
        module.m_vStruct.push_back(st);
    }
}

string GetOutputFile(string jcefile, string suffix)
{
    string prefix;
    size_t pos = jcefile.rfind('.');
    if (pos != string::npos)
        prefix = jcefile.substr(0, pos);
    else
        prefix = jcefile;
    return prefix + suffix;
}
int save(const string &filename, const string &data)
{
    ofstream ofs(filename.c_str());
    if (!ofs)
    {
        cout << "open " << filename << " for write faild" << endl;
        return -1;
    }
    ofs << data;
    return 0;
}
string GetBaseName(const string &name)
{
    size_t pos = name.rfind('/');
    if (pos != string::npos && pos < name.size() - 1)
        return name.substr(pos+1);
    return name;
}

int translate(module_t *m)
{
    Module module;
    Context ctx;
    translate2class(m, module);
    //TODO:free(m);
    int ret = module.GenerateCode(ctx);
    string headerfile = GetOutputFile(gargv[1], ".structjson.h");
    string cppfile = GetOutputFile(gargv[1], ".structjson.cpp");
    string inc;
    inc = "#include \"" + GetBaseName(GetOutputFile(gargv[1], ".h")) + "\"\n";
    if (ret == 0)
        ret = save(headerfile, inc + ctx.header.str());
	if (ret == 0)
		cout << "header:\t" << headerfile << endl;
    inc = "#include \"" + GetBaseName(headerfile) + "\"\n";
    //inc += "#include \"hydra_jce_endecode.h\"\n";
    if (ret == 0)
        ret = save(cppfile, inc + ctx.cpp.str());
	if (ret == 0)
		cout << "cpp:\t" << cppfile << endl;
    return ret;
}
extern "C" int yyparse();
extern "C" void yyrestart( FILE *new_file );
int main(int argc, const char **argv)
{
    gargc = argc;
    gargv = argv;
    if (argc != 2)
    {
        cout << argv[0] << " jcefile" << endl;
        return 1;
    }
    FILE *in = fopen(argv[1], "r");
    if (!in)
    {
        cout << "open fail: " << argv[1] << endl;
        return 2;
    }
    yyrestart(in);
    int ret = yyparse();
    fclose(in);
    cout << "ret = " << ret << endl;
    return ret;
}
