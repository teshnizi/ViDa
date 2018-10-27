#include <iostream>
#include <ginac/ginac.h>


using namespace std;
using namespace GiNaC;

int main(){
	symbol x("x"), t("t");
	ex poly = 3*pow(x,2) + 4*x*t;
	ex u1 = step(t-1) - step(t-2);
	ex u2 = step(x-t+3) - step(x-t+4);
	ex u3 = step(x);
//	cout << u1.subs(x == 0.5) << endl;
	cout << integral(x, 0, 10, poly).eval_integ() << endl;
}
