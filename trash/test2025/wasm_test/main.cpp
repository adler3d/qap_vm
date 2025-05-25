#include <emscripten.h>
#include <vector>
using namespace std;
int main() {
  vector<int> V={10,20,30};
  EM_ASM(document.body.innerHTML='<canvas id="glcanvas" width="1920" height="1024"></canvas><hr>';);
  EM_ASM({
    console.log('I received: ' + $0);
    g_data=$0;
    {
      let a=HEAP32[(g_data>>2)+0];
      let b=HEAP32[(g_data>>2)+1];
      let c=HEAP32[(g_data>>2)+2];
      console.log({a,b,c});
      let x=HEAP8[g_data+0];
      let y=HEAP8[g_data+1];
      let z=HEAP8[g_data+2];
      console.log({x,y,z});
    }
  }, int(V.data()));
  EM_ASM(main(););
  return 0;
}
