#include <iostream>
#include <vector>
#include <queue>
#include <limits>

using namespace std;

const int INF = numeric_limits<int>::max();
const int TRANSFER_TIME = 3;

// Структура для представления остановки в маршруте
struct StopNode {
    int stop;
    StopNode* next;
    StopNode* prev;

    StopNode(int stop_num) : stop(stop_num), next(nullptr), prev(nullptr) {}
};

// Структура для хранения информации о состоянии при обходе графа
struct State {
    int stop;           // Номер остановки
    int route_index;    // Индекс маршрута, на котором находимся
    int dist;           // Расстояние от начальной остановки
    int transfers;      // Количество пересадок

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

    vector<StopNode*> routes(r, nullptr); // Массив указателей на начало каждого маршрута
    for (int i = 0; i < r; ++i) {
        int route_length;
        cout << "Введите длину маршрута " << i + 1 << ": ";
        cin >> route_length;

        StopNode* head = nullptr;
        StopNode* tail = nullptr;
        cout << "Введите остановки для маршрута " << i + 1 << ": ";

        for (int j = 0; j < route_length; ++j) {
            int stop_num;
            cin >> stop_num;
            stop_num--;

            StopNode* newNode = new StopNode(stop_num);

            if (!head) {
                head = newNode;
                tail = newNode;
            }
            else {
                tail->next = newNode;
                newNode->prev = tail;
                tail = newNode;
            }
        }
        routes[i] = head; //указатель на начало маршрута
    }

    int start_stop, end_stop;
    cout << "Введите начальную остановку (I): ";
    cin >> start_stop;
    start_stop--;
    cout << "Введите конечную остановку (J): ";
    cin >> end_stop;
    end_stop--;
    vector<vector<int>> min_dist(n, vector<int>(r, INF));
    vector<vector<int>> min_transfers(n, vector<int>(r, INF));
    for (int i = 0; i < r; ++i) {
        StopNode* current = routes[i];
        while (current) {
            if (current->stop == start_stop) {
                min_dist[start_stop][i] = 0;
                min_transfers[start_stop][i] = 0;
                break;
            }
            current = current->next;
        }
    }

    priority_queue<State, vector<State>, greater<State>> pq;
    for (int i = 0; i < r; ++i) {
        StopNode* current = routes[i];
        while (current) {
            if (current->stop == start_stop) {
                pq.push({ start_stop, i, 0, 0 });
                break;
            }
            current = current->next;
        }
    }

    while (!pq.empty()) {
        State current_state = pq.top();
        pq.pop();

        int u = current_state.stop;
        int route_index = current_state.route_index;
        int d = current_state.dist;
        int t = current_state.transfers;

        if (d > min_dist[u][route_index] || t > min_transfers[u][route_index]) continue;
        StopNode* current_node = routes[route_index];
        while (current_node && current_node->stop != u) {
            current_node = current_node->next;
        }
        if (current_node) {
            if (current_node->next) {
                int v = current_node->next->stop;

                if (min_dist[v][route_index] > d + 1) {
                    min_dist[v][route_index] = d + 1;
                    min_transfers[v][route_index] = t;
                    pq.push({ v, route_index, d + 1, t });
                }
                else if (min_dist[v][route_index] == d + 1 && min_transfers[v][route_index] > t) {
                    min_transfers[v][route_index] = t;
                    pq.push({ v, route_index, d + 1, t });
                }
            }
            if (current_node->prev) {
                int v = current_node->prev->stop;

                if (min_dist[v][route_index] > d + 1) {
                    min_dist[v][route_index] = d + 1;
                    min_transfers[v][route_index] = t;
                    pq.push({ v, route_index, d + 1, t });
                }
                else if (min_dist[v][route_index] == d + 1 && min_transfers[v][route_index] > t) {
                    min_transfers[v][route_index] = t;
                    pq.push({ v, route_index, d + 1, t });
                }
            }
            //пересадка
            for (int other_route_index = 0; other_route_index < r; ++other_route_index) {
                if (other_route_index != route_index) {
                    StopNode* other_route_node = routes[other_route_index];
                    bool can_transfer = false;

                    while (other_route_node) {
                        if (other_route_node->stop == u) {
                            can_transfer = true;
                            break;
                        }
                        other_route_node = other_route_node->next;
                    }
                    if (can_transfer) {
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

    for (int i = 0; i < r; ++i) {
        StopNode* current = routes[i];
        while (current) {
            StopNode* next = current->next;
            delete current;
            current = next;
        }
    }

    return 0;
}