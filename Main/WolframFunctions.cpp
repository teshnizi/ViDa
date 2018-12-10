#include <iostream>
#include <cmath>

using namespace std;

const double Pi = 3.1415;
const double E = 2.71828;

double zet[10];

void prep(){
    for (int i = 1; i < 10; ++i) {
        zet[i] = 1;
        for (double j = 2; j < 100; ++j) {
            zet[i] += (1/pow(j,i));
        }
    }
}

struct Range{
    float left;
    float right;
};

struct Piecewise{
    int num = 2;
    Range ranges[10];
    string expressions[10];
};

bool LessEqual(float l, float y, float h){
    return (l <= y) && (y <= h);
}

double Sqrt(double x){
    return sqrt(x);
}

double Power(double x, double y){
    return pow(x, y);
}

double Erf(double x){
    return erf(x);
}

double Zeta(int s){
    return zet[s];
}

string generatePiecewise(string s){
    int it = 0;
    it = 20;
    string tmp = "";
    int diff = 3;
    string syn[20][2];
    int synIt = 0;

    while (diff > 0){
//        cout << "======================\n";
//        cout << diff << " " << it << " " << s[it] << endl;
//        cout << ans << "\n $$$ \n" << tmp << endl << endl;
//        cout << "======================\n";
//        getchar();

        if (s[it] == ')') {
            if ( diff == 1)
                break;
            diff--;
            if (diff >= 3)
                tmp += s[it];
            it++;
            continue;
        } else if (s[it] == '(') {
            diff++;
            if (diff > 3)
                tmp += s[it];
            it++;
            continue;
        }


        if (diff == 2){
            if (tmp.length() > 0) {
                syn[synIt][1] = tmp;
//                cout << tmp << endl;
                tmp = "";
                synIt++;
            }
            it++;
            continue;
        }

        if (diff == 3){
            if ( s[it] != ',')
                tmp = tmp + s[it];
            else {
                syn[synIt][0] = tmp;
                tmp = "";
            }
            it++;
            continue;
        }

        if (diff == 1){
            if ( s[it] == ','){
                syn[synIt][1] = tmp;
                synIt++;
                tmp = "";
                it++;
                continue;
            }
            tmp += s[it];
            it++;
            continue;
        }
        tmp = tmp + s[it];
        it ++;
    }

    syn[synIt++][0] = tmp;

    string ret = "";
    for (int i = 0; i < synIt; ++i) {
        string k = "if ( " + syn[i][1] + ") \n\t return ";
        if ( i == synIt - 1)
            k = "else \n\t return ";
        ret = ret + k + syn[i][0] + ";" + "\n";
    }
    return ret;
}

int main(){
    prep();
    cout << generatePiecewise("");
}
