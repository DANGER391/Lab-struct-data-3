#include <iostream>
#include <queue>
#include <limits>
#include <chrono>

using namespace std;

const int INF = numeric_limits<int>::max();
const int TRANSFER_TIME = 3;
const int MAX_STOPS = 100; //Максимальное количество остановок
const int MAX_ROUTES = 50; //Максимальное количество маршрутов

struct State {
    int stop;
    int route_index;
    int dist;
    int transfers;
};

bool compareStates(const State& a, const State& b) {
    if (a.dist != b.dist) {
        return a.dist > b.dist;
    }
    return a.transfers > b.transfers;
}

int main() {
    system("chcp 1251");
    system("cls");
    cout << "Выполнил: Карпенко Денис Иванович\nГруппа: 020303-АИСа-24о\n";
    int n, r;
    cout << "Введите количество остановок (n): ";
    cin >> n;
    cout << "Введите количество маршрутов (r): ";
    if (n > MAX_STOPS) {
        cout << "Превышено максимальное количество остановок (" << MAX_STOPS << ")" << endl;
        return 1;
    }
    cin >> r;
    if (r > MAX_ROUTES) {
        cout << "Превышено максимальное количество маршрутов (" << MAX_ROUTES << ")" << endl;
        return 1;
    }

    int routes[MAX_ROUTES][MAX_STOPS];
    int route_lengths[MAX_ROUTES];
    for (int i = 0; i < r; ++i) {
        cout << "Введите длину маршрута " << i + 1 << ": ";
        cin >> route_lengths[i];
        if (route_lengths[i] > MAX_STOPS) {
            cout << "Превышена максимальная длина маршрута (" << MAX_STOPS << ")" << endl;
            return 1;
        }

        cout << "Введите остановки для маршрута " << i + 1 << ": ";
        for (int j = 0; j < route_lengths[i]; ++j) {
            cin >> routes[i][j];
            routes[i][j]--;
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

    int min_dist[MAX_STOPS][MAX_ROUTES];
    int min_transfers[MAX_STOPS][MAX_ROUTES];

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < r; ++j) {
            min_dist[i][j] = INF;
            min_transfers[i][j] = INF;
        }
    }

    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < route_lengths[i]; ++j) {
            if (routes[i][j] == start_stop) {
                min_dist[start_stop][i] = 0;
                min_transfers[start_stop][i] = 0;
                break;
            }
        }
    }

    State pq[MAX_STOPS * MAX_ROUTES];
    int pq_size = 0;

    auto push_pq = [&](const State& s) {
        if (pq_size < MAX_STOPS * MAX_ROUTES) {
            pq[pq_size] = s;
            int i = pq_size;
            while (i > 0 && compareStates(pq[i], pq[(i - 1) / 2])) {
                swap(pq[i], pq[(i - 1) / 2]);
                i = (i - 1) / 2;
            }
            pq_size++;
        }
        else {
            cout << "Ошибка: переполнение очереди приоритетов" << endl;
        }//проверка на переполнение
    };

    auto pop_pq = [&]() -> void {
        if (pq_size == 0) return;
        pq[0] = pq[pq_size - 1];
        pq_size--;

        int i = 0;
        while (true) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int smallest = i;

            if (left < pq_size && compareStates(pq[left], pq[smallest])) {
                smallest = left;
            }
            if (right < pq_size && compareStates(pq[right], pq[smallest])) {
                smallest = right;
            }

            if (smallest == i) break;

            swap(pq[i], pq[smallest]);
            i = smallest;
        }
    };

    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < route_lengths[i]; ++j) {
            if (routes[i][j] == start_stop) {
                push_pq({ start_stop, i, 0, 0 });
                break;
            }
        }
    }

    while (pq_size > 0) {
        State current_state = pq[0];
        pop_pq();

        int u = current_state.stop;
        int current_route_index = current_state.route_index;
        int d = current_state.dist;
        int t = current_state.transfers;

        if (d > min_dist[u][current_route_index] || t > min_transfers[u][current_route_index]) continue;

        for (int u_index = 0; u_index < route_lengths[current_route_index]; ++u_index) {
            if (routes[current_route_index][u_index] == u) {
                if (u_index + 1 < route_lengths[current_route_index]) {
                    int v = routes[current_route_index][u_index + 1];

                    if (min_dist[v][current_route_index] > d + 1) {
                        min_dist[v][current_route_index] = d + 1;
                        min_transfers[v][current_route_index] = t;
                        push_pq({ v, current_route_index, d + 1, t });
                    }
                    else if (min_dist[v][current_route_index] == d + 1 && min_transfers[v][current_route_index] > t) {
                        min_transfers[v][current_route_index] = t;
                        push_pq({ v, current_route_index, d + 1, t });
                    }
                }
                if (u_index - 1 >= 0) {
                    int v = routes[current_route_index][u_index - 1];

                    if (min_dist[v][current_route_index] > d + 1) {
                        min_dist[v][current_route_index] = d + 1;
                        min_transfers[v][current_route_index] = t;
                        push_pq({ v, current_route_index, d + 1, t });
                    }
                    else if (min_dist[v][current_route_index] == d + 1 && min_transfers[v][current_route_index] > t) {
                        min_transfers[v][current_route_index] = t;
                        push_pq({ v, current_route_index, d + 1, t });
                    }
                }
                break;
            }
        }
        //пересадка
        for (int other_route_index = 0; other_route_index < r; ++other_route_index) {
            if (other_route_index != current_route_index) {
                for (int j = 0; j < route_lengths[other_route_index]; ++j) {
                    int v = routes[other_route_index][j];
                    if (v == u) {
                        if (min_dist[u][other_route_index] > d + TRANSFER_TIME) {
                            min_dist[u][other_route_index] = d + TRANSFER_TIME;
                            min_transfers[u][other_route_index] = t + 1;
                            push_pq({ u, other_route_index, d + TRANSFER_TIME, t + 1 });
                        }
                        else if (min_dist[u][other_route_index] == d + TRANSFER_TIME && min_transfers[u][other_route_index] > t + 1) {
                            min_transfers[u][other_route_index] = t + 1;
                            push_pq({ u, other_route_index, d + TRANSFER_TIME, t + 1 });
                        }
                        break;
                    }
                }
            }
        }
    }

    int best_dist = INF;
    int best_transfers = INF;

    for (int i = 0; i < r; ++i) {
        best_dist = min(best_dist, min_dist[end_stop][i]);
    }
    for (int i = 0; i < r; ++i) {
        if (min_dist[end_stop][i] == best_dist) {
            best_transfers = min(best_transfers, min_transfers[end_stop][i]);
        }
    }

    if (best_dist == INF) {
        cout << "Нет пути от остановки " << start_stop + 1 << " до остановки " << end_stop + 1 << endl;
    }
    else {
        cout << "Наименьшее время от остановки " << start_stop + 1 << " до остановки " << end_stop + 1 << ": " << best_dist << endl;
        //cout << "Количество пересадок: " << best_transfers << endl;
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed_seconds = end - start;
    double time = elapsed_seconds.count();
    cout << "Время выполнения: " << time << " секунд" << endl;

    return 0;
}
