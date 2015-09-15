#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <map> 

using namespace std;

string Permu(int n, int k, vector<int> index, int sum) {
        if (n == 0) return "";
        else {
            int m = (k - 1) / (sum / n) + 1;
            int res;
            for (int i = 1; i < index.size(); i++) {
                if (index[i] == 0) {
                    m--;
                    if (m == 0) {res = i; index[i] = 1; break;}
                }
            }
            return to_string(res) + Permu(n - 1, k - (res - 1) * (sum / n), index, sum / n);
            
        }
    }

int main(int argc, char *argv[]) {
	int n = 2;
	int k = 1;
	int sum = 1;
	        int a = n;
	        while (a != 0) {sum *= a; a--;}
	        vector<int> index(0, n + 1);
	        //index[0] = 1;
	        cout << Permu(n, k, index, sum) << endl;
}