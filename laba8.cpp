//5 Оптимальная сортировка чисел
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main() {
    int N;
    cin >> N;
    vector<int> a(N);
    for (int i = 0; i < N; i++) cin >> a[i];

    int count1 = count(a.begin(), a.end(), 1);
    int count2 = count(a.begin(), a.end(), 2);
    int count3 = N - count1 - count2;

    int wrong[4][4] = {0};
    for (int i = 0; i < count1; i++)
        wrong[1][a[i]]++;
    for (int i = count1; i < count1 + count2; i++)
        wrong[2][a[i]]++;
    for (int i = count1 + count2; i < N; i++)
        wrong[3][a[i]]++;

    int swaps = 0;

    int m12 = min(wrong[1][2], wrong[2][1]);
    swaps += m12;
    wrong[1][2] -= m12; wrong[2][1] -= m12;

    int m13 = min(wrong[1][3], wrong[3][1]);
    swaps += m13;
    wrong[1][3] -= m13; wrong[3][1] -= m13;

    int m23 = min(wrong[2][3], wrong[3][2]);
    swaps += m23;
    wrong[2][3] -= m23; wrong[3][2] -= m23;

    int remaining = wrong[1][2] + wrong[1][3] + wrong[2][1] + wrong[2][3] + wrong[3][1] + wrong[3][2];
    swaps += 2 * (remaining / 3);

    cout << swaps << endl;
    return 0;
}