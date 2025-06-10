#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <chrono>

using namespace std;

const int INF = numeric_limits<int>::max();
const int TRANSFER_TIME = 3;

struct State {
    int stop;
    int route_index;
    int dist;
    int transfers;

    bool operator>(const State& other) const {
        if (dist != other.dist) {
            return dist > other.dist;
        }
        return transfers > other.transfers;
    }
};

int main() {
    system("chcp 1251");
    system("cls");
    cout << "Выполнил: Карпенко Денис Иванович\nГруппа: 020303-АИСа-24о\n";
    int n, r;
    cout << "Введите количество остановок (n): ";
    cin >> n;
    cout << "Введите количество маршрутов (r): ";
    cin >> r;

    vector<vector<int>> routes(r);
    for (int i = 0; i < r; ++i) {
        int route_length;
        cout << "Введите длину маршрута " << i + 1 << ": ";
        cin >> route_length;
        routes[i].resize(route_length);
        cout << "Введите остановки для маршрута " << i + 1 << ": ";
        for (int j = 0; j < route_length; ++j) {
            cin >> routes[i][j];
            routes[i][j]--; // -1, чтоб исп индек
        }
    }

    int start_stop, end_stop;
    cout << "Введите начальную остановку (I): ";
    cin >> start_stop;
    start_stop--;
    cout << "Введите конечную остановку (J): ";
    cin >> end_stop;
    end_stop--;

    auto start = chrono::high_resolution_clock::now();

    vector<vector<int>> min_dist(n, vector<int>(r, INF));
    vector<vector<int>> min_transfers(n, vector<int>(r, INF));
    //иниц начальной остановки на всех маршрутах, где она есть
    for (int i = 0; i < r; ++i) {
        auto it = find(routes[i].begin(), routes[i].end(), start_stop);
        if (it != routes[i].end()) {
            min_dist[start_stop][i] = 0;
            min_transfers[start_stop][i] = 0;
        }
    }

    priority_queue<State, vector<State>, greater<State>> pq;
    for (int i = 0; i < r; ++i) {
        auto it = find(routes[i].begin(), routes[i].end(), start_stop);
        if (it != routes[i].end()) {
            pq.push({ start_stop, i, 0, 0 }); //Добавляем начальную остановку на каждый маршрут, где она есть
        }
    }

    while (!pq.empty()) {
        State current = pq.top();
        pq.pop();

        int u = current.stop;
        int current_route_index = current.route_index;
        int d = current.dist;
        int t = current.transfers;

        if (d > min_dist[u][current_route_index] || t > min_transfers[u][current_route_index]) continue;

        auto& current_route = routes[current_route_index];
        auto it = find(current_route.begin(), current_route.end(), u);

        if (it != current_route.end()) {
            int u_index = distance(current_route.begin(), it);
            //движение вперед по маршруту
            if (u_index + 1 < current_route.size()) {
                int v = current_route[u_index + 1];

                if (min_dist[v][current_route_index] > d + 1) {
                    min_dist[v][current_route_index] = d + 1;
                    min_transfers[v][current_route_index] = t;
                    pq.push({ v, current_route_index, d + 1, t });
                }
                else if (min_dist[v][current_route_index] == d + 1 && min_transfers[v][current_route_index] > t) {
                    min_transfers[v][current_route_index] = t;
                    pq.push({ v, current_route_index, d + 1, t });
                }
            }
            //движение назад по маршруту
            if (u_index - 1 >= 0) {
                int v = current_route[u_index - 1];

                if (min_dist[v][current_route_index] > d + 1) {
                    min_dist[v][current_route_index] = d + 1;
                    min_transfers[v][current_route_index] = t;
                    pq.push({ v, current_route_index, d + 1, t });
                }
                else if (min_dist[v][current_route_index] == d + 1 && min_transfers[v][current_route_index] > t) {
                    min_transfers[v][current_route_index] = t;
                    pq.push({ v, current_route_index, d + 1, t });
                }
            }
            //пересадки
            for (int other_route_index = 0; other_route_index < r; ++other_route_index) {
                if (other_route_index != current_route_index) {
                    auto it2 = find(routes[other_route_index].begin(), routes[other_route_index].end(), u);
                    if (it2 != routes[other_route_index].end()) {
                        if (min_dist[u][other_route_index] > d + TRANSFER_TIME) {
                            min_dist[u][other_route_index] = d + TRANSFER_TIME;
                            min_transfers[u][other_route_index] = t + 1;
                            pq.push({ u, other_route_index, d + TRANSFER_TIME, t + 1 });
                        }
                        else if (min_dist[u][other_route_index] == d + TRANSFER_TIME && min_transfers[u][other_route_index] > t + 1) {
                            min_transfers[u][other_route_index] = t + 1;
                            pq.push({ u, other_route_index, d + TRANSFER_TIME, t + 1 });
                        }
                    }
                }
            }
        }
    }

    int dist = INF;
    int transfers = INF;

    for (int i = 0; i < r; ++i) {
        dist = min(dist, min_dist[end_stop][i]);
    }
    for (int i = 0; i < r; ++i) {
        if (min_dist[end_stop][i] == dist) {
            transfers = min(transfers, min_transfers[end_stop][i]);
        }
    }

    if (dist == INF) {
        cout << "Нет пути от остановки " << start_stop + 1 << " до остановки " << end_stop + 1 << endl;
    }
    else {
        cout << "Наименьшее время от остановки " << start_stop + 1 << " до остановки " << end_stop + 1 << ": " << dist << endl;
        //cout << "Количество пересадок: " << transfers << endl;
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed_seconds = end - start;
    double time = elapsed_seconds.count();

    cout << "Время выполнения: " << time << " секунд" << endl;

    return 0;
}
