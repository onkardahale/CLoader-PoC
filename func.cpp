#include <iostream>
using namespace std;

#include <clang-c/Index.h>
#include <string>


string Convert(const CXString& s)
{
    string result = clang_getCString(s);
    clang_disposeString(s);
    return result;
}
void print_function_prototype(CXCursor cursor)
{
    // TODO : Print data!
    auto type = clang_getCursorType(cursor);

    auto function_name = Convert(clang_getCursorSpelling(cursor));
    auto return_type   = Convert(clang_getTypeSpelling(clang_getResultType(type)));
    cout<<return_type<<" "<<function_name<< "\n"<<"(";

    int num_args = clang_Cursor_getNumArguments(cursor);
    for (int i = 0; i < num_args; ++i)
    {
        auto arg_cursor = clang_Cursor_getArgument(cursor, i);
        auto arg_name = Convert(clang_getCursorSpelling(arg_cursor));
        if (arg_name.empty())
        {
            arg_name = "no name!";
        }

        auto arg_data_type = Convert(clang_getTypeSpelling(clang_getArgType(type, i)));
        cout<<arg_data_type<<" "<<arg_name;
    }
    cout<<")"<<"\n";
}
CXChildVisitResult functionVisitor(CXCursor cursor, CXCursor /* parent */, CXClientData /* clientData */)
{
    if (clang_Location_isFromMainFile(clang_getCursorLocation(cursor)) == 0)
        return CXChildVisit_Continue;

    CXCursorKind kind = clang_getCursorKind(cursor);
    if ((kind == CXCursorKind::CXCursor_FunctionDecl || kind == CXCursorKind::CXCursor_CXXMethod || kind == CXCursorKind::CXCursor_FunctionTemplate || \
         kind == CXCursorKind::CXCursor_Constructor))
    {
        print_function_prototype(cursor);
    }

    return CXChildVisit_Continue;
}

int main( int argc, char** argv )
{
  if( argc < 2 )
    return -1;

  CXIndex index        = clang_createIndex( 0, 1 );
  CXTranslationUnit tu = clang_createTranslationUnit( index, argv[1] );

  if( !tu )
    return -1;

  CXCursor rootCursor  = clang_getTranslationUnitCursor( tu );

  unsigned int treeLevel = 0;

  clang_visitChildren( rootCursor, functionVisitor, &treeLevel );

  clang_disposeTranslationUnit( tu );
  clang_disposeIndex( index );

  return 0;
}
