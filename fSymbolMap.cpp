#include <iostream>
using namespace std;

#include <iterator>
#include <map>

#include <elf.h>
#include <libtcc.h>
#include <ffi.h>

typedef unsigned long int	uintptr_t;

void callback(void *ctx, const char *name, const void *val) {
   map<string, const void*>* m = (map<string, const void*>*)ctx;
   //*m.insert(pair<char*, Elf32_Addr>(*name, *val));
   m->insert(pair<string, const void*>(name, val));
}

int main(){
  char myProgram[] =
  "int add(double a, double b)\n"
  "{return a + b;}\n"
  "int subtract(double a, double b)\n"
  "{return a - b;}\n"
  "int multiply(double a, double b)\n"
  "{return a*b;}\n"
  "double divide(double a, double b)\n"
  "{return a/b;}\n";

  TCCState *s = tcc_new();

  if(!s) {
      fprintf(stderr, "Could not create tcc state\n");
      exit(1);
  }

  tcc_set_output_type(s, TCC_OUTPUT_MEMORY);

  if (tcc_compile_string(s, myProgram) == -1)
      return 1;

  if (tcc_relocate(s, TCC_RELOCATE_AUTO) < 0)
      return 1;

  char name[] = "add";
  map<string, const void*> table;

  tcc_list_symbols(s, &table, &callback);
  int (*func)(double,double);

  auto it = table.find("add");
  //cout<<(uintptr_t)it->first;
  func = (int (*)(double,double))(uintptr_t)it->second;
  cout << func(2,3)<< "\n";
  for (auto const &pair: table) {
    cout << pair.first << " " << (uintptr_t)pair.second << "\n";
  }


  tcc_delete(s);

  return 0;
}
