#include <bits/stdc++.h>
using namespace std;

const int NEG_INF = -1e9;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    cin >> n >> m;

    vector<int> w(n + 1), c(n + 1);
    for (int i = 1; i <= n; i++)
        cin >> w[i] >> c[i];

    vector<vector<vector<int>>> dp(n + 1, vector<vector<int>>(n + 1, vector<int>(m + 1, NEG_INF)));
    vector<vector<vector<pair<int,int>>>> parent(n + 1, vector<vector<pair<int,int>>>(n + 1, vector<pair<int,int>>(m + 1, {-1, -1})));

    dp[0][0][0] = 0;

    for (int i = 1; i <= n; i++) {
        for (int k = 0; k <= n; k++) {
            for (int wsum = 0; wsum <= m; wsum++) {
                if (dp[i-1][k][wsum] > dp[i][k][wsum]) {
                    dp[i][k][wsum] = dp[i-1][k][wsum];
                    parent[i][k][wsum] = {wsum, -1};
                }
                if (k > 0 && wsum >= w[i] && dp[i-1][k-1][wsum - w[i]] != NEG_INF) {
                    int val = dp[i-1][k-1][wsum - w[i]] + c[i];
                    if (val > dp[i][k][wsum]) {
                        dp[i][k][wsum] = val;
                        parent[i][k][wsum] = {wsum - w[i], i};
                    }
                }
            }
        }
    }

    int best_value = 0;
    int best_i = n, best_k = 0, best_w = 0;
    for (int k = 1; k <= n; k++) {
        for (int wsum = 0; wsum <= m; wsum++) {
            if (dp[n][k][wsum] != NEG_INF) {
                int val = dp[n][k][wsum] * k;
                if (val > best_value) {
                    best_value = val;
                    best_k = k;
                    best_w = wsum;
                }
            }
        }
    }
    vector<int> items;
    int i = n, k = best_k, wsum = best_w;

    while (i > 0 && k >= 0) {
        auto [prev_w, item] = parent[i][k][wsum];
        if (item != -1) {
            items.push_back(item);
            wsum = prev_w;
            k--;
        }
        i--;
    }

    reverse(items.begin(), items.end());

    cout << best_value << "\n";
    for (int j = 0; j < (int)items.size(); j++) {
        if (j > 0) cout << " ";
        cout << items[j];
    }
    cout << "\n";

    return 0;
}